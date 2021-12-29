#ifndef WINDOWJS_TASK_QUEUE_H
#define WINDOWJS_TASK_QUEUE_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

using Task = std::function<void()>;

struct DelayedTask {
  Task task;
  double when;

  bool operator<(const DelayedTask& t) const { return when > t.when; }
};

class TaskQueue {
 public:
  TaskQueue() = default;
  ~TaskQueue() = default;

  TaskQueue(const TaskQueue&) = delete;
  TaskQueue& operator=(const TaskQueue&) = delete;
  TaskQueue(TaskQueue&&) = delete;
  TaskQueue&& operator=(TaskQueue&&) = delete;

  void Post(Task task);
  void Post(double delay_in_seconds, Task task);

  // Returns -1 if there are no delayed tasks in the queue.
  // Returns 0 if there are tasks ready to be executed immediately.
  double GetSecondsToNextTask() const;

  // Runs all the tasks that can be executed now, and returns.
  void RunTasks();

  void ResetDropAllTasks();

 private:
  mutable std::mutex lock_;
  std::queue<Task> tasks_;
  std::priority_queue<DelayedTask> delayed_tasks_;
};

class ThreadPoolTaskQueue {
 public:
  // Spawns "num_threads" that keep running and pumping tasks until the queue
  // is deleted.
  explicit ThreadPoolTaskQueue(int num_threads);

  // Joins on all threads before returning.
  ~ThreadPoolTaskQueue();

  ThreadPoolTaskQueue(const ThreadPoolTaskQueue&) = delete;
  ThreadPoolTaskQueue& operator=(const ThreadPoolTaskQueue&) = delete;
  ThreadPoolTaskQueue(ThreadPoolTaskQueue&&) = delete;
  ThreadPoolTaskQueue&& operator=(ThreadPoolTaskQueue&&) = delete;

  void Post(Task task);
  void Post(double delay_in_seconds, Task task);

  void ResetDropAllTasks();

 private:
  void Run();

  std::mutex lock_;
  std::condition_variable cond_var_;
  std::queue<Task> tasks_;
  std::priority_queue<DelayedTask> delayed_tasks_;
  bool quit_;

  std::vector<std::thread> threads_;
};

#endif  // WINDOWJS_TASK_QUEUE_H
