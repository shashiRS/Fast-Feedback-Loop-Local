/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     structTpObjects_test.cpp
 *  @brief    Testing the view3D_StructTpObjects plugin
 */

#include <cip_test_support/gtest_reporting.h>

#include <plugin_regex_searcher.hpp>
#include "structTpObjects.h"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;
using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

const std::string MOCK_URL("ADC4xx.FDP_CSB.m_tpfOutput");

TYPED_TEST_CASE(PluginTest, next::plugins::StructTpObjects);

//! Testing plugin creation
/*!
 * This test is demonstrating how to create a instance of a plugin. Afterwards the name is set and checked.
 */
TEST(PluginTest, PluginCreation) {
  next::plugins::StructTpObjects plugin("notExisting");

  const std::string plugin_name("MockPluginName");
  plugin.setName(plugin_name);
  EXPECT_EQ(plugin.getName(), plugin_name);
}

//! Testing plugin description
/*!
 * This is adding and updating configs
 */
TEST(PluginTest, GetDescription) {
  next::plugins::StructTpObjects plugin("notExisting");

  next::bridgesdk::plugin_config_t config;
  EXPECT_TRUE(plugin.getDescription(config)) << "calling FdpCsbTpObjects::getDescription(config) failed";
  EXPECT_TRUE(config.end() != config.find("url")) << "no url was in config";
}

//! Testing plugin addConfig
/*!
 * This is adding configs
 */
// Signal extractor needs either to be mocked or SDL file needs to be published so that AddConfig is not returning false
// will be adressed with test concept for plugins: SIMEN-6887
TYPED_TEST(PluginTest, DISABLED_AddingConfig) {
  std::vector<std::string> mock_topics = {MOCK_URL};
  static_cast<next::test::SignalExplorerMock *>(this->signal_explorer_ptr_.get())->setTopicByUrl(mock_topics);
  next::bridgesdk::plugin_config_t config;

  config.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE, MOCK_URL}});
  config.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Tpf 21p Traffic Participants"}});
  config.insert({"datacache", {next::bridgesdk::ConfigType::FIXED, "continuos"}});
  config.insert({"color", {next::bridgesdk::ConfigType::EDITABLE, "#00ffff"}});
  config.insert({"DATA_VIEW", {next::bridgesdk::ConfigType::EDITABLE, "objectDatasd"}});
  config.insert({"type", {next::bridgesdk::ConfigType::SELECTABLE, "BASE"}});

  EXPECT_TRUE(this->plugin_->addConfig(config));
  EXPECT_EQ(this->plugin_->numOfActiveConfig(), 1);

  // updating existing config
  config.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "test_plugin_test"}});
  EXPECT_TRUE(this->plugin_->addConfig(config));
  EXPECT_EQ(this->plugin_->numOfActiveConfig(), 2);

  // removing config
  EXPECT_TRUE(this->plugin_->removeConfig(config));
  EXPECT_EQ(this->plugin_->numOfActiveConfig(), 1);
}

