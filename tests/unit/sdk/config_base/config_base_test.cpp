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

#include <algorithm>
#include <array>
#include <string>

#include <boost/json.hpp>

#include <next/sdk/sdk.hpp>

#include <next/appsupport/config/config_client.hpp>

#include "event_manager.h"

using next::appsupport::ConfigClient;
class ConfigBaseTests : public next::sdk::testing::TestUsingEcal {
public:
  ConfigBaseTests() { this->instance_name_ = "ConfigBaseTests"; }
};

class ConfigBaseTestClass : public next::appsupport::ConfigBase {
public:
  // deactiveate filtering of input data
  ConfigBaseTestClass(std::string name) : ConfigBase(name, false) {}
  ~ConfigBaseTestClass() { ConfigClient::do_reset(); };
  bool isInitFinished() const { return ConfigBase::isInitFinished(); }
  void finishInitialization() { ConfigBase::finishInitialization(); }
};

// disabled, cout catching will be improved, see https : // jira-adas.zone2.agileci.conti.de/browse/SIMEN-6194
TEST(ConfigBaseTestsWithoutEcal, DISABLED_basic_functionality) {
  testing::internal::CaptureStdout();
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "") << "Config functions should not create console output";
  ConfigClient::do_reset();
}

TEST_F(ConfigBaseTests, push_pull) {

  // command line output check disabled in this test, will be improved, see
  // https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-6194
  // testing::internal::CaptureStdout();
  ConfigClient::do_init("test");
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->put("test:tst_int", 10);
  ConfigClient::getConfig()->put("test:tst_float", 20.0f);
  ConfigClient::getConfig()->put("test:tst_string", "teststring");
  ConfigClient::getConfig()->put_option("tst_bool", true);
  auto stringVal = ConfigClient::getConfig()->getAllConfigurationsAsJson();
  EXPECT_EQ((stringVal.size() > 10), true);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("test:tst_int", 0), 10);
  EXPECT_EQ(ConfigClient::getConfig()->get_float("test:tst_float", 0.0f), 20.0f);
  EXPECT_EQ(ConfigClient::getConfig()->get_string("test:tst_string", ""), "teststring");
  EXPECT_NE(ConfigClient::getConfig()->get_option("tst_bool", false), true);
  // std::string output = testing::internal::GetCapturedStdout();
  // EXPECT_EQ(output, "") << "Config functions should not create console output";
  ConfigClient::do_reset();
}

TEST(ConfigBaseTestsWithoutEcal, node_cfg) {
  // command line output check disabled in this test, will be improved, see
  // https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-6194
  // testing::internal::CaptureStdout();
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg.json");
  EXPECT_EQ(ConfigClient::getConfig()->get_string("local.active_state", ""), "");
  // std::string output = testing::internal::GetCapturedStdout();
  // EXPECT_EQ(output, "") << "Config functions should not create console output";
  ConfigClient::do_reset();
}

TEST(ConfigBaseTestsWithoutEcal, node_multiple_cfg) {
  std::string sampleJson = "{\"insertJson\":{\"sample_1\":\"init\",\"sample_2\":123}}";
  std::string sampleJsonOK = "{\"CEM200_COH\":{\"sample_1\":\"init\",\"sample_2\":123}}";
  // command line output check disabled in this test, will be improved, see
  // https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-6194
  // testing::internal::CaptureStdout();
  ASSERT_TRUE(ConfigClient::do_init("CEM200_COH"));
  ASSERT_TRUE(ConfigClient::finish_init());
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });

  ConfigClient::getConfig()->putCfg("test.xml");
  auto posX = ConfigClient::getConfig()->get_float("CEM200_COH:Settings:Position:X", -1.0f);
  EXPECT_EQ(posX, 2.5f);

  ConfigClient::getConfig()->putCfg("node_cfg.json");
  ConfigClient::getConfig()->insert(sampleJson);
  ConfigClient::getConfig()->insert(sampleJsonOK);

  // check value manually inserted
  // shall be ignored because insertJson component is not added
  EXPECT_NE(ConfigClient::getConfig()->get_int("insertJson:sample_2", -1), 123);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("CEM200_COH:sample_2", -1), 123);
  // check value from xml
  EXPECT_EQ(ConfigClient::getConfig()->get_float("CEM200_COH:Settings:Position:X", -1.0f), 2.5f);
  // check values from json
  EXPECT_NE(ConfigClient::getConfig()->get_string("local:active_state", ""), "init");
  EXPECT_EQ(ConfigClient::getConfig()->get_string("CEM200_COH:in_port:m_pSefInputIf:port_vaddr", ""), "0x9000");

  // std::string output = testing::internal::GetCapturedStdout();
  // EXPECT_EQ(output, "") << "Config functions should not create console output";
  ConfigClient::do_reset();
}
TEST(ConfigBaseTestsWithoutEcal, node_cfg_invalid_file) {
  ASSERT_TRUE(ConfigClient::do_init("test"));
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("unexisting.json");
  EXPECT_EQ(ConfigClient::getConfig()->get_string("local.active_state", ""), "");
  ConfigClient::do_reset();
}

TEST(ConfigBaseTestsWithoutEcal, get_children) {
  // command line output check disabled in this test, will be improved, see
  // https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-6194
  // testing::internal::CaptureStdout();
  ASSERT_TRUE(ConfigClient::do_init("CEM200_COH"));
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });
  ConfigClient::getConfig()->putCfg("node_cfg.json");
  auto stri = ConfigClient::getConfig()->getAllConfigurationsAsJson();

  std::vector<std::string> clist_fullpath = ConfigClient::getConfig()->getChildren("CEM200_COH", true);
  ASSERT_EQ(clist_fullpath.size(), 4);
  EXPECT_EQ(clist_fullpath[0], "CEM200_COH:version");
  EXPECT_EQ(clist_fullpath[1], "CEM200_COH:in_port");
  EXPECT_EQ(clist_fullpath[2], "CEM200_COH:out_port");
  EXPECT_EQ(clist_fullpath[3], "CEM200_COH:composition");

  std::vector<std::string> clist = ConfigClient::getConfig()->getChildren("CEM200_COH");
  ASSERT_EQ(clist.size(), 4);
  EXPECT_EQ(clist[0], "version");
  EXPECT_EQ(clist[1], "in_port");
  EXPECT_EQ(clist[2], "out_port");
  EXPECT_EQ(clist[3], "composition");
  EXPECT_EQ(ConfigClient::getConfig()->get_string("CEM200_COH:" + clist[0], ""), "0.0.1");
  // std::string output = testing::internal::GetCapturedStdout();
  // EXPECT_EQ(output, "") << "Config functions should not create console output";
  ConfigClient::do_reset();
}

