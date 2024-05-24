
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
class MultipleClientServerConfigTests : public testing::TestUsingEcal {
public:
  MultipleClientServerConfigTests() : TestUsingEcal() {
    this->instance_name_ = "ClientServerConfigTests";
    logger::register_to_init_chain();
  }
  ~MultipleClientServerConfigTests() { ConfigClient::do_reset(); }
};

TEST_F(MultipleClientServerConfigTests, test_MultipleClientRequestConfigAtInit_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("request_config_from_server_at_init");
  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::do_init("request_config_from_server_at_init_instance2");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  l_config_server.putCfg("client_server_load_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  ConfigClient::getConfig()->put("request_config_from_server_at_init:lower_level:lower_level2:dummy2", 12.4f);
  ConfigClient::getConfig()->put("request_config_from_server_at_init:dummy2", 12.4f);
  ConfigClient::getConfig()->put("request_config_from_server_at_init:dummy3", 2.4f);

  auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  auto server = l_config_server.getAllConfigurationsAsJson();

  ConfigClient::finish_init();
  cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  auto paramVal =
      ConfigClient::getConfig()->get_int("request_config_from_server_at_init:lower_level:lower_level2:dummy2", -1);

  paramVal = ConfigClient::getConfig()->get_int(
      "request_config_from_server_at_init_instance2:lower_level:lower_level2:dummy2", -1);
  // should be the value from the server config
  EXPECT_EQ(paramVal, 8);

  ConfigClient::do_reset();
}
TEST_F(MultipleClientServerConfigTests, test_MultipleClientLoadingSequence_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("load_configuration_sequence");
  next::appsupport::ConfigServer l_config_server(name);
  ConfigClient::do_init(name);
  ConfigClient::do_init("load_configuration_sequence_instance2");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  // process command option parametrs
  auto cmd_options = next::appsupport::CmdOptions(name);
  cmd_options.addParameterInt("debug_level,d", "debug level", "load_configuration_sequence:debug_level");
  cmd_options.addParameterInt("from_cmd2,f", "debug from_cmd2", "load_configuration_sequence:from_cmd2");
  cmd_options.addParameterInt("debug_level_instance2,x", "debug level_instance2",
                              "load_configuration_sequence_instance2:debug_level");
  cmd_options.addParameterInt("from_cmd2_instance2,z", "debug from_cmd2_instance2",
                              "load_configuration_sequence_instance2:from_cmd2");

  // add hardcoded parametrs
  ConfigClient::getConfig()->put("load_configuration_sequence:debug_level", 2);
  ConfigClient::getConfig()->put("load_configuration_sequence:lower_level:lower_level2:dummy3", 1.4f);
  ConfigClient::getConfig()->put("load_configuration_sequence_instance2:debug_level", 2);
  ConfigClient::getConfig()->put("load_configuration_sequence_instance2:lower_level:lower_level2:dummy3", 1.4f);

  const char *argv[] = {
      "cmdoption.exe", "-c", "client_server_load_config_cmd.json", "-d", "3", "-f", "7", "-x", "3", "-f", "z"};
  ASSERT_TRUE(cmd_options.parseCmd(9, argv));
  // cmdOption parameters should not be available and be loaded to the main tree only after finish_init
  auto cmdOptionParameter = ConfigClient::getConfig()->get_int("load_configuration_sequence:from_cmd2", -1);
  EXPECT_EQ(cmdOptionParameter, -1);
  cmdOptionParameter = ConfigClient::getConfig()->get_int("load_configuration_sequence_instance2:from_cmd2", -1);
  EXPECT_EQ(cmdOptionParameter, -1);

  // load default config
  ConfigClient::getConfig()->putCfg("client_server_load_config.json");
  [[maybe_unused]] auto cfgContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  ConfigClient::getConfig()->put("load_configuration_sequence:dummy_int", 12);
  ConfigClient::getConfig()->put("load_configuration_sequence:dummy3", 2.4f);
  ConfigClient::getConfig()->put("load_configuration_sequence_instance2:dummy_int", 12);
  ConfigClient::getConfig()->put("load_configuration_sequence_instance2:dummy3", 2.4f);

  // should hold the value from the default config
  auto updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);
  updateByServerParam = ConfigClient::getConfig()->get_int(
      "load_configuration_sequence_instance2:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);

  // load global config in the server
  l_config_server.putCfg("client_server_global_config.json");
  auto cfgContentServer = l_config_server.getAllConfigurationsAsJson();
  l_config_server.pushGlobalConfig();

  updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);
  updateByServerParam = ConfigClient::getConfig()->get_int(
      "load_configuration_sequence_instance2:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  ConfigClient::finish_init();

  next::sdk::logger::error(__FILE__, "finis_init_done");
  auto debugLevelParam = ConfigClient::getConfig()->get_int("load_configuration_sequence:debug_level", -1);
  // cmd provided value should be stored
  EXPECT_EQ(debugLevelParam, 3);
  debugLevelParam = ConfigClient::getConfig()->get_int("load_configuration_sequence_instance2:debug_level", -1);
  // cmd provided value should be stored
  EXPECT_EQ(debugLevelParam, 3);

  auto dummyStrParam = ConfigClient::getConfig()->get_string("load_configuration_sequence:dummy_str", "");
  // cmd configuration provided value should be stored
  EXPECT_EQ(dummyStrParam, "cmd_config");
  dummyStrParam = ConfigClient::getConfig()->get_string("load_configuration_sequence_instance2:dummy_str", "");
  // cmd configuration provided value should be stored
  EXPECT_EQ(dummyStrParam, "cmd_config");

  updateByServerParam =
      ConfigClient::getConfig()->get_int("load_configuration_sequence:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 1);
  updateByServerParam = ConfigClient::getConfig()->get_int(
      "load_configuration_sequence_instance2:lower_level:lower_level2:update_by_server", -1);
  EXPECT_EQ(updateByServerParam, 1);

  // updating parameter after finish_init should overwrite the value
  ConfigClient::getConfig()->put("load_configuration_sequence:debug_level", 12);
  debugLevelParam = ConfigClient::getConfig()->get_int("load_configuration_sequence:debug_level", -1);
  EXPECT_EQ(debugLevelParam, 12);
  ConfigClient::do_reset();
}

//! flag for requesting value from server at the same time
std::atomic<bool> startAtTheSameTime(true);
TEST_F(MultipleClientServerConfigTests, test_MultipleClientRequestFromDifferentComponent_Success) {
  eCAL::Util::EnableLoopback(true);
  next::appsupport::ConfigServer l_config_server("compTestServer");
  l_config_server.putCfg("client_server_load_config.json");

  ConfigClient::do_init("compTest1");
  ConfigClient::do_init("compTest2");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::finish_init();

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
    auto paramVal = ConfigClient::getConfig()->get_string("unused_component:strValue", "");
    EXPECT_EQ(paramVal, "string");
  }));

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  startAtTheSameTime = false;
  for (auto &thread : threadArray) {
    if (thread.joinable())
      thread.join();
  }
  ConfigClient::do_reset();
}

