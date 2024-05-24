/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_regex_searcher_test.cpp
 *  @brief    Testing the Regex Searcher for plugins
 */

#include "plugin_regex_searcher.hpp"

#include <cip_test_support/gtest_reporting.h>
#include "unittest_plugin_helper.hpp"

#include "boost/regex.hpp"
#include "regex_helper.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {
class RegexSearcherTester : public next::bridgesdk::plugin_addons::PluginRegexSearcher {
public:
  std::vector<boost::regex> GetRegexFromStrings(const std::vector<std::string> &regular_expressions) {
    return regex_helper_->GetRegexFromStrings(regular_expressions);
  };
  bool GetMatchFromURL(const std::string &url, const std::vector<boost::regex> &regular_expressions,
                       std::string &result, std::string &matched_pattern) {
    return regex_helper_->GetMatchFromURL(url, regular_expressions, result, matched_pattern);
  };

  std::string ExtractGroupFromURL(const std::string &url) const {
    return next::bridgesdk::plugin_addons::PluginRegexSearcher::ExtractGroupFromURL(url);
  };

  inline std::string addLeadingDot(const std::string &str) const {
    return next::bridgesdk::plugin_addons::PluginRegexSearcher::addLeadingDot(str);
  };

  bool CheckMandatoryDataFields(SearchResult &search_result) {
    return next::bridgesdk::plugin_addons::PluginRegexSearcher::CheckMandatoryDataFields(search_result);
  };

  void ReduceArrayToOneEntry(std::unordered_set<std::string> &url_list) const {
    return next::bridgesdk::plugin_addons::PluginRegexSearcher::regex_helper_->ReduceArrayToOneEntry(url_list);
  };
};
} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

class RegexSearcherFixture : public testing::Test {
public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;
  using Result = next::bridgesdk::plugin_addons::PluginRegexSearcher::Result;

  next::bridgesdk::plugin_addons::PluginRegexSearcher *searcher;
  std::unordered_set<std::string> test_data_struct_tp;
  std::unordered_set<std::string> test_data_point_cloud;
  std::vector<SearchRequest> struct_tp_search_requests;
  std::vector<SearchRequest> point_cloud_search_requests;
  inline bool findStr(const std::vector<std::string> &vec, const std::string &str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
  };