TEST(ConfigBaseTestsWithoutEcal, getChildren_noChildren_returnEmptyList) {
  ASSERT_TRUE(ConfigClient::do_init("CEM200_COH"));
  std::vector<std::string> clist = ConfigClient::getConfig()->getChildren("CEM200_COH");
  EXPECT_EQ(clist.size(), 0);
  ConfigClient::do_reset();
}

TEST(ConfigBaseTestsWithoutEcal, getArrayNoAlternatives) {
  auto config_base = ConfigBaseTestClass("test:getArrayNoAlternatives");
  config_base.putCfg("node_cfg_array_base.json");
  auto value = config_base.getStringList("requests:CEM200_COH:composition:m_pSefInputIf", {});
  ASSERT_EQ((value.size() > 0), true);
  EXPECT_EQ(value[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu") << "wrong value found";
}

TEST(ConfigBaseTestsWithoutEcal, getArrayAlternatives) {
  auto config_base = ConfigBaseTestClass("test:getArrayAlternatives");
  config_base.putCfg("node_cfg_array_base.json");

  auto result = config_base.getStringList("requests:CEM200_COH:composition:m_pEmlInputIf", {});

  // arrays of objects are not supported anymore
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
}

TEST(ConfigBaseTestsWithoutEcal, getArraySubtree) {
  auto config_base = ConfigBaseTestClass("test:getArraySubtree");
  config_base.putCfg("node_cfg_array_base.json");

  // returns only values at this node level not from sub childs
  auto test = config_base.getStringList("requests", {});

  ASSERT_EQ(test.size(), 0);
}

TEST(ConfigBaseTestsWithoutEcal, getArrayInvalidSchema) {
  auto config_base = ConfigBaseTestClass("test:getArrayInvalidSchema");
  config_base.putCfg("node_cfg_array_base.json");

  auto test = config_base.getStringList("requests:CEM200_COH:composition:m_pEmlInputIffail", {});

  // array of objects are ignored
  ASSERT_EQ(test.size(), 1) << "value found with invalid key";
  EXPECT_EQ(test[0], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
}

TEST(ConfigBaseTestsWithoutEcal, getKeyValueHiddenInArrayStructure) {
  auto config_base = ConfigBaseTestClass("test.getKeyValueHiddenInArrayStructure");
  config_base.putCfg("node_cfg_array_base.json");

  auto test = config_base.getStringList("requests.CEM200_COH.composition.m_pEmlInputIffail.alternatives.asdf", {});

  EXPECT_EQ(test.size(), 0) << "value found with invalid key";
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_matchingTypes_correctValue) {
  auto cb = ConfigBaseTestClass("test.get.matchingTypes");
  cb.putCfg("node_cfg_array_base.json");

  const int half_truth = 21;
  const int half_truth_neg = -21;
  const int truth = 42;
  const float abs_zero_temp = -273.15f;
  const float gravitational_const = 6.6743f; // without *10E11
  const float euler_const = 2.718f;
  const std::string archimedes{"eureka"};
  const std::string lorem_ipsum{"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt "
                                "ut labore et dolore magna aliqua."};

  EXPECT_EQ(cb.get_string("pos_int", ""), std::to_string(half_truth));
  EXPECT_EQ(cb.get_string("neg_int", ""), std::to_string(half_truth_neg));
  EXPECT_EQ(cb.get_string("pos_float", ""), "6.6743");
  EXPECT_EQ(cb.get_string("neg_float", ""), "-273.15");
  EXPECT_EQ(cb.get_string("pos_float", ""), "6.6743");
  EXPECT_EQ(cb.get_string("float_string", ""), "2.718");
  EXPECT_EQ(cb.get_string("int_string", ""), "42");
  EXPECT_EQ(cb.get_string("short_string", ""), archimedes);
  EXPECT_EQ(cb.get_string("long_string", ""), lorem_ipsum);

  EXPECT_EQ(cb.get_int("pos_int", 0), half_truth);
  EXPECT_EQ(cb.get_int("neg_int", 0), half_truth_neg);
  EXPECT_EQ(cb.get_int("int_string", 0), truth);

  EXPECT_EQ(cb.get_float("pos_int", 0), half_truth);
  EXPECT_EQ(cb.get_float("neg_int", 0), half_truth_neg);
  EXPECT_EQ(cb.get_float("pos_float", 0), gravitational_const);
  EXPECT_EQ(cb.get_float("neg_float", 0), abs_zero_temp);
  EXPECT_EQ(cb.get_float("float_string", 0), euler_const);
  EXPECT_EQ(cb.get_float("int_string", 0), truth);
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_notMatchingTypes_defaultValue) {
  auto cb = ConfigBaseTestClass("test.get.notMatchingTypes");
  cb.putCfg("node_cfg_array_base.json");

  // not testing for string here, it is possible to fetch everything as a string

  EXPECT_EQ(cb.get_int("requests.CEM200_COH.composition.m_pSefInputIf[0]", 0), 0)
      << "reading a string as int should fail";
  EXPECT_EQ(cb.get_int("requests.CEM200_COH.composition.m_floatArray[0]", 0), 0)
      << "reading a float as int should fail";

  // reading a integer as float is fine, so only testing to read a string
  EXPECT_EQ(cb.get_float("requests.CEM200_COH.composition.m_pSefInputIf[0]", 0.f), 0.f)
      << "reading a string as float should fail";
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_invalidKeys_defaultValue) {
  auto cb = ConfigBaseTestClass("test.get.invalidKeys");
  cb.putCfg("node_cfg_array_base.json");

  EXPECT_EQ(cb.get_string("invalid.key", ""), "");
  EXPECT_EQ(cb.get_string("invalid.key", "NotAvailable"), "NotAvailable");

  EXPECT_EQ(cb.get_int("invalid.key", 0), 0);
  EXPECT_EQ(cb.get_int("invalid.key", -21), -21);

  EXPECT_EQ(cb.get_float("invalid.key", 0.f), 0.f);
  EXPECT_EQ(cb.get_float("invalid.key", 6.6743f), 6.6743f); // G without *10^-11
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_readingFromArray_correctValue) {
  auto cb = ConfigBaseTestClass("test:get:readFromArray");
  cb.putCfg("node_cfg_array_base.json");

  EXPECT_EQ(cb.get_string("requests:CEM200_COH:composition:m_pEmlInputIf", ""), "MFC5xx Device.FCU.ARSSepTuple3_Fcu")
      << "failed to get string value from array";

  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intArray[0]", 0), 1) << "failed to get int value from array";
  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intArray[1]", 0), 2) << "failed to get int value from array";
  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intArray[2]", 0), 3) << "failed to get int value from array";

  // is handled in the same way as index 0
  EXPECT_EQ(cb.get_string("requests:CEM200_COH:composition:m_intArrayFail", ""), "a")
      << "not able to receive single element from intFail array";

  EXPECT_EQ(cb.get_string("requests:CEM200_COH:composition:m_intArrayFail[0]", ""), "a")
      << "not able to receive single element from intFail array";
  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intArrayFail[1]", 0), 2)
      << "not able to receive single element from intFail array";
  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intArrayFail[2]", 0), 3)
      << "not able to receive single element from intFail array";

  EXPECT_EQ(cb.get_float("requests:CEM200_COH:composition:m_floatArray", 0.f), 1.5f)
      << "not able to receive single element from float array";

  EXPECT_EQ(cb.get_string("requests:CEM200_COH:composition:m_floatArrayFail[0]", ""), "a")
      << "should not be possible to receive a non-numeric string as float";
  EXPECT_EQ(cb.get_float("requests:CEM200_COH:composition:m_floatArrayFail[1]", 0.f), 2.5f)
      << "should not be possible to receive a non-numeric string as float";
  EXPECT_EQ(cb.get_float("requests:CEM200_COH:composition:m_floatArrayFail[2]", 0.f), 3.5f)
      << "should not be possible to receive a non-numeric string as float";

  EXPECT_EQ(cb.get_int("requests:CEM200_COH:composition:m_intFail", 1), 1)
      << "it should not be possible to receive a non-numeric string as int";
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_readSimpleKeyFromArray_defaultValue) {
  auto cb = ConfigBaseTestClass("test.get.simpleKeyFromArray");
  cb.putCfg("node_cfg_array_base.json");

  EXPECT_EQ(cb.get_string("requests.CEM200_COH.composition.m_pEmlInputIf", ""), "");
  EXPECT_EQ(cb.get_int("requests.CEM200_COH.composition.m_intArray", 0), 0);
  EXPECT_EQ(cb.get_float("requests.CEM200_COH.composition.m_floatArray", 0.f), 0.f);
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_readArrayInsideObject_correctValues) {
  auto cb = ConfigBaseTestClass("test:get:arrayInsideObject");
  cb.putCfg("node_cfg_array_base.json");

  EXPECT_EQ(cb.get_int("object:array[0]", 0), 1);
  EXPECT_EQ(cb.get_int("object:array[1]", 0), 2);
  EXPECT_EQ(cb.get_int("object:array[2]", 0), 3);
  // array objects not supported
  EXPECT_EQ(cb.get_string("object:array[3]:string in array", ""), "");
}

TEST(ConfigBaseTestsWithoutEcal, getFunctions_readFromNestedArrayRootLevel_correctValue) {
  auto cb = ConfigBaseTestClass("test:get:readFromNestedArrayRootLevel");
  cb.putCfg("node_cfg_array_base.json");

  // nested arrays are not supported
  EXPECT_EQ(cb.get_int("nested_array[0][0]", 0), 0);
}

TEST(ConfigBaseTestsWithoutEcal, putFunctions_simpleKeyInEmptyConfig_keyAdded) {
  auto cb = ConfigBaseTestClass("test:put:simpleKey:success");

  {
    const std::string key{"simple:key"};
    const std::string value{"first config entry"};
    cb.put(key, value);
    // ASSERT_NO_THROW(cb.put(key, value));
    EXPECT_EQ(cb.get_string(key, ""), value);
  }

  {
    const std::string key{"even simpler key"};
    const int value{3};
    cb.put(key, value);
    // ASSERT_NO_THROW(cb.put(key, value));
    EXPECT_EQ(cb.get_int(key, value), value);
  }
}

TEST(ConfigBaseTestsWithoutEcal, putFunctions_updatingExistingSimpleValue_valueUpdated) {
  auto cb = ConfigBaseTestClass("test:put:updateExistingSimpleValue:success");
  cb.putCfg("node_cfg_array_base.json");

  {
    const std::string key{"pos_int"};
    const int old_int = cb.get_int(key, 0);
    ASSERT_EQ(old_int, 21);
    const int new_int = old_int * 2;
    ASSERT_NO_THROW(cb.put(key, new_int));
    EXPECT_EQ(cb.get_int(key, 0), new_int) << "failed to update for key '" << key << "'";
  }

  {
    const std::string key{"pos_float"};
    const float old_float = cb.get_float(key, 0.f);
    ASSERT_EQ(old_float, 6.6743f);
    const float new_float = old_float * 2.f;
    ASSERT_NO_THROW(cb.put(key, new_float));
    EXPECT_EQ(cb.get_float(key, 0.f), new_float) << "failed to update for key '" << key << "'";
  }

  {
    const std::string key{"short_string"};
    const std::string old_string = cb.get_string(key, "");
    ASSERT_EQ(old_string, "eureka");
    const std::string new_string{"Hello world!"};
    ASSERT_NO_THROW(cb.put(key, new_string));
    EXPECT_EQ(cb.get_string(key, ""), new_string) << "failed to update for key '" << key << "'";
  }
}

TEST(ConfigBaseTestsWithoutEcal, putFunctions_updatingExistingEmbeddedArray_valueUpdated) {
  auto cb = ConfigBaseTestClass("test:put:updateExistingEmbeddedArray:success");
  cb.putCfg("node_cfg_array_base.json");

  {
    const std::string key{"object:array[1]"};
    const int old_int = cb.get_int(key, 0);
    ASSERT_EQ(old_int, 2);
    const int new_int = old_int * 2;
    ASSERT_NO_THROW(cb.put(key, new_int));
    EXPECT_EQ(cb.get_int(key, 0), new_int) << "failed to update for key '" << key << "'";
  }
}

TEST(ConfigBaseTestsWithoutEcal, putFunctions_arrayInEmptyConfig_keyAdded) {

  // hint arrays on the component level are not supported
  const std::array<const std::string, 2> paths{std::string{"path:to:array"}, std::string{"array:to"}};

  for (const auto &path : paths) {
    auto cb = ConfigBaseTestClass("test:put:array:success");

    const std::array<const float, 4> values{-11.f, 21.f, 3.14f, 42.f};

    for (size_t i = 0; values.size() > i; ++i) {
      const std::string p = path + "[" + std::to_string(i) + "]";
      cb.put(p, values[i]);
    }
    for (size_t i = values.size(); i > 0; --i) {
      const std::string p = path + "[" + std::to_string(i - 1) + "]";
      EXPECT_EQ(cb.get_float(p, 0.f), values[i - 1]) << "failed for path " << p;
    }

    cb.put("path:to:object:int", 1);
    cb.put("path:to:object:float", 2.f);
    EXPECT_EQ(cb.get_int("path:to:object:int", 0), 1);
    EXPECT_EQ(cb.get_int("path:to:object:float", 0), 2.f);
  }
}

TEST(ConfigBaseTestsWithoutEcal, putFunctions_getResultAsJson_correctFormatting) {
  auto cb = ConfigBaseTestClass("test:put:getAsJson:success");

  cb.put("array:to[0]", 1);
  cb.put("array:to[1]", 2.5f);
  cb.put("array:to[2]", "three");
  cb.put("simple:object:path:int", 1);
  cb.put("path:with:array[0]", 0);
  cb.put("path:with:array[1]", 1);

  ASSERT_EQ(cb.get_int("array:to[0]", 0), 1);
  ASSERT_EQ(cb.get_float("array:to[1]", 0.f), 2.5f);
  ASSERT_EQ(cb.get_string("array:to[2]", ""), std::string("three"));
  ASSERT_EQ(cb.get_int("simple:object:path:int", 0), 1);
  ASSERT_EQ(cb.get_int("simple:object:path:int", 0), 1);
  ASSERT_EQ(cb.get_int("path:with:array[0]", 0), 0);
  ASSERT_EQ(cb.get_int("path:with:array[1]", 0), 1);

  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson());

  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"array\":{\"to\":[\"1\",\"2.5\",\"three\"]},\"path\":{\"with\":{\"array\":[\"0\",\"1\"]"
                             "}},\"simple\":{\"object\":{\"path\":{\"int\":\"1\"}}}}"};

  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, putFunctions_getResultAsJson_single_component) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("component1:dummy", 1);
  cb.put("component2:dummy", 2);

  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson());
  std::stringstream ss;
  ss << as_json;
  std::string expected{"{\"component1\":{\"dummy\":\"1\"},\"component2\":{\"dummy\":\"2\"}}"};
  EXPECT_EQ(ss.str(), expected);

  auto as_json_singleComponent = boost::json::parse(cb.getComponentConfigurationsAsJson("component1"));
  ss.str("");
  ss.clear();
  ss << as_json_singleComponent;
  expected = "{\"component1\":{\"dummy\":\"1\"}}";
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, putFunctions_addUrlWithEmptyValue) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test::value", 1);

  EXPECT_EQ(cb.get_int("test::value", -1), -1);
  cb.put("test:value", 1);
  EXPECT_EQ(cb.get_int("test:value", -1), 1);
  cb.put("test:value::value2", 1);
  EXPECT_EQ(cb.get_int("test:value::value2", -1), -1);
  cb.put(":test:value:value2", 1);
  EXPECT_EQ(cb.get_int(":test:value:value2", -1), -1);
  cb.put("[0]::value:value2", 1);
  EXPECT_EQ(cb.get_int("[0]::value:value2", -1), -1);
  // check if nothing bad formmated was added
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson());
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"value\":\"1\"}}"};
  EXPECT_EQ(ss.str(), expected);
}

