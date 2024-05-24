/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     Lifecycle_Base.cpp
 * @brief    functions for lifecycle handling
 *
 **/
#include "next/appsupport/lifecycle/lifecycle_base.hpp"

#include <next/appsupport/next_status_code/next_status_code.hpp>

#include "next/appsupport/config/config_client.hpp"
#include "next/appsupport/config/config_key_generator.hpp"

#include <chrono>
#include <ctime>

#include "next/sdk/events/lifecycle_state.h"
#include "next/sdk/events/lifecycle_trigger.h"
#include "next/sdk/logger/logger.hpp"

#include "lifecycle_statemachine.hpp"

namespace next {
namespace appsupport {

LifecycleBase::LifecycleBase(const std::string &component_name, uint32_t timeout_ms)
    : execute_watchdog_timeout_(timeout_ms), component_name_(component_name) {
  statemachine_ = std::make_unique<lifecycleStatemachine::LifeCycleStateMachine>(this);

  // setup all events
  state_event_publisher = std::make_shared<next::sdk::events::LifecycleState>("");
}

LifecycleBase::~LifecycleBase() {}

// trigger event callback
void LifecycleBase::callbackTriggerEvent() {
  std::string component_name = trigger_event_subscriber->getEventData().component_name;
  std::string trigger = trigger_event_subscriber->getEventData().trigger_event;
  next::sdk::logger::debug(__FILE__, "trigger received: {} with component_name: {}", trigger, component_name);
  // check if the trigger was for everyone or explicitly for me
  if (component_name.size() == 0 || component_name == component_name_) {
    addTriggerToQueue(trigger);
  }
}

void LifecycleBase::publishCurrentState() {
  lifecycleStatemachine::EState current_state = getOnEnteringState();
  state_event_publisher->publishState(lifecycleStatemachine::EState_toString(current_state), component_name_);
}

void LifecycleBase::addTriggerToQueue(const std::string &event_name) { trigger_queue_.push(event_name); }

void LifecycleBase::executeTriggerFromQueue() {
  std::string trigger = trigger_queue_.wait_return_and_pop();
  callEvent(trigger);
}

int LifecycleBase::run() {
  statemachine_->start();

  // NOTE: registering to all LifecycleTrigger without COMPONENT_NAME to be able to react an a broadcast. Filtering of
  // the COMPONENT wil be done in the event hook callbackTriggerEvent
  trigger_event_subscriber = std::make_shared<next::sdk::events::LifecycleTrigger>("");
  auto cb_hook = std::bind(&LifecycleBase::callbackTriggerEvent, this);
  trigger_event_subscriber->addEventHook(cb_hook);
  trigger_event_subscriber->subscribe();

  next::sdk::logger::debug(__FILE__, "run() started");
  while (!stop_lifecycle_) {
    // execute until a SHUTDOWN was triggered
    executeTriggerFromQueue(); // blocking call
  }
  return return_status_;
}

bool LifecycleBase::privateInit() {
  bool result = enterInit();

  next::sdk::logger::debug(__FILE__, "enterInit() returned {}", result);

  publishCurrentState();
  setReturnState(result);
  next::appsupport::NextStatusCode::PublishStatusCode(
      component_name_, next::appsupport::StatusCode::SUCC_COMP_INIT_FINISHED, "[LifecycleBase]::privateInit finished");
  return result;
}

bool LifecycleBase::privateReset() {
  bool result = userEnterReset();
  result &= enterReset();
  next::sdk::logger::debug(__FILE__, "enterReset() returned {}", result);

  if (next::appsupport::ConfigClient::getConfig() == nullptr) {
    config_input_execute_watchdog_timeout_ = kDefaultConfigInputTimeout;
  } else {
    config_input_execute_watchdog_timeout_ = (uint32_t)next::appsupport::ConfigClient::getConfig()->get_int(
        next::appsupport::configkey::getConfigInputTimeoutKey(component_name_), kDefaultConfigInputTimeout);
  }

  publishCurrentState();
  setReturnState(result);
  next::appsupport::NextStatusCode::PublishStatusCode(component_name_,
                                                      next::appsupport::StatusCode::SUCC_COMP_RESET_FINISHED,
                                                      "[LifecycleBase]::privateReset finished");
  return result;
}

bool LifecycleBase::privateConfig() {
  bool result = enterConfig();
  next::sdk::logger::debug(__FILE__, "enterConfig() returned {}", result);

  publishCurrentState();
  setReturnState(result);
  next::appsupport::NextStatusCode::PublishStatusCode(component_name_,
                                                      next::appsupport::StatusCode::SUCC_COMP_CONFIG_FINISHED,
                                                      "[LifecycleBase]::privateConfig finished");
  return result;
}

bool LifecycleBase::privateConfigOutput() {
  bool result = userEnterConfigOutput();
  next::sdk::logger::debug(__FILE__, "configureOutput() returned {}", result);

  publishCurrentState();
  setReturnState(result);
  next::appsupport::NextStatusCode::PublishStatusCode(component_name_,
                                                      next::appsupport::StatusCode::SUCC_COMP_CONFIG_OUTPUT_FINISHED,
                                                      "[LifecycleBase]::privateConfigOutput finished");

  if (first_time_) {
    callEvent("CONFIG_INPUT");
    return result;
  }

  // this functions waits, if the state was not externally changed unit timeout it will change the state
  waitTimeoutBeforeConfigInput();

  return result;
}

void LifecycleBase::waitTimeoutBeforeConfigInput() {
  config_input_timeout_from_execute_watchdog_ = std::thread([&]() {
    // first sleep or wait to be notified
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);

    std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
    std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
    next::sdk::logger::debug(__FILE__, "start waiting at {}", std::ctime(&time_now_t));

    config_input_execute_watchdog_condition_variable_.wait_for(
        lk, std::chrono::milliseconds(config_input_execute_watchdog_timeout_),
        [&]() { return config_input_condition_variable_flag_ == true; });

    config_input_condition_variable_flag_ = false;

    time_now = std::chrono::system_clock::now();
    time_now_t = std::chrono::system_clock::to_time_t(time_now);
    next::sdk::logger::debug(__FILE__, " finish waiting at {}", std::ctime(&time_now_t));
    next::sdk::logger::debug(__FILE__, " getOnEnteringState returns {}",
                             static_cast<int>(statemachine_->getOnEnteringState()));

    if (statemachine_->getOnEnteringState() == lifecycleStatemachine::EState::CONFIG_OUTPUT) {
      callEvent("CONFIG_INPUT");
    }
  });

