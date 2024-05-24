/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     threadsafe_list.h
 * @brief    Threadsafe list implementataion
 *
 * Multiple producer, multiple consumer thread safe list
 **/

#ifndef NEXT_SDK_THREADSAFE_LIST_H_
#define NEXT_SDK_THREADSAFE_LIST_H_

#include <condition_variable>
#include <exception>
#include <list>
#include <mutex>

namespace next {
namespace sdk {

//! template Class multi-consumer multi-producer threadsafe queue
/*!
 *
 */
template <typename T>
class threadsafe_list {
private:
  mutable std::mutex mutex_;          //! Mutex for synchronisation
  std::list<T> list_;                 //! Internal queue
  std::condition_variable data_cond_; //! Condition variable to be used in wait_and_pop

  threadsafe_list &operator=(const threadsafe_list &) = delete;
  threadsafe_list(const threadsafe_list &other) = delete;

public:
  threadsafe_list() = default;

  //! Push value into the Queue
  /*!
   * @param value value to be pushed
   */
  void push_back(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    list_.push_back(std::move(value));
    data_cond_.notify_one();
  }

  //! Try to retrieve element and return immediatelly
  /*!
   * @param value retrieved value
   */
  bool try_and_pop_back(T &value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (list_.empty()) {
      return false;
    }
    value = std::move(list_.back());
    list_.pop_back();
    return true;
  }

  //! Try to retrieve, if no items, wait till an item is available and try again
  /*!
   * @param value retrieved value
   */
  void wait_and_pop_back(T &value) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_cond_.wait(lock, [this] { return !list_.empty(); });
    value = std::move(list_.back());
    list_.pop_back();
  }

  //! Check if queue is empty
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return list_.empty();
  }
  //! Return the size of the queue
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return list_.size();
  }

  //! Removes all consecutive duplicate elements from the container. Only the first element in each group of equal
  //! elements is left. Uses '==' operator of the element
  void unique() {
    std::unique_lock<std::mutex> lock(mutex_);
    list_.unique();
  }
};

} // namespace sdk
} // namespace next
#endif // NEXT_SDK_THREADSAFE_LIST_H_
