/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_base.hpp
 * @brief    Including functions for lifecycle handling
 *
 * Include this file if you want to use the Next configuration handling.
 *
 * If only need specific parts are needed, it is sufficient to only use the corresponding include file.
 *
 *  @startuml
 * [*] -> new : transient
 * new -> Config : transient
 * Init --> Execute : transient
 * Config -> Init : transient
 * Execute --> Execute : EVENT-trigger
 * Execute --> ShutDown : EVENT-shutdown
 * Execute--> Config : EVENT-reset
 * ShutDown -> [*]
 *
 * State Config : "enterConfig() ... reset and reconfigure everything"
 * }
 * @enduml
 *
 * TODO: implement event system and generic execution flow
 *
 */

#ifndef LIFECYCLE_BASE_H_
#define LIFECYCLE_BASE_H_

#include <atomic>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>

#include <next/appsupport/appsupport_config.hpp>
#include "lifecycle_statemachine_enum.hpp"

namespace next {
namespace appsupport {

/*!
 * @brief Base class to implement the life cycle handling
 * @brief Implementing some basic life cycle states and provide the possibility to put the functionality into specific
 * states.
 *
 * This is a pure virtual ,class to be used, to implement the actual functions of the application.
 */
class LifecycleBase {

public:
  static constexpr int kDefaultConfigInputTimeout = 7000;

  /*!
   * @brief Constructor that takes a timeout value in milliseconds
   * @param timeout_ms amount of milliseconds the statemachine should wait until it goes into an Error state
   */
  DECLSPEC_appsupport LifecycleBase(const std::string &component_name, uint32_t timeout_ms = 10000);
  DECLSPEC_appsupport virtual ~LifecycleBase();

  // this virtual methods have to be overwritten by user
  // they will be called in the appropriate state from the statemachine
  virtual bool enterConfig() = 0;
  virtual bool enterReset() = 0;
  virtual bool enterInit() = 0;
  virtual bool enterShutdown() = 0;
  virtual bool onError(std::string error, std::exception *e = nullptr) = 0;

  /*!
   * @brief starts the Lifecycle Statemachine and Eventsystem. This call will not return until SHUTDOWN is triggered
   * @returns 0 if Lifecycle was Shutdown properly
   * @returns -1 if Error appeared
   * @returns +1 if one of the derived methods returned false
   * Note: internally it uses stop_lifecycle flag and return_status
   */
  int DECLSPEC_appsupport run();

  /*!
   * @brief could be useful for debugging
   * @returns current state of the  statemachine
   */
  // we should be still in Execute
  lifecycleStatemachine::EState DECLSPEC_appsupport getCurrentState();

  /*!
   * @brief could be useful for debugging
   * @returns current state which was entering
   */
  // we should be still in Execute
  lifecycleStatemachine::EState DECLSPEC_appsupport getOnEnteringState();

  /*!
   * @brief triggers the statemachine to switch from Ready to the Execute state
   * @returns true if statemachine is in Execute state
   */
  //[[deprecated("virtual bool enterExecute() will be used in future")]]
  bool DECLSPEC_appsupport startExecution();

  /*!
   * @brief triggers the statemachine to switch from Execute to the Ready state
   * @returns true if statemachine is in Ready state
   */
  //[[deprecated("virtual bool enterExecute() will be used in future")]]
  bool DECLSPEC_appsupport exitExecution();

  /*!
   * @brief triggers the shutdown, can be used e.b. by CTRL+C event
   */
  void DECLSPEC_appsupport shutdown();

protected:
  // parses the event and call ths appropriate statemachine event
  void DECLSPEC_appsupport callEvent(const std::string &event_name);

  // using the event system to notify about current state
  std::shared_ptr<next::sdk::events::LifecycleState> state_event_publisher;

  // using the event system to trigger state machine
  std::shared_ptr<next::sdk::events::LifecycleTrigger> trigger_event_subscriber;

  // virtual helper function for next interface
  virtual bool userEnterReset() { return true; }

  // two-step config functions
  virtual bool userEnterConfigOutput() { return true; }
  virtual bool userEnterConfigInput() { return true; }

private:
  // internal methods which will be called by the statemachine
  // this methods will call the appropriate public methods from above
  bool privateInit();
  bool privateReset();
  bool privateConfig();
  bool privateConfigOutput();
  bool privateConfigInput();
  bool privateReady();
  bool privateExecute();
  bool privateShutdown();
  bool privateError(std::string error, std::exception *e = nullptr);

  // this method creates a new thread which sleeps for a period which is defined in TIMEOUT
  // after the timeout is expired it checks if
  void waitTimeoutBeforeProcessEvent(std::string event);
  std::thread timeout_from_execute_watchdog_;
  std::condition_variable execute_watchdog_condition_variable_;
  std::atomic<bool> condition_variable_flag_{false};
  uint32_t execute_watchdog_timeout_;

  bool first_time_ = true;

  std::unique_ptr<lifecycleStatemachine::LifeCycleStateMachine> statemachine_;

  // publish via Eventsystem current state
  void publishCurrentState();

  // trigger event callback
  void callbackTriggerEvent();

  // holds the app name, which will be used to create Lifecycle Events
  std::string component_name_;

  // the method run() will block until this flag is set to true
  std::atomic<bool> stop_lifecycle_{false};
  // the method run() will return this value;
  std::atomic<int> return_status_{0};

  // sets return_status to +1 when "isTrue" is false
  void setReturnState(bool isTrue);
  // adds new trigger event into the queue and returns
  void addTriggerToQueue(const std::string &event_name);
  // executes next trigger event from queue, this call blocks if queue is empty
  void executeTriggerFromQueue();

  // contains all strings of triggers to be executed
  lifecycleStatemachine::threadsafe_queue<std::string> trigger_queue_;

  void waitTimeoutBeforeConfigInput();
  std::thread config_input_timeout_from_execute_watchdog_;
  std::condition_variable config_input_execute_watchdog_condition_variable_;
  std::atomic<bool> config_input_condition_variable_flag_{false};
  uint32_t config_input_execute_watchdog_timeout_ = 0;

public:
  // the statemachine needs to call private methods like privateInit()
  friend struct lifecycleStatemachine::LifeCycleStateMachineImpl;
  friend struct lifecycleStatemachine::INIT_STATE;
  friend struct lifecycleStatemachine::RESET_STATE;
  friend struct lifecycleStatemachine::CONFIG_STATE;
  friend struct lifecycleStatemachine::CONFIG_OUTPUT;
  friend struct lifecycleStatemachine::CONFIG_INPUT;
  friend struct lifecycleStatemachine::READY_STATE;
  friend struct lifecycleStatemachine::EXECUTE_STATE;
  friend struct lifecycleStatemachine::SHUTDOWN_STATE;
  friend struct lifecycleStatemachine::ERROR_STATE;

  // only for unit test
  friend class LifecycleUnitTest;
};

} // namespace appsupport
} // namespace next

#endif // LIFECYCLE_BASE_H_