  void SetUp() {
    searcher = new next::bridgesdk::plugin_addons::PluginRegexSearcher();

    test_data_struct_tp = {// ADC4xx.FDP_Base.p_TpfOutputIf.aObject[x]
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[1].kinematic.fDistX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[1].shapePoints.aShapePointCoordinates[0].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[1].shapePoints.aShapePointCoordinates[1].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[1].shapePoints.aShapePointCoordinates[2].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[1].shapePoints.aShapePointCoordinates[3].fPosX",

                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[10].kinematic.fDistX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[10].shapePoints.aShapePointCoordinates[0].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[10].shapePoints.aShapePointCoordinates[1].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[10].shapePoints.aShapePointCoordinates[2].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[10].shapePoints.aShapePointCoordinates[3].fPosX",

                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[123].kinematic.fDistX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[123].shapePoints.aShapePointCoordinates[0].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[123].shapePoints.aShapePointCoordinates[1].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[123].shapePoints.aShapePointCoordinates[2].fPosX",
                           "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[123].shapePoints.aShapePointCoordinates[3].fPosX",

                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistX",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistY",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistXStd",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistYStd",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsX",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsY",
                           "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].classification",
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
                           "ADC4xx.FDP_Base.p_TpfOutputIf.HeaderObjList.iNumOfUsedObjects"};

    struct_tp_search_requests = {
        {"num_objects_", {R"(([objlist]*?header.*?num.*objects?|num.*objects?)$)"}, false},
        {"aObject_", {R"(\.((aObject|obj|objects)))"}, false},
        {"pos_x_", {R"(((kinematic\.)?[f.]+distx*)$)"}, false},
        {"pos_y_", {R"(((kinematic\.)?[f.]+disty*)$)"}, false},
        {"life_time_", {R"(([general]*[\.][f]?LifeTime|age)$)"}, false},
        {"vel_x_", {R"(((\.kinematic\.)fVabsX|absVelX))"}, false},
        {"vel_y_", {R"(((\.kinematic\.)fVabsY|absVelY))"}, false},
        {"yaw_", {R"(\.(kinematic\.fYaw|addKinematic\.fYaw|heading|orientation|objectDynamics\..*\.yaw))"}, false},
        {"classification_", {R"(\.([classification]*\.*[ec]*lassification)$)"}, false},
        {"shapepoint_0_pos_x_", {R"(\.(shapePoints\..+\[0\]\.fPosX|length|geometry\.length)$)"}, false},
        {"shapepoint_0_pos_y_", {R"(\.(shapePoints\..+\[0\]\.fPosY|width|geometry\.width)$)"}, false},
        {"shapepoint_1_pos_x_", {R"(\.(shapePoints\..+\[1\]\.fPosX|length|geometry\.length)$)"}, false},
        {"shapepoint_1_pos_y_", {R"(\.(shapePoints\..+\[1\]\.fPosY)$)"}, false},
        {"shapepoint_2_pos_x_", {R"(\.(shapePoints\..+\[2\]\.fPosX)$)"}, false},
        {"shapepoint_2_pos_y_", {R"(\.(shapePoints\..+\[2\]\.fPosY)$)"}, false}};

    point_cloud_search_requests = {
        {"num_points", {R"((num.*point.*))"}, false},
        {"point_", {R"(\.([^.]+)\[\d+\])"}, false},
        {"xpos", {R"((\.[x]?position.*(?:_m|[x]|_m_f32)$))", R"((x_m$))"}, false},
        {"ypos", {R"((\.[y]?position.*(?:_m|[y]|_m_f32)$))", R"((y_m$))"}, false},
        {"zpos", {R"((\.[z]?position.*(?:_m|[z]|_m_f32)$))", R"((z_m$))"}, true},
    };

    test_data_point_cloud = {
        // ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput
        "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.numberOfPoints",
        "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points",
        "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Xposition_m",
        "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Yposition_m",
        "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Zposition_m",

        // parking_us_point_list_CarPC
        "DummyDevice.DummyView.point_list_CarPC.NumberOfPoints_nu_u32",
        "DummyDevice.DummyView.point_list_CarPC.Points_as",
        "DummyDevice.DummyView.point_list_CarPC.Points_as[0].Xposition_m_f32",
        "DummyDevice.DummyView.point_list_CarPC.Points_as[0].Yposition_m_f32",
        "DummyDevice.DummyView.point_list_CarPC.Points_as[0].Zposition_m_f32",

        // parking_us_point_list_M7
        "DummyDevice.DummyView.point_list_M7.numberOfPoints",
        "DummyDevice.DummyView.point_list_M7.points",
        "DummyDevice.DummyView.point_list_M7.points[0].xPosition_m",
        "DummyDevice.DummyView.point_list_M7.points[0].yPosition_m",

        // parking_sfm_list
        "DummyDevice.DummyView.sfm_list.numberOfPoints",
        "DummyDevice.DummyView.sfm_list.pointList",
        "DummyDevice.DummyView.sfm_list.pointList[0].x_m",
        "DummyDevice.DummyView.sfm_list.pointList[0].y_m",
        "DummyDevice.DummyView.sfm_list.pointList[0].z_m",

        // CEM_UssOutput
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.numberOfPoints",
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].relativeVelocityX",
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].standardDeviationVelX",
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].positionX",
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].pointHeader.measurementStatus",
        "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.ussPointList.ussPointsArray[0].positionY",
    };
  };

  void TearDown() { delete searcher; };
};

TEST_F(RegexSearcherFixture, testArrayReductionToOneEntry) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;
  const std::unordered_set<std::string> expected_output_after_reduction = {
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistXStd",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistYStd",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsX",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fVabsY",
      "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].classification",
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
      "ADC4xx.FDP_Base.p_TpfOutputIf.HeaderObjList.iNumOfUsedObjects"};

  searcher_tester.ReduceArrayToOneEntry(test_data_struct_tp);
  EXPECT_EQ(test_data_struct_tp.size(), expected_output_after_reduction.size())
      << "Array reduction to one entry went wrong";

  for (const auto &url : expected_output_after_reduction) {
    EXPECT_TRUE(test_data_struct_tp.count(url) == 1) << "Expected output url not found: {0}" << url;
  }
}