TEST(ConfigBaseTestsWithoutEcal, put_bool_default) {
  auto cb = ConfigBaseTestClass("test");
  cb.put_option("test:bool", false, next::appsupport::ConfigSource::Component);
  // should overwrite because default source should be "ConfigSource::Component"
  cb.put_option("test:bool", true);
  cb.finishInitialization();

  EXPECT_TRUE(cb.get_option("test:bool", false));
}

TEST(ConfigBaseTestsWithoutEcal, put_bool_differentSource) {
  auto cb = ConfigBaseTestClass("test");
  cb.put_option("test:bool", false, next::appsupport::ConfigSource::CmdOptionsConfig);
  // should not overwrite because of default ConfigSource "ConfigSource::Component"
  cb.put_option("test:bool", true);
  cb.finishInitialization();

  EXPECT_FALSE(cb.get_option("test:bool", true));
}

TEST(ConfigBaseTestsWithoutEcal, put_int_default) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:int", 1, next::appsupport::ConfigSource::Component);
  // should overwrite because default source should be "ConfigSource::Component"
  cb.put("test:int", 2);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test:int", 0), 2);
}

TEST(ConfigBaseTestsWithoutEcal, put_int_differentSource) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:int", 1, next::appsupport::ConfigSource::CmdOptionsParameters);
  // should not overwrite because of default ConfigSource "ConfigSource::Component"
  cb.put("test:int", 2);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test:int", 0), 1);
}

