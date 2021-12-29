#ifndef WINDOWJS_SIGNAL_H
#define WINDOWJS_SIGNAL_H

#include <chrono>
#include <condition_variable>
#include <mutex>

class Signal {
 public:
  Signal() : signalled_(false) {}
  ~Signal() {}

  Signal(const Signal&) = delete;
  Signal& operator=(const Signal&) = delete;

  // Returns if the Signal is already signalled, or after it is signalled at
  // least once in the future.
  void Wait() {
    std::unique_lock lock(lock_);
    while (!signalled_) {
      cond_.wait(lock);
    }
  }

  // Similar to Wait, but leaves the Signal cleared before returning.
  void WaitAndClear() {
    std::unique_lock lock(lock_);
    while (!signalled_) {
      cond_.wait(lock);
    }
    signalled_ = false;
  }

  // Returns true if the Signal is already signalled, or after it is signalled
  // at least once in the future. Returns false if "seconds" or more elapses
  // before receiving a signal.
  bool WaitFor(double seconds) {
    std::unique_lock lock(lock_);
    if (!signalled_) {
      std::chrono::duration<double> duration(seconds);
      cond_.wait_for(lock, duration);
    }
    return signalled_;
  }

  // Sets the Signal as signalled and wakes up any threads blocked on it.
  void SetAndNotify() {
    {
      std::lock_guard lock(lock_);
      signalled_ = true;
    }
    cond_.notify_all();
  }

  // Clears the signal. Subsequent Waits() will block again.
  void Clear() {
    std::lock_guard lock(lock_);
    signalled_ = false;
  }

 private:
  std::mutex lock_;
  std::condition_variable cond_;
  bool signalled_;
};

#endif  // WINDOWJS_SIGNAL_H
