/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     sleep.hpp
 * @brief    interruptable sleep
 *
 **/

#ifndef PERFORMANCETEST_SLEEP_H_
#define PERFORMANCETEST_SLEEP_H_

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace next {

/**
 * interruptable sleep
 *
 * This class is providing two sleep functions: wait_for and wait_until. Also an interrupt function, shutdown, is
 * provided.
 *
 * They behave like the normal sleep_for and sleep_unitl of std::this_thread. Only diffrence is that they are
 * interruptable, e.g. by shutdown requests (for example when the user presses CTRL+C). To interrupt a sleep the
 * function shutdown can be called. All threads which are using one of the sleep functions are immediatly returning.
 */
class sleep {
public:
  /**
   * Calling thread will sleep for given amount of time, or until it gots interrupted.
   * Will return immediatly if shutdown was called before.
   * Returns true on timeout (sleep time reached), false otherwise (e.g. sleep got interrupted by calling shutdown).
   */
  template <class R, class P>
  bool wait_for(const std::chrono::duration<R, P> &time) const {
    std::unique_lock<std::mutex> lck(mtx_);
    if (!term_) {
      if (std::chrono::duration<R, P>(0) != time) {
        return !cv_.wait_for(lck, time, [&] { return term_; });
      }
      return true;
    } else {
      return false;
    }
  }

  /**
   * Calling thread will sleep until given time point, or until it gots interrupted.
   * Will return immediatly if shutdown was called before.
   * Returns true on timeout (sleep time reached), false otherwise (e.g. sleep got interrupted by calling shutdown).
   */
  template <class C>
  bool wait_until(const std::chrono::time_point<C> &time) const {
    std::unique_lock<std::mutex> lck(mtx_);
    if (!term_) {
      return !cv_.wait_until(lck, time, [&] { return term_; });
    } else {
      return false;
    }
  }

  /**
   * Interrupt the wait of all sleeping threads.
   */
  void shutdown() {
    std::unique_lock<std::mutex> lck(mtx_);
    term_ = true;
    cv_.notify_all();
  }

  sleep() = default;
  sleep(sleep &&) = delete;
  sleep(const sleep &) = delete;
  sleep &operator=(sleep &&) = delete;
  sleep &operator=(const sleep &) = delete;

private:
  mutable std::mutex mtx_;
  mutable std::condition_variable cv_;
  bool term_{false};
};

} // namespace next

#endif // PERFORMANCETEST_SLEEP_H_
