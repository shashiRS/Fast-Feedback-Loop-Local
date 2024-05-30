/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     logger_initialization_test.cpp
 *  @brief    Testing the logger initalization
 */

#include <next/sdk/events/log_event.hpp>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class LogEventTest : public next::sdk::events::LogEvent {
public:
  LogEventTest(const std::string &name) : LogEvent(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::LogEventData &message) const {
    std::vector<next::sdk::events::LogEventData> vec;
    vec.push_back(message);
    return serializeEventPayload(vec);
  }

  next::sdk::events::payload_type serialize(const std::vector<next::sdk::events::LogEventData> &message) const {
    return serializeEventPayload(message);
  }

  std::vector<next::sdk::events::LogEventData> deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

TEST(LogEventTestFixture, serialize_deserialize_logevent_data_with_many_elements) {
  LogEventTest log_event("log_event_test");
  int count = 5;
  std::vector<next::sdk::events::LogEventData> input_data;
  for (int i = 0; i < count; i++) {
    next::sdk::events::LogEventData log_event_data;
    log_event_data.time_stamp = i;
    log_event_data.channel_name = "test_channel_" + std::to_string(i);
    log_event_data.component_name = "test_component_" + std::to_string(i);
    log_event_data.log_level = next::sdk::logger::LOGLEVEL::INFO;
    log_event_data.log_message = "this is test_" + std::to_string(i);
    input_data.push_back(log_event_data);
  }

  next::sdk::events::payload_type ser_data = log_event.serialize(input_data);
  std::vector<next::sdk::events::LogEventData> output_data = log_event.deserialize(ser_data);

  while (!output_data.empty() && !input_data.empty()) {
    next::sdk::events::LogEventData actual_data = output_data.front();
    next::sdk::events::LogEventData expected_data = input_data.front();

    EXPECT_EQ(actual_data.time_stamp, expected_data.time_stamp);
    EXPECT_EQ(actual_data.channel_name, expected_data.channel_name);
    EXPECT_EQ(actual_data.component_name, expected_data.component_name);
    EXPECT_EQ(actual_data.log_level, expected_data.log_level);
    EXPECT_EQ(actual_data.log_message, expected_data.log_message);

    output_data.pop_back();
    input_data.pop_back();
  }
}