TEST_F(RegexSearcherFixture, testInitOfUrlsToRegexMap) {

  EXPECT_NO_THROW(searcher->InitializeSearchRequests(struct_tp_search_requests));
}

TEST_F(RegexSearcherFixture, testFindAvailableRootUrls) {
  const unsigned int expected_urls_to_find = 2;
  const std::string tp_base_url = "ADC4xx.FDP_Base.p_TpfOutputIf";
  const std::string tp_csb_url = "ADC4xx.FDP_CSB.m_tpfOutput";

  EXPECT_NO_THROW(searcher->InitializeSearchRequests(struct_tp_search_requests));
  const std::vector<std::string> output = searcher->FindAvailableRootUrls(test_data_struct_tp);

  EXPECT_EQ(output.size(), expected_urls_to_find);
  EXPECT_TRUE(findStr(output, tp_base_url)) << "URL not found in output: " << tp_base_url;
  EXPECT_TRUE(findStr(output, tp_csb_url)) << "URL not found in output: " << tp_csb_url;
}

TEST_F(RegexSearcherFixture, testNamingMapExtraction) {
  SearchResult expected_naming_map_tp_csb = {
      {"ADC4xx.FDP_Base.p_TpfOutputIf"},
      {{"num_objects_", {".HeaderObjList.iNumOfUsedObjects", "regexp"}},
       {"aObject_", {".aObject", "regxp"}},
       {"pos_x_", {".kinematic.fDistX", "regxp"}},
       {"pos_y_", {".kinematic.fDistY", "regexp"}},
       {"life_time_", {".general.fLifeTime", "regexp"}},
       {"vel_x_", {".kinematic.fVabsX", "regexp"}},
       {"vel_y_", {".kinematic.fVabsY", "regexp"}},
       {"yaw_", {".addKinematic.fYaw", "regexp"}},
       {"classification_", {".classification", "regexp"}},
       {"shapepoint_0_pos_x_", {".shapePoints.aShapePointCoordinates[0].fPosX", "regexp"}},
       {"shapepoint_0_pos_y_", {".shapePoints.aShapePointCoordinates[0].fPosY", "regexp"}},
       {"shapepoint_1_pos_x_", {".shapePoints.aShapePointCoordinates[1].fPosX", "regexp"}},
       {"shapepoint_1_pos_y_", {".shapePoints.aShapePointCoordinates[1].fPosY", "regexp"}},
       {"shapepoint_2_pos_x_", {".shapePoints.aShapePointCoordinates[2].fPosX", "regexp"}},
       {"shapepoint_2_pos_y_", {".shapePoints.aShapePointCoordinates[2].fPosY", "regexp"}}}};

  const std::unordered_set<std::string> csb_tp_urls{
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistXStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistYStd",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fVabsY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].classification",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYaw",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].addKinematic.fYawRate",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].general.fLifeTime",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[0].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[1].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].shapePoints.aShapePointCoordinates[2].fPosY",
      "ADC4xx.FDP_Base.p_TpfOutputIf.HeaderObjList.iNumOfUsedObjects"};

  EXPECT_NO_THROW(searcher->InitializeSearchRequests(struct_tp_search_requests));

  const std::string root_url = "ADC4xx.FDP_Base.p_TpfOutputIf";

  SearchResult output = searcher->GetSearchResultForUrl(root_url, csb_tp_urls);
  EXPECT_EQ(output.root_url, expected_naming_map_tp_csb.root_url) << "Search result url not matched to expected value";
  EXPECT_EQ(output.data_type_to_result["num_objects_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["num_objects_"].result_url);

  EXPECT_EQ(output.data_type_to_result["aObject_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["aObject_"].result_url);

  EXPECT_EQ(output.data_type_to_result["pos_x_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["pos_x_"].result_url);

  EXPECT_EQ(output.data_type_to_result["pos_y_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["pos_y_"].result_url);

  EXPECT_EQ(output.data_type_to_result["life_time_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["life_time_"].result_url);

  EXPECT_EQ(output.data_type_to_result["vel_x_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["vel_x_"].result_url);

  EXPECT_EQ(output.data_type_to_result["vel_y_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["vel_y_"].result_url);

  EXPECT_EQ(output.data_type_to_result["yaw_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["yaw_"].result_url);

  EXPECT_EQ(output.data_type_to_result["classification_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["classification_"].result_url);

  EXPECT_EQ(output.data_type_to_result["shapepoint_0_pos_x_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["shapepoint_0_pos_x_"].result_url);

  EXPECT_EQ(output.data_type_to_result["shapepoint_0_pos_y_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["shapepoint_0_pos_y_"].result_url);

  EXPECT_EQ(output.data_type_to_result["shapepoint_1_pos_x_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["shapepoint_1_pos_x_"].result_url);

  EXPECT_EQ(output.data_type_to_result["shapepoint_1_pos_y_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["shapepoint_1_pos_y_"].result_url);

  EXPECT_EQ(output.data_type_to_result["shapepoint_2_pos_x_"].result_url,
            expected_naming_map_tp_csb.data_type_to_result["shapepoint_2_pos_x_"].result_url);
}

TEST_F(RegexSearcherFixture, testPointCloud) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;
  std::unordered_set<std::string> point_cloud_array_reduction_data = {
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Xposition_m",
      "DummyDevice.DummyView.point_list_CarPC.Points_as[0].Xposition_m_f32",
      "DummyDevice.DummyView.point_list_M7.points[0].xPosition_m",
      "DummyDevice.DummyView.sfm_list.pointList[0].x_m",

      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[40].Xposition_m",
      "DummyDevice.DummyView.point_list_CarPC.Points_as[30].Xposition_m_f32",
      "DummyDevice.DummyView.point_list_M7.points[2].xPosition_m",
      "DummyDevice.DummyView.sfm_list.pointList[1].x_m"};

  const std::unordered_set<std::string> expected_point_cloud_data_after_reduction = {
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput.points[0].Xposition_m",
      "DummyDevice.DummyView.point_list_CarPC.Points_as[0].Xposition_m_f32",
      "DummyDevice.DummyView.point_list_M7.points[0].xPosition_m", "DummyDevice.DummyView.sfm_list.pointList[0].x_m"};

  const size_t expected_size_after_reduction_urls = expected_point_cloud_data_after_reduction.size();
  const size_t expected_root_urls = 5;

  EXPECT_NO_THROW(searcher->InitializeSearchRequests(point_cloud_search_requests));

  searcher_tester.ReduceArrayToOneEntry(point_cloud_array_reduction_data);

  const std::vector<std::string> available_root_urls = searcher->FindAvailableRootUrls(test_data_point_cloud);

  // check array reduction
  EXPECT_EQ(point_cloud_array_reduction_data.size(), expected_size_after_reduction_urls)
      << "Array reduction to one entry went wrong";

  for (const auto &url : expected_point_cloud_data_after_reduction) {
    EXPECT_TRUE(point_cloud_array_reduction_data.count(url) == 1) << "Expected output url not found: {0}" << url;
  }

  // check available urls
  const std::string spu_root_url = "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput";
  const std::string M7_root_url = "DummyDevice.DummyView.point_list_M7";
  const std::string CarPC_root_url = "DummyDevice.DummyView.point_list_CarPC";
  const std::string sfm_root_url = "DummyDevice.DummyView.sfm_list";
  const std::string cem_root_url = "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput";
  EXPECT_EQ(available_root_urls.size(), expected_root_urls) << "Not enough available urls were found";
  EXPECT_TRUE(findStr(available_root_urls, spu_root_url)) << "URL not found in output: " << spu_root_url;
  EXPECT_TRUE(findStr(available_root_urls, M7_root_url)) << "URL not found in output: " << M7_root_url;
  EXPECT_TRUE(findStr(available_root_urls, CarPC_root_url)) << "URL not found in output: " << CarPC_root_url;
  EXPECT_TRUE(findStr(available_root_urls, sfm_root_url)) << "URL not found in output: " << sfm_root_url;
  EXPECT_TRUE(findStr(available_root_urls, cem_root_url)) << "URL not found in output: " << cem_root_url;

  // check naming maps
  SearchResult expected_point_list_ADC5 = {
      "ADC5xx_Device.USP_DATA.SpuUsProcessingPointListOutput",
      {{"num_points", Result{".numberOfPoints", R"((num.*point.*))"}},
       {"point_", Result{".points", R"(\.([^.]+)\[\d+\])"}},
       {"xpos", Result{".Xposition_m", R"((\.[x]?position.*(?:_m|[x]|_m_f32)$))"}},
       {"ypos", Result{".Yposition_m", R"((\.[y]?position.*(?:_m|[y]|_m_f32)$))"}},
       {"zpos", Result{".Zposition_m", R"((\.[z]?position.*(?:_m|[z]|_m_f32)$))"}}}};

  SearchResult expected_point_list_CarPC{
      "DummyDevice.DummyView.point_list_CarPC",
      {{"num_points", Result{".NumberOfPoints_nu_u32", R"(\.([^.]+)\[\d+\])"}},
       {"point_", Result{".Points_as", R"(\.([^.]+)\[\d+\])"}},
       {"xpos", Result{".Xposition_m_f32", R"((\.[x]?position.*(?:_m|[x]|_m_f32)$))"}},
       {"ypos", Result{".Yposition_m_f32", R"((\.[y]?position.*(?:_m|[y]|_m_f32)$))"}},
       {"zpos", Result{".Zposition_m_f32", R"((\.[z]?position.*(?:_m|[z]|_m_f32)$))"}}}};

  SearchResult expected_point_list_M7{"DummyDevice.DummyView.point_list_M7",
                                      {{"num_points", Result{".numberOfPoints", R"(\.([^.]+)\[\d+\])"}},
                                       {"point_", Result{".points", R"(\.([^.]+)\[\d+\])"}},
                                       {"xpos", Result{".xPosition_m", R"((\.[x]?position.*(?:_m|[x]|_m_f32)$))"}},
                                       {"ypos", Result{".yPosition_m", R"((\.[y]?position.*(?:_m|[y]|_m_f32)$))"}},
                                       {"zpos", Result{"", ""}}}};

  SearchResult expected_point_list_sfm{
      "DummyDevice.DummyView.sfm_list",
      {{"num_points", {".numberOfPoints", R"((num.*point.*))"}},
       {"point_", {".pointList", R"(\.([^.]+)\[\d+\])"}},
       {"xpos", {".x_m", R"((x_m$))"}},
       {"ypos", {".y_m", R"((y_m$))"}},
       {"zpos", {".z_m", R"((z_m$))"}}},
  };

  SearchResult expected_point_list_cem{"TEST",
                                       {{"num_points", {".numberOfPoints", ""}},
                                        {"point_", {".ussPointsArray", ""}},
                                        {"xpos", {".positionX", ""}},
                                        {"ypos", {".positionY", ""}},
                                        {"zpos", {"", ""}}}};

  auto output = searcher->GetSearchResults(test_data_point_cloud);

  auto findSearchResult = [](const std::string root_url, const std::vector<SearchResult> &vec) {
    for (const auto &result : vec) {
      if (result.root_url == root_url)
        return result;
    }

    return SearchResult();
  };

  // check result urls
  // ADC5
  EXPECT_EQ(findSearchResult(spu_root_url, output).data_type_to_result["num_points"].result_url,
            expected_point_list_ADC5.data_type_to_result["num_points"].result_url);

  EXPECT_EQ(findSearchResult(spu_root_url, output).data_type_to_result["point_"].result_url,
            expected_point_list_ADC5.data_type_to_result["point_"].result_url);

  EXPECT_EQ(findSearchResult(spu_root_url, output).data_type_to_result["xpos"].result_url,
            expected_point_list_ADC5.data_type_to_result["xpos"].result_url);

  EXPECT_EQ(findSearchResult(spu_root_url, output).data_type_to_result["ypos"].result_url,
            expected_point_list_ADC5.data_type_to_result["ypos"].result_url);

  EXPECT_EQ(findSearchResult(spu_root_url, output).data_type_to_result["zpos"].result_url,
            expected_point_list_ADC5.data_type_to_result["zpos"].result_url);

  // Car PC
  EXPECT_EQ(findSearchResult(CarPC_root_url, output).data_type_to_result["num_points"].result_url,
            expected_point_list_CarPC.data_type_to_result["num_points"].result_url);

  EXPECT_EQ(findSearchResult(CarPC_root_url, output).data_type_to_result["point_"].result_url,
            expected_point_list_CarPC.data_type_to_result["point_"].result_url);

  EXPECT_EQ(findSearchResult(CarPC_root_url, output).data_type_to_result["xpos"].result_url,
            expected_point_list_CarPC.data_type_to_result["xpos"].result_url);

  EXPECT_EQ(findSearchResult(CarPC_root_url, output).data_type_to_result["ypos"].result_url,
            expected_point_list_CarPC.data_type_to_result["ypos"].result_url);

  EXPECT_EQ(findSearchResult(CarPC_root_url, output).data_type_to_result["zpos"].result_url,
            expected_point_list_CarPC.data_type_to_result["zpos"].result_url);

  // M7
  EXPECT_EQ(findSearchResult(M7_root_url, output).data_type_to_result["num_points"].result_url,
            expected_point_list_M7.data_type_to_result["num_points"].result_url);

  EXPECT_EQ(findSearchResult(M7_root_url, output).data_type_to_result["point_"].result_url,
            expected_point_list_M7.data_type_to_result["point_"].result_url);

  EXPECT_EQ(findSearchResult(M7_root_url, output).data_type_to_result["xpos"].result_url,
            expected_point_list_M7.data_type_to_result["xpos"].result_url);

  EXPECT_EQ(findSearchResult(M7_root_url, output).data_type_to_result["ypos"].result_url,
            expected_point_list_M7.data_type_to_result["ypos"].result_url);

  EXPECT_EQ(findSearchResult(M7_root_url, output).data_type_to_result["zpos"].result_url,
            expected_point_list_M7.data_type_to_result["zpos"].result_url);

  // SFM
  EXPECT_EQ(findSearchResult(sfm_root_url, output).data_type_to_result["num_points"].result_url,
            expected_point_list_sfm.data_type_to_result["num_points"].result_url);

  EXPECT_EQ(findSearchResult(sfm_root_url, output).data_type_to_result["point_"].result_url,
            expected_point_list_sfm.data_type_to_result["point_"].result_url);

  EXPECT_EQ(findSearchResult(sfm_root_url, output).data_type_to_result["xpos"].result_url,
            expected_point_list_sfm.data_type_to_result["xpos"].result_url);

  EXPECT_EQ(findSearchResult(sfm_root_url, output).data_type_to_result["ypos"].result_url,
            expected_point_list_sfm.data_type_to_result["ypos"].result_url);

  EXPECT_EQ(findSearchResult(sfm_root_url, output).data_type_to_result["zpos"].result_url,
            expected_point_list_sfm.data_type_to_result["zpos"].result_url);

  // CEM
  EXPECT_EQ(findSearchResult(cem_root_url, output).data_type_to_result["num_points"].result_url,
            expected_point_list_cem.data_type_to_result["num_points"].result_url);

  EXPECT_EQ(findSearchResult(cem_root_url, output).data_type_to_result["point_"].result_url,
            expected_point_list_cem.data_type_to_result["point_"].result_url);

  EXPECT_EQ(findSearchResult(cem_root_url, output).data_type_to_result["xpos"].result_url,
            expected_point_list_cem.data_type_to_result["xpos"].result_url);

  EXPECT_EQ(findSearchResult(cem_root_url, output).data_type_to_result["ypos"].result_url,
            expected_point_list_cem.data_type_to_result["ypos"].result_url);

  EXPECT_EQ(findSearchResult(cem_root_url, output).data_type_to_result["zpos"].result_url,
            expected_point_list_cem.data_type_to_result["zpos"].result_url);
}

TEST_F(RegexSearcherFixture, testNotInitializedState) {
  std::vector<std::string> available_root_urls;
  SearchResult naming_map;

  available_root_urls = searcher->FindAvailableRootUrls(test_data_point_cloud);
  naming_map = searcher->GetSearchResultForUrl("", test_data_point_cloud);

  EXPECT_EQ(available_root_urls.size(), 0);
  EXPECT_EQ(naming_map.data_type_to_result.size(), 0);
}

TEST_F(RegexSearcherFixture, testAddLeadingDotPrivMethod) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;

  const std::string test_string = "DummyString";
  const std::string test_string_with_dot = ".DummyString";

  EXPECT_EQ(searcher_tester.addLeadingDot(test_string), test_string_with_dot) << "Dot '.' not added in front of string";
  EXPECT_EQ(searcher_tester.addLeadingDot(test_string_with_dot), test_string_with_dot)
      << "Dot '.' added in front of string but not expected";
}

TEST_F(RegexSearcherFixture, testExtractGroupFromUrlPrivMethod) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;

  EXPECT_EQ(searcher_tester.ExtractGroupFromURL("Device.View.Group.SubGroup.Array"), "Device.View.Group");
  EXPECT_EQ(searcher_tester.ExtractGroupFromURL("Device.View.Group"), "Device.View.Group");
  EXPECT_EQ(searcher_tester.ExtractGroupFromURL("Device.View"), "");
}

TEST_F(RegexSearcherFixture, testGetRegexFromStringsPrivMethod) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;

  // test for invalid regex pattern
  std::string invalid_regex_pattern = R"(([\d))";
  std::vector<boost::regex> result;

  result = searcher_tester.GetRegexFromStrings({invalid_regex_pattern});

  EXPECT_TRUE(result.empty()) << "GetRegexFromStrings returned non empty vector for invalid pattern";
}

TEST_F(RegexSearcherFixture, testGetMatchFromUrlPrivMethod) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;

  std::string test_string;
  std::string matched_pattern;
  std::string pattern;
  std::string res;

  // test index array
  pattern = R"(([\d]))";
  test_string = "Array[0]";

  EXPECT_TRUE(searcher_tester.GetMatchFromURL(test_string, searcher_tester.GetRegexFromStrings({pattern}), res,
                                              matched_pattern));
  EXPECT_EQ(res, "0");

  // test for x position url
  pattern = R"(((kinematic\.)?[f.]+distx*)$)";
  test_string = "ADC4xx.FDP_CSB.m_tpfOutput.aObject[0].kinematic.fDistX";

  EXPECT_TRUE(searcher_tester.GetMatchFromURL(test_string, searcher_tester.GetRegexFromStrings({pattern}), res,
                                              matched_pattern));
  EXPECT_EQ(res, "kinematic.fDistX");
}

TEST_F(RegexSearcherFixture, testCheckMandatoryDataFields) {
  std::vector<SearchRequest> search_requests = {{"pos_x_", {R"(((kinematic\.)?[f.]+distx*)$)"}, true},
                                                {"pos_y_", {R"(((kinematic\.)?[f.]+disty*)$)"}, false}};

  std::unordered_set<std::string> test_data = {
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinem.fX",
      "ADC4xx.FDP_Base.p_TpfOutputIf.aObject[0].kinematic.fDistY",
  };

  SearchResult expected_search_result_optional_member;
  expected_search_result_optional_member.root_url = "ADC4xx.FDP_Base.p_TpfOutputIf";
  expected_search_result_optional_member.data_type_to_result = {
      {"pos_x_", {"", ""}}, {"pos_y_", {".kinematic.fDistY", R"(((kinematic\.)?[f.]+disty*)$)"}}};

  // test for search request with optional member
  searcher->InitializeSearchRequests(search_requests);
  auto search_result = searcher->GetSearchResultForUrl("ADC4xx.FDP_Base.p_TpfOutputIf", test_data);

  bool isResultAsExpected = true;
  isResultAsExpected &= search_result.root_url == expected_search_result_optional_member.root_url;

  isResultAsExpected &= search_result.data_type_to_result["pos_x"].result_url ==
                        expected_search_result_optional_member.data_type_to_result["pos_x"].result_url;

  isResultAsExpected &= search_result.data_type_to_result["pos_y"].result_url ==
                        expected_search_result_optional_member.data_type_to_result["pos_y"].result_url;

  isResultAsExpected &= search_result.data_type_to_result["pos_x"].regex_pattern ==
                        expected_search_result_optional_member.data_type_to_result["pos_x"].regex_pattern;

  isResultAsExpected &= search_result.data_type_to_result["pos_y"].regex_pattern ==
                        expected_search_result_optional_member.data_type_to_result["pos_y"].regex_pattern;

  EXPECT_TRUE(isResultAsExpected) << "Searcher returned search result different than expected";

  // test for search request without optional member
  search_requests = {{"pos_x_", {R"(((kinematic\.)?[f.]+distx*)$)"}, false},
                     {"pos_y_", {R"(((kinematic\.)?[f.]+disty*)$)"}, false}};

  searcher->InitializeSearchRequests(search_requests);
  search_result = searcher->GetSearchResultForUrl("ADC4xx.FDP_Base.p_TpfOutputIf", test_data);

  SearchResult expected_search_result;

  isResultAsExpected = true;
  isResultAsExpected &= search_result.root_url == expected_search_result.root_url;
  isResultAsExpected &= search_result.data_type_to_result.size() == expected_search_result.data_type_to_result.size();
  EXPECT_TRUE(isResultAsExpected) << "Searcher returned search result without having all mandatory fields!";
}

TEST_F(RegexSearcherFixture, testCheckMandatoryDataFieldsMultipleResults) {
  next::bridgesdk::plugin_addons::RegexSearcherTester searcher_tester;

  std::vector<SearchRequest> search_requests = {{"pos_x_", {R"(((kinematic\.)?[f.]+distx*)$)"}, false},
                                                {"pos_y_", {R"(((kinematic\.)?[f.]+disty*)$)"}, false}};

  std::unordered_set<std::string> test_data = {
      "DummyDevice1.DummyView1.DummyGroup1.aObject[0].kinematic.fDistX",
      "DummyDevice1.DummyView1.DummyGroup1.aObject[0].kinematic.fDistY",
      "DummyDevice2.DummyView2.DummyGroup2.aObject[0].kinem///c.cxzwwx",
      "DummyDevice2.DummyView2.DummyGroup2.aObject[0].kinematic.ffff",

  };

  SearchResult expected_search_result;
  expected_search_result.root_url = "DummyDevice1.DummyView1.DummyGroup1";
  expected_search_result.data_type_to_result = {{"pos_x_", {".kinematic.fDistX", R"(((kinematic\.)?[f.]+distx*)$)"}},
                                                {"pos_y_", {".kinematic.fDistY", R"(((kinematic\.)?[f.]+disty*)$)"}}};

  // test for search request with optional member
  searcher->InitializeSearchRequests(search_requests);
  auto search_results = searcher->GetSearchResults(test_data);
  EXPECT_TRUE(search_results.size() == 1) << "{0} search results returned, expected 1" << search_results.size();

  bool isResultAsExpected = true;
  isResultAsExpected &= search_results[0].root_url == expected_search_result.root_url;
  // check matched results
  isResultAsExpected &= search_results[0].data_type_to_result["pos_x"].result_url ==
                        expected_search_result.data_type_to_result["pos_x"].result_url;

  isResultAsExpected &= search_results[0].data_type_to_result["pos_y"].result_url ==
                        expected_search_result.data_type_to_result["pos_y"].result_url;
  // check regexp
  isResultAsExpected &= search_results[0].data_type_to_result["pos_y"].regex_pattern ==
                        expected_search_result.data_type_to_result["pos_y"].regex_pattern;

  isResultAsExpected &= search_results[0].data_type_to_result["pos_y"].regex_pattern ==
                        expected_search_result.data_type_to_result["pos_y"].regex_pattern;

  EXPECT_TRUE(isResultAsExpected) << "Searcher returned search result different than expected";
}