TEST(PluginTest, TestRegexTPList) {
  const std::unordered_set<std::string> input_test_data = {
      // ADC4xx.FDP_Base.p_TpfOutputIf.aObject[x]
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistXStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistYStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].classification.eClassification",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYaw",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYawRate",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].general.fLifeTime",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.HeaderObjList.iNumOfUsedObjects",

      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistXStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistYStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].classification.eClassification",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYaw",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYawRate",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].general.fLifeTime",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosY",

      // ADC4xx.FDP_CSB.m_tpfOutput.aObject[x]
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistXStd",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistYStd",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsY", "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].classification",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].addKinematic.fYaw",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].addKinematic.fYawStd",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].general.fLifeTime",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[0].fPosX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[0].fPosY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[1].fPosX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[1].fPosY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[2].fPosX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[2].fPosY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[3].fPosX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].shapePoints.aShapePointCoordinates[3].fPosY",
      "ADC4xx.FDP_CSB.m_tpfOutput.HeaderObjList.iNumOfUsedObjects",

      // SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[x]
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].distX",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].distY",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].age",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].absVelX",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].absVelY",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].heading",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].length",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.obj[0].width",
      "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas.numOfObjects",

      // Aurora LowLevelFusion
      // HDF5_Device.CEM200_TPO.m_llfTpObjectList
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objListHeader.numOfUsedObjects",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].objectDynamics.position.posX",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].objectDynamics.position.posY",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].objectDynamics.dynamics.VelX",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].objectDynamics.dynamics.VelY",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].objectDynamics.orientation.yaw",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].classification",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].geometry.length",
      "HDF5_Device.CEM200_TPO.m_llfTpObjectList.objects[0].geometry.width",

      // HDF5_Device.CEM200_TPF2.m_tpObjectList
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objListHeader.numOfUsedObjects",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].objectDynamics.position.posX",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].objectDynamics.position.posY",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].objectDynamics.dynamics.VelX",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].objectDynamics.dynamics.VelY",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].objectDynamics.orientation.yaw",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].classification",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].geometry.length",
      "HDF5_Device.CEM200_TPF2.m_tpObjectList.objects[0].geometry.width",

      // CEM_EM_DATA.AUPDF_DynamicObjects
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.numberOfObjects",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.referencePoint.x",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.referencePoint.y",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].lifetime",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].velocity.x",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].velocity.y",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].yawRate",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[0].position.x",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[0].position.y",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[1].position.x",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[1].position.y",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[2].position.x",
      "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects.objects[0].shape.points[2].position.y",

      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.numberOfObjects",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].objectSize.length",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].objectSize.width",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].objectYaw",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].centerPoint_x",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].centerPoint_y",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].centerPoint_z",
      "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput.objects[0].classType"};

  const uint8_t expected_keys_to_find = 7;
  next::plugins::StructTpObjects plugin("test_plugin");
  plugin.init();

  next::bridgesdk::plugin_addons::PluginRegexSearcher searcher;

  searcher.InitializeSearchRequests(plugin.search_requests_);

  std::vector<SearchResult> search_results = searcher.GetSearchResults(input_test_data);
  EXPECT_TRUE(search_results.size() > 0) << "Unable to find traffic participant lists";
  EXPECT_EQ(search_results.size(), expected_keys_to_find);

  // test if list url root was found
  const std::string fdp_csb_root = "ADC4xx.FDP_CSB.m_tpfOutput";
  const std::string fdp_base_root = "ADC4xx.FDP_Base.p_TpfOutputIf";
  const std::string mos_objlist_root = "SRR630_RL.MOS_Cycle.MOS_ShortObjectListMeas";
  const std::string llf_tplist_root = "HDF5_Device.CEM200_TPO.m_llfTpObjectList";
  const std::string llf2_tplist_root = "HDF5_Device.CEM200_TPF2.m_tpObjectList";
  const std::string lsm_tplist_root = "ADC5xx_Device.CEM_EM_DATA.AUPDF_DynamicObjects";
  const std::string rum_tpplist_root = "MTA_ADC5.RUM2TPP_FC_DATA.pRum2ObjectDetection3DOutput";

  auto countRootUrlInSearchResults = [&search_results](const std::string &root_url) {
    size_t counter = 0;
    for (const auto &search_result : search_results) {
      if (search_result.root_url == root_url)
        counter++;
    }
    return counter;
  };

  EXPECT_TRUE(countRootUrlInSearchResults(fdp_csb_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(fdp_csb_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(fdp_base_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(mos_objlist_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(llf_tplist_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(llf2_tplist_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(lsm_tplist_root) == 1);
  EXPECT_TRUE(countRootUrlInSearchResults(rum_tpplist_root) == 1);

  auto getSearchResultByUrl = [&search_results](const std::string &url) {
    for (const auto &search_result : search_results) {
      if (search_result.root_url == url)
        return search_result;
    }
    return SearchResult{};
  };

  // test if num of objects url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["num_objects_"].result_url ==
              ".HeaderObjList.iNumOfUsedObjects");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["num_objects_"].result_url ==
              ".HeaderObjList.iNumOfUsedObjects");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["num_objects_"].result_url == ".numOfObjects");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["num_objects_"].result_url ==
              ".objListHeader.numOfUsedObjects");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["num_objects_"].result_url ==
              ".objListHeader.numOfUsedObjects");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["num_objects_"].result_url ==
              ".numberOfObjects");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["num_objects_"].result_url ==
              ".numberOfObjects");

  // test if array url was found
  const std::string fdp_csb_array = ".aObject";
  const std::string fdp_base_array = ".aObject";
  const std::string mos_objlist_array = ".obj";
  const std::string llf_objlist_array = ".objects";
  const std::string llf2_objlist_array = ".objects";
  const std::string cem_objlist_array = ".objects";
  const std::string rum_tpplist_array = ".objects";

  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["aObject_"].result_url == fdp_csb_array);
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["aObject_"].result_url == fdp_base_array);
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["aObject_"].result_url == mos_objlist_array);
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["aObject_"].result_url == llf_objlist_array);
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["aObject_"].result_url == llf2_objlist_array);
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["aObject_"].result_url == cem_objlist_array);
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["aObject_"].result_url == rum_tpplist_array);

  // test if position x url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["pos_x_"].result_url == ".kinematic.fDistX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["pos_x_"].result_url == ".kinematic.fDistX");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["pos_x_"].result_url == ".distX");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["pos_x_"].result_url ==
              ".objectDynamics.position.posX");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["pos_x_"].result_url ==
              ".objectDynamics.position.posX");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["pos_x_"].result_url ==
              ".shape.referencePoint.x");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["pos_x_"].result_url == ".centerPoint_x");

  // test if position y url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["pos_y_"].result_url == ".kinematic.fDistY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["pos_y_"].result_url == ".kinematic.fDistY");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["pos_y_"].result_url == ".distY");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["pos_y_"].result_url ==
              ".objectDynamics.position.posY");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["pos_y_"].result_url ==
              ".objectDynamics.position.posY");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["pos_y_"].result_url ==
              ".shape.referencePoint.y");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["pos_y_"].result_url == ".centerPoint_y");

  // test if lifetime url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["life_time_"].result_url == ".general.fLifeTime");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["life_time_"].result_url == ".general.fLifeTime");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["life_time_"].result_url == ".age");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["life_time_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["life_time_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["life_time_"].result_url == ".lifetime");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["life_time_"].result_url == "");

  // test if absolute velocity X url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["vel_x_"].result_url == ".kinematic.fVabsX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["vel_x_"].result_url == ".kinematic.fVabsX");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["vel_x_"].result_url == ".absVelX");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["vel_x_"].result_url ==
              ".objectDynamics.dynamics.VelX");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["vel_x_"].result_url ==
              ".objectDynamics.dynamics.VelX");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["vel_x_"].result_url == ".velocity.x");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["vel_x_"].result_url == "");

  // test if absolute velocity Y url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["vel_y_"].result_url == ".kinematic.fVabsY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["vel_y_"].result_url == ".kinematic.fVabsY");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["vel_y_"].result_url == ".absVelY");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["vel_y_"].result_url ==
              ".objectDynamics.dynamics.VelY");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["vel_y_"].result_url ==
              ".objectDynamics.dynamics.VelY");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["vel_y_"].result_url == ".velocity.y");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["vel_y_"].result_url == "");

  // test if object class url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["classification_"].result_url ==
              ".classification");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["classification_"].result_url ==
              ".classification.eClassification");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["classification_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["classification_"].result_url ==
              ".classification");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["classification_"].result_url ==
              ".classification");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["classification_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["classification_"].result_url == ".classType");

  // test if object yaw url was found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["yaw_"].result_url == ".addKinematic.fYaw");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["yaw_"].result_url == ".addKinematic.fYaw");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["yaw_"].result_url == ".heading");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["yaw_"].result_url ==
              ".objectDynamics.orientation.yaw");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["yaw_"].result_url ==
              ".objectDynamics.orientation.yaw");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["yaw_"].result_url == ".yawRate");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["yaw_"].result_url == ".objectYaw");

  // test if object shapepoints were found
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[0].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[0].fPosY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[1].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_1_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[1].fPosY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_2_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[2].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_csb_root).data_type_to_result["shapepoint_2_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[2].fPosY");

  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[0].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[0].fPosY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[1].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_1_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[1].fPosY");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_2_pos_x_"].result_url ==
              ".shapePoints.aShapePointCoordinates[2].fPosX");
  EXPECT_TRUE(getSearchResultByUrl(fdp_base_root).data_type_to_result["shapepoint_2_pos_y_"].result_url ==
              ".shapePoints.aShapePointCoordinates[2].fPosY");

  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".length");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_0_pos_y_"].result_url == ".width");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".length");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_1_pos_y_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_2_pos_x_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(mos_objlist_root).data_type_to_result["shapepoint_2_pos_y_"].result_url == "");

  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".geometry.length");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".geometry.width");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".geometry.length");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_1_pos_y_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_2_pos_x_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf_tplist_root).data_type_to_result["shapepoint_2_pos_y_"].result_url == "");

  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".geometry.length");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".geometry.width");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".geometry.length");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_1_pos_y_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_2_pos_x_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(llf2_tplist_root).data_type_to_result["shapepoint_2_pos_y_"].result_url == "");

  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".shape.points[0].position.x");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".shape.points[0].position.y");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".shape.points[1].position.x");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_1_pos_y_"].result_url ==
              ".shape.points[1].position.y");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_2_pos_x_"].result_url ==
              ".shape.points[2].position.x");
  EXPECT_TRUE(getSearchResultByUrl(lsm_tplist_root).data_type_to_result["shapepoint_2_pos_y_"].result_url ==
              ".shape.points[2].position.y");

  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_0_pos_x_"].result_url ==
              ".objectSize.length");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_0_pos_y_"].result_url ==
              ".objectSize.width");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_1_pos_x_"].result_url ==
              ".objectSize.length");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_1_pos_y_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_2_pos_x_"].result_url == "");
  EXPECT_TRUE(getSearchResultByUrl(rum_tpplist_root).data_type_to_result["shapepoint_2_pos_y_"].result_url == "");
}
