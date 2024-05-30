/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_override_test.cpp
 * @brief
 *
 **/

#include <atomic>
#include <thread>

#include <cip_test_support/gtest_reporting.h>

#ifdef _WIN32
#pragma warning(push, 0) // disable all warnings from ecal
#endif
#ifdef _WIN32
#pragma warning(pop)
#endif

#include <ecal/ecal.h>

#include <next/sdk/event_base/base_event_binary.h>
#include <next/sdk/event_base/base_event_binary_blocking.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

struct DummyMsg {
  int a = 0;
};

class OverrideBinary : public BaseEventBinary<DummyMsg> {
public:
  OverrideBinary(std::string name) : BaseEventBinary<DummyMsg>(name){};
  ~OverrideBinary(){};
  payload_type serializeEventPayload(const DummyMsg & /*message*/) const override { return StringToPayload("1"); }
  DummyMsg deserializeEventPayload(const payload_type & /*buf*/) const override {
    DummyMsg a = {2};
    return a;
  }
};

class OverrideTestFixture : public testing::TestUsingEcal {
public:
  OverrideTestFixture() {
    // enable for debugging testcase
    // initmap_[logger::getConsoleLoglevelCfgKey()] = logger::LOGLEVEL::DEBUG;
    logger::register_to_init_chain();

    this->instance_name_ = "OverrideTestFixture";
  }
};
//! flag for endless loop
std::atomic<bool> send_ecal_messages_override_test(true);
std::atomic<bool> publisher_ready_override_test(false);
std::atomic<int> subscribe_callback_called(0);
std::atomic<int> hook_was_called_override_test(0);
std::atomic<int> hook_was_called_override_test1(0);
std::atomic<int> hook_listener_added_was_called_override_test(0);

void test_hook_override_test() { hook_was_called_override_test++; }
void test_hook_override_test1() { hook_was_called_override_test1++; }
void test_hook_addListener_override_test(std::string) { hook_listener_added_was_called_override_test++; }

std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}

TEST_F(OverrideTestFixture, Binary_event_test) {
  // reset defaults

  send_ecal_messages_override_test = true;
  publisher_ready_override_test = false;
  hook_was_called_override_test = 0;

  eCAL::Util::EnableLoopback(true);
  std::string event_name = generateUniqueName("Binary_event_test");

  // create publisher
  OverrideBinary event(event_name);
  event.addListenerAddedHook(test_hook_addListener_override_test);

  auto pub_thread = std::thread([&] {
    std::list<std::string> res;
    while (send_ecal_messages_override_test) {
      DummyMsg dummy_msg;
      event.publish(dummy_msg, res);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      publisher_ready_override_test = true;
    }
  });

  // wait for the thread above to be initilized
  while (!publisher_ready_override_test) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  OverrideBinary *event1 = new OverrideBinary(event_name);

  // check connections are zero
  EXPECT_EQ(event.getNumberOfListeners(), 0);
  EXPECT_EQ(event1->getNumberOfListeners(), 0);

  // Add event hooks
  event1->addEventHook(test_hook_override_test);
  event1->subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  OverrideBinary event2(event_name);

  event2.addEventHook(test_hook_override_test1);
  event2.subscribe();

  uint32_t timeout = 0;
  while ((!hook_was_called_override_test || !hook_was_called_override_test1) && timeout < 200) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout++;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // check if callbacks were done
  EXPECT_EQ(event.getNumberOfListeners(), 1);
  // TODO Add this with SIMEN-9093
  // EXPECT_EQ(hook_listener_added_was_called_override_test, 1);
  EXPECT_EQ(event1->getNumberOfListeners(), 1);
  EXPECT_EQ(event2.getNumberOfListeners(), 1);
  // check if header was also receivced
  EXPECT_TRUE(0 != event1->getHeader().id);
  EXPECT_TRUE(0 != event1->getHeader().timestamp);

  EXPECT_TRUE(hook_was_called_override_test);
  EXPECT_TRUE(hook_was_called_override_test1);

  // now remove one event
  delete event1;

  timeout = 0;
  while (event.getNumberOfListeners() != 1 && timeout < 200) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout++;
  }
  EXPECT_EQ(event2.getNumberOfListeners(), 1);
  EXPECT_EQ(event.getNumberOfListeners(), 1);

  // stop publisher thread
  send_ecal_messages_override_test = false;
  pub_thread.join();
}

