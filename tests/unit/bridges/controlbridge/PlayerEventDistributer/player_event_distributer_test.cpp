#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <json/json.h>

#include <next/appsupport/config/config_key_generator_player.hpp>

#include "player_event_distributor.h"
#include "web_server_mock.hpp"

namespace next {
namespace controlbridge {
TEST(Player_Event_Distributer, createJson_SilFactor) {
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
