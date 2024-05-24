/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     ego_vehicle_test.cpp
 *  @brief    Testing the view3D_EgoVehicle plugin
 */

#include "ego_vehicle.h"

#include <cip_test_support/gtest_reporting.h>
#include "unittest_plugin_helper.hpp"

class EgoVehicleFixture : public testing::Test {
public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  void SetUp() {}

  void TearDown() {}
};

TEST_F(EgoVehicleFixture, testSearchResultsForUrls) {
  const std::unordered_set<std::string> test_urls = {"ADC5xx_Device.VD_DATA.IuOdoEstimationPort.sSigHeader.uiTimeStamp",
                                                     "ADC5xx_Device.VD_DATA.IuOdoEstimationPort.xPosition_m",
                                                     "ADC5xx_Device.VD_DATA.IuOdoEstimationPort.yPosition_m",
                                                     "ADC5xx_Device.VD_DATA.IuOdoEstimationPort.yawAngle_rad",
                                                     "ADC5xx_Device.VD_DATA.IuOdoEstimationPort.xVelocity_mps",
                                                     "ADC5xx_Device.VD_DATA.IuOdoEstimationPort.yawRate_radps"};

  next::plugins::EgoVehiclePlugin plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "ADC5xx_Device.VD_DATA.IuOdoEstimationPort";
  expected_result.data_type_to_result = {{"ego_pos_x", {".xPosition_m", "dummyregexpattern"}},
                                         {"ego_pos_y", {".yPosition_m", "dummyregexpattern"}},
                                         {"ego_yaw_angle", {".yawAngle_rad", "dummyregexpattern"}},
                                         {"ego_vel_x", {".xVelocity_mps", "dummyregexpattern"}},
                                         {"ego_yaw_rate", {".yawRate_radps", "dummyregexpattern"}},
                                         {"timestamp", {".sSigHeader.uiTimeStamp", "dummyregexpattern"}}};

  auto result = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(result.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(result[0].root_url, expected_result.root_url) << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("ego_pos_x").result_url,
            expected_result.data_type_to_result.at("ego_pos_x").result_url)
      << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("ego_pos_y").result_url,
            expected_result.data_type_to_result.at("ego_pos_y").result_url)
      << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("ego_yaw_angle").result_url,
            expected_result.data_type_to_result.at("ego_yaw_angle").result_url)
      << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("ego_vel_x").result_url,
            expected_result.data_type_to_result.at("ego_vel_x").result_url)
      << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("ego_yaw_rate").result_url,
            expected_result.data_type_to_result.at("ego_yaw_rate").result_url)
      << "Root url is different than expected value";

  EXPECT_EQ(result[0].data_type_to_result.at("timestamp").result_url,
            expected_result.data_type_to_result.at("timestamp").result_url)
      << "Root url is different than expected value";
};
