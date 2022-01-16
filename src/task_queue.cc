#include "task_queue.h"

#include <GLFW/glfw3.h>

static inline double Now() {
  return glfwGetTime();
}

TaskQueue::TaskQueue() : post_empty_event_(false) {}

TaskQueue::~TaskQueue() {}

void TaskQueue::Post(Task task) {
  {
    std::lock_guard<std::mutex> lock(lock_);
    tasks_.emplace(std::move(task));
  }
  if (post_empty_event_) {
    glfwPostEmptyEvent();
  }
}

void TaskQueue::Post(double delay_in_seconds, Task task) {
  {
    double when = Now() + delay_in_seconds;
    std::lock_guard<std::mutex> lock(lock_);
    delayed_tasks_.emplace(DelayedTask{std::move(task), when});
  }
  if (post_empty_event_) {
    glfwPostEmptyEvent();
  }
}

double TaskQueue::GetSecondsToNextTask() const {
  std::lock_guard<std::mutex> lock(lock_);
  if (!tasks_.empty()) {
    return 0;
  }
  if (delayed_tasks_.empty()) {
    return -1;
  }
  double interval = delayed_tasks_.top().when - Now();
  return interval <= 0 ? 0 : interval;
}

void TaskQueue::RunTasks() {
  for (;;) {
    Task task;

    {
      std::lock_guard<std::mutex> lock(lock_);

      // Process delayed tasks first, to prevent immediate tasks from delaying
      // these indefinitely.
      if (!delayed_tasks_.empty() && Now() >= delayed_tasks_.top().when) {
        task = std::move(delayed_tasks_.top().task);
        delayed_tasks_.pop();
      } else if (!tasks_.empty()) {
        task = std::move(tasks_.front());
        tasks_.pop();
      } else {
        return;
      }
    }

    task();
  }
}

void TaskQueue::ResetDropAllTasks() {
  std::queue<Task> tasks;
  std::priority_queue<DelayedTask> delayed_tasks;
  {
    std::lock_guard<std::mutex> lock(lock_);
    tasks_.swap(tasks);
    delayed_tasks_.swap(delayed_tasks);
  }
  // Run destructors without the lock.
}

ThreadPoolTaskQueue::ThreadPoolTaskQueue(int num_threads) : quit_(false) {
  threads_.resize(num_threads);
  for (int i = 0; i < num_threads; i++) {
    threads_[i] = std::thread(&ThreadPoolTaskQueue::Run, this);
  }
}

ThreadPoolTaskQueue::~ThreadPoolTaskQueue() {
  {
    std::lock_guard<std::mutex> lock(lock_);
    quit_ = true;
  }
  cond_var_.notify_all();
  for (unsigned int i = 0; i < threads_.size(); i++) {
    threads_[i].join();
  }
}

void ThreadPoolTaskQueue::Post(Task task) {
  {
    std::lock_guard<std::mutex> lock(lock_);
    tasks_.emplace(std::move(task));
  }
  cond_var_.notify_one();
}

void ThreadPoolTaskQueue::Post(double delay_in_seconds, Task task) {
  double when = Now() + delay_in_seconds;
  {
    std::lock_guard<std::mutex> lock(lock_);
    delayed_tasks_.emplace(DelayedTask{std::move(task), when});
  }
  cond_var_.notify_one();
}

void ThreadPoolTaskQueue::Run() {
  for (;;) {
    Task task;

    {
      std::unique_lock<std::mutex> lock(lock_);

      for (;;) {
        double now = Now();
        if (quit_) {
          return;
        } else if (!delayed_tasks_.empty() &&
                   now >= delayed_tasks_.top().when) {
          task = std::move(delayed_tasks_.top().task);
          delayed_tasks_.pop();
          break;
        } else if (!tasks_.empty()) {
          task = std::move(tasks_.front());
          tasks_.pop();
          break;
        } else {
          if (delayed_tasks_.empty()) {
            cond_var_.wait(lock);
          } else {
            double timeout = delayed_tasks_.top().when - now;
            cond_var_.wait_for(lock, std::chrono::duration<double>(timeout));
          }
        }
      }
    }

    task();
  }
}

void ThreadPoolTaskQueue::ResetDropAllTasks() {
  std::queue<Task> tasks;
  std::priority_queue<DelayedTask> delayed_tasks;
  {
    std::lock_guard<std::mutex> lock(lock_);
    tasks_.swap(tasks);
    delayed_tasks_.swap(delayed_tasks);
  }
  // Run destructors without the lock.
}
