#include "thread.h"

#include <thread>

#include "fail.h"

void InitMainThread() {
  // Sets up the static std::thread:id in IsMainThread.
  ASSERT(IsMainThread());
}

bool IsMainThread() {
  static std::thread::id main_thread_id = std::this_thread::get_id();
  return std::this_thread::get_id() == main_thread_id;
}
