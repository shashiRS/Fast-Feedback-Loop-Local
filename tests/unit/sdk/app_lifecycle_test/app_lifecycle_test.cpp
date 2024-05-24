/*, NEXT Project,
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *, NEXT Project,
 */
/**
 *  @file     app_lifecycle_test.cpp
 *  @brief    Testing the app_lifecycle interface
 */

#include <cip_test_support/gtest_reporting.h>

#include <chrono>
#include <mutex>
#include <thread>

#include <ecal/ecal.h>

#include <next/appsupport/lifecycle/lifecycle_base.hpp>
#include <next/sdk/sdk.hpp>
#include "next/appsupport/config/config_client.hpp"
#include "next/appsupport/config/config_key_generator.hpp"

#include <next/sdk/events/lifecycle_state.h>
#include <next/sdk/events/lifecycle_trigger.h>

#include "event_manager.h"

namespace next {
namespace appsupport {

using namespace next::sdk;

//! template Class multi-consumer multi-producer threadsafe list
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

  T wait_return_and_pop_front() {
    std::unique_lock<std::mutex> lock(mutex_);
    data_cond_.wait_for(lock, std::chrono::milliseconds(6000), [this] { return !list_.empty(); });
    T value;
    value = std::move(list_.front());
    list_.pop_front();
    return value;
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
};

// using namespace next::sdk::testing;
class AppLifecycleTest : public testing::TestUsingEcal {
public:
  AppLifecycleTest() {
    // enable for debugging testcase
    // initmap_[logger::getConsoleLoglevelCfgKey()] = logger::LOGLEVEL::DEBUG;

    logger::register_to_init_chain();
    this->instance_name_ = "AppLifecycleTest";
  }
};

struct {
  uint16_t init = 0;
  uint16_t reset = 0;
  uint16_t config = 0;
  uint16_t shutdown = 0;

  void clear() { init = reset = config = shutdown = 0; }
} transition_counter;

class LifecycleUnitTest : public next::appsupport::LifecycleBase {
public:
  LifecycleUnitTest(const std::string &name, uint32_t timeout = 1000)
      : next::appsupport::LifecycleBase(name, timeout) {}

  virtual bool enterConfig() {
    logger::debug(__FILE__, "enterConfig");
    transition_counter.config++;
    return true;
  }
  virtual bool enterInit() {
    logger::debug(__FILE__, "enterInit");
    transition_counter.init++;
    return true;
  }
  virtual bool enterReset() {
    logger::debug(__FILE__, "enterReset");
    transition_counter.reset++;
    return true;
  }
  virtual bool enterShutdown() {
    logger::debug(__FILE__, "enterShutdown");
    transition_counter.shutdown++;
    return true;
  }

  virtual bool onError(std::string error, [[maybe_unused]] std::exception *e = nullptr) {
    logger::debug(__FILE__, "onError: {}", error);
    return true;
  }

  void wrapperCallEvent(const std::string &eventName) { next::appsupport::LifecycleBase::callEvent(eventName); }

  std::shared_ptr<next::sdk::events::LifecycleState> wrapper_state_event_publisher() {
    return this->state_event_publisher;
  }

  std::shared_ptr<next::sdk::events::LifecycleTrigger> wrapper_trigger_event_subscriber() {
    return this->trigger_event_subscriber;
  }

  void setConfigInputTimeout(const std::string component_name, const int timeout) {
    next::appsupport::ConfigClient::do_init(component_name);
    auto config = next::appsupport::ConfigClient::getConfig();
    config->put(next::appsupport::configkey::getConfigInputTimeoutKey(component_name), timeout);
  }
};

constexpr int INVALID_RETURN = 42;

TEST_F(AppLifecycleTest, simple_transition_test) {
  // reset test data
  transition_counter.clear();

  // Testcase starts here
  LifecycleUnitTest lfc("simple_transition_test");
  logger::debug(__FILE__, "start statemachine");

  int return_value = INVALID_RETURN;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  lfc.wrapperCallEvent("RESET");
  // reset event triggers reset and config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  lfc.wrapperCallEvent("TRIGGERED"); // unknown event, this will be ignored

  // wait a little bit for user thread to be executed
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  lfc.wrapperCallEvent("SHUTDOWN");
  // shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  lfc.wrapperCallEvent("RESET");
  // after shutdown no other state should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, 0);
}

// this tests simulates the exceptional case when statemachine is in Execute state but receives a Reset Event
TEST_F(AppLifecycleTest, DISABLED_waitForReady_test) {
  // reset test data
  transition_counter.clear();

  // Testcase starts here
  LifecycleUnitTest lfc("waitForReady_test", 1000);
  logger::debug(__FILE__, "start statemachine");

  int return_value = 42;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  // start execution
  lfc.startExecution();
  // check if we are in the right state
  EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::EXECUTE_STATE);

