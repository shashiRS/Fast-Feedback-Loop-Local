/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     canTpObjects_test.cpp
 *  @brief    Testing the view3D_CanTpObjects plugin
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/udex/publisher/data_publisher.hpp>

#include "canTpObjects.h"
#include "canTpObjects_test.hpp"

namespace next {
TYPED_TEST_CASE(PluginTest, next::plugins::CanTpObjects);
TEST_F(CanTpObjectsPluginTest, TestRegexTPList) {
  const std::unordered_set<std::string> test_urls = {
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fDistX_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fDistY_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.eRefPointPos_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fOrientationStd_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fWidth_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.flength_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.eClassification_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fVrelX_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.fVrelY_OBJ_00",
      "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01.uiID_OBJ_00",
  };
  plugins::CanTpObjectsPlugin plugin("TestPlugin");
  plugin.init();

  SearchResult expected_result;
  expected_result.root_url = "Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01";
  expected_result.data_type_to_result = {{"ref_tmp_", {".eRefPointPos_OBJ", ""}},
                                         {"pos_x_", {".fDistX_OBJ", ""}},
                                         {"pos_y_", {".fDistY_OBJ", ""}},
                                         {"yaw_", {".fOrientationStd_OBJ", ""}},
                                         {"width_", {".fWidth_OBJ", ""}},
                                         {"classification_ars_", {".eClassification_OBJ", ""}},
                                         {"vel_x_", {".fVrelX_OBJ", ""}},
                                         {"vel_y_", {".fVrelY_OBJ", ""}},
                                         {"id_", {".uiID_OBJ", ""}},
                                         {"length_", {".flength_OBJ", ""}}};

  auto results = plugin.GetSearchResults("", test_urls);
  ASSERT_EQ(results.size(), 1) << "Result size is different than expected!";

  EXPECT_EQ(results[0].root_url, expected_result.root_url) << "Root url is different than expected value";
  for (const auto [key, result] : expected_result.data_type_to_result) {
    EXPECT_EQ(results[0].data_type_to_result[key].result_url, result.result_url);
  }
}

TEST_F(CanTpObjectsPluginTest, SetUpUrls) {
  auto publisher = std::make_unique<udex::publisher::DataPublisher>("CanTpObject");

  ASSERT_TRUE(publisher->SetDataSourceInfo("Local_ARS_CANFD", 22, 33)) << "Failed to set data source info.";
  ASSERT_TRUE(publisher->RegisterDataDescription("ARS510HA22_Private.dbc", true, DescriptionType::DESCRIPTION_TYPE_DBC))
      << "Failed to register data description.";

  auto test_cantp = plugins::CanTpObjects(udex::extractor::emptyRequestBasic);
  test_cantp.urls_ = {{"xpos_", ".fDistX_OBJ"},          {"ypos_", ".fDistY_OBJ"},
                      {"yaw_", ".fOrientationStd_OBJ"},  {"width_", ".fWidth_OBJ"},
                      {"length_", ".flength_OBJ"},       {"classification_ars_", ".eClassification_OBJ"},
                      {"vel_x_", ".fVrelX_OBJ"},         {"vel_y_", ".fVrelY_OBJ"},
                      {"ref_tmp_", ".eRefPointPos_OBJ"}, {"id_", ".uiID_OBJ"}};
  test_cantp.SetUrls("Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_00_01");
  test_cantp.SetUrls("Local_ARS_CANFD.ARS510HA22_Private.ARS_OBJ_02_03");

  auto object_list = test_cantp.GetCanTpOjects();
  EXPECT_EQ(object_list.size(), 2);
}

} // namespace next
