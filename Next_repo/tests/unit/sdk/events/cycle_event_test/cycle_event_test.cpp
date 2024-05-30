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

#include <next/sdk/events/cycle_event.hpp>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class CycleEventTest : public next::sdk::events::CycleEvent {
public:
  CycleEventTest(const std::string &name) : CycleEvent(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::Cycle &message) const {
    return serializeEventPayload(message);
  }

  next::sdk::events::Cycle deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

class CycleEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  CycleEventTestFixture() {
    std::cout << "CycleEventTestFixture()" << std::endl;
    this->instance_name_ = "CycleEventTest";
    // enable logging
    next::sdk::logger::register_to_init_chain();
  }
  ~CycleEventTestFixture() { std::cout << "~CycleEventTestFixture()" << std::endl; }
};

TEST_F(CycleEventTestFixture, serialize_deserialize_CycleEvent_data) {
  CycleEventTest cycle_event("cycle_event_test");

  next::sdk::events::Cycle cycle_event_data;
  cycle_event_data.cycle_id = 150;
  cycle_event_data.cycle_state = next::sdk::events::CycleState::start;

  next::sdk::events::payload_type ser_data = cycle_event.serialize(cycle_event_data);

  Json::Value json_val;
  Json::Reader json_reader;
  ASSERT_TRUE(json_reader.parse(next::sdk::events::PayloadToString(ser_data), json_val));
  ASSERT_TRUE(json_val.isMember("cycle_id"));
  EXPECT_EQ(json_val["cycle_id"].asUInt(), cycle_event_data.cycle_id);
  ASSERT_TRUE(json_val.isMember("cycle_state"));
  EXPECT_EQ(json_val["cycle_state"].asString(), "CYCLE_START");

  next::sdk::events::Cycle cycle_event_data_recreated = cycle_event.deserialize(ser_data);

  EXPECT_EQ(cycle_event_data_recreated.cycle_id, cycle_event_data.cycle_id);
  EXPECT_EQ(cycle_event_data_recreated.cycle_state, cycle_event_data.cycle_state);
}
