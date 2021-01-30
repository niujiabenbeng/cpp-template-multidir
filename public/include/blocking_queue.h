#ifndef PUBLIC_BLOCKING_QUEUE_H_
#define PUBLIC_BLOCKING_QUEUE_H_

#include "common.h"

template <class T> class BlockingQueue {
 public:
  explicit BlockingQueue(int capacity) : capacity_(capacity) {}
  DISABLE_COPY_ASIGN(BlockingQueue);
  DISABLE_MOVE_ASIGN(BlockingQueue);
  ~BlockingQueue() { abort(); }

  // 下面这些获取队列状态的函数获取的只是当前的队列状态, 在多线程的情况下
  // 容易产生race condition, 使用的时候需要特别注意.
  bool full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size() == capacity_;
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }
  int size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }
  void clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) { queue_.pop(); }
  }
  int capacity() const { return capacity_; }

  bool push(T value) {
    if (true) {
      std::unique_lock<std::mutex> lock(mutex_);
      // clang-format off
      condition_push_.wait(lock, [this] {
          return (queue_.size() < capacity_) || aborted_;
      });
      // clang-format on
      // abort状态下直接返回
      if (aborted_) { return false; }
      queue_.push(std::move(value));
    }
    condition_pop_.notify_one();
    return true;
  }
  bool pop(T& value) {
    if (true) {
      std::unique_lock<std::mutex> lock(mutex_);
      // clang-format off
      condition_pop_.wait(lock, [this] {
          return (!queue_.empty()) || aborted_;
      });
      // clang-format on
      // 如果queue中有数据则优先取出数据
      if (aborted_ && queue_.empty()) { return false; }
      value = std::move(queue_.front());
      queue_.pop();
    }
    condition_push_.notify_one();
    return true;
  }
  void abort() {
    if (true) {
      std::lock_guard<std::mutex> lock(mutex_);
      aborted_ = true;
    }
    condition_pop_.notify_all();
    condition_push_.notify_all();
  }

 private:
  int capacity_;
  std::queue<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable condition_pop_;
  std::condition_variable condition_push_;
  bool aborted_ = false;
};

template <class T>  // 这里只能用using
using BlockingQueuePtr = std::shared_ptr<BlockingQueue<T>>;

#endif  // PUBLIC_BLOCKING_QUEUE_H_
