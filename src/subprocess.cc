#include "subprocess.h"

#include <string.h>

#include <algorithm>
#include <sstream>

#include "fail.h"
#include "file.h"
#include "thread.h"

// static
std::unique_ptr<Pipe> Pipe::Spawn(std::string exe_path,
                                  std::vector<std::string> args, bool log,
                                  OnMessage on_message, OnClose on_close) {
  ASSERT(IsMainThread());

  std::unique_ptr<Pipe> pipe{
      new Pipe(std::move(on_message), std::move(on_close))};

  pipe->thread_ = std::thread([exe_path = std::move(exe_path),
                               args = std::move(args), log, pipe = pipe.get()] {
    std::vector<char*> argv;
    argv.resize(args.size() + 1);
    for (unsigned i = 0; i < args.size(); i++) {
      argv[i] = (char*) args[i].c_str();
    }
    argv[args.size()] = nullptr;

    uv_process_options_t options = {0};
    options.exit_cb = OnChildExit;
    options.file = exe_path.c_str();
    options.args = argv.data();
    options.flags = UV_PROCESS_WINDOWS_HIDE | UV_PROCESS_WINDOWS_HIDE_CONSOLE |
                    UV_PROCESS_WINDOWS_HIDE_GUI;
    options.stdio_count = 4;
    uv_stdio_container_t child_stdio[4];
    child_stdio[0].flags = UV_IGNORE;
    if (log) {
      child_stdio[1].flags = UV_INHERIT_FD;
      child_stdio[1].data.fd = 1;
      child_stdio[2].flags = UV_INHERIT_FD;
      child_stdio[2].data.fd = 2;
    } else {
      child_stdio[1].flags = UV_IGNORE;
      child_stdio[2].flags = UV_IGNORE;
    }
    // Pipe used for parent <-> child IPC.
    child_stdio[3].flags =
        (uv_stdio_flags) (UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
    child_stdio[3].data.stream = (uv_stream_t*) &pipe->pipe_;
    options.stdio = child_stdio;

    pipe->child_.reset(new uv_process_t);
    ASSERT_UV(uv_spawn(&pipe->loop_, pipe->child_.get(), &options));
    ASSERT_UV(uv_read_start((uv_stream_t*) &pipe->pipe_, AllocReadBuf, OnRead));
    pipe->Run();
  });

  return pipe;
}

// static
std::unique_ptr<Pipe> Pipe::AttachToParent(OnMessage on_message,
                                           OnClose on_close) {
  ASSERT(IsMainThread());

  std::unique_ptr<Pipe> pipe{
      new Pipe(std::move(on_message), std::move(on_close))};

  // File descriptor 3 is the pipe created by the parent; see child_stdio[3]
  // in Spawn.
  ASSERT_UV(uv_pipe_open(&pipe->pipe_, 3));
  ASSERT_UV(uv_read_start((uv_stream_t*) &pipe->pipe_, AllocReadBuf, OnRead));

  pipe->thread_ = std::thread([pipe = pipe.get()] {
    pipe->Run();
  });

  return pipe;
}

Pipe::Pipe(OnMessage on_message, OnClose on_close)
    : on_message_(std::move(on_message)),
      on_close_(std::move(on_close)),
      expecting_read_header_(true) {
  ASSERT_UV(uv_loop_init(&loop_));
  ASSERT_UV(uv_async_init(&loop_, &async_quit_, OnAsyncQuit));
  ASSERT_UV(uv_async_init(&loop_, &async_send_, OnAsyncSend));
  // TODO: why doesn't this work when IPC is set to 0?
  ASSERT_UV(uv_pipe_init(&loop_, &pipe_, 1));
  loop_.data = this;
}

Pipe::~Pipe() {
  ASSERT(IsMainThread());

  uv_async_send(&async_quit_);

  thread_.join();

  if (!is_child_process() && !uv_is_closing((const uv_handle_t*) &pipe_)) {
    // Don't close the pipe on child processes, so that they can reload.
    // The pipe is closed anyway once they exit.
    uv_close((uv_handle_t*) &pipe_, nullptr);
  }

  uv_close((uv_handle_t*) &async_quit_, nullptr);
  uv_close((uv_handle_t*) &async_send_, nullptr);
  if (child_) {
    uv_close((uv_handle_t*) child_.get(), nullptr);
  }

  uv_loop_close(&loop_);
}

// static
void Pipe::OnAsyncQuit(uv_async_t* handle) {
  ASSERT(!IsMainThread());
  uv_stop(handle->loop);
}

// static
void Pipe::OnChildExit(uv_process_t* child, int64_t exit_status,
                       int term_signal) {
  ASSERT(!IsMainThread());
  Pipe* pipe = (Pipe*) child->loop->data;
  std::string error;
  if (exit_status != 0 || term_signal != 0) {
    std::stringstream ss;
    ss << "Child exited with non-zero status or signal. Status: " << exit_status
       << " signal: " << term_signal;
    error = ss.str();
  }
  pipe->ReportClosed(exit_status, std::move(error));
  uv_stop(child->loop);
}

void Pipe::ReportClosed(int64_t status, std::string error) {
  if (on_close_) {
    on_close_(status, std::move(error));
    on_close_ = {};
  }
}

void Pipe::SendMessage(uint32_t type, std::string message) {
  {
    std::lock_guard lock(send_lock_);
    send_types_.push_back(type);
    send_messages_.emplace_back(std::move(message));
  }
  uv_async_send(&async_send_);
}

// static
void Pipe::OnAsyncSend(uv_async_t* handle) {
  ASSERT(!IsMainThread());

  Pipe* pipe = (Pipe*) handle->loop->data;

  std::vector<uint32_t> types;
  std::vector<std::string> messages;
  {
    std::lock_guard lock(pipe->send_lock_);
    pipe->send_types_.swap(types);
    pipe->send_messages_.swap(messages);
  }

  for (unsigned i = 0; i < messages.size(); i++) {
    WriteMessage* write =
        pipe->AllocateWriteMessage(types[i], std::move(messages[i]));
    uv_buf_t buf[3];
    buf[0].base = (char*) &write->type;
    buf[0].len = 4;
    buf[1].base = (char*) &write->size;
    buf[1].len = 4;
    buf[2].base = write->message.data();
    buf[2].len = write->message.size();
    uv_write(&write->write, (uv_stream_t*) &pipe->pipe_, buf, 3,
             OnSendCallback);
  }
}

Pipe::WriteMessage* Pipe::AllocateWriteMessage(uint32_t type,
                                               std::string message) {
  ASSERT(!IsMainThread());
  int index = -1;
  for (unsigned i = 0; i < writes_.size(); i++) {
    if (writes_[i]->size == 0) {
      index = i;
      break;
    }
  }
  if (index < 0) {
    index = writes_.size();
    writes_.emplace_back(new WriteMessage);
  }
  writes_[index]->size = message.size();
  writes_[index]->message = std::move(message);
  writes_[index]->type = type;
  writes_[index]->write.data = writes_[index].get();
  return writes_[index].get();
}

// static
void Pipe::OnSendCallback(uv_write_t* write, int status) {
  ASSERT(!IsMainThread());
  WriteMessage* w = (WriteMessage*) write->data;
  w->message.clear();
  w->size = 0;
}

// static
void Pipe::AllocReadBuf(uv_handle_t* handle, size_t suggested_size,
                        uv_buf_t* buf) {
  buf->base = (char*) malloc(suggested_size);
  buf->len = suggested_size;
}

// static
void Pipe::OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  ASSERT(!IsMainThread());
  Pipe* pipe = (Pipe*) stream->loop->data;
  if (nread == UV_EOF) {
    // If a child process quits then its exit is handled via OnChildExit,
    // which also captures the exit status. But if a parent process exits,
    // or otherwise closes the pipe (e.g. by reloading), then the child
    // will read UV_EOF here and should propagate the close event.
    if (pipe->is_child_process()) {
      // We don't have a child, so we're attached to a parent process that
      // has closed its end of the pipe; OnChildExit won't be called in this
      // case.
      pipe->ReportClosed(0, "Parent process closed the pipe.");
    }
    uv_read_stop(stream);
    uv_close((uv_handle_t*) stream, nullptr);
  } else if (nread < 0) {
    std::stringstream ss;
    ss << "Failed to read from pipe: " << uv_strerror(nread);
    pipe->ReportClosed(-1, ss.str());
    uv_read_stop(stream);
    uv_close((uv_handle_t*) stream, nullptr);
  } else if (nread > 0) {
    ASSERT(nread <= (ssize_t) buf->len);
    char* base = buf->base;
    uint32_t size = nread;
    while (size > 0) {
      if (pipe->expecting_read_header_) {
        // TODO: it's not guaranteed that we have 8 bytes here every time ...
        ASSERT(size >= 8);
        pipe->read_type_ = ((uint32_t*) base)[0];
        pipe->read_size_ = ((uint32_t*) base)[1];
        pipe->read_done_ = 0;
        base += 8;
        size -= 8;
        pipe->read_.resize(pipe->read_size_);
        pipe->expecting_read_header_ = false;
      }
#undef min
      int can_read = std::min(size, pipe->read_size_ - pipe->read_done_);
      if (can_read > 0) {
        memcpy(pipe->read_.data() + pipe->read_done_, base, can_read);
        size -= can_read;
        base += can_read;
        pipe->read_done_ += can_read;
      }
      if (pipe->read_done_ == pipe->read_size_) {
        pipe->ReportMessage(pipe->read_type_, std::move(pipe->read_));
        pipe->read_.clear();
        pipe->read_size_ = 0;
        pipe->read_done_ = 0;
        pipe->expecting_read_header_ = true;
      }
    }
  }
  free(buf->base);
}

void Pipe::ReportMessage(uint32_t type, std::string message) {
  ASSERT(!IsMainThread());
  on_message_(type, std::move(message));
}

void Pipe::Run() {
  ASSERT(!IsMainThread());
  uv_run(&loop_, UV_RUN_DEFAULT);
}
