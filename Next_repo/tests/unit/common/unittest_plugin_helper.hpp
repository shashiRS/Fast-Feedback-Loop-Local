/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     unittest_plugin_helper.hpp
 *  @brief    Helper class for unit testing plugins
 */

#ifndef UNITTEST_PLUGIN_HELPER_H_
#define UNITTEST_PLUGIN_HELPER_H_

#include <gtest/gtest.h>
#include <memory>

#include <next/bridgesdk/plugin.h>

#include "data_manager_mock.hpp"
#include "data_subscriber_mock.hpp"
#include "plugin_data_publisher_mock.hpp"
#include "plugin_data_subscriber_mock.hpp"
#include "plugin_signal_explorer_mock.hpp"
#include "plugin_signal_extractor_mock.hpp"
#include "signal_explorer_mock.hpp"

namespace next {
namespace test {

//! A class helping to initialize the basic things for unit tests
/*!
 * This class is used for unit testing using GoogleTest.
 * What you have to do in your code:
 *   #include "unittest_plugin_helper.hpp"
 *   #include "my_plugin.hpp" // <-- the plugin which shall be tested
 *   using next::test::PluginTest;
 *   TYPED_TEST_CASE(PluginTest, next::plugins::MyPlugin);
 *
 * Then you can alreay define your own tests, for example:
 *   TEST(PluginTest, PluginCreation) {
 *     next::plugins::MyPlugin my_instance("some/path");
 *     const std::string plugin_name("PluginName");
 *     plugin.setName(plugin_name);
 *     EXPECT_EQ(plugin.getName(), plugin_name);
 *   }
 * The used name "PluginCreation" is free to chose, "PluginTest" is fixed (its the name of the template class)
 *
 * To use the mocked interface use the connected and initialized plugin as
 *   TEST(PluginTest, PluginCreation) {
 *     next::bridgesdk::plugin_config_t config;
 *     plugin->getDescription(config);
 *     EXPECT_TRUE(config.end() != config.find("url")) << "no url was in config";
 *   }
 *
 * For more information see gtest-typed-test.h
 * Have a look on plugin_unittest_test.cpp for a small example
 *
 * Note: TYPED_TEST_CASE is no longer existing in current GoogleTest, it was replaced by TYPED_TEST_SUITE. But we are
 * still using the old version of GoogleTest.
 */

template <class T>
class PluginTest : public ::testing::Test {
public:
  PluginTest() = default;
  ~PluginTest() {
    plugin_.reset();
    // making sure that the plugin itself is destroyed first, it is unsubscribing itself
  }

  void SetUp() {
    dm_mock_ = new next::test::DataManagerMock();
    dm_mock_ptr_.reset(dm_mock_);

    signal_explorer_ptr_.reset(new next::test::SignalExplorerMock());
    subscriber_mock_ = new next::test::DataSubscriberMock("asdf");
    subscriber_mock_ptr_.reset(subscriber_mock_);

    plugin_ = std::make_shared<T>("NonExistingPath");

    plugin_->connectPlugin(subscriber_mock_ptr_, dm_mock_ptr_, signal_explorer_ptr_);
    plugin_->init();

    plugin_sig_extractor_ = std::make_shared<next::test::PluginSignalExtractorMock>(plugin_.get());
    plugin_signal_publisher_ = std::make_shared<next::test::PluginDataPublisherMock>(plugin_.get());
    plugin_signal_subscriber_ = std::make_shared<next::test::PluginDataSubscriberMock>(plugin_.get());
    plugin_signal_explorer_ = std::make_shared<next::test::PluginSignalExplorerMock>(plugin_.get());
  }

  void TearDown() override {
    signal_explorer_ptr_.reset();
    subscriber_mock_ptr_.reset();
    plugin_sig_extractor_.reset();
    plugin_signal_publisher_.reset();
    plugin_signal_subscriber_.reset();
    plugin_signal_explorer_.reset();

    // reset uid counter for sensors
    dm_mock_ptr_->GetRegisteredSensors()->uid_counter = 1;
    // clear sensorlist
    dm_mock_ptr_->GetRegisteredSensors()->sensor_list.clear();
  }

  next::test::DataManagerMock *dm_mock_;
  std::shared_ptr<next::databridge::data_manager::IDataManager> dm_mock_ptr_;
  std::shared_ptr<T> plugin_;
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_ptr_;
  next::test::DataSubscriberMock *subscriber_mock_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> subscriber_mock_ptr_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_sig_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_signal_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_signal_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
};

} // namespace test
} // namespace next

#endif // UNITTEST_PLUGIN_HELPER_H_
