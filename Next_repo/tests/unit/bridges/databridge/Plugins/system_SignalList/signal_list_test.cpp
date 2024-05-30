/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     signal_list_test.cpp
 *  @brief    Testing the system_SignalList plugin
 */

#include <map>

#include <gtest/gtest.h>
#include <next/sdk/sdk.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include "signal_list.h"
#include "signal_list_test_data.hpp"
#include "unittest_plugin_helper.hpp"

namespace next {
namespace plugins {

class SignalExplTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  SignalExplTestFixture() { this->instance_name_ = "SignalExplTestFixture"; }
};

class SignalListTester : public SignalList {
public:
  SignalListTester(const char *path) : SignalList(path){};
  Json::Value
  testcreatePayloadJSONByURL(const std::string &current_URL,
                             std::vector<bridgesdk::plugin_addons::signal_child_info> &vector_childs_URL) const {
    return createPayloadJSONByURL(current_URL, vector_childs_URL);
  };

  Json::Value createPayloadJsonBySearchKeyword(const std::string &keyword) const {
    return createPayloadJsonBySearchKeyword(keyword);
  }

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> getPublisher() { return plugin_publisher_; }

private:
};

TEST_F(SignalExplTestFixture, signalChild_test) {

  SignalListTester signal_list_tester("dummy");
  std::unique_ptr<next::udex::publisher::DataPublisher> publisher =
      std::make_unique<next::udex::publisher::DataPublisher>("get_child_by_url_test");
  std::unique_ptr<next::udex::explorer::SignalExplorer> signal_explorer =
      std::make_unique<next::udex::explorer::SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("get_child_by_url_test", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT",
                                                 (void *)next::plugin::test::SDL_CONTENT.data(),
                                                 next::plugin::test::SDL_CONTENT.size(), true))
      << "Failed to register data description";

  auto childs_info = signal_explorer->GetChildByUrl("get_child_by_url_test.AlgoVehCycle.VehDyn");
  std::vector<next::bridgesdk::plugin_addons::signal_child_info> vec;
  for (const auto &info : childs_info) {
    vec.push_back({info.name, info.child_count, info.array_lenght});
  }

  auto init_result = signal_list_tester.init();
  EXPECT_EQ(init_result, true);

  auto json = signal_list_tester.testcreatePayloadJSONByURL("multi_thread_access_test.AlgoVehCycle.VehDyn", vec);
  auto json_elements = json["dataStreamSchema"];

  std::map<std::string, bool> map_elements;
  std::transform(json_elements.begin(), json_elements.end(), std::inserter(map_elements, map_elements.end()),
                 [](const auto &element) -> std::pair<std::string, bool> {
                   return std::make_pair(element["text"].asString(), element["isLeaf"].asBool());
                 });

  // array name with the size is the new more efficient convention for the GUI
  auto result = std::find_if(map_elements.begin(), map_elements.end(),
                             [](const auto &pair) { return pair.first == "State[12]"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, true);

  result =
      std::find_if(map_elements.begin(), map_elements.end(), [](const auto &pair) { return pair.first == "State[0]"; });
  EXPECT_EQ(result, map_elements.end());

  result = std::find_if(map_elements.begin(), map_elements.end(),
                        [](const auto &pair) { return pair.first == "Longitudinal"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, false);

  result = std::find_if(map_elements.begin(), map_elements.end(),
                        [](const auto &pair) { return pair.first == "MotionState"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, false);

  result =
      std::find_if(map_elements.begin(), map_elements.end(), [](const auto &pair) { return pair.first == "Lateral"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, false);

  result = std::find_if(map_elements.begin(), map_elements.end(),
                        [](const auto &pair) { return pair.first == "sSigHeader"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, false);

  result = std::find_if(map_elements.begin(), map_elements.end(),
                        [](const auto &pair) { return pair.first == "uiVersionNumber"; });
  EXPECT_NE(result, map_elements.end());
  EXPECT_EQ(result->second, true);
}
} // namespace plugins
} // namespace next
