#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <json/json.h>

#include <next/appsupport/config/config_key_generator_player.hpp>

#include "player_event_distributor.h"
#include "web_server_mock.hpp"

namespace next {
namespace controlbridge {

TEST(Meta_Event_Handler, DISABLED_getMessageFromMetaInformation) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);
  next::control::events::PlayerMetaInformationMessage send_message;
  send_message.max_timestamp = 42;
  send_message.min_timestamp = 33;
  send_message.timestamp = 22;
  send_message.speed_factor = 11.5;
  send_message.status = next::control::events::PlayerState::ON_NEW;
  send_message.recording_name = "test_recording.rrec";
  // meta_event_distributor.player_event_subscriber_.setEventData(send_message);

  next::control::events::PlayerMetaInformationMessage received_message;
  received_message = meta_event_distributor.getMessage();

  EXPECT_EQ(received_message.max_timestamp,
            meta_event_distributor.player_event_subscriber_.getEventData().max_timestamp);
  EXPECT_EQ(received_message.min_timestamp,
            meta_event_distributor.player_event_subscriber_.getEventData().min_timestamp);
  EXPECT_EQ(received_message.timestamp, meta_event_distributor.player_event_subscriber_.getEventData().timestamp);
  EXPECT_EQ(received_message.speed_factor, meta_event_distributor.player_event_subscriber_.getEventData().speed_factor);
  EXPECT_EQ(received_message.status, meta_event_distributor.player_event_subscriber_.getEventData().status);
  EXPECT_EQ(received_message.recording_name,
            meta_event_distributor.player_event_subscriber_.getEventData().recording_name);
}

TEST(Meta_Event_Handler, createJson_PlayerInitialized) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);

  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::ON_NEW;
  received_message.recording_name = "test_recording.rrec";

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_source;
  expected_channel = "player";
  expected_source = "NextBridge";
  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
}

TEST(Meta_Event_Handler, createJson_RecordingLoaded) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);
  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;
  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::EXIT_OPENING;
  received_message.recording_name = "test_recording.rrec";
  created_json_string = meta_event_distributor.createJson(received_message);

  received_message.status = next::control::events::PlayerState::ON_READY;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_event, expected_source;
  int expected_currentTimestamp;
  expected_channel = "player";
  expected_source = "NextBridge";
  expected_event = "RecordingIsLoaded";
  expected_currentTimestamp = int(received_message.timestamp);

  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
  EXPECT_EQ(created_json["event"], expected_event) << "Correct Event not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["currentTimestamp"], expected_currentTimestamp)
      << "Correct currentTimestamp not found in" << created_json_string;
}

TEST(Meta_Event_Handler, createJson_PlayingRecording) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);

  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::ON_PLAY;
  received_message.recording_name = "test_recording.rrec";

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_event, expected_source;
  int expected_startTimestamp, expected_endTimestamp, expected_currentTimestamp;
  float expected_speedMultiple;
  expected_channel = "player";
  expected_source = "NextBridge";
  expected_event = "PlaybackIsPlaying";
  expected_startTimestamp = int(received_message.min_timestamp);
  expected_endTimestamp = int(received_message.max_timestamp);
  expected_currentTimestamp = int(received_message.timestamp);
  expected_speedMultiple = received_message.speed_factor;

  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
  EXPECT_EQ(created_json["event"], expected_event) << "Correct Event not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["startTimeStamp"], expected_startTimestamp)
      << "Correct startTimeStamp not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["endTimeStamp"], expected_endTimestamp)
      << "Correct endTimeStamp not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["currentTimestamp"], expected_currentTimestamp)
      << "Correct currentTimestamp not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["speedMultiple"], expected_speedMultiple)
      << "Correct speedMultiple not found in" << created_json_string;
}

TEST(Meta_Event_Handler, createJson_RecordingPaused) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);

  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::ON_READY;
  received_message.recording_name = "test_recording.rrec";

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_event, expected_source;
  int expected_currentTimestamp;
  expected_channel = "player";
  expected_source = "NextBridge";
  expected_event = "PlaybackIsPaused";
  expected_currentTimestamp = int(received_message.timestamp);

  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
  EXPECT_EQ(created_json["event"], expected_event) << "Correct Event not found in" << created_json_string;
  EXPECT_EQ(created_json["payload"]["currentTimestamp"], expected_currentTimestamp)
      << "Correct currentTimestamp not found in" << created_json_string;
}

TEST(Meta_Event_Handler, createJson_RecordingClosed) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);

  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::ENTER_NEW;
  received_message.recording_name = "test_recording.rrec";

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;

  created_json_string = meta_event_distributor.createJson(received_message);
  received_message.status = next::control::events::PlayerState::ON_NEW;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_event, expected_source;
  expected_channel = "player";
  expected_source = "NextBridge";
  expected_event = "SimulationStateIsIdle";

  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
  EXPECT_EQ(created_json["event"], expected_event) << "Correct Event not found in" << created_json_string;
}

