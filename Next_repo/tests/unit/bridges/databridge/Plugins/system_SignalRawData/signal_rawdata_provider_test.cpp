/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     performance_viewer_test.cpp
 *  @brief    Unit tests for next_cpu_profiler
 */

#include <cip_test_support/gtest_reporting.h>
#include <next/sdk/sdk.hpp>

#include "signal_rawdata_provider.hpp"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

namespace next {
namespace plugins {

class RawDataProviderTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  RawDataProviderTestFixture() { this->instance_name_ = "RawDataProviderTest"; }
};

class RawDataProviderTester : public RawDataProvider {

public:
  RawDataProviderTester(const char *path) : RawDataProvider(path) {}

  void convertToJsonValueTest(Json::Value &json,
                              const next::bridgesdk::plugin_addons::PluginSignalDescription &topic_info,
                              const next::bridgesdk::plugin_addons::SignalInfo &info) {
    convertToJsonValue(json, topic_info, info);
  }

  std::string SignalTypeToStringTest(next::bridgesdk::plugin_addons::SignalType type) {
    return SignalTypeToString(type);
  }

  void publishRawInformationPerPackageTest(const std::string topic) { publishRawInformationPerPackage(topic); }

  void setAddonMocks(std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> sig_publisher,
                     std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> sig_extractor,
                     std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> sig_explorer) {
    plugin_publisher_ = sig_publisher;
    plugin_signal_extractor_ = sig_extractor;
    plugin_signal_explorer_ = sig_explorer;
  }
  bool combineJsonStringFromReceivedDataTester(unsigned long long int timestamp, std::string &result_msg,
                                               std::map<std::string, Json::Value> &json_list) {
    return combineJsonStringFromReceivedData(timestamp, result_msg, json_list);
  };
};

TEST_F(RawDataProviderTestFixture, ConstructorInitializesValuesCorrectly) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProvider provider(path);

  EXPECT_EQ(provider.getVersion(), "0.0.1-alpha1");
  EXPECT_EQ(provider.getName(), "system_SignalRawDataPlugin");
};

TEST_F(RawDataProviderTestFixture, CheckJsonFromDataInfo_success) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);

  Json::Value result;
  provider.convertToJsonValueTest(result, {}, {});

  ASSERT_TRUE(result.isMember("signal"));
  ASSERT_TRUE(result["signal"].isMember("type"));
  EXPECT_EQ(result["signal"]["type"], "bool");
}

TEST_F(RawDataProviderTestFixture, CheckJsonStichFromEmtpyJson_fails) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);
  std::map<std::string, Json::Value> input;
  std::string message_result;
  provider.combineJsonStringFromReceivedDataTester(1, message_result, input);
  ASSERT_FALSE(message_result.empty());
  Json::Value resultJson;
  Json::Reader reader;
  ASSERT_TRUE(reader.parse(message_result, resultJson));
  ASSERT_TRUE(resultJson.isMember("dataStream"));
}

TEST_F(RawDataProviderTestFixture, CheckJsonfaultInput_success) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);
  std::map<std::string, Json::Value> input;
  input["adf."] = Json::Value();
  Json::Value value;
  value["asdf"] = 1;
  input["adfd."] = value;
  input["adfd.asdf"] = value;
  input["adfd.asdfa"] = Json::Value();

  std::string message_result;
  provider.combineJsonStringFromReceivedDataTester(1, message_result, input);

  ASSERT_FALSE(message_result.empty());
  Json::Value resultJson;
  Json::Reader reader;
  ASSERT_TRUE(reader.parse(message_result, resultJson));
  ASSERT_TRUE(resultJson.isMember("dataStream"));
  ASSERT_TRUE(resultJson["dataStream"].isMember("signalRawDetails"));
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"].isMember("adf"));
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"]["adf"].isObject());
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"].isMember("adfd"));
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"]["adfd"].isMember(""));
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"]["adfd"][""].isMember("asdf"));
  ASSERT_EQ(resultJson["dataStream"]["signalRawDetails"]["adfd"][""]["asdf"].asInt(), 1);
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"]["adfd"].isMember("asdf"));
  ASSERT_EQ(resultJson["dataStream"]["signalRawDetails"]["adfd"]["asdf"]["asdf"].asInt(), 1);
  ASSERT_TRUE(resultJson["dataStream"]["signalRawDetails"]["adfd"].isMember("asdfa"));
}

TEST_F(RawDataProviderTestFixture, ConvertToJsonValueCorrectlyHandlesEmptySignalInfo) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);
  Json::Value json;
  next::bridgesdk::plugin_addons::PluginSignalDescription topic_info = {};
  next::bridgesdk::plugin_addons::SignalInfo info = {};

  provider.convertToJsonValueTest(json, topic_info, info);

  EXPECT_EQ(json["signal"]["type"].asString(), "bool");
  EXPECT_EQ(json["signal"]["size"].asUInt(), 0u);
  EXPECT_EQ(json["signal"]["array_size"].asUInt(), 0u);
  EXPECT_EQ(json["signal"]["offset"].asUInt(), 0u);
  EXPECT_EQ(json["package"]["cycleId"].asUInt(), 0u);
  EXPECT_EQ(json["package"]["instanceId"].asUInt(), 0u);
  EXPECT_EQ(json["package"]["sourceId"].asUInt(), 0u);
  EXPECT_EQ(json["package"]["vaddr"].asUInt64(), 0u);
}

