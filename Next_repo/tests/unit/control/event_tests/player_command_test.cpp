/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_command_test.cpp
 * @brief
 *
 **/

#include <gtest/gtest.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <thread>

#include <json/json.h>

#include <ecal/ecal.h>
#include <next/control/event_types/player_command.h>

namespace next {
namespace control {
namespace test {
class TestSerialize : events::PlayerCommandEvent {
public:
  TestSerialize(const std::string &name) : events::PlayerCommandEvent(name){};
  ~TestSerialize(){};
  next::sdk::events::payload_type serializeEvent(next::control::message::request in) {
    return events::PlayerCommandEvent::serializeEventPayload(in);
  }
  auto deserializeEvent(const next::sdk::events::payload_type &in) {
    return events::PlayerCommandEvent::deserializeEventPayload(in);
  }
};

std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}
//! Test to serialize a message and deserialize it
TEST(PlayerCommand, SerializeAndDesedrialize) {
  std::string event_name = generateUniqueName("SerializeAndDesedrialize");
  TestSerialize command_event(event_name);
  next::control::message::request req{};
  req.id = 11;
  req.command = next::control::commands::PlayerCommand::OPEN;
  req.file_path.push_back("file path");
  req.play_forward = true;
  req.requested_steps = 2333;
  req.requested_timestamp = 446436;
  req.timestamp_type = "hour";

  next::sdk::events::payload_type mess_buf = command_event.serializeEvent(req);

  next::control::message::request message = command_event.deserializeEvent(mess_buf);

  ASSERT_TRUE(message.id == req.id);
  ASSERT_TRUE(message.command == req.command);
  ASSERT_TRUE(message.file_path == req.file_path);
  ASSERT_TRUE(message.play_forward == req.play_forward);
  ASSERT_TRUE(message.requested_steps == req.requested_steps);
  ASSERT_TRUE(message.requested_timestamp == req.requested_timestamp);
  ASSERT_TRUE(message.timestamp_type == req.timestamp_type);
}

TEST(PlayerCommand, SerializeAndDeserializeEmptyData) {
  std::string event_name = generateUniqueName("SerializeAndDeserializeEmptyData");
  TestSerialize commandEvent(event_name);
  next::control::message::request empty_msg;
  auto ser_msg = commandEvent.serializeEvent(empty_msg);
  next::control::message::request result_msg = commandEvent.deserializeEvent(ser_msg);
  EXPECT_EQ(empty_msg, result_msg);
}

TEST(PlayerStatusEvent, DeserializeBadDataJson) {
  std::string event_name = generateUniqueName("DeserializeBadDataJson");
  TestSerialize commandEvent(event_name);
  next::sdk::events::payload_type wrong_json_data = next::sdk::events::StringToPayload("TEMP=HELLO");
  next::control::message::request empty_msg;
  next::control::message::request result_msg = commandEvent.deserializeEvent(wrong_json_data);
  EXPECT_EQ(empty_msg, result_msg);
}

TEST(PlayerStatusEvent, DeserializeBoolData) {
  std::string event_name = generateUniqueName("DeserializeBoolData");
  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;

  in_msg.play_forward = true;
  next::sdk::events::payload_type serial_msg = commandEvent.serializeEvent(in_msg);
  next::control::message::request out_msg = commandEvent.deserializeEvent(serial_msg);
  EXPECT_TRUE(in_msg.play_forward == out_msg.play_forward);

  in_msg.play_forward = false;
  serial_msg = commandEvent.serializeEvent(in_msg);
  out_msg = commandEvent.deserializeEvent(serial_msg);
  EXPECT_TRUE(in_msg.play_forward == out_msg.play_forward);

  in_msg.play_forward = "FALSE";
  serial_msg = commandEvent.serializeEvent(in_msg);
  out_msg = commandEvent.deserializeEvent(serial_msg);
  EXPECT_TRUE(in_msg.play_forward == out_msg.play_forward);

  in_msg.play_forward = 1;
  serial_msg = commandEvent.serializeEvent(in_msg);
  out_msg = commandEvent.deserializeEvent(serial_msg);
  EXPECT_TRUE(in_msg.play_forward == out_msg.play_forward);

  in_msg.play_forward = 0;
  serial_msg = commandEvent.serializeEvent(in_msg);
  out_msg = commandEvent.deserializeEvent(serial_msg);
  EXPECT_TRUE(in_msg.play_forward == out_msg.play_forward);
}

TEST(PlayerStatusEvent, stringToCommandAndCommandToStringTest) {
  std::string event_name = generateUniqueName("stringToCommandAndCommandToStringTest");
  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;

  in_msg.command = static_cast<commands::PlayerCommand>(100);
  next::sdk::events::payload_type serial_msg = commandEvent.serializeEvent(in_msg);
  next::control::message::request out_msg = commandEvent.deserializeEvent(serial_msg); // create an empty string to test
  EXPECT_TRUE(out_msg.command == commands::PlayerCommand::NONE);
}

TEST(PlayerStatusEvent, SerialisationCheckUsingJsonInput_singlePath) {
  std::string event_name = generateUniqueName("SerialisationCheckUsingJsonInput_singlePath");
  std::string expected_json_str =
      "{\"command\":\"JUMP\",\"file_path\":[\"test/one/"
      "two\"],\"id\":123,\"play_forward\":\"1\",\"play_until_timestamp\":0,\"requested_steps\":1122,\"requested_"
      "timestamp\":54321,\"sil\":5.4000000953674316,\"timestamp_type\":\"some value\"}\n";
  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;
  in_msg.command = commands::PlayerCommand::JUMP;
  in_msg.file_path.push_back("test/one/two");
  in_msg.id = 123;
  in_msg.play_forward = true;
  in_msg.requested_steps = 1122;
  in_msg.requested_timestamp = 54321;
  in_msg.sil = 5.4f;
  in_msg.timestamp_type = "some value";

  next::sdk::events::payload_type result_msg = commandEvent.serializeEvent(in_msg);
  EXPECT_EQ(expected_json_str, next::sdk::events::PayloadToString(result_msg));
}

TEST(PlayerStatusEvent, SerialisationCheckUsingJsonInput_multiplePaths) {
  std::string event_name = generateUniqueName("SerialisationCheckUsingJsonInput_multiplePaths");
  std::string expected_json_str =
      "{\"command\":\"JUMP\",\"file_path\":[\"test/one/"
      "two\",\"test/one/"
      "two\"],\"id\":123,\"play_forward\":\"1\",\"play_until_timestamp\":0,\"requested_steps\":1122,\"requested_"
      "timestamp\":54321,\"sil\":5.4000000953674316,\"timestamp_type\":\"some value\"}\n";
  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;
  in_msg.command = commands::PlayerCommand::JUMP;
  in_msg.file_path.push_back("test/one/two");
  in_msg.file_path.push_back("test/one/two");
  in_msg.id = 123;
  in_msg.play_forward = true;
  in_msg.requested_steps = 1122;
  in_msg.requested_timestamp = 54321;
  in_msg.sil = 5.4f;
  in_msg.timestamp_type = "some value";

  next::sdk::events::payload_type result_msg = commandEvent.serializeEvent(in_msg);
  EXPECT_EQ(expected_json_str, next::sdk::events::PayloadToString(result_msg));
}

TEST(PlayerStatusEvent, DeserializationUsingJsonInput_singlePath) {
  std::string event_name = generateUniqueName("stringToCommandAndCommandToStringTest_singlePath");
  std::stringstream input_json;
  std::ifstream file("test_input_json_singlePath.json");
  input_json << file.rdbuf();
  std::string input_json_str = input_json.str();

  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;
  next::control::message::request out_msg =
      commandEvent.deserializeEvent(next::sdk::events::StringToPayload(input_json_str));
  EXPECT_EQ(out_msg.command, commands::PlayerCommand::OPEN);
  EXPECT_EQ(out_msg.file_path[0], "abc/cdef/ghi");
  EXPECT_EQ(out_msg.id, 11);
  EXPECT_TRUE(out_msg.play_forward);
  EXPECT_EQ(out_msg.requested_steps, 2333);
  EXPECT_EQ(out_msg.requested_timestamp, 446436);
  EXPECT_EQ(out_msg.sil, 0.0);
  EXPECT_EQ(out_msg.timestamp_type, "hour");
}

TEST(PlayerStatusEvent, DeserializationUsingJsonInput_multiplePaths) {
  std::string event_name = generateUniqueName("stringToCommandAndCommandToStringTest_multiplePaths");
  std::stringstream input_json;
  std::ifstream file("test_input_json_multiplePaths.json");
  input_json << file.rdbuf();
  std::string input_json_str = input_json.str();

  TestSerialize commandEvent(event_name);
  next::control::message::request in_msg;
  next::control::message::request out_msg =
      commandEvent.deserializeEvent(next::sdk::events::StringToPayload(input_json_str));
  EXPECT_EQ(out_msg.command, commands::PlayerCommand::OPEN);
  for (const auto &element : out_msg.file_path) {
    EXPECT_EQ(element, "abc/cdef/ghi");
  }
  EXPECT_EQ(out_msg.id, 11);
  EXPECT_TRUE(out_msg.play_forward);
  EXPECT_EQ(out_msg.requested_steps, 2333);
  EXPECT_EQ(out_msg.requested_timestamp, 446436);
  EXPECT_EQ(out_msg.sil, 0.0);
  EXPECT_EQ(out_msg.timestamp_type, "hour");
}
} // namespace test
} // namespace control
} // namespace next
