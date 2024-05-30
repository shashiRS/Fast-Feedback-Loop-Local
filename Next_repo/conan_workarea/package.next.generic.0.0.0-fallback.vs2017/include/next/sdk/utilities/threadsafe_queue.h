/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     threadsafe_queue.h
 * @brief    Threadsafe queue implementataion
 *
 * Multiple producer, multiple consumer thread safe queue
 **/

#ifndef NEXT_SDK_THREADSAFE_QUEUE_H_
#define NEXT_SDK_THREADSAFE_QUEUE_H_

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

namespace next {
namespace sdk {

//! template Class multi-consumer multi-producer threadsafe queue
/*!
 *
 */
template <typename T>
class threadsafe_queue {
private:
  mutable std::mutex mutex_;          //! Mutex for synchronisation
  std::queue<T> queue_;               //! Internal queue
  std::condition_variable data_cond_; //! Condition variable to be used in wait_and_pop

  threadsafe_queue &operator=(const threadsafe_queue &) = delete;
  threadsafe_queue(const threadsafe_queue &other) = delete;

public:
  threadsafe_queue() = default;

  //! Push value into the Queue
  /*!
   * @param value value to be pushed
   */
  void push(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(value));
    data_cond_.notify_one();
  }

  //! Try to retrieve element and return immediatelly
  /*!
   * @param value retrieved value
   */
  bool try_and_pop(T &value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  //! Try to retrieve, if no items, wait till an item is available and try again
  /*!
   * @param value retrieved value
   */
  void wait_and_pop(T &value) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_cond_.wait(lock, [this] { return !queue_.empty(); });
    value = std::move(queue_.front());
    queue_.pop();
  }

  //! Check if queue is empty
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }
  //! Return the size of the queue
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }
};

} // namespace sdk
} // namespace next
#endif // NEXT_SDK_THREADSAFE_QUEUE_H_