TEST(ConfigBaseTestsWithoutEcal, put_float_default) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:float", 1.f, next::appsupport::ConfigSource::Component);
  // should overwrite because default source should be "ConfigSource::Component"
  cb.put("test:float", 2.f);
  cb.finishInitialization();

  EXPECT_FLOAT_EQ(cb.get_float("test:float", 0), 2.f);
}

TEST(ConfigBaseTestsWithoutEcal, put_float_differentSource) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:float", 1.f, next::appsupport::ConfigSource::ConfigServer);
  // should not overwrite because of default ConfigSource "ConfigSource::Component"
  cb.put("test:float", 2.f);
  cb.finishInitialization();

  EXPECT_FLOAT_EQ(cb.get_float("test:float", 0), 1.f);
}

TEST(ConfigBaseTestsWithoutEcal, put_string_default) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:string", "1", next::appsupport::ConfigSource::Component);
  // should not be overwritten by default "ConfigSource"
  cb.put("test:string", "2");
  cb.finishInitialization();

  EXPECT_EQ(cb.get_string("test:string", "0"), "2");
}

TEST(ConfigBaseTestsWithoutEcal, put_string_differentSource) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:string", "1", next::appsupport::ConfigSource::ConfigServer);
  // should not be overwritten by default "ConfigSource"
  cb.put("test:string", "2");
  cb.finishInitialization();

  EXPECT_EQ(cb.get_string("test:string", "0"), "1");
}

TEST(ConfigBaseTestsWithoutEcal, put_afterInit) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:int", 2, next::appsupport::ConfigSource::Component);
  // will be overwritten by "ConfigSource::ConfigServer" during finish_init()
  cb.put("test:int", 1, next::appsupport::ConfigSource::ConfigServer);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test:int", 0), 1);

  // will overwrite value because after init all values are added to the current tree
  cb.put("test:int", 3, next::appsupport::ConfigSource::Component);
  EXPECT_EQ(cb.get_int("test:int", 0), 3);
  cb.put("test:int", 4, next::appsupport::ConfigSource::ConfigServer);
  EXPECT_EQ(cb.get_int("test:int", 0), 4);
}

TEST(ConfigBaseTestsWithoutEcal, put_different_sources) {
  auto cb = ConfigBaseTestClass("test");
  // "ConfigSource::CmdOptionsParameters" has the highest priority of the following ConfigSources and
  // shall not be overwriten by the afterwards added values
  cb.put("test:int", 1, next::appsupport::ConfigSource::CmdOptionsParameters);
  cb.put("test:int", 2, next::appsupport::ConfigSource::CmdOptionsConfig);
  cb.put("test:int", 3, next::appsupport::ConfigSource::ConfigServer);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test:int", 0), 1);
}

TEST(ConfigBaseTestsWithoutEcal, insert_default) {
  std::string sampleJsonDefault = "{\"test\":1}";
  std::string sampleJsonServer = "{\"test\":2}";
  auto cb = ConfigBaseTestClass("test");
  cb.insert(sampleJsonServer, false, false, next::appsupport::ConfigSource::ConfigServer);
  // should not overwrite because of lower prio
  cb.insert(sampleJsonDefault);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test", 0), 2);
}

