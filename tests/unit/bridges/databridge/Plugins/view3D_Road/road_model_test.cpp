/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     road_model_test.cpp
 *  @brief    Testing the view3D_Road plugin
 */
#include <cip_test_support/gtest_reporting.h>
#include <flatbuffers/flatbuffers.h>

#include <boost/dll.hpp>

#include <next/bridgesdk/schema/3d_view/groundline_generated.h>
#include <next/plugins_utils/fdp_dummy_data_creator.h>

#include "roadmodel.h"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

TYPED_TEST_CASE(PluginTest, next::plugins::VisuRoadModel);

TYPED_TEST(PluginTest, DISABLED_TestingFDPRoadModelPluginWithRealValue) {
  std::vector<std::string> topic_by_url{"ADC4xx.FDP_Base.p_RmfOutputIf"};

  next::udex::Signal signal;

  auto file_path = boost::dll::program_location().parent_path().string();

  next::plugins_utils::createGeneralInfoOutofDumpedFile(file_path + "\\general_info_file_dump.txt", signal);
  next::bridgesdk::plugin_config_t config_fields;

  config_fields.insert({"url", {next::bridgesdk::ConfigType::EDITABLE, "ADC4xx.FDP_Base.p_RmfOutputIf"}});
  config_fields.insert({"sensor", {next::bridgesdk::ConfigType::EDITABLE, "FDP base road model measfreeze"}});
  config_fields.insert({"datacache", {next::bridgesdk::ConfigType::FIXED, "Dummy1"}});
  config_fields.insert({"DATA_VIEW", {next::bridgesdk::ConfigType::FIXED, "objectData"}});

  auto signal_explorer_mock = std::dynamic_pointer_cast<next::test::SignalExplorerMock>(this->signal_explorer_ptr_);
  signal_explorer_mock->setTopicByUrl(topic_by_url);

  this->plugin_->addConfig(config_fields);

  next::bridgesdk::ChannelMessagePointers message{};
  message.channel_name = "testFdpRoadModel";
  std::vector<int> result;
  next::plugins_utils::createVectorOutOfDumpedPayloadFile(file_path + "\\payload_data_file_dump.txt", result);

  message.data = &result[0];
  message.size = sizeof(int) * result.size();

  next::plugins_utils::SensorConfig sensor_config{};
  sensor_config.data_cache_name = "dummy";
  sensor_config.sensor_url = "ADC4xx.FDP_Base.p_RmfOutputIf";

  const std::byte *charBuffer = (std::byte *)message.data;
  std::vector<std::byte> vectorBuffer(charBuffer, charBuffer + message.size);

  this->subscriber_mock_->forwardData(sensor_config.sensor_url, vectorBuffer);

  auto data_manager_mock = std::dynamic_pointer_cast<next::test::DataManagerMock>(this->dm_mock_ptr_);

  ASSERT_FALSE(data_manager_mock->flatbuffer_data_.empty());

  auto ground_lines = GuiSchema::GetGroundLineList(data_manager_mock->flatbuffer_data_.data());

  auto glines = ground_lines->groundLines();
  ASSERT_EQ(glines->size(), 3u);

  // test points on pointsx and pointsy
  EXPECT_NEAR(glines->Get(0)->pointsX()->Get(0), 0.0, 1e-5);
  EXPECT_NEAR(glines->Get(0)->pointsY()->Get(0), 0.150021, 1e-5);

  EXPECT_NEAR(glines->Get(1)->pointsX()->Get(0), 0.0, 1e-5);
  EXPECT_NEAR(glines->Get(1)->pointsY()->Get(0), 2.17002, 1e-5);

  EXPECT_EQ(glines->Get(1)->pointsX()->size(), 9u);
  EXPECT_EQ(glines->Get(1)->pointsY()->size(), 9u);

  EXPECT_EQ(glines->Get(2)->pointsX()->size(), 9u);
  EXPECT_EQ(glines->Get(2)->pointsY()->size(), 9u);
}

TYPED_TEST(PluginTest, TestUrlSearch) {
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  const std::unordered_set<std::string> test_urls = {
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.egoLane",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.egoPosition",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.numLaneSegments",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.numConnections",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].centerline.startIndex",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].centerline.endIndex",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].id",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].leftLane",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].rightLane",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].numLeftBoundaryParts",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].numRightBoundaryParts",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].leftBoundaryParts[0]",
      "ADC4xx.FDP_Base.p_RmfOutputIf.laneTopology.laneSegments[0].rightBoundaryParts[0]",
      "ADC4xx.FDP_Base.p_RmfOutputIf.numPoints",
      "ADC4xx.FDP_Base.p_RmfOutputIf.points",
      "ADC4xx.FDP_Base.p_RmfOutputIf.points[0].x",
      "ADC4xx.FDP_Base.p_RmfOutputIf.points[0].y",
      "ADC4xx.FDP_Base.p_RmfOutputIf.numLinearObjects",
      "ADC4xx.FDP_Base.p_RmfOutputIf.linearObjects[0].geometry.startIndex",
      "ADC4xx.FDP_Base.p_RmfOutputIf.linearObjects[0].geometry.endIndex"};

  const size_t expected_result_size = 1;
  const SearchResult expected_search_result = {"ADC4xx.FDP_Base.p_RmfOutputIf",
                                               {{"ego_lane_", {".laneTopology.egoLane", ""}},
                                                {"ego_position_", {".laneTopology.egoPosition", ""}},
                                                {"num_connections_", {".laneTopology.numConnections", ""}},
                                                {"num_lane_segments_", {".laneTopology.numLaneSegments", ""}},
                                                {"laneTopology_laneSegments_", {".laneTopology.laneSegments", ""}},
                                                /**/ {"line_start_index_", {".centerline.startIndex", ""}},
                                                /**/ {"line_end_index_", {".centerline.endIndex", ""}},
                                                /**/ {"id_", {".id", ""}},
                                                /**/ {"left_lane_", {".leftLane", ""}},
                                                /**/ {"right_lane_", {".rightLane", ""}},
                                                /**/ {"num_left_boundary_", {".numLeftBoundaryParts", ""}},
                                                /**/ {"num_right_boundary_", {".numRightBoundaryParts", ""}},
                                                /**/ {"leftBoundaryParts_", {".leftBoundaryParts", ""}},
                                                /**/ {"rightBoundaryParts_", {".rightBoundaryParts", ""}},
                                                {"num_points_", {".numPoints", ""}},
                                                {"points_", {".points", ""}},
                                                /**/ {"point_x_", {".x", ""}},
                                                /**/ {"point_y_", {".y", ""}},
                                                {"num_linear_obj_", {".numLinearObjects", ""}},
                                                {"linearObjects_", {".linearObjects", ""}},
                                                /**/ {"geo_start_index_", {".geometry.startIndex", ""}},
                                                /**/ {"geo_end_index_", {".geometry.endIndex", ""}}}};

  next::plugins::VisuRoadModel plugin("TestPlugin");
  plugin.init();
  const auto results = plugin.GetSearchResults("", test_urls);

  ASSERT_TRUE(expected_result_size == results.size()) << "Number of results different than expected";
  EXPECT_EQ(expected_search_result.root_url, results[0].root_url);
  for (const auto &[key, result] : results[0].data_type_to_result) {
    EXPECT_EQ(expected_search_result.data_type_to_result.at(key).result_url, result.result_url);
  }
}
