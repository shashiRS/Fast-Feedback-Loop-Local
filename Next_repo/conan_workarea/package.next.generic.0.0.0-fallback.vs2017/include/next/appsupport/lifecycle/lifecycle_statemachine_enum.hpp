/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_statemachine_enum.h
 * @brief    contains just an ENUM of possible states
 *           mainly needed by getCurrentState()
 */

#ifndef LIFECYCLE_STATEMACHINE_ENUM_H_
#define LIFECYCLE_STATEMACHINE_ENUM_H_

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>
#include <string>

// forward declaration
class LifecycleUnitTest;

namespace next {
namespace appsupport {
namespace lifecycleStatemachine {

/*!
 * @brief this enum represents the possible states of the statemachine
 */
enum class EState {
  INIT_STATE,
  RESET_STATE,
  CONFIG_STATE,
  CONFIG_OUTPUT,
  CONFIG_INPUT,
  READY_STATE,
  EXECUTE_STATE,
  SHUTDOWN_STATE,
  ERROR_STATE,
  UNKNOWN_STATE
};

inline std::string EState_toString(const EState &state) {
  switch (state) {
  case EState::INIT_STATE:
    return "INIT";
  case EState::RESET_STATE:
    return "RESET";
  case EState::CONFIG_STATE:
    return "CONFIG";
  case EState::CONFIG_OUTPUT:
    return "CONFIG_OUTPUT";
  case EState::CONFIG_INPUT:
    return "CONFIG_INPUT";
  case EState::READY_STATE:
    return "READY";
  case EState::EXECUTE_STATE:
    return "EXECUTE";
  case EState::SHUTDOWN_STATE:
    return "SHUTDOWN";
  case EState::ERROR_STATE:
    return "ERROR";
  case EState::UNKNOWN_STATE:
    return "UNKNOWN";
  default:
    return "";
  }
}

inline EState EState_fromString(const std::string &str) {
  if (str == "INIT") {
    return EState::INIT_STATE;
  } else if (str == "RESET") {
    return EState::RESET_STATE;
  } else if (str == "CONFIG") {
    return EState::CONFIG_STATE;
  } else if (str == "CONFIG_OUTPUT") {
    return EState::CONFIG_OUTPUT;
  } else if (str == "CONFIG_INPUT") {
    return EState::CONFIG_INPUT;
  } else if (str == "READY") {
    return EState::READY_STATE;
  } else if (str == "EXECUTE") {
    return EState::EXECUTE_STATE;
  } else if (str == "SHUTDOWN") {
    return EState::SHUTDOWN_STATE;
  } else if (str == "ERROR") {
    return EState::ERROR_STATE;
  } else {
    return EState::UNKNOWN_STATE;
  }
}

// forward deceleration
struct LifeCycleStateMachine;
struct LifeCycleStateMachineImpl;
struct INIT_STATE;
struct RESET_STATE;
struct CONFIG_STATE;
struct CONFIG_OUTPUT;
struct CONFIG_INPUT;
struct READY_STATE;
struct EXECUTE_STATE;
struct SHUTDOWN_STATE;
struct ERROR_STATE;

//! TODO: this should be taken from next_bridges -> BridgeSDK
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

  //! Try to retrieve, if no items, wait till an item is available and try again
  /*!
   * @return retrieved value or empty
   */
  T wait_return_and_pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    data_cond_.wait_for(lock, std::chrono::milliseconds(100), [this] { return !queue_.empty(); });
    T value;
    // if it was just a timeout return empty element
    if (queue_.empty()) {
      return value;
    } else {
      value = std::move(queue_.front());
      queue_.pop();
      return value;
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

} // namespace lifecycleStatemachine
} // namespace appsupport
} // namespace next

// forward deceleration
namespace next {
namespace sdk {
namespace events {
class LifecycleState;
class LifecycleTrigger;
} // namespace events
} // namespace sdk
} // namespace next

#endif // LIFECYCLE_STATEMACHINE_ENUM_H_