  config_input_timeout_from_execute_watchdog_.detach();
}

bool LifecycleBase::privateConfigInput() {

  // thread will be created only on 2nd reset not on the first startup
  if (!first_time_) {
    // notify the wait the state was changed so the thread can finish
    config_input_condition_variable_flag_ = true;
    config_input_execute_watchdog_condition_variable_.notify_all();
  }

  bool result = userEnterConfigInput();
  next::sdk::logger::debug(__FILE__, "configureInput() returned {}", result);

  publishCurrentState();
  setReturnState(result);
  next::appsupport::NextStatusCode::PublishStatusCode(component_name_,
                                                      next::appsupport::StatusCode::SUCC_COMP_CONFIG_INPUT_FINISHED,
                                                      "[LifecycleBase]::privateConfigInput finished");

  return result;
}

bool LifecycleBase::privateReady() {
  first_time_ = false;

  condition_variable_flag_ = true;
  execute_watchdog_condition_variable_.notify_all();
  condition_variable_flag_ = false;

  bool result = true;

  next::sdk::logger::debug(__FILE__, " privateReady() executed");
  publishCurrentState();
  setReturnState(result);
  return result;
}

bool LifecycleBase::privateExecute() {
  next::sdk::logger::debug(__FILE__, " privateExecute() executed");
  publishCurrentState();
  setReturnState(true);
  return true;
}

bool LifecycleBase::privateError(std::string error, std::exception *e) {
  bool result = onError(error, e);
  next::sdk::logger::debug(__FILE__, " onError() returned {}", result);
  publishCurrentState();
  return_status_ = -1;
  return result;
}

bool LifecycleBase::privateShutdown() {
  bool result = enterShutdown();
  next::sdk::logger::debug(__FILE__, " enterShutdown() returned {}", result);

  publishCurrentState();
  // stop endless loop in run()
  stop_lifecycle_ = true;
  next::appsupport::NextStatusCode::PublishStatusCode(component_name_,
                                                      next::appsupport::StatusCode::SUCC_COMP_SHUTDOWN_FINISHED,
                                                      "[LifecycleBase]::privateShutdown finished");
  return result;
}

void LifecycleBase::callEvent(const std::string &event_name) {

  if (event_name.empty() || event_name == "") {
    // do nothing
    return;
  }

  next::sdk::logger::debug(__FILE__, "callEvent: {}", event_name);

  // statemachine->process_event is not thread safe and must be synced
  // recursive mutex because RESET will lead to EVENT_CONFIG_INPUT being called
  std::recursive_mutex statemachine_mutex;
  std::lock_guard<std::recursive_mutex> guard{statemachine_mutex};

  if (event_name == "RESET") {
    statemachine_->process_event(lifecycleStatemachine::EVENT_RESET{});
  } else if (event_name == "SHUTDOWN") {
    statemachine_->process_event(lifecycleStatemachine::EVENT_SHUTDOWN{});
  } else if (event_name == "START_EXECUTION") {
    statemachine_->process_event(lifecycleStatemachine::EVENT_START_EXECUTION{});
  } else if (event_name == "EXIT_EXECUTION") {
    statemachine_->process_event(lifecycleStatemachine::EVENT_EXIT_EXECUTION{});
  } else if (event_name == "CONFIG_INPUT") {
    statemachine_->process_event(lifecycleStatemachine::EVENT_CONFIG_INPUT{});
  } else {
    // statemachine->process_event(lifecycleStatemachine::EVENT_ANY_OTHER{});
  }
}

bool LifecycleBase::startExecution() {
  callEvent("START_EXECUTION");
  // check if statemachine is really in EXECUTION state
  bool result = statemachine_->getCurrentState() == lifecycleStatemachine::EState::EXECUTE_STATE;
  next::sdk::logger::debug(__FILE__, "startExecution() returned {}", result);
  return result;
}

bool LifecycleBase::exitExecution() {
  callEvent("EXIT_EXECUTION");
  // check if statemachine is really in EXECUTION state
  bool result = statemachine_->getCurrentState() == lifecycleStatemachine::EState::READY_STATE;
  next::sdk::logger::debug(__FILE__, "exitExecution() returned {}", result);
  return result;
}

void LifecycleBase::waitTimeoutBeforeProcessEvent(std::string event) {
  std::thread t1([&] {
    std::string event_to_call = event;
    // first sleep or wait to be notified
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);

    std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
    std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
    next::sdk::logger::debug(__FILE__, "start waiting at {}", std::ctime(&time_now_t));

    execute_watchdog_condition_variable_.wait_for(lk, std::chrono::milliseconds(execute_watchdog_timeout_),
                                                  [&]() { return condition_variable_flag_ == true; });

    condition_variable_flag_ = false;

    time_now = std::chrono::system_clock::now();
    time_now_t = std::chrono::system_clock::to_time_t(time_now);
    next::sdk::logger::debug(__FILE__, " finish waiting at {}", std::ctime(&time_now_t));
    next::sdk::logger::debug(__FILE__, " getOnEnteringState returns {}",
                             static_cast<int>(statemachine_->getOnEnteringState()));
    next::sdk::logger::debug(__FILE__, " event string is  {}", event_to_call);

    if (statemachine_->getOnEnteringState() == lifecycleStatemachine::EState::READY_STATE) {
      // everything is fine we can process normal state transistion
      callEvent(event_to_call);
    } else {
      privateError("timeout failed, shutting down");
      privateShutdown();
    }
  });
  t1.detach();
  timeout_from_execute_watchdog_ = std::move(t1);
}

lifecycleStatemachine::EState LifecycleBase::getCurrentState() { return statemachine_->getCurrentState(); }

lifecycleStatemachine::EState LifecycleBase::getOnEnteringState() { return statemachine_->getOnEnteringState(); }

void LifecycleBase::setReturnState(bool isTrue) {
  if (isTrue) {
    return_status_ = 0;
  } else {
    return_status_ = 1;
  }
}

void LifecycleBase::shutdown() { callEvent("SHUTDOWN"); }

} // namespace appsupport
} // namespace next
