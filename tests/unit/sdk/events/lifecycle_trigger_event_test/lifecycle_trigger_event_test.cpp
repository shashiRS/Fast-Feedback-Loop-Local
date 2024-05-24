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

#include <next/sdk/events/lifecycle_trigger.h>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class LifecycleTriggerEventTest : public next::sdk::events::LifecycleTrigger {
public:
  LifecycleTriggerEventTest(const std::string &name) : LifecycleTrigger(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::LifecycleTriggerMessage &message) const {
    return serializeEventPayload(message);
  }

  next::sdk::events::LifecycleTriggerMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

class LifecycleTriggerEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  LifecycleTriggerEventTestFixture() {
    std::cout << "LifecycleTriggerEventTestFixture()" << std::endl;
    this->instance_name_ = "LifecycleTriggerEventTest";
    // enable logging
    next::sdk::logger::register_to_init_chain();
  }
  ~LifecycleTriggerEventTestFixture() { std::cout << "~LifecycleTriggerEventTestFixture()" << std::endl; }
};

TEST_F(LifecycleTriggerEventTestFixture, serialize_deserialize_CycleEvent_data) {
  LifecycleTriggerEventTest lifecycle_trigger_event_test("lifecycle_trigger_event_test");

  next::sdk::events::LifecycleTriggerMessage trigger_message;
  trigger_message.trigger_event = "TRIGGER_EVENT";
  trigger_message.component_name = "component_name";
  trigger_message.timestamp = 999999;

  next::sdk::events::payload_type ser_data = lifecycle_trigger_event_test.serialize(trigger_message);

  Json::Value json_val;
  Json::Reader json_reader;
  ASSERT_TRUE(json_reader.parse(next::sdk::events::PayloadToString(ser_data), json_val));
  ASSERT_TRUE(json_val.isMember("trigger_event"));
  EXPECT_EQ(json_val["trigger_event"].asString(), "TRIGGER_EVENT");
  ASSERT_TRUE(json_val.isMember("component_name"));
  EXPECT_EQ(json_val["component_name"].asString(), "component_name");
  ASSERT_TRUE(json_val.isMember("timestamp"));
  EXPECT_EQ(std::stoull(json_val["timestamp"].asCString()), 999999);

  next::sdk::events::LifecycleTriggerMessage trigger_message_recreated =
      lifecycle_trigger_event_test.deserialize(ser_data);

  EXPECT_EQ(trigger_message_recreated.trigger_event, trigger_message.trigger_event);
  EXPECT_EQ(trigger_message_recreated.component_name, trigger_message.component_name);
  EXPECT_EQ(trigger_message_recreated.timestamp, trigger_message.timestamp);
}