TEST_F(RawDataProviderTestFixture, ConvertToJsonValueCorrectlyHandlesSignalInfo) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);
  Json::Value json;
  next::bridgesdk::plugin_addons::PluginSignalDescription topic_info;
  next::bridgesdk::plugin_addons::SignalInfo info;

  info.signal_type = next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32;
  info.signal_size = 4;
  info.array_size = 10;
  info.offset = 2;

  topic_info.cycle_id = 123;
  topic_info.instance_id = 456;
  topic_info.source_id = 789;
  topic_info.vaddr = 0xABCDEF;

  provider.convertToJsonValueTest(json, topic_info, info);

  ASSERT_EQ(json["signal"]["type"].asString(), "int_32");
  ASSERT_EQ(json["signal"]["size"].asUInt(), 4u);
  ASSERT_EQ(json["signal"]["array_size"].asUInt(), 10u);
  ASSERT_EQ(json["signal"]["offset"].asUInt(), 2u);
  ASSERT_EQ(json["package"]["cycleId"].asUInt(), 123u);
  ASSERT_EQ(json["package"]["instanceId"].asUInt(), 456u);
  ASSERT_EQ(json["package"]["sourceId"].asUInt(), 789u);
  ASSERT_EQ(json["package"]["vaddr"].asUInt64(), 0xABCDEF);
}

TEST_F(RawDataProviderTestFixture, ReturnsCorrectStringForSignalType) {
  const char *path = "/dummy/path";
  next::plugins::RawDataProviderTester provider(path);
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT), "struct");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL), "bool");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR), "char");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8), "uint_8");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8), "int_8");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16), "uint_16");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16), "int_16");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32), "uint_32");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32), "int_32");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64), "uint_64");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64), "int_64");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT), "float");
  EXPECT_EQ(provider.SignalTypeToStringTest(bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE), "double");
}

TYPED_TEST_CASE(PluginTest, next::plugins::RawDataProviderTester);

TYPED_TEST(PluginTest, GetTheURLsCorrectly) {
  RawDataProviderTester provider("/dummy/path");

  provider.setAddonMocks(this->plugin_signal_publisher_, this->plugin_sig_extractor_, this->plugin_signal_explorer_);

  std::vector<std::string> topic_by_url{"ADC5xx_Device.CEM_EM_DATA.USS_UssOutput"};

  std::vector<std::string> requested_urls = {"ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiTimeStamp",
                                             "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiMeasurementCounter",
                                             "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiCycleCounter",
                                             "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.eSigStatus"};

  std::string expected_topic = "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput";
  next::bridgesdk::plugin_addons::PluginSignalDescription desc;
  desc.vaddr = 1078324768;
  desc.source_id = 8009;
  desc.instance_id = 0;
  desc.data_source_name = "ADC5xx_Device";
  desc.format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA;
  desc.cycle_id = 134;

  auto signal_explorer_mock =
      std::dynamic_pointer_cast<next::test::PluginSignalExplorerMock>(this->plugin_signal_explorer_);

  for (const auto &url : requested_urls) {
    signal_explorer_mock->setURLs(url, {expected_topic});
    signal_explorer_mock->setPackageDetailsMock(url, desc);
  }
  auto extractor_mock = std::dynamic_pointer_cast<next::test::PluginSignalExtractorMock>(this->plugin_sig_extractor_);
  std::unordered_map<std::string, next::bridgesdk::plugin_addons::SignalInfo> info;
  info["ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiTimeStamp"] = {
      bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64, 8, 1, 0};
  extractor_mock->setSignalInfo(info);

  std::string json_request = R"(
  { "usedUris": ["ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiTimeStamp",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiMeasurementCounter",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiMeasurementCounter",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiMeasurementCounter",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiMeasurementCounter",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.uiCycleCounter",
                 "ADC5xx_Device.CEM_EM_DATA.USS_UssOutput.sSigHeader.eSigStatus"]
  })";
  provider.update({json_request});
  auto publisher_mock = std::dynamic_pointer_cast<next::test::PluginDataPublisherMock>(this->plugin_signal_publisher_);
  std::string result(publisher_mock->capturedData.begin(), publisher_mock->capturedData.end());
  Json::Value jsonResult;
  Json::Reader reader;
  ASSERT_TRUE(reader.parse(result, jsonResult));
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["package"]["cycleId"]
                          .asInt(),
            134);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["package"]["sourceId"]
                          .asInt(),
            8009);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["package"]["instanceId"]
                          .asInt(),
            0);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["package"]["vaddr"]
                          .asUInt64(),
            1078324768ULL);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["signal"]["array_size"]
                          .asInt(),
            1);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["signal"]["offset"]
                          .asInt(),
            0);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["signal"]["size"]
                          .asInt(),
            8);
  EXPECT_EQ(jsonResult["dataStream"]["signalRawDetails"]["ADC5xx_Device"]["CEM_EM_DATA"]["USS_UssOutput"]["sSigHeader"]
                      ["uiTimeStamp"]["signal"]["type"]
                          .asString(),
            "uint_64");
}

} // namespace plugins
} // namespace next