TEST(Meta_Event_Handler, sendJsonToGUIFromMetaInformation) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);
  next::control::events::PlayerMetaInformationMessage received_message;
  received_message.max_timestamp = 42;
  received_message.min_timestamp = 33;
  received_message.timestamp = 22;
  received_message.speed_factor = 11.5;
  received_message.status = next::control::events::PlayerState::ON_READY;
  received_message.recording_name = "test_recording.rrec";

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string;
  created_json_string = meta_event_distributor.createJson(received_message);

  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));
  std::string message_to_send;
  message_to_send = "test_message";

  meta_event_distributor.sendJsonToGUI(message_to_send);

  std::vector<uint8_t> first_out = webserver_mock->data_.front();
  std::string sent_data((char *)first_out.data(), first_out.size());
  webserver_mock->data_.pop_front();

  EXPECT_EQ(sent_data, message_to_send);
}

TEST(Meta_Event_Handler, DISABLED_runMetaInformationDistribution) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock);
  Json::FastWriter writer;
  Json::Value json_test_root, json_test_info, stepping;

  meta_event_distributor.player_event_subscriber_._message.max_timestamp = 42;
  meta_event_distributor.player_event_subscriber_._message.min_timestamp = 33;
  meta_event_distributor.player_event_subscriber_._message.timestamp = 22;
  meta_event_distributor.player_event_subscriber_._message.speed_factor = 11.5;
  meta_event_distributor.player_event_subscriber_._message.status = next::control::events::PlayerState::ON_PLAY;
  meta_event_distributor.player_event_subscriber_._message.recording_name = "test_recording.rrec";
  meta_event_distributor.player_event_subscriber_._message.stepping_type = "";
  meta_event_distributor.player_event_subscriber_._message.stepping_value = 0;

  // meta_event_distributor.player_event_subscriber_.setEventData(meta_event_distributor.player_event_subscriber_._message);
  json_test_root["channel"] = "player";
  json_test_root["source"] = "NextBridge";
  json_test_root["event"] = "PlaybackIsPlaying";
  json_test_info["startTimeStamp"] = meta_event_distributor.player_event_subscriber_._message.min_timestamp;
  json_test_info["endTimeStamp"] = meta_event_distributor.player_event_subscriber_._message.max_timestamp;
  json_test_info["currentTimestamp"] = meta_event_distributor.player_event_subscriber_._message.timestamp;
  json_test_info["speedMultiple"] = meta_event_distributor.player_event_subscriber_._message.speed_factor;
  json_test_info["unit"] = "us";
  stepping["stepType"] = meta_event_distributor.player_event_subscriber_._message.stepping_type;
  stepping["stepValue"] = meta_event_distributor.player_event_subscriber_._message.stepping_value;
  json_test_info["metaData"] = stepping;
  json_test_root["payload"] = json_test_info;
  std::string expected_sent_message = writer.write(json_test_root);

  meta_event_distributor.runMetaInformationDistribution();

  std::vector<uint8_t> first_out = webserver_mock->data_.front();
  std::string sent_data((char *)first_out.data(), first_out.size());
  webserver_mock->data_.pop_front();

  EXPECT_EQ(sent_data, expected_sent_message);
}

TEST(Meta_Event_Handler, createJson_SilFactor) {
  const float kSilFactorValue = 2.5;
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();
  auto configServer = std::make_shared<next::appsupport::ConfigServer>("control_bridge_config_server");

  event_handler::PlayerEventDistributor meta_event_distributor(webserver_mock, "", configServer);

  Json::Reader reader;
  Json::Value created_json;
  std::string created_json_string = meta_event_distributor.createSilFactorJson();
  EXPECT_EQ(created_json_string, "") << "Config not set for sil factor";
  // set the config value for sil factor as it would be set from GUI
  configServer->put(next::appsupport::configkey::player::getRealTimeFactorKey(), kSilFactorValue);
  configServer->pushGlobalConfig();

  created_json_string = meta_event_distributor.createSilFactorJson();
  ASSERT_TRUE(reader.parse(created_json_string, created_json, false));

  std::string expected_channel, expected_event, expected_source;
  // int expected_currentTimestamp;
  expected_channel = "player";
  expected_source = "NextBridge";
  expected_event = "OverwriteSilFactor";

  EXPECT_EQ(created_json["channel"], expected_channel) << "Correct Channel not found in" << created_json_string;
  EXPECT_EQ(created_json["source"], expected_source) << "Correct Source not found in" << created_json_string;
  EXPECT_EQ(created_json["event"], expected_event) << "Correct Event not found in" << created_json_string;
  EXPECT_NEAR(created_json["payload"]["expectedSpeedMultiple"].asFloat(), kSilFactorValue, 0.1);
}
} // namespace controlbridge
} // namespace next
