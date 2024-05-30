// Copyright (C) 2020  Aleksey Romanov (aleksey at voltanet dot io)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// https://github.com/alex4747-pub/simple_init_chain/

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     simple_init_chain.hpp
 * @brief    init chain
 */

#ifndef NEXTSDK_SIMPLE_INIT_CHAIN_H_
#define NEXTSDK_SIMPLE_INIT_CHAIN_H_

#include <any>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include "../sdk_config.hpp"

#if __cplusplus < 201703L
#error "At least c++17 is required (on MSVC you have to add '/Zc:__cplusplus' as compiler flag)"
#endif

namespace next {
namespace sdk {

enum class InitPriority : int {
  LOGGER = 100,   // least prio,init first
  ECAL = 120,     // uses logger so logger should be init before this
  PROFILER = 140, // same Prio as Version Manager
  VERSION_MANAGER = 160,
  EVENT_MANAGER = 150,   // uses logger and eCAL; same Prio as Signal Watcher
  SIGNAL_WATCHER = 150,  // uses logger and eCAL
  LOG_ROUTER = 200,      // uses logger and eCAL
  USER_LEVEL_BASE = 250, // for user modules, making sure that logger, eCAL etc. are already initialized (if needed)
};

InitPriority DECLSPEC_nextsdk operator+(const InitPriority &level, const int &in);

//! InitChain is setting up a initialization chain.
/**
 * Create a init and a reset function, register them to the init chain by creating a init element:
 *  `InitChain::El init_el(level, my_init_function, my_reset_function);`
 * Where:
 * * `level`: int value. Technically any value would be possible, but some suggestions:
 *   * negative number to make sure to be the very first (e.g. unit testing), should not be needed for productive use
 *   * For the actually used priorities see \ref InitPriority
 *   * to make sure it is the last to be initialized use 999
 * * `my_init_function`: Function pointer to the init function. See typedef InitFunc. Where the `ConfigMap` is used for
 * carrying config parameters
 * * `my_reset_function`: Function pointer to the reset function. See typedef ResetFunc.
 *
 * During initialization first all init functions are called, starting with the lowest id, highest id last.
 * During reset the reset functions are called, starting with the highest id, lowest id last.
 *
 * The main application has to call InitChain::Run *once*. The map argument is optional. But if something
 * needs some arguments (e.g. it is possible to give the log filename to the logger) then it has to be given. All
 * modules, which are not needing the specific config option, have to ignore the given config parameter.
 * The main application has to call InitChain::Reset at the very end *once*.
 *
 * There must be good reasons why the Run and Init functions are called more often, e.g. in unit test applications it
 * might be necessary to call Run and Init per test case.
 *
 * See also https://github.com/alex4747-pub/simple_init_chain/
 */
class DECLSPEC_nextsdk InitChain {
public:
  // Just to save on typing
  typedef std::map<std::string, std::any> ConfigMap;

  //! Calling the init function of all registered modules
  /**
   * Starting with the first element all init functions are called. If for one element no reset function is registered
   * (its function pointer is nullptr) the element is removed from the list. Such that later calls of \ref Reset or this
   * function are not considering them anymore.
   *
   * If one of the init functions returns false this function will immediately return false. The remaining elements will
   * not be called.
   *
   * Calling this function again is possible, but \ref Reset should be called first. Otherwise only false is returned.
   *
   * @param config_map the configuration provided to all init functions
   * @return false if one of the init functions fails
   */
  static bool Run(ConfigMap const &config_map = ConfigMap()) noexcept;

  //! Calling the reset function (if available) of all registered modules.
  /**
   * For all of the chain elements where a reset function was given it's called.
   *
   * @param config_map the configuration provided to all reset functions
   */
  static void Reset(ConfigMap const &config_map = ConfigMap()) noexcept;

  //! Returns the base level for user modules
  /**
   * For registering own modules use values above this value. E.g.
   * ```
   * InitChain::El init_el(UserLevelBase() + 10, my_init_function, my_reset_function);
   * ```
   */
  [[deprecated("Use InitPriority::USER_LEVEL_BASE instead.")]] inline static int UserLevelBase() noexcept {
    return static_cast<int>(InitPriority::USER_LEVEL_BASE);
  }

