/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     point_cloud_test.cpp
 *  @brief    Testing the view3D_PointCloud plugin
 */

#include <cip_test_support/gtest_reporting.h>
#include "flatbuffers/flatbuffers.h"

#include <regex>
#include <string>

#include "point_cloud.h"
#include "point_cloud_test.hpp"
#include "unittest_plugin_helper.hpp"

#include <next/udex/publisher/data_publisher.hpp>

namespace next {
using next::test::PluginTest;

TYPED_TEST_CASE(PluginTest, next::plugins::PointCloud);

// Disabled as the internal UDex structure does not allow for data manipulation that was done here before
// Can be enabled when signals can be inserted into UDex
TYPED_TEST(PluginTest, DISABLED_TestingPointCloudPluginWithRealValue) {

  this->plugin_->setDependencies(this->plugin_signal_publisher_, this->plugin_sig_extractor_,
                                 this->plugin_signal_subscriber_, this->plugin_signal_explorer_);

  std::vector<std::string> topic_by_url{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints"};

  std::unordered_map<std::string, next::bridgesdk::plugin_addons::SignalInfo> signal;
  signal.insert({{"MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT, 1000, 1, 0}}});
  signal.insert({{"MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort.xPosition_m",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 0}}});
  signal.insert({{"MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort.yPosition_m",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 4}}});
  signal.insert({{"MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort.yawAngle_rad",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 8}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT, 1000, 10, 14}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints.numPoints",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32, 2, 1, 12}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints.pointList",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT, 1000, 1, 14}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints.pointList[0].x_m",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 14}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints.pointList[0].z_m",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 18}}});
  signal.insert({{"MTA_ADC5.SPP_FC_DATA.pSppSemPoints.pointList[0].y_m",
                  {next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, 4, 1, 22}}});

  next::bridgesdk::plugin_config_t config_fields;
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Parking Point Cloud"}});
  config_fields.insert({"url_point", {next::bridgesdk::ConfigType::SELECTABLE, "MTA_ADC5.SPP_FC_DATA.pSppSemPoints"}});
  config_fields.insert(
      {"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort"}});
  config_fields.insert({"naming_type", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "Point"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#FFD67F"}});
  config_fields.insert({"DATA_VIEW", {next::bridgesdk::ConfigType::EDITABLE, "dataObject"}});
  config_fields.insert({"coordinate_system", {next::bridgesdk::ConfigType::EDITABLE, "ego_vehicle"}});

  auto signal_explorer_mock =
      std::dynamic_pointer_cast<next::test::PluginSignalExplorerMock>(this->plugin_signal_explorer_);
  signal_explorer_mock->setURLs("MTA_ADC5.SPP_FC_DATA.pSppSemPoints", {"MTA_ADC5.SPP_FC_DATA.pSppSemPoints"});
  signal_explorer_mock->setURLs("MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort",
                                {"MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort"});

  auto signal_extractor_mock =
      std::dynamic_pointer_cast<next::test::PluginSignalExtractorMock>(this->plugin_sig_extractor_);
  signal_extractor_mock->setSignalInfo(signal);
  auto signal_subscriber_mock =
      std::dynamic_pointer_cast<next::test::PluginDataSubscriberMock>(this->plugin_signal_subscriber_);
  ASSERT_TRUE(this->plugin_->addConfig(config_fields));

  std::vector<uint8_t> data{1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 128, 63, 0, 0, 0, 64, 0, 0, 64, 64};

  signal_extractor_mock->updatePayload(data);

  next::plugins_utils::SensorConfig sensor_config{};
  sensor_config.data_cache_name = "dummy";

  next::bridgesdk::ChannelMessagePointers message{nullptr, nullptr, 0, 0};
  message.data = reinterpret_cast<const void *>(data.data());
  message.size = data.size();

  auto callable_obj = signal_subscriber_mock->callbacks_["MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort"];
  callable_obj("test", &message);

  callable_obj = signal_subscriber_mock->callbacks_["MTA_ADC5.SPP_FC_DATA.pSppSemPoints"];
  callable_obj("test", &message);

  const auto publisher_mock =
      std::dynamic_pointer_cast<next::test::PluginDataPublisherMock>(this->plugin_signal_publisher_);
  EXPECT_EQ(publisher_mock->flatbuffer_data_.empty(), false);

  if (!publisher_mock->flatbuffer_data_.empty()) {
    auto point_cloud = GuiSchema::GetPointCloudList(publisher_mock->flatbuffer_data_.data());
    EXPECT_FLOAT_EQ(point_cloud->pointClouds()->Get(0)->points()->Get(0), 1.F);
    EXPECT_FLOAT_EQ(point_cloud->pointClouds()->Get(0)->points()->Get(1), 2.F);
    EXPECT_FLOAT_EQ(point_cloud->pointClouds()->Get(0)->points()->Get(2), -3.F);
  }
}

TEST_F(PointCloudPluginTest, regular_expression_pointsArray_fromPointList) {
  const std::unordered_set<std::string> test_urls = {
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].confidence",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.timestamp",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].standardDeviationVelY",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].pointHeader.pointID",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.numberOfPoints",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].relativeVelocityX",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].standardDeviationVelX",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].positionX",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].pointHeader.measurementStatus",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].positionY",
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.sSigHeader.uiTimeStamp",
      "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].positionZ",
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.numberOfPoints",
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Xposition_m",
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Yposition_m",
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Zposition_m"};

  next::plugins::PointCloud plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result_CEM;
  expected_result_CEM.root_url = "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput";

  expected_result_CEM.data_type_to_result = {{"num_points", {".ussPointList.numberOfPoints", ""}},
                                             {"point_", {".ussPointList.ussPointsArray", ""}},
                                             {"xpos", {".positionX", ""}},
                                             {"ypos", {".positionY", ""}},
                                             {"zpos", {".positionZ", ""}}};

  SearchResult expected_result_USP;
  expected_result_USP.root_url = "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput";

  expected_result_USP.data_type_to_result = {{"num_points", {".numberOfPoints", ""}},
                                             {"point_", {".points", ""}},
                                             {"xpos", {".Xposition_m", ""}},
                                             {"ypos", {".Yposition_m", ""}},
                                             {"zpos", {".Zposition_m", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 2) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result_CEM.root_url) << "Root url is different than expected value";
  EXPECT_EQ(results[1].root_url, expected_result_USP.root_url) << "Root url is different than expected value";

  for (const auto [key, result] : expected_result_CEM.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }

  for (const auto [key, result] : expected_result_USP.data_type_to_result) {
    EXPECT_EQ(results[1].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(PointCloudPluginTest, regular_expression_pointsArray_SFM) {
  const std::unordered_set<std::string> test_urls = {
      "ADC5xx_Device.CEM_EM_DATA.CemEmSfmPointListLeft.numPoints",
      "ADC5xx_Device.CEM_EM_DATA.CemEmSfmPointListLeft.pointList[0].x_m",
      "ADC5xx_Device.CEM_EM_DATA.CemEmSfmPointListLeft.pointList[0].y_m",
      "ADC5xx_Device.CEM_EM_DATA.CemEmSfmPointListLeft.pointList[0].z_m"};

  next::plugins::PointCloud plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "ADC5xx_Device.CEM_EM_DATA.CemEmSfmPointListLeft";
  expected_result.data_type_to_result = {{"num_points", {".numPoints", ""}},
                                         {"point_", {".pointList", ""}},
                                         {"xpos", {".x_m", ""}},
                                         {"ypos", {".y_m", ""}},
                                         {"zpos", {".z_m", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result.root_url) << "Root url is different than expected value";
  for (const auto [key, result] : expected_result.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(PointCloudPluginTest, regular_expression_poinstArray_CarPC) {
  const std::unordered_set<std::string> test_urls = {
      "ADC5xx_Device.CEM_EM_DATA.CemEmUsProcessingPointList.NumberOfPoints_nu_u32",
      "ADC5xx_Device.CEM_EM_DATA.CemEmUsProcessingPointList.Points_as[0].Xposition_m_f32",
      "ADC5xx_Device.CEM_EM_DATA.CemEmUsProcessingPointList.Points_as[0].Yposition_m_f32",
      "ADC5xx_Device.CEM_EM_DATA.CemEmUsProcessingPointList.Points_as[0].Zposition_m_f32"};

  next::plugins::PointCloud plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "ADC5xx_Device.CEM_EM_DATA.CemEmUsProcessingPointList";
  expected_result.data_type_to_result = {{"num_points", {".NumberOfPoints_nu_u32", ""}},
                                         {"point_", {".Points_as", ""}},
                                         {"xpos", {".Xposition_m_f32", ""}},
                                         {"ypos", {".Yposition_m_f32", ""}},
                                         {"zpos", {".Zposition_m_f32", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result.root_url) << "Root url is different than expected value";
  for (const auto [key, result] : expected_result.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(PointCloudPluginTest, regular_expression_poinstArray_M7) {
  const std::unordered_set<std::string> test_urls = {"M7.TEST_DATA.TestPointList.numberOfPoints",
                                                     "M7.TEST_DATA.TestPointList.points[0].xPosition_m",
                                                     "M7.TEST_DATA.TestPointList.points[0].yPosition_m"};

  next::plugins::PointCloud plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "M7.TEST_DATA.TestPointList";
  expected_result.data_type_to_result = {{"num_points", {".numberOfPoints", ""}},
                                         {"point_", {".points", ""}},
                                         {"xpos", {".xPosition_m", ""}},
                                         {"ypos", {".yPosition_m", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result.root_url) << "Root url is different than expected value";
  for (const auto [key, result] : expected_result.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(PointCloudPluginTest, regular_expression_poinstArray_GDR) {
  const std::unordered_set<std::string> test_urls = {"ADC5xx_Device.GDR_FC_DATA.GdrOutputFC.numPoints",
                                                     "ADC5xx_Device.GDR_FC_DATA.GdrOutputFC.pointList[0].x_m",
                                                     "ADC5xx_Device.GDR_FC_DATA.GdrOutputFC.pointList[0].y_m",
                                                     "ADC5xx_Device.GDR_FC_DATA.GdrOutputFC.pointList[0].z_m"};

  next::plugins::PointCloud plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "ADC5xx_Device.GDR_FC_DATA.GdrOutputFC";
  expected_result.data_type_to_result = {{"num_points", {".numPoints", ""}},
                                         {"point_", {".pointList", ""}},
                                         {"xpos", {".x_m", ""}},
                                         {"ypos", {".y_m", ""}},
                                         {"zpos", {".z_m", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result.root_url) << "Root url is different than expected value";
  for (const auto [key, result] : expected_result.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(PointCloudArrayTest, SphericalCoordinates) {
  auto publisher = std::make_unique<udex::publisher::DataPublisher>("SphericalCoordinates");

  ASSERT_TRUE(publisher->SetDataSourceInfo("SphericalCoordinates", 22, 33)) << "Failed to set data source info.";
  ASSERT_TRUE(publisher->RegisterDataDescription("SRR630_RR.sdl", true)) << "Failed to register data description.";

  auto test_cloud = plugins::Cloud(udex::extractor::emptyRequestBasic);
  test_cloud.urls_ = {
      {"radius_", ".a_RangeRad"},       // RSP_ClusterListMeas.ClusterListArrays.a_RangeRad
      {"theta_", ".a_AzAng"},           // RSP_ClusterListMeas.ClusterListArrays.a_AzAng
      {"phi_", ".a_ElevAng"},           // RSP_ClusterListMeas.ClusterListArrays.a_ElevAng
      {"arrays", ".ClusterListArrays"}, // RSP_ClusterListMeas.ClusterListArrays
  };
  test_cloud.SetUrls("SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas");

  size_t expected_num_points = 1056;
  EXPECT_EQ(test_cloud.points_as_array_->num_points_, expected_num_points);
  EXPECT_EQ(test_cloud.points_as_array_->radius_->ArraySize(), expected_num_points);
  EXPECT_EQ(test_cloud.points_as_array_->theta_->ArraySize(), expected_num_points);
  EXPECT_EQ(test_cloud.points_as_array_->phi_->ArraySize(), expected_num_points);

  SetSphericalCoordinates(5.0f, testing::pi / 4, testing::pi / 6, 0);
  test_cloud.SetAddress(GetDummBlock(), GetDummBlockSize());

  float expected_x = 1.76776755f;
  float expected_y = 1.76777387f;
  float expected_z = 4.330f;
  EXPECT_NEAR(test_cloud.GetPosX(0), expected_x, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosY(0), expected_y, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosZ(0), expected_z, testing::tolerance);

  SetSphericalCoordinates(3.0f, testing::pi / 6, testing::pi / 4, 1);
  test_cloud.SetAddress(GetDummBlock(), GetDummBlockSize());

  expected_x = 1.83712f;
  expected_y = 1.06066f;
  expected_z = 2.12132f;
  EXPECT_NEAR(test_cloud.GetPosX(1), expected_x, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosY(1), expected_y, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosZ(1), expected_z, testing::tolerance);

  SetSphericalCoordinates(3.0f, (2 * testing::pi) / 3, testing::pi / 4, 2);
  test_cloud.SetAddress(GetDummBlock(), GetDummBlockSize());

  expected_x = -1.06066f;
  expected_y = 1.83712f;
  expected_z = 2.12132f;
  EXPECT_NEAR(test_cloud.GetPosX(2), expected_x, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosY(2), expected_y, testing::tolerance);
  EXPECT_NEAR(test_cloud.GetPosZ(2), expected_z, testing::tolerance);
}

TEST(PointCloudRegexTest, MatchesValidPattern) {
  std::regex pattern{R"(.*\.a_RangeRad\[\d+\]$)"};
  std::string input{"SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_RangeRad[0]"};
  EXPECT_TRUE(std::regex_match(input, pattern));

  pattern = R"(.*\.a_AzAng\[\d+\]$)";
  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_AzAng[0]";
  EXPECT_TRUE(std::regex_match(input, pattern));

  pattern = R"(.*\.a_ElevAng\[\d+\]$)";
  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_ElevAng[0]";
  EXPECT_TRUE(std::regex_match(input, pattern));
}

TEST(PointCloudRegexTest, DoesNotMatchInvalidPattern) {
  std::regex pattern{R"(.*\.a_RangeRad\[\d+\]$)"};
  std::string input{"SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.xyz[0]"};
  EXPECT_FALSE(std::regex_match(input, pattern));

  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_RangeRad";
  EXPECT_FALSE(std::regex_match(input, pattern));

  pattern = R"(.*\.a_AzAng\[\d+\]$)";
  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.xyz[0]";
  EXPECT_FALSE(std::regex_match(input, pattern));

  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_AzAng";
  EXPECT_FALSE(std::regex_match(input, pattern));

  pattern = R"(.*\.a_ElevAng\[\d+\]$)";
  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.xyz[0]";
  EXPECT_FALSE(std::regex_match(input, pattern));

  input = "SphericalCoordinates.RSP_Cycle.RSP_ClusterListMeas.ClusterListArrays.a_ElevAng";
  EXPECT_FALSE(std::regex_match(input, pattern));
}

} // namespace next