TEST(ConfigBaseTestsWithoutEcal, insert_different_sources) {
  std::string sampleJsonDefault = "{\"test\":1}";
  std::string sampleJsonServer = "{\"test\":2}";
  auto cb = ConfigBaseTestClass("test");
  cb.insert(sampleJsonDefault, false, false, next::appsupport::ConfigSource::CmdOptionsParameters);
  // should not overwrite because of lower prio
  cb.insert(sampleJsonServer, false, false, next::appsupport::ConfigSource::ConfigServer);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_int("test", 0), 1);
}

TEST(ConfigBaseTestsWithoutEcal, putCfg_default) {
  auto cb = ConfigBaseTestClass("list_check_test");

  cb.putCfg("node_cfg.json", next::appsupport::ConfigSource::ConfigServer);
  // should not overwrite because of lower prio
  cb.putCfg("node_cfg_diff.json");
  cb.finishInitialization();

  EXPECT_EQ(cb.get_string("list_check_test:normal_variable", ""), "val1");
}

TEST(ConfigBaseTestsWithoutEcal, putCfg_different_sources) {
  auto cb = ConfigBaseTestClass("list_check_test");
  cb.putCfg("node_cfg.json", next::appsupport::ConfigSource::CmdOptionsParameters);
  cb.putCfg("node_cfg_diff.json", next::appsupport::ConfigSource::ConfigServer);
  cb.finishInitialization();

  EXPECT_EQ(cb.get_string("list_check_test:normal_variable", ""), "val1");
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyDefinition_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:version", "test:profiling"});
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson());
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"@next_private_key_list#\":[\"test:version\",\"test:profiling\"]}}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyFilteringSimpleKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:dummy", "test:version"});
  cb.put("test:dummy", 1);
  cb.put("test:dummy2", 2);
  cb.put("test:version", 7);
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy2\":\"2\"}}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyFilteringEntireComponent_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test", "test:version"});
  cb.put("test:dummy", 1);
  cb.put("test:dummy2", 2);
  cb.put("test:version", 7);
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyFilteringArrayKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:array"});
  cb.put("test:dummy", 1);
  cb.put("test:dummy2", 2);
  cb.put("test:array[0]", 1);
  cb.put("test:array[1]", 2.5f);
  cb.put("test:array[2]", "three");
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy\":\"1\",\"dummy2\":\"2\"}}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyFilteringEntireObject_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:object"});
  cb.put("test:dummy", 1);
  cb.put("test:object:secondLevel", 1);
  cb.put("test:object:secondLevel2", 2);
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy\":\"1\"}}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyFilteringObjectKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:object:secondLevel", "test:object1:secondLevel"});
  cb.put("test:dummy", 1);
  cb.put("test:object1:secondLevel", 1);
  cb.put("test:object:secondLevel", 1);
  cb.put("test:object:secondLevel2", 2);
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy\":\"1\",\"object1\":\"\",\"object\":{\"secondLevel2\":\"2\"}}}"};
  EXPECT_EQ(ss.str(), expected);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_PrivateKeyInsertKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  // comment in once it's implemented
  // cb.definePrivateConfigKeys({"test:object:secondLevel"});
  /*cb.put("test:dummy", 1);
  cb.put("test:object1:secondLevel", 1);
  cb.put("test:object:secondLevel", 1);
  cb.put("test:object:secondLevel2", 2);

  cb.putPrivateKey("test:object1:secondLevel");*/
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy\":\"1\",\"object1\":\"\",\"object\":{\"secondLevel2\":\"2\"}}}"};
  EXPECT_EQ(ss.str(), expected);
}