  typedef std::function<bool(ConfigMap const &, int)> InitFunc;
  typedef std::function<void(ConfigMap const &, int)> ResetFunc;

  //! Checks if init was called already
  inline static bool IsInitialized() noexcept { return !ready_ && !failed_; }

  //! A chain element of the InitChain.
  /**
   * Providing the underlying data structure of the InitChain. Each element is having a pointer to its successor, to a
   * init function and, optionally, to a reset function. The level, or priority, given in the constructor defines the
   * order of the init and reset execution.
   *
   * The DoInit method is internally catching all exceptions of the called init function. If an exception is catched it
   * returns false. The DoRest function also catches all exceptions, but they are simply ignored.
   */
  class El {
  public:
    //! Creates an InitChain element, sorting it depending on the priority to the correct place.
    /**
     * The constructor throws [std::invalid_argument](https://en.cppreference.com/w/cpp/error/invalid_argument) when
     * nullptr is given for init_func.
     *
     * @param level the priority of this element (lower means higher priority in initialization)
     * @param init_func pointer to the initialization function
     * @param reset_func pointer to the reset function (optional)
     */
    DECLSPEC_nextsdk El(int level, InitFunc init_func, ResetFunc reset_func = nullptr) noexcept(false);

    //! Creates an InitChain element, sorting it depending on the priority to the correct place.
    /**
     * The constructor throws [std::invalid_argument](https://en.cppreference.com/w/cpp/error/invalid_argument) when
     * nullptr is given for init_func.
     *
     * @param level the priority of this element of type InitPriority (lower means higher priority in initialization)
     * @param init_func pointer to the initialization function
     * @param reset_func pointer to the reset function (optional)
     */
    DECLSPEC_nextsdk El(InitPriority level, InitFunc init_func, ResetFunc reset_func = nullptr) noexcept(false);

    DECLSPEC_nextsdk virtual ~El() noexcept;

    //! returns the level (priority) of this element
    int GetLevel() const noexcept;

    //! returns the pointer to the successor (nullptr if no successor is available)
    El *GetNext() const noexcept;

    //! returns the pointer to the predecessor (nullptr if no predecessor is available)
    El *GetPrev() const noexcept;

    //! set the pointer to the successor, nullptr is a valid value
    /**
     * When changing something in the chain, don't forget to call SetPrev of the successor, this function will not do it
     * for you.
     *
     * @param [in] next the pointer to the following chain element
     */
    void SetNext(El *next) noexcept;

    //! set the pointer to the predecessor, nullptr is a valid value
    /**
     * When changing something in the chain, don't forget to call SetNext of the predecessor, this function will not do
     * it for you.
     *
     * @param [in] prev the pointer to the preceding chain element.
     */
    void SetPrev(El *prev) noexcept;

    //! check whether the reset function is set
    /**
     * @return true if the function pointer is set, false otherwise
     */
    bool HaveReset() const noexcept;

    //! trigger calling the init function
    /**
     * Calls the init function with given configuration.
     *
     * @param config_map the configuration provided to the init function
     * @return false on failure or catched exception, true when everything was fine
     */
    bool DoInit(ConfigMap const &config_map) noexcept;

    //! trigger the reset function (if set)
    /**
     * If the pointer to the reset function is set it will be called with the given configuration.
     *
     * @param config_map the configuration provided to the reset function
     */
    void DoReset(ConfigMap const &config_map) noexcept;

  private:
    El *next_{nullptr};
    El *prev_{nullptr};
    int level_;
    InitFunc init_func_;
    ResetFunc reset_func_;
  };

protected:
  static void Insert(El *el);

  // Note: no danger of static initialization order
  // problem.
  static inline El *head_;
  static inline El *tail_;

  // We expect that init operation will be
  // performed once.
  //
  // However, if  this code is executed in
  // a unit test environment it could be called
  // thousand times. Hence we have reset possibility.
  //
  // Also we do want to prevent repeated calls
  // after failure was declared.

  static bool ready_;
  static bool failed_;
};

} // namespace sdk
} // namespace next

#endif // NEXTSDK_SIMPLE_INIT_CHAIN_H_