TEST_F(MultipleClientServerConfigTests, test_MultipleClientPushLocalUpdateToServer_Success) {

  eCAL::Util::EnableLoopback(true);
  std::string name("component_test");

  appsupport::ConfigClient::do_init(name);
  appsupport::ConfigClient::do_init("component_test2");
  ConfigClient::getConfig()->setConfigServerSendTime(10);

  appsupport::ConfigClient::finish_init();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ConfigServer l_config_server(name);

  ConfigClient::getConfig()->put("component_test:value", 1);
  ConfigClient::getConfig()->put("component_test2:value", 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  auto contentServer = l_config_server.getAllConfigurationsAsJson();
  auto contentClient = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ(contentServer, contentClient);
}

// TODO: activate once tag system is implemented in config classes
TEST_F(MultipleClientServerConfigTests, DISABLED_test_MultipleClientUpdateFromGenericAtRuntime_Success) {
  eCAL::Util::EnableLoopback(true);
  std::string name("test_ClientUpdateFromGenericAtRuntime_Success");

  next::appsupport::ConfigServer l_config_server(name);

  ConfigClient::do_init(name);
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtRuntime_Success:test", 10);
  ConfigClient::getConfig()->put("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", 10);

  ConfigClient::do_init("Comp2");
  ConfigClient::getConfig()->SetValidConfigServer(l_config_server.getRootName());
  ConfigClient::getConfig()->put("Comp2:test", 10);
  ConfigClient::getConfig()->put("Comp2:test_to_update", 10);

  ConfigClient::finish_init();

  auto strContent = ConfigClient::getConfig()->getAllConfigurationsAsJson();

  auto paramVal =
      ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", -1);
  ASSERT_EQ(paramVal, 10);

  l_config_server.pushConfig("{\"GENERIC\":{\"test_to_update\":5}}");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  paramVal = ConfigClient::getConfig()->get_int("test_ClientUpdateFromGenericAtRuntime_Success:test_to_update", -1);
  // should be value from server GENERIC
  ASSERT_EQ(paramVal, 5);
  paramVal = ConfigClient::getConfig()->get_int("Comp2:test_to_update", -1);
  // should be value from server GENERIC
  ASSERT_EQ(paramVal, 5);

  ConfigClient::do_reset();
}

TEST_F(MultipleClientServerConfigTests, DISABLED_test_insertMultipleClientsWithPrivateKey_Success) {
  ASSERT_TRUE(ConfigClient::do_init("test1"));
  ASSERT_TRUE(ConfigClient::do_init("test2"));
  ASSERT_TRUE(ConfigClient::do_init("test3"));
  // should not comunicate with the server
  ConfigClient::getConfig()->putCfg("client_server_load_config.json");
  // comment in once it's implemented
  /*ConfigClient::getConfig()->definePrivateConfigKeys({"test1:singleValue", "test1:singleValue2"});
  ConfigClient::getConfig()->definePrivateConfigKeys({"test2:singleValue_t2"});
  ConfigClient::getConfig()->definePrivateConfigKeys({"test3:singleValueEmpty"});*/

  ConfigClient::finish_init();
  ConfigClient::getConfig()->insert("{\"test1\": { \"singleValue2\": \"insert\", \"singleValueEmpty1\": "
                                    "\"insert\"},\"test2\": { \"new\": \"insert\", \"singleValue_t2\": "
                                    "\"insert\"},\"test3\": { \"new\": \"insert\", \"singleValueEmpty\": \"insert\"} }",
                                    true);
  auto str_content = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  auto var = ConfigClient::getConfig()->get_string("test1:singleValue", "");
  ASSERT_EQ(var, "from_CFG");
  var = ConfigClient::getConfig()->get_string("test2:singleValue_t2", "");
  ASSERT_EQ(var, "from_CFG");
  var = ConfigClient::getConfig()->get_string("test3:singleValueEmpty", "");
  ASSERT_EQ(var, "from_CFG");
}
TEST_F(MultipleClientServerConfigTests, DISABLED_test_filterByTag_Success) {

  ASSERT_TRUE(ConfigClient::do_init("test1"));
  ASSERT_TRUE(ConfigClient::do_init("test2"));

  ConfigClient::getConfig()->put("test1:dummy:subdumy:var1", 1);
  ConfigClient::getConfig()->put("test1:dummy:subdumy:var2", 1);
  ConfigClient::getConfig()->put("test1:dummy:subdumy:var3", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var1", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var2", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var3", 1);

  // comment in once it's implemented
  /* ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag");
   ConfigClient::getConfig()->setTag("test2:dummy:subdumy:var1", "custom_tag");
   ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag2");
   ConfigClient::getConfig()->setTag("test2:dummy:subdumy:var1", "custom_tag2");
   ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag3");
   ConfigClient::getConfig()->setTag("test2:dummy:subdumy:var1", "custom_tag3");
   ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var2", "custom_tag2");
   ConfigClient::getConfig()->setTag("test2:dummy:subdumy:var2", "custom_tag2");
   ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var3", "custom_tag3");
   ConfigClient::getConfig()->setTag("test2:dummy:subdumy:var3", "custom_tag3");

   auto as_json = boost::json::parse(ConfigClient::getConfig()->getTreeFilteredByTagAsJson("custom_tag"));
   std::stringstream ss;
   ss << as_json;
   const std::string expected{
       "{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}},\"test2\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}}}"};
   EXPECT_EQ(ss.str(), expected);

   as_json =
       boost::json::parse(ConfigClient::getConfig()->getTreeFilteredByTagsUnionAsJson({"custom_tag", "custom_tag2"}));
   std::stringstream ssUnion;
   ssUnion << as_json;
   const std::string expectedUnion{"{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\",\"var2\":\"1\"}}},\"test2\":{"
                                   "\"dummy\":{\"subdumy\":{\"var1\":\"1\",\"var2\":\"1\"}}}}"};
   EXPECT_EQ(ssUnion.str(), expectedUnion);
   as_json = boost::json::parse(
       ConfigClient::getConfig()->getTreeFilteredByTagsIntersectionAsJson({"custom_tag", "custom_tag2",
   "custom_tag3"})); std::stringstream ssIntersection; ssIntersection << as_json; const std::string
   expectedIntersection{
       "{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}},\"test2\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}}}"};
   EXPECT_EQ(ssIntersection.str(), expectedIntersection);*/
}
TEST_F(MultipleClientServerConfigTests, DISABLED_test_filterByTagsInOnlyOneCOmponent_Success) {

  ASSERT_TRUE(ConfigClient::do_init("test1"));
  ASSERT_TRUE(ConfigClient::do_init("test2"));

  ConfigClient::getConfig()->put("test1:dummy:subdumy:var1", 1);
  ConfigClient::getConfig()->put("test1:dummy:subdumy:var2", 1);
  ConfigClient::getConfig()->put("test1:dummy:subdumy:var3", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var1", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var2", 1);
  ConfigClient::getConfig()->put("test2:dummy:subdumy:var3", 1);

  // comment in once it's implemented
  /*ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag");
  ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag2");
  ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var1", "custom_tag3");
  ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var2", "custom_tag2");
  ConfigClient::getConfig()->setTag("test1:dummy:subdumy:var3", "custom_tag3");

  auto as_json = boost::json::parse(ConfigClient::getConfig()->getTreeFilteredByTagAsJson("custom_tag"));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}}}"};
  EXPECT_EQ(ss.str(), expected);

  as_json =
      boost::json::parse(ConfigClient::getConfig()->getTreeFilteredByTagsUnionAsJson({"custom_tag", "custom_tag2"}));
  std::stringstream ssUnion;
  ssUnion << as_json;
  const std::string expectedUnion{"{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\",\"var2\":\"1\"}}}}"};
  EXPECT_EQ(ssUnion.str(), expectedUnion);
  as_json = boost::json::parse(
      ConfigClient::getConfig()->getTreeFilteredByTagsIntersectionAsJson({"custom_tag", "custom_tag2", "custom_tag3"}));
  std::stringstream ssIntersection;
  ssIntersection << as_json;
  const std::string expectedIntersection{"{\"test1\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\"}}}}"};
  EXPECT_EQ(ssIntersection.str(), expectedIntersection);*/
}
} // namespace events
} // namespace sdk
} // namespace next
