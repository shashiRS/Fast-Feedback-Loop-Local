/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     unittest_mock_plugin_manager.hpp
 *  @brief    Helper class for unit testing of the plugins
 */

#ifndef NEXT_UNITTEST_MOCK_PLUGIN_MANAGER_H_
#define NEXT_UNITTEST_MOCK_PLUGIN_MANAGER_H_

#include <gtest/gtest.h>

#include <memory>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>

namespace next {
namespace test {

//! A class helping to mocking the plugin manager to initialize the basic things of the plugins,
//! e.g. to test the process manager
/*!
 * This class is used for unit testing using GoogleTest.
 * What you have to do in your code:
 *   #include "unittest_mock_plugin_manager.hpp"
 *   using next::test::MockPluginManager;
 *   TYPED_TEST_CASE(MockPluginManager, next::plugins::<PluginMockClass>);
 *
 * For more information see gtest-typed-test.h
 *
 * Note: TYPED_TEST_CASE is no longer existing in current GoogleTest, it was replaced by TYPED_TEST_SUITE. But we are
 * still using the old version of GoogleTest.
 */

#define SDL_DATA_NAME "SDL_data"

NEXT_DISABLE_DEPRECATED_WARNING
template <class T>
class MockPluginManager : public ::testing::Test {
public:
  MockPluginManager() = default;
  ~MockPluginManager() {
    // making sure that the plugin itself is destroyed first, it is unsubscribing itself
  }

  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  virtual void SetUp() override {
    publisher_ = std::make_unique<next::udex::publisher::DataPublisher>("testNode");
    InitDataPublisher(SDL_DATA_NAME, "ap_pdo_tool.sdl");
    signal_explorer_ = std::make_shared<next::udex::explorer::SignalExplorer>();
    data_subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>("Next-DataBridge-EcalNode");
  }

  // Per-test-suite tear-down.
  // Called after the last test in this test suite.
  virtual void TearDown() override {
    signal_explorer_.reset();
    data_subscriber_.reset();
    dummy_output_data_manager_.reset();
    publisher_.reset();
  }

  void InitDataPublisher(const std::string &strSDLDataName, const std::string &strSDLFile) {
    ASSERT_TRUE(publisher_->SetDataSourceInfo(strSDLDataName, 22, 33)) << "SetDataSourceInfo failed";
    ASSERT_TRUE(publisher_->RegisterDataDescription(strSDLFile, true)) << "RegisterDataDescription failed";
  }

  void initPlugin(std::shared_ptr<T> plugin, [[maybe_unused]] const std::string &strPluginName) {
    plugin->strDataSrc = SDL_DATA_NAME;
    plugin->connectPlugin(data_subscriber_, dummy_output_data_manager_, signal_explorer_);
    plugin->init();
  }

  // helper function to set which topics shall be extraced by a plugin
  void setTopicRangePlugin(std::shared_ptr<T> plugin, int iStartIndexTopic, int iNumOfTopics) {
    auto topicList = publisher_->getTopicsAndHashes();

    plugin->itMapTopicsStart = topicList.begin();
    std::advance(plugin->itMapTopicsStart, iStartIndexTopic);

    plugin->itMapTopicsEnd = plugin->itMapTopicsStart;
    std::advance(plugin->itMapTopicsEnd, iNumOfTopics);
  }

  // helper function to set which urls shall be subscribed by a plugin
  void setSubscriptRangePlugin(std::shared_ptr<T> plugin, int iStartIndexSubscribe, int iNumOfSubscription) {
    plugin->iStartIDObject = iStartIndexSubscribe;
    plugin->iNumOfObject = iStartIndexSubscribe + iNumOfSubscription;
  }
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_;
  std::shared_ptr<next::databridge::data_manager::IDataManager> dummy_output_data_manager_;
  std::unique_ptr<next::udex::publisher::DataPublisher> publisher_;
};
NEXT_RESTORE_WARNINGS

} // namespace test
} // namespace next

#endif // NEXT_UNITTEST_MOCK_PLUGIN_MANAGER_H_