  // trigger a reset
  lfc.wrapperCallEvent("RESET");

  // we should be still in Execute
  EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::EXECUTE_STATE);

  // finish execution this should trigger a full reset cycle
  lfc.exitExecution();
  // wait a little bit for user thread to be executed
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  // now lets test the scenario when Execution never exits
  // we should be in READY
  EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);

  // start execution
  lfc.startExecution();
  // we should be still in Execute
  EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::EXECUTE_STATE);

  // trigger a reset
  lfc.wrapperCallEvent("RESET");

  // now wait longer than the lifecycle timeout
  std::this_thread::sleep_for(std::chrono::milliseconds(4000));

  // a shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, -1);
}

threadsafe_list<std::string> messages{};
void event_hook(std::shared_ptr<next::sdk::events::LifecycleState> &lfc_event) {
  messages.push_back(lfc_event->getEventData().state);
  logger::debug(__FILE__, "callback received message: {}", lfc_event->getEventData().state);
}
// testing if the  lifecycle is sending ecal event messages in each transition
TEST_F(AppLifecycleTest, lifecycle_state_events_test) {
  // reset test data
  transition_counter.clear();

  // Testcase starts here
  LifecycleUnitTest lfc("lifecycle_state_events_test");
  logger::debug(__FILE__, "start statemachine");

  eCAL::Util::EnableLoopback(true);

  std::shared_ptr<next::sdk::events::LifecycleState> lfc_event = lfc.wrapper_state_event_publisher();
  ASSERT_TRUE(lfc_event) << "wrapper_state_event_publisher() returned NULL";
  auto msg_hook = std::bind(&event_hook, lfc_event);
  lfc_event->addEventHook(msg_hook);
  lfc_event->subscribe();

  int return_value = 42;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "INIT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  lfc.wrapperCallEvent("RESET");
  // reset event triggers reset and config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  lfc.wrapperCallEvent("CONFIG_INPUT");

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  lfc.wrapperCallEvent("TRIGGERED"); // unknown event, this will be ignored

  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);
  EXPECT_EQ(lfc.getCurrentState(),
            next::appsupport::lifecycleStatemachine::EState::READY_STATE); // we should still stay in READY

  lfc.wrapperCallEvent("SHUTDOWN");
  // shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(messages.wait_return_and_pop_front(), "SHUTDOWN");
  }

  lfc.wrapperCallEvent("RESET");
  // after shutdown no other state should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, 0);
}

TEST_F(AppLifecycleTest, lifecycle_state_events_test_timeout_default) {
  // reset test data
  transition_counter.clear();

  // Testcase starts here
  LifecycleUnitTest lfc("lifecycle_state_events_test_timeout_default");
  logger::debug(__FILE__, "start statemachine");

  eCAL::Util::EnableLoopback(true);

  std::shared_ptr<next::sdk::events::LifecycleState> lfc_event = lfc.wrapper_state_event_publisher();
  ASSERT_TRUE(lfc_event) << "wrapper_state_event_publisher() returned NULL";
  auto msg_hook = std::bind(&event_hook, lfc_event);
  lfc_event->addEventHook(msg_hook);
  lfc_event->subscribe();

  auto time_start =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

  int return_value = 42;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  auto time_ready =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  auto init_to_ready_duration = time_ready - time_start;
  EXPECT_TRUE(init_to_ready_duration.count() < LifecycleBase::kDefaultConfigInputTimeout);

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "INIT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  auto time_start_reset =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  lfc.wrapperCallEvent("RESET");
  // reset event triggers reset and config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  auto time_after_reset =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

  auto duration = time_after_reset - time_start_reset;
  EXPECT_TRUE(duration.count() > LifecycleBase::kDefaultConfigInputTimeout);

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  lfc.wrapperCallEvent("SHUTDOWN");
  // shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(messages.wait_return_and_pop_front(), "SHUTDOWN");
  }

  lfc.wrapperCallEvent("RESET");
  // after shutdown no other state should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, 0);
}

