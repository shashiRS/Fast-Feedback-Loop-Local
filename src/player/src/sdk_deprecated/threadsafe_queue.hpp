/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     threadsafe_queue.hpp
 * @brief    Threadsafe queue implementataion
 *
 * Multiple producer, multiple consumer thread safe queue
 **/

#ifndef THREADSAFE_QUEUE_
#define THREADSAFE_QUEUE_

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

namespace next {
namespace player {

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
  bool term_{false};
  threadsafe_queue &operator=(const threadsafe_queue &) = delete;
  threadsafe_queue(const threadsafe_queue &other) = delete;

public:
  threadsafe_queue() = default;
  virtual ~threadsafe_queue() { Shutdown(); };
  void Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    term_ = true;
    data_cond_.notify_one();
  };
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
    if (!term_) {
      data_cond_.wait(lock, [this] {
        bool retVal = false;
        if (!queue_.empty())
          retVal = true;
        if (term_)
          retVal = true;
        return retVal;
      });
      if (!term_) {
        value = std::move(queue_.front());
        queue_.pop();
      }
    }
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

} // namespace player
} // namespace next
#endif