TEST(ConfigBaseTestsWithoutEcal, test_OldDelimiterInKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy.subdumy:var", 1);
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:out_port:port.name:port:size", 2);
  cb.put("test:out_port:port.name:port:version", 21);
  auto as_json = boost::json::parse(cb.getAllConfigurationsAsJson(true));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test\":{\"dummy\":{\"subdumy\":{\"var2\":\"1\"}},\"dummy.subdumy\":{\"var\":\"1\"},"
                             "\"out_port\":{\"port.name\":{\"port\":{\"size\":\"2\",\"version\":\"21\"}}}}}"};
  EXPECT_EQ(ss.str(), expected);
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_AddTagForKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:v1", 54);
  // comment in once it's implemented
  // cb.setTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::ACTIVE);
  // cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  // auto getTags = cb.getTags("test:dummy:subdumy:var2");
  // EXPECT_EQ(getTags.size(), 2);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_AddTagForArrayKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:array[2]:var2", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var2", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var2", next::appsupport::configtags::TAG::ACTIVE);
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[0]", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_tag");
  auto getTags = cb.getTags("test:dummy:subdumy:array[1]:var2");
  EXPECT_EQ(getTags.size(), 2);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_RemoveTagForKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:v1", 54);
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::ACTIVE);
  cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  auto getTags = cb.getTags("test:dummy:subdumy:var2");
  EXPECT_EQ(getTags.size(), 2);
  cb.removeTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::ACTIVE);
  getTags = cb.getTags("test:dummy:subdumy:var2");
  EXPECT_EQ(getTags.size(), 1);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_RemoveTagForArrayKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:array[1]:var2", next::appsupport::configtags::TAG::ACTIVE);

  auto str_json = cb.getAllConfigurationsAsJson();
  auto getTags = cb.getTags("test:dummy:subdumy:array[1]:var2");

  str_json = cb.getAllConfigurationsAsJson();
  EXPECT_EQ(getTags.size(), 1);
  cb.removeTag("test:dummy:subdumy:array[1]:var2", next::appsupport::configtags::TAG::ACTIVE);
  getTags = cb.getTags("test:dummy:subdumy:array[1]:var2");
  EXPECT_EQ(getTags.size(), 0);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_RemoveAllTagsForKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:v1", 54);
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::ACTIVE);
  cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  auto getTags = cb.getTags("test:dummy:subdumy:var2");
  EXPECT_EQ(getTags.size(), 2);
  cb.removeAllTagsForKey("test:dummy:subdumy:var2");
  getTags = cb.getTags("test:dummy:subdumy:var2");
  EXPECT_EQ(getTags.size(), 0);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_RemoveAllTagsForArrayKey_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);

  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);

  cb.put("test:v1", 54);
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::ACTIVE);
  cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[2]:var2", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[0]:var2", "custom_tag");

  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[0]", "custom_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_tag");

  auto getTags = cb.getTags("test:dummy:subdumy:var2");

  getTags = cb.getTags("test:dummy:subdumy:array[2]:var2");
  EXPECT_EQ(getTags.size(), 1);
  auto str_json = cb.getAllConfigurationsAsJson();
  cb.removeAllTagsForKey("test:dummy:subdumy:array[2]:var2");
  getTags = cb.getTags("test:dummy:subdumy:array[2]:var2");
  str_json = cb.getAllConfigurationsAsJson();
  EXPECT_EQ(getTags.size(), 0);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTreeFilteredByTagsIntersection_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  // comment in once it's implemented
  // cb.setTag("test:dummy:subdumy:array[0]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[1]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[1]:var2", "tag2");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag2");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag3");
  // auto getTags = cb.getTags("test:dummy:subdumy:array[2]:var2");

  //// filter for keys that have tag1 && tag2 && tag3
  // auto as_json = boost::json::parse(cb.getTreeFilteredByTagsIntersectionAsJson({"tag1", "tag2", "tag3"}));
  // std::stringstream ss3Tags;
  // ss3Tags << as_json;
  // const std::string expected3Tags{"{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"3\"}]}}}}"};
  // EXPECT_EQ(ss3Tags.str(), expected3Tags);

  // as_json = boost::json::parse(cb.getTreeFilteredByTagsIntersectionAsJson({"tag1", "tag2"}));
  // std::stringstream ss2Tags;
  // ss2Tags << as_json;
  // const std::string expected2Tags{
  //    "{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"2\"},{\"var2\":\"3\"}]}}}}"};
  // EXPECT_EQ(ss2Tags.str(), expected2Tags);

  // as_json = boost::json::parse(cb.getTreeFilteredByTagsIntersectionAsJson({"tag1"}));
  // std::stringstream ss1Tag;
  // ss1Tag << as_json;
  // const std::string expected1Tag{
  //    "{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"1\"},{\"var2\":\"2\"},{\"var2\":\"3\"}]}}}}"};
  // EXPECT_EQ(ss1Tag.str(), expected1Tag);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTreeFilteredByTagsIntersectionWrongTag_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  // comment in once it's implemented
  // cb.setTag("test:dummy:subdumy:array[0]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[1]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[1]:var2", "tag2");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag2");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag3");
  // auto getTags = cb.getTags("test:dummy:subdumy:array[2]:var2");

  //// filter for keys that have tag1 && tag2 && tag3
  // auto tree = cb.getTreeFilteredByTagsIntersectionAsJson({"tag1", "tag5", "tag6"});
  // EXPECT_EQ(tree, "{\n}\n");
  // tree = cb.getTreeFilteredByTagsIntersectionAsJson(std::vector<std::string>{"tag5", "tag2"});
  // EXPECT_EQ(tree, "{\n}\n");
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTreeFilteredByTagsReunion_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  // comment in once it's implemented
  // cb.setTag("test:dummy:subdumy:array[0]:var2", "tag1");
  // cb.setTag("test:dummy:subdumy:array[1]:var2", "tag2");
  // cb.setTag("test:dummy:subdumy:array[2]:var2", "tag3");
  // auto getTags = cb.getTags("test:dummy:subdumy:array[2]:var2");
  //// filter for keys that have tag1 || tag2 || tag3
  // auto as_json = boost::json::parse(cb.getTreeFilteredByTagsUnionAsJson({"tag1", "tag2", "tag3"}));
  // std::stringstream ss3Tags;
  // ss3Tags << as_json;
  // const std::string expected3Tags{
  //    "{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"1\"},{\"var2\":\"2\"},{\"var2\":\"3\"}]}}}}"};
  // EXPECT_EQ(ss3Tags.str(), expected3Tags);

  // as_json = boost::json::parse(cb.getTreeFilteredByTagsUnionAsJson({"tag1", "tag2"}));
  // std::stringstream ss2Tags;
  // ss2Tags << as_json;
  // const std::string expected2Tags{
  //    "{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"1\"},{\"var2\":\"2\"}]}}}}"};
  // EXPECT_EQ(ss2Tags.str(), expected2Tags);

  // as_json = boost::json::parse(cb.getTreeFilteredByTagsUnionAsJson({"tag1"}));
  // std::stringstream ss1Tag;
  // ss1Tag << as_json;
  // const std::string expected1Tag{"{\"test\":{\"dummy\":{\"subdumy\":{\"array\":[{\"var2\":\"1\"}]}}}}"};
  // EXPECT_EQ(ss1Tag.str(), expected1Tag);
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_filterByTag_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);

  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  cb.put("test:array[0]", 0);
  cb.put("test:array[1]", 1);

  // comment in once it's implemented
  /*cb.setTag("test:array[1]", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_aray_tag");

  auto as_json = boost::json::parse(cb.getTreeFilteredByTagAsJson("custom_aray_tag"));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{
      "{\"test\":{\"array\":[\"1\"],\"dummy\":{\"subdumy\":{\"array\":[{\"var3\":{\"ar2\":[\"2\"]}}]}}}}"};
  EXPECT_EQ(ss.str(), expected);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_filterByInvalidTag_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);

  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  cb.put("test:array[0]", 0);
  cb.put("test:array[1]", 1);

  // comment in once it's implemented
  /*cb.setTag("test:array[1]", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_aray_tag");

  auto as_json = boost::json::parse(cb.getTreeFilteredByTagAsJson("notExisting"));
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{}"};
  EXPECT_EQ(ss.str(), expected);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTagsAsJson_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);

  cb.put("test:dummy:subdumy:array[0]:var2", 1);
  cb.put("test:dummy:subdumy:array[1]:var2", 2);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", 0);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[1]", 1);
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[2]", 2);
  cb.put("test:dummy:subdumy:array[2]:var2", 3);
  cb.put("test:array[0]", 0);
  cb.put("test:array[1]", 1);

  // comment in once it's implemented
  /*cb.setTag("test:array[1]", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[0]", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[0]:var2", "custom_tag2");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_aray_tag");

  auto as_json = boost::json::parse(cb.getTagsAsJsonStr());
  std::stringstream ss;
  ss << as_json;
  const std::string expected{
      "{\"test\":{\"custom_aray_tag\":[\"test:array[1]\",\"test:dummy:subdumy:array[1]:var3:ar2[0]\","
      "\"test:dummy:subdumy:array[1]:var3:ar2[2]\"],\"custom_tag2\":[\"test:dummy:subdumy:array[0]:"
      "var2\"]}}"};
  EXPECT_EQ(ss.str(), expected);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTagsAsJsonForGUI_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:array[0]", 0);
  cb.put("test:array[1]", 1);

  // comment in once it's implemented
  /*cb.setTag("test:array[1]", "custom_aray_tag");
  cb.setTag("test:array[1]", "custom_aray_tag2");
  cb.setTag("test:array[0]", "custom_aray_tag");

  cb.setTag("test:dummy:subdumy:var2", "custom_aray_tag");

  auto as_json = boost::json::parse(cb.getTagsAsJsonStrForGUI());
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"test:array[0]\":[\"custom_aray_tag\"],\"test:array[1]\":[\"custom_aray_tag\",\"custom_"
                             "aray_tag2\"],\"test:dummy:subdumy:var2\":[\"custom_aray_tag\"]}"};
  EXPECT_EQ(ss.str(), expected);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_getTagsAsJsonMultipleComponents_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.putCfg("node_cfg.json");
  auto asJsonStr = cb.getAllConfigurationsAsJson();
  cb.put("test:dummy:subdumy:array[0]:var3:ar2[0]", "a");
  cb.put("test:dummy:subdumy:array[1]:var3:ar2[0]", "aa");

  // comment in once it's implemented
  /*cb.setTag("local:active_state", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[0]", "custom_aray_tag");
  cb.setTag("test:dummy:subdumy:array[0]:var2", "custom_tag2");
  cb.setTag("test:dummy:subdumy:array[1]:var3:ar2[2]", "custom_aray_tag");
  auto as_json = boost::json::parse(cb.getTagsAsJsonStr());
  std::stringstream ss;
  ss << as_json;
  const std::string expected{"{\"local\":{\"custom_aray_tag\":[\"local:active_state\"]},\"test\":{\"custom_aray_tag\":["
                             "\"test:dummy:subdumy:array[1]:var3:ar2[0]\"]}}"};
  EXPECT_EQ(ss.str(), expected);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_updateTagsFromStr_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.putCfg("node_cfg.json");
  auto asJsonStr = cb.getAllConfigurationsAsJson();
  const std::string expected{
      "{\"test\":{\"custom_aray_tag\":[\"test:array[1]\",\"test:dummy:subdumy:array[1]:var3:ar2[0]\","
      "\"test:dummy:subdumy:array[1]:var3:ar2[2]\"],\"custom_tag2\":[\"test:dummy:subdumy:array[0]:"
      "var2\"]}}"};
  // comment in once it's implemented
  /*cb.updateTagsFromJsonString(expected);
  auto as_json = boost::json::parse(cb.getTagsAsJsonStr());
  std::stringstream ss;
  ss << as_json;
  EXPECT_EQ(ss.str(), expected);*/
}