TEST_F(AppLifecycleTest, lifecycle_state_events_test_timeout_set) {
  // reset test data
  transition_counter.clear();

  // Testcase starts here
  std::string component_name = "lifecycle_state_events_test_timeout_set";
  LifecycleUnitTest lfc(component_name);
  logger::debug(__FILE__, "start statemachine");

  eCAL::Util::EnableLoopback(true);

  std::shared_ptr<next::sdk::events::LifecycleState> lfc_event = lfc.wrapper_state_event_publisher();
  ASSERT_TRUE(lfc_event) << "wrapper_state_event_publisher() returned NULL";
  auto msg_hook = std::bind(&event_hook, lfc_event);
  lfc_event->addEventHook(msg_hook);
  lfc_event->subscribe();

  int return_value = 42;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "INIT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  int shorter_timeout = 3000;
  lfc.setConfigInputTimeout(component_name, shorter_timeout);

  auto time_start_reset =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  lfc.wrapperCallEvent("RESET");
  // reset event triggers reset and config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  auto time_after_reset =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

  auto duration = time_after_reset - time_start_reset;
  EXPECT_TRUE(duration.count() > shorter_timeout);
  EXPECT_TRUE(duration.count() < LifecycleBase::kDefaultConfigInputTimeout);

  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(lfc.getCurrentState(), next::appsupport::lifecycleStatemachine::EState::READY_STATE);
    EXPECT_EQ(messages.wait_return_and_pop_front(), "RESET");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_OUTPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "CONFIG_INPUT");
    EXPECT_EQ(messages.wait_return_and_pop_front(), "READY");
  }

  lfc.wrapperCallEvent("SHUTDOWN");
  // shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  {
    // expecting to receive 4 messages with 4 different states
    EXPECT_EQ(messages.wait_return_and_pop_front(), "SHUTDOWN");
  }

  lfc.wrapperCallEvent("RESET");
  // after shutdown no other state should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, 0);
}

// controlling state machine via lifecycleTrigger Event through Eventsystem
TEST_F(AppLifecycleTest, DISABLED_trigger_event_test) {
  // reset test data
  transition_counter.clear();
  std::string component_name = "trigger_event_test";
  // Testcase starts here
  LifecycleUnitTest lfc(component_name);
  logger::debug(__FILE__, "start statemachine");

  eCAL::Util::EnableLoopback(true);

  int return_value = 42;
  std::thread t1([&] {
    return_value = lfc.run(); // start statemachine
  });
  t1.detach();

  // wait for statemachine
  while (lfc.getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  std::shared_ptr<next::sdk::events::LifecycleTrigger> trigger_event = lfc.wrapper_trigger_event_subscriber();

  // after first start the statemachine should already call Init, Reset and Config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 1);
  EXPECT_EQ(transition_counter.config, 1);
  EXPECT_EQ(transition_counter.shutdown, 0);

  trigger_event->publishTrigger("RESET", component_name); // send trigger with my component_name
  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // reset event triggers reset and config
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  trigger_event->publishTrigger("TRIGGERED"); // unknown event, this will be ignored
  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  trigger_event->publishTrigger("RESET", "HAU_MICH_BLAU"); // unknown component_name, this will be ignored
  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // wait a little bit for user thread to be executed
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 0);

  trigger_event->publishTrigger("SHUTDOWN"); // send trigger without component_name, this should still trigger
  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // shutdown should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  trigger_event->publishTrigger("RESET");
  // wait for ecal
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // after shutdown no other state should be triggered
  EXPECT_EQ(transition_counter.init, 1);
  EXPECT_EQ(transition_counter.reset, 2);
  EXPECT_EQ(transition_counter.config, 2);
  EXPECT_EQ(transition_counter.shutdown, 1);

  logger::debug(__FILE__, "stopping statemachine");

  // wait for subthread t1 to return
  for (int i = 0; i < 1000; i++) {
    if (return_value != INVALID_RETURN)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // check if statemachine shutdown properly
  EXPECT_EQ(return_value, 0);
}

} // namespace appsupport
} // namespace next