class OverrideBinaryBlocking : public BaseEventBinaryBlocking<DummyMsg> {
public:
  OverrideBinaryBlocking(std::string name) : BaseEventBinaryBlocking<DummyMsg>(name){};
  ~OverrideBinaryBlocking(){};
  payload_type serializeEventPayload(const DummyMsg & /*message*/) const override { return StringToPayload("1"); }
  DummyMsg deserializeEventPayload(const payload_type & /*buf*/) const override {
    DummyMsg a = {2};
    return a;
  }
};
// Enable this test once the ticket SIMEN-7491 resolved
TEST_F(OverrideTestFixture, Binary_blocking_event_test) {
  std::atomic<bool> send_ecal_messages_override_blocker_test(true);
  std::atomic<bool> publisher_ready_override_blocker_test(false);
  std::atomic<int> hook_was_called_override_blocker_test(0);
  std::atomic<int> hook_was_called_override_blocker_test1(0);
  std::atomic<int> hook_listener_added_was_called_override_blocker_test(0);
  std::string event_name = generateUniqueName("Binary_blocking_event_test");

  auto test_hook_override_blocker_test = [&]() { hook_was_called_override_blocker_test++; };

  auto test_hook_override_blocker_test1 = [&]() { hook_was_called_override_blocker_test1++; };

  auto test_hook_addListener_override_blocker_test = [&](std::string) {
    hook_listener_added_was_called_override_blocker_test++;
  };

  // reset defaults
  send_ecal_messages_override_blocker_test = true;
  publisher_ready_override_blocker_test = false;
  hook_was_called_override_test = 0;

  eCAL::Util::EnableLoopback(true);

  // create publisher
  OverrideBinaryBlocking event(event_name);
  event.addListenerAddedHook(test_hook_addListener_override_blocker_test);

  auto pub_thread = std::thread([&] {
    std::list<std::string> res;
    while (send_ecal_messages_override_blocker_test) {
      DummyMsg dummy_msg;
      event.publish(dummy_msg, res);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      publisher_ready_override_blocker_test = true;
    }
  });

  // wait for the thread above to be initilized
  while (!publisher_ready_override_blocker_test) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  OverrideBinaryBlocking *event1 = new OverrideBinaryBlocking(event_name);

  ASSERT_TRUE(event1 != nullptr);
  // Add event hooks
  event1->addEventHook(test_hook_override_blocker_test);
  event1->subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  EXPECT_EQ(event1->getEventData().a, 2);

  OverrideBinaryBlocking event2(event_name);

  event2.addEventHook(test_hook_override_blocker_test1);
  event2.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  EXPECT_EQ(event2.getEventData().a, 2);

  uint32_t timeout = 0;
  while ((!hook_was_called_override_blocker_test || !hook_was_called_override_blocker_test1) && timeout < 20) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timeout++;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  // check if callbacks were done
  EXPECT_GT(hook_listener_added_was_called_override_blocker_test, 0);
  // check if header was also receivced
  EXPECT_EQ(0, event1->getHeader().id);
  EXPECT_EQ(0, event1->getHeader().timestamp);

  EXPECT_GT(hook_was_called_override_blocker_test, 0);
  EXPECT_GT(hook_was_called_override_blocker_test1, 0);

  // now remove one event
  delete event1;

  // stop publisher thread
  send_ecal_messages_override_blocker_test = false;
  pub_thread.join();
}

class OverrideOnEvent : public BaseEventBinaryBlocking<DummyMsg> {
public:
  OverrideOnEvent(std::string name)
      : BaseEventBinaryBlocking<DummyMsg>(name){

        };
  ~OverrideOnEvent(){};
  payload_type serializeEventPayload(const DummyMsg & /*message*/) const override { return StringToPayload("1"); }
  DummyMsg deserializeEventPayload(const payload_type & /*buf*/) const override {
    DummyMsg a = {2};
    return a;
  }

  void onEvent(const payload_type & /*message_raw*/, long long /*time*/, long long /*id*/) override { value = 99; }

  int value{0};
};

TEST_F(OverrideTestFixture, override_on_event) {
  std::atomic<bool> send_ecal_messages_override_blocker_test(true);
  std::atomic<bool> publisher_ready_override_blocker_test(false);

  // reset defaults
  send_ecal_messages_override_blocker_test = true;
  publisher_ready_override_blocker_test = false;
  std::string event_name = generateUniqueName("override_on_event");

  // create publisher
  OverrideBinaryBlocking event(event_name);

  auto pub_thread = std::thread([&] {
    std::list<std::string> res;
    while (send_ecal_messages_override_blocker_test) {
      DummyMsg dummy_msg;
      event.publish(dummy_msg, res);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      publisher_ready_override_blocker_test = true;
    }
  });

  // wait for the thread above to be initilized
  while (!publisher_ready_override_blocker_test) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  OverrideOnEvent event2(event_name);
  event2.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  EXPECT_EQ(event2.value, 99);

  // stop publisher thread
  send_ecal_messages_override_blocker_test = false;
  pub_thread.join();
}

} // namespace events
} // namespace sdk
} // namespace next