TEST(ConfigBaseTestsWithoutEcal, test_isList_Success) {
  auto cb = ConfigBaseTestClass("list_check_test");
  cb.putCfg("node_cfg.json");
  auto isList = cb.isList("list_check_test:normal_variable");
  EXPECT_EQ(isList, false);
  isList = cb.isList("list_check_test:array_value");
  EXPECT_EQ(isList, true);
  isList = cb.isList("list_check_test:invalidValue");
  EXPECT_EQ(isList, false);
}

TEST(ConfigBaseTestsWithoutEcal, test_isList_notAvailable) {
  auto cb = ConfigBaseTestClass("list_check_test");
  cb.putCfg("node_cfg.json");
  auto isList = cb.isList("list_check_test:object_array");
  EXPECT_EQ(isList, false);
}

TEST(ConfigBaseTestsWithoutEcal, test_isInitFinished_beforeInit_false) {
  auto cb = ConfigBaseTestClass("test");
  EXPECT_FALSE(cb.isInitFinished());
}

TEST(ConfigBaseTestsWithoutEcal, test_isInitFinished_afterInit_true) {
  auto cb = ConfigBaseTestClass("test");
  cb.finishInitialization();
  EXPECT_TRUE(cb.isInitFinished());
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_AddActiveTagOnGet_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2", 1);
  cb.put("test:v1", 54);
  auto strVal = cb.get_string("test:dummy:subdumy:var2", "");
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  auto getTags = cb.getTags("test:dummy:subdumy:var2");
  EXPECT_EQ(getTags.size(), 2);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_AddActiveTagOnGet_Array_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2[0]", 1);
  cb.put("test:dummy:subdumy:var2[1]", 2);
  cb.put("test:dummy:subdumy:var2[2]", 3);
  cb.put("test:v1", 54);
  auto strVal = cb.get_string("test:dummy:subdumy:var2[1]", "");
  // comment in once it's implemented
  /*cb.setTag("test:dummy:subdumy:var2", "custom_tag");
  auto getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_AddActiveTagOnGetStringListSuccess) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2[0]", 1);
  cb.put("test:dummy:subdumy:var2[1]", 2);
  cb.put("test:dummy:subdumy:var2[2]", 3);
  cb.put("test:v1", 54);
  auto strList = cb.getStringList("test:dummy:subdumy:var2", {});
  // comment in once it's implemented
  /*auto getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_SetActiveSuccess) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2[0]", 1);
  cb.put("test:dummy:subdumy:var2[1]", 2);
  cb.put("test:dummy:subdumy:var2[2]", 3);
  cb.put("test:v1", 54);
  auto strList = cb.getStringList("test:dummy:subdumy:var2", {});
  EXPECT_EQ(strList.size(), 3);
  // comment in once it's implemented
  /*auto getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");

  auto strListAfterActive = cb.getStringList("test:dummy:subdumy:var2");
  EXPECT_EQ(strListAfterActive.size(), 3);

  cb.setActive("test:dummy:subdumy:var2", "2");
  cb.removeTag("test:dummy:subdumy:var2[0]", next::appsupport::configtags::TAG::ACTIVE);
  getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 0);
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 0);

  auto getV1setActive = cb.getStringList("test:v1");
  ASSERT_EQ(getV1setActive.size(), 1);
  EXPECT_EQ(getV1setActive[0], "54");
  auto getV1alreadyOnActive = cb.getStringList("test:v1");
  ASSERT_EQ(getV1alreadyOnActive.size(), 1);
  EXPECT_EQ(getV1alreadyOnActive[0], "54");
  EXPECT_EQ(cb.get_int("test:v1"), 54);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_ResetActiveOnInsertSuccess) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2[0]", 1);
  cb.put("test:dummy:subdumy:var2[1]", 2);
  cb.put("test:dummy:subdumy:var2[2]", 3);
  cb.put("test:v1", 54);
  auto strList = cb.getStringList("test:dummy:subdumy:var2", {});
  // comment in once it's implemented
  /*auto getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  cb.insert("{}", false, true);
  getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 0);
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 0);
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 0);*/
}
TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_ResetActiveSuccess) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var2[0]", 1);
  cb.put("test:dummy:subdumy:var2[1]", 2);
  cb.put("test:dummy:subdumy:var2[2]", 3);
  cb.put("test:v1", 54);
  auto strList = cb.getStringList("test:dummy:subdumy:var2", {});
  // comment in once it's implemented
  /*auto getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  auto getList = cb.getStringList("test:dummy:subdumy:var2");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");

  ASSERT_EQ(getList.size(), 3);
  EXPECT_EQ(getList[0], "1");
  EXPECT_EQ(getList[1], "2");
  EXPECT_EQ(getList[2], "3");

  cb.setActive("test:dummy:subdumy:var2", "2");
  getTags = cb.getTags("test:dummy:subdumy:var2[0]");
  ASSERT_EQ(getTags.size(), 0);
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 1);
  EXPECT_EQ(getTags[0], "ACTIVE");
  getTags = cb.getTags("test:dummy:subdumy:var2[2]");
  ASSERT_EQ(getTags.size(), 0);

  getList = cb.getStringList("test:dummy:subdumy:var2");
  ASSERT_EQ(getList.size(), 3);
  EXPECT_EQ(getList[0], "1");
  EXPECT_EQ(getList[1], "2");
  EXPECT_EQ(getList[2], "3");

  cb.resetActive("test:dummy:subdumy:var2", "2");
  getTags = cb.getTags("test:dummy:subdumy:var2[1]");
  ASSERT_EQ(getTags.size(), 0);

  getList = cb.getStringList("test:dummy:subdumy:var2");
  ASSERT_EQ(getList.size(), 3);
  EXPECT_EQ(getList[0], "1");
  EXPECT_EQ(getList[1], "2");
  EXPECT_EQ(getList[2], "3");*/
}

