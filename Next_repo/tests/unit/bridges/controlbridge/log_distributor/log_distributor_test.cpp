#include "log_distributor.hpp"
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <json/json.h>
#include <next/bridgesdk/web_server.hpp>
#include "web_server_mock.hpp"

namespace next {
namespace controlbridge {

std::string CreateJsonString(const std::vector<sdk::events::LogEventData> &log_data_list) {
  Json::Value root;
  Json::Value payload;

  root["channel"] = "logger";
  root["source"] = "NextBridge";
  root["event"] = "UpdateLogInfo";

  for (const auto &log_data : log_data_list) {
    Json::Value log_event_data;

    int64_t seconds =
        log_data.time_stamp * std::chrono::system_clock::period::num / (std::chrono::system_clock::period::den);
    // convert back to get ticks in seconds without decimal places
    auto ticksOnlySec = seconds * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num;
    uint32_t nanoseconds = static_cast<uint32_t>(log_data.time_stamp - ticksOnlySec);

    Json::Value json_timestamp;
    json_timestamp["sec"] = Json::Value(seconds);
    json_timestamp["nsec"] = Json::Value(nanoseconds);

    log_event_data["timestamp"] = json_timestamp;
    int k_fix_foxglove_offset = 1;
    log_event_data["level"] = Json::Value(static_cast<int>(log_data.log_level) + k_fix_foxglove_offset);
    log_event_data["message"] = log_data.log_message;
    log_event_data["name"] = log_data.component_name;
    log_event_data["file"] = "";
    log_event_data["line"] = 0;
    log_event_data["thread_id"] = log_data.thread_id;
    log_event_data["channel_name"] = log_data.channel_name;

    root["payload"]["LogEventData"].append(log_event_data);
  }

  Json::FastWriter writer;
  return writer.write(root);
}

TEST(LOG_DISTRIBUTOR_TEST, SendData) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  LogDistributor log_distributor(webserver_mock, next::sdk::logger::LOGLEVEL::OFF, std::chrono::milliseconds(1));

  std::vector<sdk::events::LogEventData> log_data_list;
  sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_distributor.SendLogEventData(log_data_list);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";
  std::vector<uint8_t> firstData = *webserver_mock->data_.begin();
  std::string send_data(firstData.begin(), firstData.end());

  std::string expected_data = CreateJsonString(log_data_list);

  // check received against expected data
  EXPECT_EQ(expected_data, send_data);
}

TEST(LOG_DISTRIBUTOR_TEST, BufferDataTillWebServerRegistration) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  LogDistributor log_distributor(next::sdk::logger::LOGLEVEL::OFF, std::chrono::milliseconds(1));

  std::vector<sdk::events::LogEventData> log_data_list;
  sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_distributor.SendLogEventData(log_data_list);

  log_distributor.SetWebserver(webserver_mock);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";
  std::vector<uint8_t> firstData = *webserver_mock->data_.begin();
  std::string send_data(firstData.begin(), firstData.end());

  std::string expected_data = CreateJsonString(log_data_list);

  // check received against expected data
  EXPECT_EQ(expected_data, send_data);
}

TEST(LOG_DISTRIBUTOR_TEST, SetWaitTimer) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  LogDistributor log_distributor;
  log_distributor.SetWaitTime(std::chrono::milliseconds(1));
  // wait till while loop inside log_distributor finished once
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::vector<sdk::events::LogEventData> log_data_list;
  sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_distributor.SendLogEventData(log_data_list);

  log_distributor.SetWebserver(webserver_mock);

  std::this_thread::sleep_for(std::chrono::milliseconds(990));

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received. Wait timee wasn't changed";
  std::vector<uint8_t> firstData = *webserver_mock->data_.begin();
  std::string send_data(firstData.begin(), firstData.end());

  std::string expected_data = CreateJsonString(log_data_list);

  // check received against expected data
  EXPECT_EQ(expected_data, send_data);
}

TEST(LOG_DISTRIBUTOR_TEST, AbortSendJsonGui) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  std::shared_ptr<LogDistributor> log_distributor =
      std::make_shared<LogDistributor>(webserver_mock, next::sdk::logger::LOGLEVEL::OFF, std::chrono::milliseconds(1));

  log_distributor.reset();

  // if end of file is reached, deconstructor stopped SendJsonGui thread
}

TEST(LOG_DISTRIBUTOR_TEST, sendDataWhenFlushLevelIsAboveThreshHold) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  LogDistributor log_distributor(webserver_mock, next::sdk::logger::LOGLEVEL::INFO, std::chrono::milliseconds(1));

  std::vector<sdk::events::LogEventData> log_data_list;
  sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::OFF;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_distributor.SendLogEventData(log_data_list);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";
  std::vector<uint8_t> firstData = *webserver_mock->data_.begin();
  std::string send_data(firstData.begin(), firstData.end());

  std::string expected_data = CreateJsonString(log_data_list);

  // check received against expected data
  EXPECT_EQ(expected_data, send_data);
}

TEST(LOG_DISTRIBUTOR_TEST, broadcastingShallNotHappenWhenLoggingMessageIsEmpty) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  LogDistributor log_distributor(webserver_mock, next::sdk::logger::LOGLEVEL::INFO, std::chrono::milliseconds(1));

  std::vector<sdk::events::LogEventData> log_data_list;

  log_distributor.SendLogEventData(log_data_list);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  EXPECT_EQ(0, webserver_mock->data_.size());
}

} // namespace controlbridge
} // namespace next
