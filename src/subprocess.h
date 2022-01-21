#ifndef WINDOWJS_SUBPROCESS_H
#define WINDOWJS_SUBPROCESS_H

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <uv.h>

#include "platform.h"

#if defined(WINDOWJS_WIN)
// Defined as macros in winuser.h, included via <uv.h>.
#undef PostMessage
#undef SendMessage
#endif

// Pipe enables parent <-> child process IPC.
//
// Each Pipe runs in a background thread that wraps a libuv loop.
//
// Pipes must be created and deleted in the main thread.
// Messages can be sent from the main thread too.
//
// Messages are received and dispatched in the background thread.
//
// Deleting a Pipe syncs on the background thread terminating and cleans up
// any libuv state.
//
// Each messages contains exactly 3 parts:
// 1. 4 bytes "type"
// 2. 4 bytes "length"
// 3. "length" bytes for the remaining "message".
//
// The uint32 "type" and std::string "message" are passed to callbacks.
class Pipe final {
 public:
  // These get called in the background thread for this Pipe.
  // The "type" and "message" are defined by the client and any values can
  // be used. Having the "type" separated from the "message" enables passing
  // buffers and strings efficiently, without requiring parsing an outer
  // message encoded in the "message", for example.
  using OnMessage = std::function<void(uint32_t type, std::string message)>;

  // If this Pipe is the child-process side then this gets called when the
  // parent closes its side of the pipe, and the "status" is always 0.
  // Otherwise, this gets called in the parent process when the child-side
  // of the pipe exists, and "status" is the exit code of the child process.
  using OnClose = std::function<void(int64_t status, std::string error)>;

  static std::unique_ptr<Pipe> Spawn(std::string exe_path,
                                     std::vector<std::string> args, bool log,
                                     OnMessage on_message, OnClose on_close);

  static std::unique_ptr<Pipe> AttachToParent(OnMessage on_message,
                                              OnClose on_close);

  // Can be called on any thread.
  void SendMessage(uint32_t type, std::string message);

  bool is_child_process() const { return child_ == nullptr; }

  ~Pipe();

 private:
  struct WriteMessage {
    std::string message;
    uint32_t type;
    uint32_t size;
    uv_write_t write;
  };

  Pipe(OnMessage on_message, OnClose on_close);

  void Run();

  static void OnAsyncQuit(uv_async_t* handle);
  static void OnAsyncSend(uv_async_t* handle);
  WriteMessage* AllocateWriteMessage(uint32_t type, std::string message);
  static void OnSendCallback(uv_write_t* write, int status);
  static void OnChildExit(uv_process_t* child, int64_t exit_status,
                          int term_signal);
  void ReportClosed(int64_t status, std::string error);
  static void AllocReadBuf(uv_handle_t* handle, size_t suggested_size,
                           uv_buf_t* buf);
  static void OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  void ReportMessage(uint32_t type, std::string message);

  std::thread thread_;

  uv_loop_t loop_;
  uv_async_t async_quit_;
  uv_async_t async_send_;
  uv_pipe_t pipe_;
  std::unique_ptr<uv_process_t> child_;

  OnMessage on_message_;
  OnClose on_close_;

  std::vector<uint32_t> send_types_;
  std::vector<std::string> send_messages_;
  std::mutex send_lock_;

  // PendingMessages and their contents must have stable pointers.
  std::vector<std::unique_ptr<WriteMessage>> writes_;

  uint32_t read_type_;
  uint32_t read_size_;
  uint32_t read_done_;
  std::string read_;
  bool expecting_read_header_;
};

#endif  // WINDOWJS_SUBPROCESS_H