TEST(ConfigBaseTestsWithoutEcal, test_GetStringListActiveDoesNotChangeValue) {

  next::appsupport::ConfigClient::do_init("test");
  auto configClient = ConfigClient::getConfig();
  configClient->put("test:v1[0]", "54");
  configClient->put("test:v1[1]", "asdf");
  configClient->put("test:v1[2]", "asdf");
  auto stringList = configClient->getStringList("test:v1", {});
  ASSERT_EQ(stringList.size(), 3);
  EXPECT_EQ(stringList[0], "54");
  EXPECT_EQ(stringList[1], "asdf");
  EXPECT_EQ(stringList[2], "asdf");
  stringList = configClient->getStringList("test:v1", {});

  ASSERT_EQ(stringList.size(), 3);
  EXPECT_EQ(stringList[0], "54");
  EXPECT_EQ(stringList[1], "asdf");
  EXPECT_EQ(stringList[2], "asdf");
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_test_FilterOutReadonlyFromConfig_Success) {
  auto cb = ConfigBaseTestClass("test");
  cb.put("test:dummy:subdumy:var1", 1);
  cb.put("test:dummy:subdumy:var2", 2);
  cb.put("test:dummy:subdumy:var3", 3);
  cb.put("test:l1:l2:l3:object:a", 1);
  cb.put("test:l1:l2:l3:object:b", 1);

  cb.put("test:dummy:list[0]", 1);
  cb.put("test:dummy:list[1]", 2);
  cb.put("test:dummy:list[2]", 3);
  cb.put("test:v1", 54);

  cb.put("test:dummy:list2[0]", 1);
  cb.put("test:dummy:list2[1]", 2);

  // comment in once it's implemented
  /*cb.setTag("test:dummy:list[1]", next::appsupport::configtags::TAG::READONLY);
  cb.setTag("test:dummy:list", next::appsupport::configtags::TAG::ACTIVE);

  cb.setTag("test:dummy:subdumy:var2", next::appsupport::configtags::TAG::READONLY);
  cb.setTag("test:dummy:subdumy:var2", "custom_tag");

  cb.setTag("test:v1", next::appsupport::configtags::TAG::READONLY);

  cb.setTag("test:l1:l2:l3:object:a", next::appsupport::configtags::TAG::READONLY);
  cb.setTag("test:l1:l2:l3:object:b", next::appsupport::configtags::TAG::READONLY);
  cb.setTag("test:l1:l2:l3:object", next::appsupport::configtags::TAG::ACTIVE);

  cb.setTag("test:dummy:list2[0]", next::appsupport::configtags::TAG::READONLY);
  cb.setTag("test:dummy:list2[1]", next::appsupport::configtags::TAG::READONLY);

  auto asStr = cb.getAllConfigurationsAsJson(false, false, true);
  auto as_json = boost::json::parse(asStr);
  std::stringstream ss;
  ss << as_json;
  const std::string expected{
      "{\"test\":{\"dummy\":{\"subdumy\":{\"var1\":\"1\",\"var3\":\"3\"},\"list\":[\"1\",\"3\"]}}}"};
  EXPECT_EQ(ss.str(), expected);*/
}

TEST(ConfigBaseTestsWithoutEcal, DISABLED_put_performance_test) {
  auto cb = ConfigBaseTestClass("list_check_test");
  cb.disableFilterByComponent();

  unsigned int size_keys = 1000000;
  unsigned int level_increase = 10000;
  std::vector<std::string> key_list_int;
  key_list_int.resize(size_keys);

  std::vector<std::string> key_list_float;
  key_list_float.resize(size_keys);

  std::vector<std::string> key_list_string;
  key_list_string.resize(size_keys);

  std::vector<std::string> key_list_char_array;
  key_list_char_array.resize(size_keys);

  std::vector<std::string> key_list_bool;
  key_list_bool.resize(size_keys);

  std::string key_int = "list_check_test:testabcint";
  std::string key_float = "list_check_test:testabcfloat";
  std::string key_string = "list_check_test:testabcstring";
  std::string key_char_array = "list_check_test:testabcchar_array";
  std::string key_bool = "list_check_test:testabcbool";

  for (unsigned int i = 0; i < size_keys; ++i) {
    if (i % level_increase == 0 && i != 0) {
      key_int += ":testnew";
      key_float += ":testnew";
      key_string += ":testnew";
      key_char_array += ":testnew";
      key_bool += ":testnew";
    }
    key_list_int[i] = key_int + std::to_string(i);
    key_list_float[i] = key_float + std::to_string(i);
    key_list_string[i] = key_string + std::to_string(i);
    key_list_char_array[i] = key_char_array + std::to_string(i);
    key_list_bool[i] = key_bool + std::to_string(i);
  }

  int i = 0;
  for (const auto &key_elem : key_list_int) {
    if (i % 4 == int(next::appsupport::ConfigSource::CmdOptionsConfig)) {
      cb.put(key_elem, 10, next::appsupport::ConfigSource::CmdOptionsConfig);
    } else if (i % 4 == int(next::appsupport::ConfigSource::CmdOptionsParameters)) {
      cb.put(key_elem, 11, next::appsupport::ConfigSource::CmdOptionsParameters);
    } else if (i % 4 == int(next::appsupport::ConfigSource::Component)) {
      cb.put(key_elem, 12, next::appsupport::ConfigSource::Component);
    } else if (i % 4 == int(next::appsupport::ConfigSource::ConfigServer)) {
      cb.put(key_elem, 13, next::appsupport::ConfigSource::ConfigServer);
    }
    ++i;
  }

  for (const auto &key_elem : key_list_int) {
    cb.put(key_elem, 10, next::appsupport::ConfigSource::Component);
  }

  cb.finishInitialization();
}
