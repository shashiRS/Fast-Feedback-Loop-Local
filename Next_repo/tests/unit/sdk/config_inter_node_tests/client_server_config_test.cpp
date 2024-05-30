
#include <gtest/gtest.h>
#include <thread>

#include <ecal/ecal.h>
#include <boost/json.hpp>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_server.hpp>
#include <next/sdk/sdk.hpp>

#include "event_manager.h"

namespace next {
namespace sdk {

namespace events {

using namespace next::appsupport;
class ClientServerConfigTests : public testing::TestUsingEcal {
public:
  ClientServerConfigTests() : TestUsingEcal() {
    this->instance_name_ = "ClientServerConfigTests";
    logger::register_to_init_chain();
  }
  ~ClientServerConfigTests() { ConfigClient::do_reset(); }
};

class ClientServerConfigTestsWithoutEcal : public ::testing::Test {
public:
  ClientServerConfigTestsWithoutEcal() {}
  ~ClientServerConfigTestsWithoutEcal() { ConfigClient::do_reset(); }
};

TEST_F(ClientServerConfigTests, send_global_config_to_client_save_and_get_the_conf) {

  eCAL::Util::EnableLoopback(true);
  std::string name("component_test");

  ConfigServer l_config_server(name);

  appsupport::ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  appsupport::ConfigClient::finish_init();

  std::string key = "component_test:ip";
  l_config_server.put(key, "1.1.1.1");
  key = "component_test:port";
  l_config_server.put(key, 80);
  key = "component_test:url";
  l_config_server.put(key, "myurl.com");
  key = "component_test:is_ssl_enabled";
  l_config_server.put(key, true);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.pushGlobalConfig();
  auto global_conf = l_config_server.getComponentConfigurationsAsJson(name);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  auto client_conf = appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ(global_conf, client_conf);

  // adding a new key outide the namespace of the curent config client name will create a different
  // config tree on the server side so the result must not be equal
  key = "different_component:is_ssl_enabled";
  l_config_server.put(key, true);
  l_config_server.pushGlobalConfig();
  global_conf = l_config_server.getAllConfigurationsAsJson();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  client_conf = appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_NE(global_conf, client_conf);
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTestsWithoutEcal, check_put_call_back_function) {

  std::string name("component_test");

  ConfigServer l_config_server(name);
  std::string key = "ip";

  l_config_server.addChangeHook([&]([[maybe_unused]] std::string property) {
    bool propertyCheck = false;
    if (property.compare(key) == 0 || property.compare("full_cfg") == 0)
      propertyCheck = true;
    EXPECT_EQ(propertyCheck, true);
    return true;
  });

  l_config_server.put(key, "1.1.1.1");
  key = "port";

  l_config_server.addChangeHook([&]([[maybe_unused]] std::string property) {
    bool propertyCheck = false;
    if (property.compare(key) == 0 || property.compare("full_cfg") == 0)
      propertyCheck = true;
    EXPECT_EQ(propertyCheck, true);
    return true;
  });

  l_config_server.put(key, 80);
}

TEST_F(ClientServerConfigTestsWithoutEcal, check_put_remove_callback) {

  std::string name("component_test");

  ConfigServer l_config_server(name);
  std::string key = "ip";
  bool is_call_back_called = false;

  auto id = l_config_server.addChangeHook([&]([[maybe_unused]] std::string property) {
    EXPECT_EQ(property, key);
    is_call_back_called = true;
    return true;
  });

  l_config_server.removeChangeHook(id);

  l_config_server.put(key, "127.0.0.1");
  EXPECT_EQ(is_call_back_called, false);
}

TEST_F(ClientServerConfigTestsWithoutEcal, get_put_and_getputoptions_tests) {

  std::string name("get_put_and_getputoptions_tests");

  ConfigServer l_config_server(name);
  std::string key = "is_ssl_enabled";
  l_config_server.put(key, false);
  EXPECT_EQ(l_config_server.get_option(key, true), false);
}

TEST_F(ClientServerConfigTestsWithoutEcal, check_put_two_call_backs_added_and_then_removed) {

  std::string name("component_test");
  std::string key = "port";

  bool should_be_called = false;
  ConfigServer l_config_server(name);

  auto id = l_config_server.addChangeHook([&]([[maybe_unused]] std::string property) {
    should_be_called = true;
    return true;
  });

  l_config_server.put(key, 80);
  EXPECT_EQ(should_be_called, true);

  should_be_called = false;
  l_config_server.removeChangeHook(id);
  l_config_server.put(key, 80);
  EXPECT_EQ(should_be_called, false);

  auto id1 = l_config_server.addChangeHook([&]([[maybe_unused]] std::string property) {
    should_be_called = true;
    return true;
  });

  l_config_server.put(key, 80);
  EXPECT_EQ(should_be_called, true);

  should_be_called = false;
  l_config_server.removeChangeHook(id1);
  EXPECT_EQ(should_be_called, false);
}

TEST_F(ClientServerConfigTestsWithoutEcal, reset_config_tests) {
  std::string name("reset_config_tests");
  ConfigServer l_config_server(name);
  std::string key = "ip";
  l_config_server.put(key, "1.1.1.1");
  EXPECT_EQ(l_config_server.get_string(key, ""), "1.1.1.1");
  l_config_server.clearConfig();
  EXPECT_EQ(l_config_server.get_string(key, ""), "");
}
TEST_F(ClientServerConfigTests, load_filtered_config_command_Line) {
  ConfigClient::do_init("load_filtered_config_command_Line");
  ConfigClient::getConfig()->SetValidConfigServer("dummyServer");
  auto cmd_options = next::appsupport::CmdOptions("load_filtered_config_command_Line");
  cmd_options.addParameterInt("debug_level,d", "debug level", "load_filtered_config_command_Line:debug_level");
  ConfigClient::getConfig()->put("test:debug_level", 2);
  ConfigClient::getConfig()->put("test:lower_level.lower_level2.dummy3", 1.4f);
  cmd_options.addParameterInt("from_cmd2,f", "debug from_cmd2", "load_filtered_config_command_Line:from_cmd2");

  const char *argv[] = {"cmdoption.exe", "-c", "client_server_load_config_cmd.json", "-d", "3", "-f", "7"};
  ASSERT_TRUE(cmd_options.parseCmd(7, argv));
  [[maybe_unused]] auto cfgContentAfterCmd = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  ConfigClient::getConfig()->putCfg("client_server_load_config.json");
  [[maybe_unused]] auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  ConfigClient::getConfig()->put("load_filtered_config_command_Line:dummy2", 12.4f);
  ConfigClient::getConfig()->put("load_filtered_config_command_Line:dummy3", 2.4f);
  ConfigClient::finish_init();

  auto debugLevelParam = ConfigClient::getConfig()->get_int("load_filtered_config_command_Line:debug_level", -1);
  // cmd provided value should be stored
  EXPECT_EQ(debugLevelParam, 3);

  auto dummyStrParam = ConfigClient::getConfig()->get_string("load_filtered_config_command_Line:dummy_str", "");
  // cmd configuration provided value should be stored
  EXPECT_EQ(dummyStrParam, "cmd_config");

  // update after finish_init should overwrite
  ConfigClient::getConfig()->put("load_filtered_config_command_Line:debug_level", 12);
  debugLevelParam = ConfigClient::getConfig()->get_int("load_filtered_config_command_Line:debug_level", -1);
  EXPECT_EQ(debugLevelParam, 12);
  ConfigClient::do_reset();
}
TEST_F(ClientServerConfigTests, request_config_from_server_at_init) {
  eCAL::Util::EnableLoopback(true);
  std::string name("request_config_from_server_at_init");
  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  l_config_server.putCfg("client_server_load_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  ConfigClient::getConfig()->put("request_config_from_server_at_init:lower_level:lower_level2:dummy2", 12.4f);
  ConfigClient::getConfig()->put("request_config_from_server_at_init:dummy2", 12.4f);
  ConfigClient::getConfig()->put("request_config_from_server_at_init:dummy3", 2.4f);

  auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();

  ConfigClient::finish_init();
  cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  auto paramVal =
      ConfigClient::getConfig()->get_int("request_config_from_server_at_init:lower_level:lower_level2:dummy2", -1);

  // should be the value from the server config
  EXPECT_EQ(paramVal, 8);
  ConfigClient::do_reset();
}
TEST_F(ClientServerConfigTests, server_update_after_init) {
  eCAL::Util::EnableLoopback(true);
  std::string name("server_update_after_init");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("server_update_after_init:lower_level:lower_level2:dummy2", 12);
  ConfigClient::getConfig()->put("server_update_after_init:dummy2", 12);
  ConfigClient::getConfig()->put("server_update_after_init:dummy3", 2);
  auto paramVal = ConfigClient::getConfig()->get_int("server_update_after_init:lower_level:lower_level2:dummy2", -1);
  EXPECT_EQ(paramVal, 12);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.putCfg("client_server_load_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  paramVal = ConfigClient::getConfig()->get_int("server_update_after_init:lower_level:lower_level2:dummy2", -1);
  // should be value from server config
  EXPECT_EQ(paramVal, 8);
  paramVal = ConfigClient::getConfig()->get_int("server_update_after_init:dummy3", -1);
  EXPECT_EQ(paramVal, 2);
  paramVal = ConfigClient::getConfig()->get_int("unused_component:realtime_factor", -1);
  EXPECT_EQ(paramVal, 10);
  ConfigClient::do_reset();
}
TEST_F(ClientServerConfigTests, load_configuration_sequence) {
  eCAL::Util::EnableLoopback(true);
  std::string name("load_configuration_sequence");
  next::appsupport::ConfigServer l_config_server(name);
  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  // process command option parametrs
  auto cmd_options = next::appsupport::CmdOptions(name);
  cmd_options.addParameterInt("debug_level,d", "debug level", "load_configuration_sequence:debug_level");
  cmd_options.addParameterInt("from_cmd2,f", "debug from_cmd2", "load_configuration_sequence:from_cmd2");

  // add hardcoded parametrs
  ConfigClient::getConfig()->put("load_configuration_sequence:debug_level", 2);
  ConfigClient::getConfig()->put("load_configuration_sequence:lower_level:lower_level2:dummy3", 1.4f);

  const char *argv[] = {"cmdoption.exe", "-c", "client_server_load_config_cmd.json", "-d", "3", "-f", "7"};
  ASSERT_TRUE(cmd_options.parseCmd(7, argv));
  // cmdOption parameters should not be available and be loaded to the main tree only after finish_init
  auto cmdOptionParameter = ConfigClient::getConfig()->get_int("load_configuration_sequence:from_cmd2", -1);
  EXPECT_EQ(cmdOptionParameter, -1);

  // load default config
  ConfigClient::getConfig()->putCfg("client_server_load_config.json");
  [[maybe_unused]] auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  ConfigClient::getConfig()->put("load_configuration_sequence:dummy_int", 12);
  ConfigClient::getConfig()->put("load_configuration_sequence:dummy3", 2.4f);

  // should hold the value from the default config
  auto updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);

  // load global config in the server
  l_config_server.putCfg("client_server_global_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();

  updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  ConfigClient::finish_init();

  next::sdk::logger::error(__FILE__, "finis_init_done");
  auto debugLevelParam = ConfigClient::getConfig()->get_int("load_configuration_sequence:debug_level", -1);
  // cmd provided value should be stored
  EXPECT_EQ(debugLevelParam, 3);

  auto dummyStrParam = ConfigClient::getConfig()->get_string("load_configuration_sequence:dummy_str", "");
  // cmd configuration provided value should be stored
  EXPECT_EQ(dummyStrParam, "cmd_config");

  updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 1);

  // updating parameter after finish_init should overwrite the value
  ConfigClient::getConfig()->put("load_configuration_sequence:debug_level", 12);
  debugLevelParam = ConfigClient::getConfig()->get_int("load_configuration_sequence:debug_level", -1);
  EXPECT_EQ(debugLevelParam, 12);
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, client_request_key_from_different_component) {
  eCAL::Util::EnableLoopback(true);
  std::string name("client_request_key_from_different_component");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("client_request_key_from_different_component:lower_level:lower_level2:dummy2", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  l_config_server.putCfg("client_server_load_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  auto paramVal = ConfigClient::getConfig()->get_int(
      "client_request_key_from_different_component:lower_level:lower_level2:dummy2", -1);
  // should be value from server config
  EXPECT_EQ(paramVal, 8);
  paramVal = ConfigClient::getConfig()->get_int("unused_component:realtime_factor", -1);
  EXPECT_EQ(paramVal, 10);
  auto floatParamVal = ConfigClient::getConfig()->get_float("unused_component:floatValue", -1.1f);
  EXPECT_FLOAT_EQ(floatParamVal, 1.10f);
  auto boolParamVal = ConfigClient::getConfig()->get_option("unused_component:boolValue", false);
  EXPECT_EQ(boolParamVal, true);
  auto strParamVal = ConfigClient::getConfig()->get_string("unused_component:strValue", "aa");
  EXPECT_EQ(strParamVal, "string");
  ConfigClient::do_reset();
}
//! flag for endless loop
std::atomic<bool> publishMessage(true);
TEST_F(ClientServerConfigTests, server_bad_request_for_value) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test");

  next::appsupport::ConfigServer l_config_server(name);
  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::sdk::events::RequestConfigValueFromServer eventPublisher{""};
  next::sdk::events::RequestConfigValueFromServerInformationMessage eventMessage{};

  auto publisher_thread = std::thread([&eventPublisher, &eventMessage] {
    eventMessage.requester_name = "component_test";
    eventMessage.request_default_value = "bad_value";
    eventMessage.request_key = "ip";
    uint16_t count = 100;
    while (publishMessage && count--) {
      eventMessage.request_type = "bad_value";
      eventPublisher.publish(eventMessage);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      eventMessage.request_type = "int";
      eventPublisher.publish(eventMessage);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      eventMessage.request_type = "float";
      eventPublisher.publish(eventMessage);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  publishMessage = false;
  if (publisher_thread.joinable())
    publisher_thread.join();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_EQ(cfgContentServer.size() > 0, true);
}
TEST_F(ClientServerConfigTests, test_PushLocalUpdateToServer) {

  eCAL::Util::EnableLoopback(true);
  std::string name("component_test");

  appsupport::ConfigClient::do_init(name);
  ConfigClient::getConfig()->setConfigServerSendTime(50);
  appsupport::ConfigClient::finish_init();

  ConfigServer l_config_server(name);

  ConfigClient::getConfig()->put("component_test:value", 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  auto contentServer = l_config_server.getAllConfigurationsAsJson();
  auto contentClient = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ(contentServer, contentClient);
}

TEST_F(ClientServerConfigTests, DISABLED_test_PrivateKeyFiltering_Success) {
  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  eCAL::Util::EnableLoopback(true);
  std::string name("test");
  next::appsupport::ConfigServer l_config_server(name);

  // component_test1
  ConfigClient::do_init("PrivateKeyFiltering_test1");
  ConfigClient::getConfig()->setConfigServerSendTime(50);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  // comment in once implemented
  // ConfigClient::getConfig()->definePrivateConfigKeys({"PrivateKeyFiltering_test1:object"});
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test1:dummy", 1);
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test1:object:secondLevel", 1);
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test1:object:secondLevel2", 2);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ConfigClient::do_reset();

  // component_test2
  ConfigClient::do_init("PrivateKeyFiltering_test2");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  // comment in once implemented
  // ConfigClient::getConfig()->definePrivateConfigKeys({"PrivateKeyFiltering_test2:object:secondLevel"});
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test2:dummy", 2);
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test2:object:secondLevel", 1);
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test2:object:secondLevel2", 2);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ConfigClient::do_reset();

  // component_test3
  ConfigClient::do_init("PrivateKeyFiltering_test3");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test3:dummy", 3);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ConfigClient::do_reset();

  // component_test4
  ConfigClient::do_init("PrivateKeyFiltering_test4");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  // commment in once it's implemented
  // ConfigClient::getConfig()->definePrivateConfigKeys({"PrivateKeyFiltering_test4"});
  ConfigClient::getConfig()->put("PrivateKeyFiltering_test4:dummy", 3);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ConfigClient::do_reset();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  EXPECT_EQ(cfgContentServer.size() > 0, true);
  //  auto configAsStr = l_config_server.getAllConfigurationsAsJson(true);
  //  auto as_json = boost::json::parse(configAsStr);
  //  int notExistingCount = 0;
  //  try {
  //
  //    auto child = as_json.at_pointer("/PrivateKeyFiltering_test1/object");
  //  } catch (...) {
  //    notExistingCount++;
  //  }
  //  try {
  //
  //    auto child = as_json.at_pointer("/PrivateKeyFiltering_test2/object/secondLevel");
  //  } catch (...) {
  //    notExistingCount++;
  //  }
  //  try {
  //
  //    auto child = as_json.at_pointer("/PrivateKeyFiltering_test4");
  //  } catch (...) {
  //    notExistingCount++;
  //  }
  //  // none of the blacklisted values should be found
  //  EXPECT_EQ(notExistingCount, 3);
  //  // this should be unchanged
  //  EXPECT_EQ(as_json.at_pointer("/PrivateKeyFiltering_test3/dummy"), "3");
  //  EXPECT_EQ(as_json.at_pointer("/PrivateKeyFiltering_test2/object/secondLevel2"), "2");
  //  EXPECT_EQ(as_json.at_pointer("/PrivateKeyFiltering_test1/dummy"), "1");
}

TEST_F(ClientServerConfigTests, DISABLED_test_PrivateInsertFromServerFiltering_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_PrivateInsertFromServerFiltering_Success");
  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init("test_PrivateInsertFromServerFiltering_Success");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_PrivateInsertFromServerFiltering_Success:port", 3);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.clearConfig();
  l_config_server.putCfg("client_server_load_config.json");
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  auto portValue = ConfigClient::getConfig()->get_int("test_PrivateInsertFromServerFiltering_Success:port", 0);
  // should be updated from server
  EXPECT_EQ(portValue, 80);
  ConfigClient::do_reset();
  l_config_server.clearConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ConfigClient::do_init("component_test");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("component_test:port", 3);
  // comment in once it's implemented
  // ConfigClient::getConfig()->definePrivateConfigKeys({"component_test:port"});
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.putCfg("client_server_load_config.json");
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  portValue = ConfigClient::getConfig()->get_int("component_test:port", 0);
  // should be the local value as the blacklisted value is not updated from the server
  EXPECT_EQ(portValue, 3);
  ConfigClient::do_reset();
}

//! flag for requesting value from server at the same time
std::atomic<bool> startAtTheSameTime(true);
TEST_F(ClientServerConfigTests, test_ClientRequestFromDifferentThreads_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("ClientRequestFromDifferentThreads_Success");
  next::appsupport::ConfigServer l_config_server(name);
  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("ClientRequestFromDifferentThreads_Success:dummy2", 12);
  ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.putCfg("client_server_load_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::vector<std::thread> threadArray;
  threadArray.push_back(std::thread([this] {
    while (startAtTheSameTime) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto paramVal = ConfigClient::getConfig()->get_int("unused_component:realtime_factor", -1);
    EXPECT_EQ(paramVal, 10);
  }));
  threadArray.push_back(std::thread([this] {
    while (startAtTheSameTime) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto floatParamVal = ConfigClient::getConfig()->get_float("unused_component:floatValue", -1.1f);
    EXPECT_FLOAT_EQ(floatParamVal, 1.10f);
  }));
  threadArray.push_back(std::thread([this] {
    while (startAtTheSameTime) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto strParamVal = ConfigClient::getConfig()->get_string("unused_component:strValue", "aa");
    EXPECT_EQ(strParamVal, "string");
  }));
  threadArray.push_back(std::thread([this] {
    while (startAtTheSameTime) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto boolParamVal = ConfigClient::getConfig()->get_option("unused_component:boolValue", false);
    EXPECT_EQ(boolParamVal, true);
  }));
  threadArray.push_back(std::thread([this] {
    while (startAtTheSameTime) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto paramVal = ConfigClient::getConfig()->get_int("unused_component:realtime_factor", -1);
    EXPECT_EQ(paramVal, 10);
  }));
  startAtTheSameTime = false;
  for (auto &thread : threadArray) {
    if (thread.joinable())
      thread.join();
  }
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestStringListFromDifferentComponent_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestStringListFromDifferentComponent_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientRequestStringListFromDifferentComponent_Success:test", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto paramVal = ConfigClient::getConfig()->getStringList("differentComponentStrList:orchestrator:flow1:clients", {});
  // should be value from server config
  EXPECT_EQ(paramVal.size(), 2);
  if (paramVal.size() == 2) {
    EXPECT_EQ(paramVal[0], "EmfNodeTriggering");
    EXPECT_EQ(paramVal[1], "runSequencer_response");
  }
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestStringListFromDifferentComponentMixedArray_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestStringListFromDifferentComponentMixedArray_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientRequestStringListFromDifferentComponentMixedArray_Success:test", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto paramVal =
      ConfigClient::getConfig()->getStringList("differentComponentStrList:orchestrator:flow1:clientsMixedValues", {});
  // should be value from server config
  EXPECT_EQ(paramVal.size(), 2);
  if (paramVal.size() == 2) {
    EXPECT_EQ(paramVal[0], "EmfNodeTriggering");
    EXPECT_EQ(paramVal[1], "100");
  }
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestStringListFromDifferentComponentEmptyArray_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestStringListFromDifferentComponentEmptyArray_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientRequestStringListFromDifferentComponentEmptyArray_Success:test", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto paramVal = ConfigClient::getConfig()->getStringList("differentComponentStrList:orchestrator:flow1:emptyArray",
                                                           {"default", "default2"});
  // should be value from server config
  EXPECT_EQ(paramVal.size(), 0);
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestStringListFromDifferentComponentNotExisting_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestStringListFromDifferentComponentNotExisting_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientRequestStringListFromDifferentComponentNotExisting_Success.test", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  auto paramVal = ConfigClient::getConfig()->getStringList("differentComponentStrList.orchestrator.flow1.Invalid",
                                                           {"default", "default2"});
  // should be value from server config
  ASSERT_EQ(paramVal.size(), 2);
  EXPECT_EQ(paramVal[0], "default");
  EXPECT_EQ(paramVal[1], "default2");
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestStringListFromDifferentComponentTimeout_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestStringListFromDifferentComponentTimeout_Success");

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->put("test_ClientRequestStringListFromDifferentComponentTimeout_Success:test", 12);
  ConfigClient::finish_init();

  auto paramVal = ConfigClient::getConfig()->getStringList("differentComponentStrList:orchestrator:flow1:Invalid",
                                                           {"default", "default2"});
  // should be default values as serveris unavailable
  ASSERT_EQ(paramVal.size(), 2);
  EXPECT_EQ(paramVal[0], "default");
  EXPECT_EQ(paramVal[1], "default2");
  ConfigClient::do_reset();
}
TEST_F(ClientServerConfigTests, test_ClientRequestChildListFromDifferentComponent_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestChildListFromDifferentComponent_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientRequestChildListFromDifferentComponent_Success:test", 12);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  l_config_server.putCfg("client_server_load_config.json");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto paramVal = ConfigClient::getConfig()->getChildren("differentComponentStrList:orchestrator");
  // should be value from server config
  ASSERT_EQ(paramVal.size(), 2);
  EXPECT_EQ(paramVal[0], "flow1");
  EXPECT_EQ(paramVal[1], "flow2");
  ConfigClient::do_reset();
}

TEST_F(ClientServerConfigTests, test_ClientRequestChildListFromDifferentComponentTimeout_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientRequestChildListFromDifferentComponentTimeout_Success");
  ConfigClient::do_init(name);
  ConfigClient::getConfig()->put("test_ClientRequestChildListFromDifferentComponentTimeout_Success:test", 12);
  ConfigClient::finish_init();

  auto paramVal = ConfigClient::getConfig()->getChildren("differentComponentStrList:orchestrator");
  // should be value from server config
  ASSERT_EQ(paramVal.size(), 0);
  ConfigClient::do_reset();
}

// TODO currently generic tags are not working. Enabled once tag system is implemented
TEST_F(ClientServerConfigTests, DISABLED_test_ClientUpdateFromGenericAtInit_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientUpdateFromGenericAtInit_Success");

  next::appsupport::ConfigServer l_config_server(name);
  l_config_server.put("GENERIC:test_to_update", 5);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtInit_Success:test", 10);
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtInit_Success:test_to_update", 10);
  ConfigClient::finish_init();

  auto paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtInit_Success:test", -1);
  // should be value from local config
  ASSERT_EQ(paramVal, 10);
  paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtInit_Success:test_to_update", -1);
  // should be value from server GENERIC
  ASSERT_EQ(paramVal, 5);
  ConfigClient::do_reset();
}
// TODO currently generic tags are not working. Enabled once tag system is implemented
TEST_F(ClientServerConfigTests, DISABLED_test_ClientUpdateFromGenericAtInitWithCMD_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientUpdateFromGenericAtInitWithCMD_Success");

  next::appsupport::ConfigServer l_config_server(name);
  l_config_server.put("GENERIC:test_to_update", 5);
  l_config_server.put("GENERIC:from_cmd", 5);

  auto cmd_options = next::appsupport::CmdOptions(name);
  cmd_options.addParameterInt("from_cmd,f", "debug from_cmd2",
                              "test_ClientUpdateFromGenericAtInitWithCMD_Success:from_cmd");

  const char *argv[] = {"cmdoption.exe", "-f", "1"};

  ConfigClient::do_init(name);
  ASSERT_TRUE(cmd_options.parseCmd(3, argv));
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtInitWithCMD_Success:test", 10);
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtInitWithCMD_Success:test_to_update", 10);
  ConfigClient::finish_init();

  auto paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtInitWithCMD_Success:test", -1);
  // should be value from local config
  ASSERT_EQ(paramVal, 10);
  paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtInitWithCMD_Success:test_to_update", -1);
  // should be value from server GENERIC
  ASSERT_EQ(paramVal, 5);
  paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtInitWithCMD_Success:from_cmd", -1);
  // should be value from cmd param
  ASSERT_EQ(paramVal, 1);
  ConfigClient::do_reset();
}
// TODO currently generic tags are not working. Enabled once tag system is implemented
TEST_F(ClientServerConfigTests, DISABLED_test_ClientUpdateFromGenericAtRuntime_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientUpdateFromGenericAtRuntime_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtRuntime_Success:test", 10);
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", 10);
  ConfigClient::finish_init();

  auto paramVal =
      ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", -1);
  ASSERT_EQ(paramVal, 10);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.pushConfig("{\"GENERIC\":{\"test_to_update\":5}}");

  paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", -1);
  // should be value from server GENERIC
  ASSERT_EQ(paramVal, 5);

  ConfigClient::do_reset();
}
// TODO currently generic tags are not working. Enabled once tag system is implemented
TEST_F(ClientServerConfigTests, DISABLED_test_ClientOverwriteGenericAtRuntime_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientOverwriteGenericAtRuntime_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientOverwriteGenericAtRuntime_Success:test", 10);
  ConfigClient::getConfig()->put("test_ClientOverwriteGenericAtRuntime_Success:test_to_update", 10);
  ConfigClient::getConfig()->put("test_ClientOverwriteGenericAtRuntime_Success:test_to_update_from_generic", 10);
  ConfigClient::finish_init();

  auto paramVal = ConfigClient::getConfig()->get_int("test_ClientOverwriteGenericAtRuntime_Success:test_to_update", -1);
  ASSERT_EQ(paramVal, 10);
  paramVal = ConfigClient::getConfig()->get_int(
      "test_ClientOverwriteGenericAtRuntime_Success:test_to_update_from_generic", -1);
  ASSERT_EQ(paramVal, 10);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.pushConfig("{\"GENERIC\":{\"test_to_update\":5,\"test_to_update_from_generic\":5}}");

  paramVal = ConfigClient::getConfig()->get_int("test_ClientOverwriteGenericAtRuntime_Success:test_to_update", -1);
  // should be value from server config
  ASSERT_EQ(paramVal, 5);
  paramVal = ConfigClient::getConfig()->get_int(
      "test_ClientOverwriteGenericAtRuntime_Success:test_to_update_from_generic", -1);
  // should be value from server config
  ASSERT_EQ(paramVal, 5);

  l_config_server.pushConfig("{\"GENERIC\":{\"test_to_update\":5,\"test_to_update_from_generic\":5},"
                             "\"test_ClientOverwriteGenericAtRuntime_Success\": {\"test_to_update\":15}}");
  paramVal = ConfigClient::getConfig()->get_int("test_ClientOverwriteGenericAtRuntime_Success:test_to_update", -1);
  // should be value from server config component side
  ASSERT_EQ(paramVal, 15);
  paramVal = ConfigClient::getConfig()->get_int(
      "test_ClientOverwriteGenericAtRuntime_Success:test_to_update_from_generic", -1);
  // should be value from server config
  ASSERT_EQ(paramVal, 5);

  ConfigClient::do_reset();
}
// TODO currently generic tags are not working. Enabled once tag system is implemented
TEST_F(ClientServerConfigTests, DISABLED_test_RemoveGenericAtSave_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_RemoveGenericAtSave_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->setConfigServerSendTime(50);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_RemoveGenericAtSave_Success:test", 10);
  ConfigClient::getConfig()->put("test_RemoveGenericAtSave_Success:test_to_update", 10);
  ConfigClient::finish_init();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  l_config_server.insert("{\"GENERIC\":{\"test_to_update\":10,\"test\":5}}");

  // should remove all values from the component that are equal with the values in the GENERIC
  auto as_json = boost::json::parse(l_config_server.getAllConfigurationsAsJson(false, true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test_RemoveGenericAtSave_Success\":{\"test\":\"10\"},\"GENERIC\":{\"test_to_update\":"
                             "\"10\",\"test\":\"5\"}}"};
  ConfigClient::do_reset();
  EXPECT_EQ(ss.str(), expected);
}

TEST_F(ClientServerConfigTests, test_PushLocalUpdateToServerOldDelimiter_success) {

  eCAL::Util::EnableLoopback(true);
  std::string name("component_test");

  appsupport::ConfigClient::do_init(name);
  appsupport::ConfigClient::finish_init();
  ConfigClient::getConfig()->setConfigServerSendTime(50);

  ConfigServer l_config_server(name);

  ConfigClient::getConfig()->put("component_test:value", 1);
  ConfigClient::getConfig()->put("component_test:out_port:port.name:port:size", 2);
  ConfigClient::getConfig()->put("component_test:out_port:port.name:port:version", 21);
  int counter = 0;
  while (l_config_server.get_int("component_test:out_port:port.name:port:version", 0) != 21) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_LE(counter++, 10) << "Test aborted due to timeout. Value was not received";
  }
  auto contentServer = l_config_server.getAllConfigurationsAsJson();
  auto contentClient = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ(contentServer, contentClient);
}
TEST_F(ClientServerConfigTests, DISABLED_test_UpdateTaggedVariableByServer) {

  eCAL::Util::EnableLoopback(true);
  std::string name("test");

  appsupport::ConfigClient::do_init(name);
  appsupport::ConfigClient::finish_init();
  ConfigClient::getConfig()->setConfigServerSendTime(50);
  ConfigClient::getConfig()->put("test:dummy_int", 99);
  // comment in once it's implemented
  /*ConfigClient::getConfig()->setTag("test:dummy_int", "custom_tag");
  auto tagValue = ConfigClient::getConfig()->getTags("test:dummy_int");
  EXPECT_EQ(tagValue.size(), 1);
  ConfigServer l_config_server(name);
  l_config_server.putCfg("client_server_load_config.json");
  l_config_server.pushGlobalConfig();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  auto intVal = ConfigClient::getConfig()->get_int("test:dummy_int");
  EXPECT_EQ(intVal, 80);
  tagValue = ConfigClient::getConfig()->getTags("test:dummy_int");
  auto strjsn = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ(tagValue.size(), 2);
  if (tagValue.size()) {
    EXPECT_EQ(tagValue[0], "custom_tag");
  }*/
}

} // namespace events
} // namespace sdk
} // namespace next
