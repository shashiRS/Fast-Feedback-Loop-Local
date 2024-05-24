/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     event_handler_test.cpp
 *  @brief    DataRequestServer unit tests
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include "next/sdk/sdk.hpp"

#include <../DataClassManager/event_handler_helper.h>
#include "data_class_manager.h"

using namespace std::chrono_literals;

class RequestEventHandlerTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  RequestEventHandlerTestFixture() { this->instance_name_ = "request_event_handler"; }
};

Json::Value ConvertTxtToJson(const char *txt) {
  Json::Value root;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(txt, root); // parse process
  if (!parsingSuccessful) {
    std::cout << "Failed to parse" << reader.getFormattedErrorMessages();
  }

  return root;
}

//! Test of removed plugin config after forwarding empty configuration
/*!
 * The Goal of the test is to forward empty config list and checking for the removed plugin if they appear in the config
 * queue.
 */
TEST_F(RequestEventHandlerTestFixture, CheckForEmptyConfig) {
  auto handler = std::make_shared<next::databridge::ui_request::EventHandlerHelper>();
  auto sp_manager = std::make_shared<next::databridge::DataClassManager>();

  // clang-format off
  auto msg_cmd = R"({
    "channel" : "dataStreamConfiguration",
		"source" : "NextGUI",
        "event" : "UsedDataClassesChanged",
        "id" : "7183acb5-c0e7-4d76-9726-6d979d52b65a",
        "payload" :
        {
                "configuredDataStreams" :
                [
                        {
                                "configurations" :
                                [
                                        {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin1"
                                        },
                                         {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin2"
                                        }
                                       
                                ],
                                "dataStreamName" : "objectData"
                        }
                ]
        }
        
  }
  )";
  // clang-format on

  handler->HandleEvent(ConvertTxtToJson(msg_cmd), sp_manager);

  next::databridge::DataClassManager::ConfigDataRequestItem config_request;
  for (auto ind = 1; ind < 3; ++ind) {
    sp_manager->GetConfigDataClassRequestNonBlocking(config_request);
    auto cmp_str = "Plugin" + std::to_string(ind);
    EXPECT_EQ(config_request.plugin_name, cmp_str);
  }

  // clang-format off
  auto msg_cmd2 = R"({
    "channel" : "dataStreamConfiguration",
		"source" : "NextGUI",
        "event" : "UsedDataClassesChanged",
        "id" : "7183acb5-c0e7-4d76-9726-6d979d52b65a",
        "payload" :
        {
                "configuredDataStreams" :
                [
                        {
                                "dataStreamName" : "objectData"
                        }
                ]
        }
          
  }
  )";
  // clang-format on

  handler->HandleEvent(ConvertTxtToJson(msg_cmd2), sp_manager);
  for (auto ind = 0; ind < 2; ++ind) {
    sp_manager->GetConfigDataClassRequestNonBlocking(config_request);
    EXPECT_TRUE(config_request.configs.empty());
  }
}

//! Test of removing a plugin configuration from config list
/*!
 * The Goal of the test is to remove a plugin configuration from the config list and check if the removed plugin
 * has been added to the config queue with the property having empty config size.
 */
TEST_F(RequestEventHandlerTestFixture, RemovePluginConfigAndCheckForEmptyConfigSize) {
  auto handler = std::make_shared<next::databridge::ui_request::EventHandlerHelper>();
  auto sp_manager = std::make_shared<next::databridge::DataClassManager>();

  // clang-format off
  auto msg_cmd = R"({
    "channel" : "dataStreamConfiguration",
		"source" : "NextGUI",
        "event" : "UsedDataClassesChanged",
        "id" : "7183acb5-c0e7-4d76-9726-6d979d52b65a",
        "payload" :
        {
                "configuredDataStreams" :
                [
                        {
                                "configurations" :
                                [
                                        {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin1"
                                        },
                                        {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin2"
                                        }
                                       
                                ],
                                "dataStreamName" : "objectData"
                        }
                ]
        }
        
  }
  )";
  // clang-format on

  handler->HandleEvent(ConvertTxtToJson(msg_cmd), sp_manager);
  next::databridge::DataClassManager::ConfigDataRequestItem config_request;

  // Make queue empty for next step
  for (auto ind = 0; ind < 2; ++ind) {
    sp_manager->GetConfigDataClassRequestNonBlocking(config_request);
  }

  // clang-format off
  auto msg_cmd2 =  R"({
    "channel" : "dataStreamConfiguration",
		"source" : "NextGUI",
        "event" : "UsedDataClassesChanged",
        "id" : "7183acb5-c0e7-4d76-9726-6d979d52b65a",
        "payload" :
        {
                "configuredDataStreams" :
                [
                        {
                                "configurations" :
                                [
                                        {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin2"
                                        }
                                       
                                ],
                                "dataStreamName" : "objectData"
                        }
                ]
        }
        
  }
  )";
  // clang-format on

  handler->HandleEvent(ConvertTxtToJson(msg_cmd2), sp_manager);
  sp_manager->GetConfigDataClassRequestNonBlocking(config_request);

  EXPECT_EQ(config_request.plugin_name, "Plugin1");
  EXPECT_TRUE(config_request.configs.empty());

  sp_manager->GetConfigDataClassRequestNonBlocking(config_request);
  EXPECT_EQ(config_request.plugin_name, "Plugin2");
}
//! Test of checking available plugin in config
/*!
 * Forwarding json plugin config and expecting given plugin be added into the Queue of configs.
 */
TEST_F(RequestEventHandlerTestFixture, CheckAvailablePluginConfig) {

  auto handler = std::make_shared<next::databridge::ui_request::EventHandlerHelper>();
  auto sp_manager = std::make_shared<next::databridge::DataClassManager>();

  // clang-format off
  auto msg_cmd = R"({
    "channel" : "dataStreamConfiguration",
		"source" : "NextGUI",
        "event" : "UsedDataClassesChanged",
        "id" : "7183acb5-c0e7-4d76-9726-6d979d52b65a",
        "payload" :
        {
                "configuredDataStreams" :
                [
                        {
                                "configurations" :
                                [
                                        {
                                                "name" : "groundLines view3D_FdpBaseRoad",
                                                "parameters" :
                                                [
                                                        {
                                                                "editable" : true,
                                                                "multipleSelection" : false,
                                                                "name" : "datacache",
                                                                "selectable" : false,
                                                                "value" : "Dummy1"
                                                        }
                                                ],
                                                "plugin" : "Plugin1"
                                        }
                                ],
                                "dataStreamName" : "objectData"
                        }
                ]
        }

}
)";
  // clang-format on
  handler->HandleEvent(ConvertTxtToJson(msg_cmd), sp_manager);
  next::databridge::DataClassManager::ConfigDataRequestItem config_request;
  sp_manager->GetConfigDataClassRequestNonBlocking(config_request);
  EXPECT_EQ(config_request.plugin_name, "Plugin1");

  next::databridge::DataClassManager::ConfigDataRequestItem config_request2;
  EXPECT_FALSE(sp_manager->GetConfigDataClassRequestNonBlocking(config_request2));
  EXPECT_EQ(config_request2.plugin_name, "");
}