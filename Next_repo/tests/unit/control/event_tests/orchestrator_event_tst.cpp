/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_event_tst.cpp
 * @brief
 *
 **/

#include <gtest/gtest.h>

#include <atomic>
#include <iostream>
#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>

#include <next/control/event_types/orchestrator_event_binary.h>

namespace next {
namespace control {
namespace events {

//! Test to serialize a message and deserialize it
TEST(OrchestratorEventBinary, de_serializeEventPayloadTest) {

  class TestSerializeCommandHandlerEvent : events::OrchestratorEventBinary {
  public:
    TestSerializeCommandHandlerEvent() : events::OrchestratorEventBinary("NextOrchestratorEvent"){};
    ~TestSerializeCommandHandlerEvent(){};
    next::sdk::events::payload_type serializeEvent(OrchestratorTrigger in) {
      return events::OrchestratorEventBinary::serializeEventPayload(in);
    }
    auto deserializeEvent(const next::sdk::events::payload_type &in) {
      return events::OrchestratorEventBinary::deserializeEventPayload(in);
    }
  };
  TestSerializeCommandHandlerEvent bin_event;
  OrchestratorTrigger message;
  message.trigger_timestamp = "123456789";
  message.flow_id = "1";

  next::sdk::events::payload_type mess_buf = bin_event.serializeEvent(message);

  OrchestratorTrigger message_back = bin_event.deserializeEvent(mess_buf);

  ASSERT_TRUE(message_back.trigger_timestamp == message.trigger_timestamp);
  ASSERT_TRUE(message_back.flow_id == message.flow_id);
}

std::atomic<bool> callback_was_called(false);

void method_hook() {
  // simulate a long rpc
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  callback_was_called = true;
}

class OrchestratorEventTest : public next::sdk::testing::TestUsingEcal {
public:
  OrchestratorEventTest() {
    this->instance_name_ = "OrchestratorEventTest";
    // enable logging
    next::sdk::logger::register_to_init_chain();
  }
};

TEST_F(OrchestratorEventTest, listenAndCallMethod) {
  // reset defaults
  callback_was_called = false;

  // create publisher
  OrchestratorEventBinary event("listenAndCallMethod");

  eCAL::Util::EnableLoopback(true);

  event.subscribe(); // start lsitening
  // give time to subscribe
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  next::sdk::events::notify_hook hook = method_hook;
  event.addEventHook(hook);

  // just perform a call several times
  for (int i = 0; i < 3; i++) {
    callback_was_called = false;

    OrchestratorTrigger message;
    message.trigger_timestamp = "123456789";
    std::list<std::string> resp;
    event.publish(message, resp); // this should block until response was received from server
    EXPECT_TRUE(callback_was_called);
  }
}
/*namespace next {
namespace coresdk {
namespace event*/
std::string g_response = "orechestrator_response";
void method_hook_with_response(const next::sdk::events::payload_type &payload, std::string &response) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  response = g_response;
  callback_was_called = true;
  std::ignore = payload;
}

TEST_F(OrchestratorEventTest, listenAndCallMethodWithResponse) {

  // reset defaults
  callback_was_called = false;

  // create publisher
  OrchestratorEventBinary event("listenAndCallMethodWithResponse");
  eCAL::Util::EnableLoopback(true);

  event.subscribe(); // start lsitening

  // give time to subscribe
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  next::sdk::events::notify_sync_hook hook = method_hook_with_response;
  event.addSyncEventHook(hook);

  // just perform a call several times
  for (int i = 0; i < 3; i++) {
    callback_was_called = false;
    OrchestratorTrigger message;
    message.trigger_timestamp = "123456789";
    std::list<std::string> resp;
    event.publish(message, resp); // this should block until response was received from server
    EXPECT_TRUE(callback_was_called);
    EXPECT_TRUE(resp.size() == 1);
    auto ret_resp = resp.front();
    EXPECT_EQ(ret_resp, g_response);
  }
}

} // namespace events
} // namespace control
} // namespace next
