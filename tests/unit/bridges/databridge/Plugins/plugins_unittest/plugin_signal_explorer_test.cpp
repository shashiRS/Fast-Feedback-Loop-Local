/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_3D_view_test.cpp
 *  @brief    Testing some mock functionalities
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>

#include "plugin_mock.hpp"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

const std::string MOCK_URL("MockUrl");
TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

TYPED_TEST(PluginTest, getChildsbyUrl_test) {
  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExplorer SignalExplorer(Plugin.get());
  const std::string test_parent_url = "test_url";
  std::vector<next::udex::ChildInfo> child_info = {{"child1", 0, 1}, {"child2", 0, 2}, {"child3", 0, 3}};
  auto signal_explorer_mock = std::dynamic_pointer_cast<next::test::SignalExplorerMock>(this->signal_explorer_ptr_);

  signal_explorer_mock->SetChildInfo(child_info);
  auto return_value = SignalExplorer.getChildsbyUrl(test_parent_url);
  ASSERT_EQ(return_value[0].name, "child1");
  ASSERT_EQ(return_value[1].name, "child2");
  ASSERT_EQ(return_value[2].name, "child3");
  ASSERT_EQ(return_value[0].child_count, 1);
  ASSERT_EQ(return_value[1].child_count, 2);
  ASSERT_EQ(return_value[2].child_count, 3);
}

TYPED_TEST(PluginTest, getDataSourceNames_test) {
  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExplorer SignalExplorer(Plugin.get());
  const std::string format_id = "test_id";
  std::vector<std::string> test_data_source_names = {"src1", "src2", "src3"};
  std::vector<std::string> data_source_names;
  auto signal_explorer_mock = std::dynamic_pointer_cast<next::test::SignalExplorerMock>(this->signal_explorer_ptr_);
  signal_explorer_mock->setDeviceByFormatType(test_data_source_names);

  SignalExplorer.getDataSourceNames(format_id, data_source_names);
  ASSERT_EQ(data_source_names.size(), 3);
  ASSERT_EQ(data_source_names[0], "src1");
  ASSERT_EQ(data_source_names[1], "src2");
  ASSERT_EQ(data_source_names[2], "src3");
}

TYPED_TEST(PluginTest, getUrlTree_test) {
  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExplorer SignalExplorer(Plugin.get());
  std::vector<std::string> test_vec1 = {"test1", "test1", "test1"};
  std::vector<std::string> test_vec2 = {"test2", "test2", "test2"};
  std::vector<std::string> test_vec3 = {"test3", "test3", "test3"};
  std::unordered_map<std::string, std::vector<std::string>> test_url_tree = {{"url1", {"test1", "test1", "test1"}},
                                                                             {"url2", {"test2", "test2", "test2"}},
                                                                             {"url3", {"test3", "test3", "test3"}}};
  auto signal_explorer_mock = std::dynamic_pointer_cast<next::test::SignalExplorerMock>(this->signal_explorer_ptr_);

  signal_explorer_mock->SetFullUrlTree(test_url_tree);
  auto return_data = SignalExplorer.getUrlTree();
  ASSERT_EQ(return_data.at("url1"), test_vec1);
  ASSERT_EQ(return_data.at("url2"), test_vec2);
  ASSERT_EQ(return_data.at("url3"), test_vec3);
}

TYPED_TEST(PluginTest, getTopicsByUrl_test) {
  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExplorer SignalExplorer(Plugin.get());

  std::vector<std::string> set_url{"rul1", "url2", "url3"};
  std::vector<std::string> return_url;
  const std::string signal_url = "test";
  auto signal_explorer_mock = std::dynamic_pointer_cast<next::test::SignalExplorerMock>(this->signal_explorer_ptr_);

  signal_explorer_mock->setTopicByUrl(set_url);
  return_url = SignalExplorer.getTopicsByUrl(signal_url);
  ASSERT_EQ(return_url, set_url);
}
