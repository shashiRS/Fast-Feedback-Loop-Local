/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     cycle_information_event_test.cpp
 *  @brief    Testing the Cycle Information Event
 */

#include <next/sdk/events/cycle_information_event.hpp>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class CycleInformationEventTest : public next::sdk::events::CycleInformationEvent {
public:
  CycleInformationEventTest(const std::string &name) : CycleInformationEvent(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::CycleInformation &message) const {
    return serializeEventPayload(message);
  }

  next::sdk::events::CycleInformation deserialize(const next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

class CycleInformationEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  CycleInformationEventTestFixture() {
    std::cout << "CycleInformationEventTestFixture()" << std::endl;
    this->instance_name_ = "CycleInformationEventTest";
    // enable logging
    next::sdk::logger::register_to_init_chain();
  }
};

TEST_F(CycleInformationEventTestFixture, serialize_deserialize_CycleInformationEvent_data) {
  CycleInformationEventTest cycle_event("cycle_information_event_test");

  next::sdk::events::CycleInformation cycle_event_data;
  cycle_event_data.push_back({"ARS620", 1001u});
  cycle_event_data.push_back({"SomeDeviceName", 42u});

  next::sdk::events::payload_type ser_data = cycle_event.serialize(cycle_event_data);
  next::sdk::events::CycleInformation cycle_event_data_recreated = cycle_event.deserialize(ser_data);

  ASSERT_EQ(cycle_event_data_recreated.size(), cycle_event_data.size());
  for (size_t pos = 0; pos < cycle_event_data.size(); ++pos) {
    EXPECT_EQ(cycle_event_data_recreated[pos].first, cycle_event_data[pos].first)
        << "device name at position " << pos << " incorrect";
    EXPECT_EQ(cycle_event_data_recreated[pos].second, cycle_event_data[pos].second)
        << "id at position " << pos << " incorrect";
  }
}
