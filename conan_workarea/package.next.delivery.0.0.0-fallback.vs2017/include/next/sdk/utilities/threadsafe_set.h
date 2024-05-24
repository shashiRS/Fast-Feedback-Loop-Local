/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     threadsafe_set.h
 * @brief    Threadsafe set implementataion
 *
 * Multiple producer, multiple consumer thread safe set container
 * Uses in the background a map-container to be able to replace
 * easily existing objects. Don't use boost multi_index container
 * to be independent of 3rd party libraries.
 **/

#ifndef NEXT_SDK_THREADSAFE_SET_H_
#define NEXT_SDK_THREADSAFE_SET_H_

#include <condition_variable>
#include <exception>
#include <map>
#include <mutex>

namespace next {
namespace sdk {

//! template Class multi-consumer multi-producer threadsafe set container
/*!
 *
 */
template <typename T>
class threadsafe_set {
private:
  mutable std::mutex mutex_;          //! Mutex for synchronisation
  std::map<T, unsigned char> set_;    //! Internal list of unique elements
  std::condition_variable data_cond_; //! Condition variable to be used in wait_and_pop

  threadsafe_set &operator=(const threadsafe_set &) = delete;
  threadsafe_set(const threadsafe_set &other) = delete;

public:
  threadsafe_set() = default;

  //! insert value and replace existing one
  /*!
   * @param value value to be pushed
   */
  void insert(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    set_.insert_or_assign(std::move(value), uint8_t(0));
    data_cond_.notify_one();
  }

  //! Try to retrieve an element and return immediatelly
  //! Elements in the set are ordered based on their key
  /*!
   * @param value retrieved value
   */
  bool try_and_pop_back(T &value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (set_.empty()) {
      return false;
    }
    value = set_.extract(prev(set_.end())).key();
    return true;
  }

  //! Try to retrieve, if no items, wait till an item is available and try again
  //! Elements in the set are ordered based on their key
  /*!
   * @param value retrieved value
   */
  void wait_and_pop_back(T &value) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_cond_.wait(lock, [this] { return !set_.empty(); });
    value = set_.extract(prev(set_.end())).key();
  }

  //! Check if queue is empty
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return set_.empty();
  }
  //! Return the size of the queue
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return set_.size();
  }
};

} // namespace sdk
} // namespace next
#endif // NEXT_SDK_THREADSAFE_SET_H_
