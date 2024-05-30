/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_base_tests.cpp
 *  @brief    Testing the config interface
 */

#include <atomic>
#include <thread>

#include <next/sdk/sdk.hpp>

#include <ecal/ecal.h>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>
#include "event_manager.h"
using next::appsupport::ConfigClient;
class ConfigClientTests : public next::sdk::testing::TestUsingEcal {
public:
  ConfigClientTests() { this->instance_name_ = "ConfigBaseTests"; }
  ~ConfigClientTests() { ConfigClient::do_reset(); }
};
class ConfigClientTestsWithoutEcal : public ::testing::Test {
public:
  ConfigClientTestsWithoutEcal() {}
  ~ConfigClientTestsWithoutEcal() { ConfigClient::do_reset(); }
};

// TODO refactor tests here after refactoring of ConfigClient class!!!!
TEST_F(ConfigClientTestsWithoutEcal, getStringListArray) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();
  std::vector<std::string> clist =
      ConfigClient::getConfig()->getStringList("test:requests:CEM200_COH:composition:m_pEmlInputIf", {});

  // arrays above the final node level are not supported anymore and are ignored
  ASSERT_EQ(clist.size(), 1);
  EXPECT_EQ(clist[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
}

TEST_F(ConfigClientTestsWithoutEcal, getStringListWithInvalidKey) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist =
      ConfigClient::getConfig()->getStringList("test:requests:CEM200_COH:composition:m_pEmlInputIffail", {});

  // arrays above the final node level are not supported anymore and are ignored
  ASSERT_EQ(clist.size(), 1);
  EXPECT_EQ(clist[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
}

TEST_F(ConfigClientTests, getStringListFromInvalidKey) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  // should not comunicate with the server
  ConfigClient::getConfig()->SetValidConfigServer("dummyServer");
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist = ConfigClient::getConfig()->getStringList(
      "test:requests:CEM200_COH:composition:m_pEmlInputIffail:alternatives", {});

  ASSERT_EQ(clist.size(), 0);
}

TEST_F(ConfigClientTestsWithoutEcal, getStringListSimpleList) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist =
      ConfigClient::getConfig()->getStringList("test:requests:CEM200_COH:composition:m_pSefInputIf", {});

  ASSERT_EQ(clist.size(), 3) << "Values for subtree reported";
  EXPECT_EQ(clist[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(clist[1], "MFC5xx Device.FCU.ARSSepTuple3_Fcu1");
  EXPECT_EQ(clist[2], "");
}

TEST_F(ConfigClientTestsWithoutEcal, getStringListSingleValue) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist =
      ConfigClient::getConfig()->getStringList("test:requests:CEM200_COH:composition:singleValue", {});

  ASSERT_EQ(clist.size(), 1) << "Values for subtree reported";
  EXPECT_EQ(clist[0], "asdf");
}
TEST_F(ConfigClientTestsWithoutEcal, getStringListSingleValueNotExisting) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist = ConfigClient::getConfig()->getStringList(
      "test.requests.CEM200_COH.composition.singleValue2", {"default1", "default2"});

  ASSERT_EQ(clist.size(), 2) << "Values for subtree reported";
  EXPECT_EQ(clist[0], "default1");
  EXPECT_EQ(clist[1], "default2");
}
TEST_F(ConfigClientTestsWithoutEcal, getStringListSingleEmptyValue) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg_array.json");
  ConfigClient::finish_init();

  std::vector<std::string> clist = ConfigClient::getConfig()->getStringList(
      "test:requests:CEM200_COH:composition:singleValueEmpty", {"default1", "default2"});

  ASSERT_EQ(clist.size(), 1) << "Values for subtree reported";
  EXPECT_EQ(clist[0], "");
}
TEST_F(ConfigClientTests, loadFilteredConfig) {
  ASSERT_TRUE(ConfigClient::do_init("component_test"));
  // should not comunicate with the server
  ConfigClient::getConfig()->SetValidConfigServer("dummyServer");
  ConfigClient::getConfig()->putCfg("client_load_test.json");
  ConfigClient::finish_init();

  [[maybe_unused]] auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  std::vector<std::string> clist;
  auto port = ConfigClient::getConfig()->get_int("component_test:port", 0);
  EXPECT_EQ(port, 80);
  // check if config value is filtered out during "putCfg" because component "test" doesn't exist
  auto debug_level = ConfigClient::getConfig()->get_int("test:debug_level", -1);
  EXPECT_EQ(debug_level, -1);
}
//! flag for endless loop
std::atomic<bool> publishMessage(true);
TEST_F(ConfigClientTests, receiveBadConfigValue) {
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  eCAL::Util::EnableLoopback(true);
  ASSERT_TRUE(ConfigClient::do_init("component_test"));
  ConfigClient::getConfig()->disableFilterByComponent(); // disable filtering to be able to add config file
  // should not comunicate with the server
  ConfigClient::getConfig()->SetValidConfigServer("dummyServer");
  ConfigClient::getConfig()->putCfg("client_load_test.json");
  ConfigClient::finish_init();

  next::sdk::events::SendConfigValueToClient eventPublisher{""};
  next::sdk::events::SendConfigValueToClientInformationMessage eventMessage{};

  auto publisher_thread = std::thread([&eventPublisher, &eventMessage] {
    eventMessage.receiver_name = "component_test";
    eventMessage.config_value = "bad_value";
    while (publishMessage) {
      eventPublisher.publish(eventMessage);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    eventMessage.config_value = "bad_value";
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  auto intValue = ConfigClient::getConfig()->get_int("component_test1:port", -1);
  auto floatValue = ConfigClient::getConfig()->get_float("component_test1:port", -1.f);
  auto boolValue = ConfigClient::getConfig()->get_option("component_test1:port", false);

  publishMessage = false;
  if (publisher_thread.joinable())
    publisher_thread.join();
  // handle ok the bad value and get the default one
  EXPECT_EQ(intValue, -1);
  EXPECT_FLOAT_EQ(floatValue, -1.0);
  EXPECT_EQ(boolValue, false);
}
TEST_F(ConfigClientTestsWithoutEcal, DISABLED_test_PrivateKeyFilterCommandLineParameters_Success) {
  ASSERT_TRUE(ConfigClient::do_init("component_test"));
  // should not comunicate with the server
  ConfigClient::getConfig()->SetValidConfigServer("dummyServer");
  // comment in once it's implemented
  // ConfigClient::getConfig()->definePrivateConfigKeys({"component_test:dummy1"});
  auto cmd_options = next::appsupport::CmdOptions("component_test");
  cmd_options.addParameterInt("dummy1", "dummy param", "component_test:dummy1");
  cmd_options.addParameterInt("dummy2", "dummy param", "component_test:dummy2");

  // add hardcoded parametrs
  ConfigClient::getConfig()->put("component_test:dummy1", 1);
  ConfigClient::getConfig()->put("component_test:dummy2", 2);

  const char *argv[] = {"cmdoption.exe", "--dummy1", "11", "--dummy2", "22"};
  ASSERT_TRUE(cmd_options.parseCmd(5, argv));

  ConfigClient::finish_init();

  auto dummy1 = ConfigClient::getConfig()->get_int("component_test:dummy1", -1);
  auto dummy2 = ConfigClient::getConfig()->get_int("component_test:dummy2", -1);

  // as "component_test.dummy1" is blacklisted it will store the local value
  EXPECT_EQ(dummy1, 1);
  // dummy 2 is updated from the command line
  EXPECT_EQ(dummy2, 22);
}

TEST_F(ConfigClientTestsWithoutEcal, put_bool) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->put_option("test:bool", true);
  ConfigClient::finish_init();

  EXPECT_TRUE(ConfigClient::getConfig()->get_option("test:bool", false));
}

TEST_F(ConfigClientTestsWithoutEcal, put_int) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->put("test:int", 1);
  ConfigClient::finish_init();

  EXPECT_EQ(ConfigClient::getConfig()->get_int("test:int", 0), 1);
}

TEST_F(ConfigClientTestsWithoutEcal, put_float) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->put("test:float", 1.f);
  ConfigClient::finish_init();

  EXPECT_FLOAT_EQ(ConfigClient::getConfig()->get_float("test:float", 0), 1.f);
}

TEST_F(ConfigClientTestsWithoutEcal, put_string) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->put("test:string", "1");
  ConfigClient::finish_init();

  EXPECT_EQ(ConfigClient::getConfig()->get_string("test:string", "0"), "1");
}

TEST_F(ConfigClientTestsWithoutEcal, put_afterInit) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->put("test:int", 1);
  ConfigClient::finish_init();

  EXPECT_EQ(ConfigClient::getConfig()->get_int("test:int", 0), 1);

  // will overwrite value because after init all values are added to the current tree
  ConfigClient::getConfig()->put("test:int", 3);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("test:int", 0), 3);
}

TEST_F(ConfigClientTestsWithoutEcal, insert) {
  std::string sampleJsonDefault = "{\"test\":1}";
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->insert(sampleJsonDefault, false, false);
  ASSERT_TRUE(ConfigClient::finish_init());

  EXPECT_EQ(ConfigClient::getConfig()->get_int("test", 0), 1);
}