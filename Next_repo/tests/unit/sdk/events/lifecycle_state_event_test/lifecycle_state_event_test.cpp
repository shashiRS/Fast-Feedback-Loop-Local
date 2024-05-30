/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     cycle_event_test.cpp
 *  @brief    Testing the Cycle Event
 */

#include <json/json.h>

#include <next/sdk/events/lifecycle_state.h>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class LifecycleStateEventTest : public next::sdk::events::LifecycleState {
public:
  LifecycleStateEventTest(const std::string &name) : LifecycleState(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::LifecycleStateMessage &message) const {
    return serializeEventPayload(message);
  }

  next::sdk::events::LifecycleStateMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

class LifecycleStateEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  LifecycleStateEventTestFixture() {
    std::cout << "LifecycleStateEventTestFixture()" << std::endl;
    this->instance_name_ = "LifecycleStateEventTest";
    // enable logging
    next::sdk::logger::register_to_init_chain();
  }
  ~LifecycleStateEventTestFixture() { std::cout << "~LifecycleStateEventTestFixture()" << std::endl; }
};

TEST_F(LifecycleStateEventTestFixture, serialize_deserialize_CycleEvent_data) {
  LifecycleStateEventTest lifecycle_state_event_test("lifecycle_state_event_test");

  next::sdk::events::LifecycleStateMessage state_message;
  state_message.state = "STATE";
  state_message.component_name = "component_name";
  state_message.timestamp = 999999;

  next::sdk::events::payload_type ser_data = lifecycle_state_event_test.serialize(state_message);

  Json::Value json_val;
  Json::Reader json_reader;
  ASSERT_TRUE(json_reader.parse(next::sdk::events::PayloadToString(ser_data), json_val));
  ASSERT_TRUE(json_val.isMember("state"));
  EXPECT_EQ(json_val["state"].asString(), "STATE");
  ASSERT_TRUE(json_val.isMember("component_name"));
  EXPECT_EQ(json_val["component_name"].asString(), "component_name");
  ASSERT_TRUE(json_val.isMember("timestamp"));
  EXPECT_EQ(std::stoull(json_val["timestamp"].asCString()), 999999);

  next::sdk::events::LifecycleStateMessage state_message_recreated = lifecycle_state_event_test.deserialize(ser_data);

  EXPECT_EQ(state_message_recreated.state, state_message.state);
  EXPECT_EQ(state_message_recreated.component_name, state_message.component_name);
  EXPECT_EQ(state_message_recreated.timestamp, state_message.timestamp);
}
