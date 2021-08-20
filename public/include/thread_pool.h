#ifndef PUBLIC_THREAD_POOL_H_
#define PUBLIC_THREAD_POOL_H_

#include "common.h"

// copy from: https://github.com/progschj/ThreadPool
class ThreadPool {
 public:
  explicit ThreadPool(int num_threads);
  DISABLE_COPY_ASIGN(ThreadPool);
  DISABLE_MOVE_ASIGN(ThreadPool);
  ~ThreadPool();

  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  bool stop_{false};
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    workers_.emplace_back([this] {
      while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
        if (stop_ && tasks_.empty()) { return; }
        auto task = std::move(tasks_.front());
        tasks_.pop();
        // task运行耗时较长, 所以这里得先unlock
        lock.unlock();
        task();
      }
    });
  }
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  CHECK(!stop_) << "Enqueueing is not allowed when the pool is stopped.";

  using return_type = typename std::result_of<F(Args...)>::type;
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.emplace([task]() { (*task)(); });
  }
  condition_.notify_one();
  return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  ATOMIC_SET(mutex_, stop_, true);
  condition_.notify_all();
  for (std::thread& worker : workers_) { worker.join(); }
}

#endif  // PUBLIC_THREAD_POOL_H_
