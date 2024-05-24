#include <gtest/gtest.h>

#include <list>

#include "boost/lexical_cast.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/sdk/sdk.hpp>

#include "base_ctrl_config_reader.hpp"

namespace next {
namespace control {
namespace orchestrator {

class BaseDataReadOutConfigTester : public BaseDataReadOutConfig {
public:
  static TriggerDataInfo CreateFallbackTriggerDataInfoFromFlowId(std::string flow_id) {
    return BaseDataReadOutConfig::CreateFallbackTriggerDataInfoFromFlowId(flow_id);
  }
  static MemberDataInfo CreateMemberInfo(const std::string &member, const std::string &key) {
    return BaseDataReadOutConfig::CreateMemberInfo(member, key);
  }
  static TriggerDataInfo CreateTriggerDataInfo(const std::string &flow_id, const std::string &member,
                                               const std::string &execution_name) {
    return BaseDataReadOutConfig::CreateTriggerDataInfo(flow_id, member, execution_name);
  };
  static void MergeFallbackIntoTriggerDataInfo(const TriggerDataInfo &fallback, TriggerDataInfo &current) {
    return BaseDataReadOutConfig::MergeFallbackIntoTriggerDataInfo(fallback, current);
  }
};

bool operator==(const ControlDataInfo &left_member, const ControlDataInfo &right_member) {
  if (left_member.flow_name == right_member.flow_name && left_member.sdl_file == right_member.sdl_file &&
      left_member.contol_data_urls == right_member.contol_data_urls) {
    return true;
  } else {
    return false;
  }
}

bool operator==(const TriggerDataInfo &left_member, const TriggerDataInfo &right_member) {
  if (left_member.component_name == right_member.component_name &&
      left_member.execution_name == right_member.execution_name && left_member.info == right_member.info) {
    return true;
  } else {
    return false;
  }
}

bool operator==(const MemberDataInfo &left_member, const MemberDataInfo &right_member) {
  if (left_member.connection_name == right_member.connection_name &&
      left_member.control_data_url_name == right_member.control_data_url_name &&
      left_member.type == right_member.type && left_member.fixed_value_array == right_member.fixed_value_array) {
    return true;
  } else {
    return false;
  }
}

class BaseControlConfigReaderFixture : public next::sdk::testing::TestUsingEcal {
public:
  BaseControlConfigReaderFixture() { instance_name_ = "BaseControlConfigReaderFixture"; }
  void TearDown() override {
    next::appsupport::ConfigClient::do_reset();
    TestUsingEcal::TearDown();
  }
};

TEST_F(BaseControlConfigReaderFixture, read_config_by_flow_id) {

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  ControlDataInfo ref_control_data;
  ref_control_data.flow_name = "flow1";
  ref_control_data.sdl_file = "sig_header.sdl";
  ref_control_data.contol_data_urls.push_back(
      TriggerDataInfo{"some_component",
                      "runSequencer_response",
                      {MemberDataInfo{"SigHeader.uiTimeStamp",
                                      SignalType::connected,
                                      "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiTimeStamp",
                                      {}},
                       MemberDataInfo{"SigHeader.uiMeasurementCounter",
                                      SignalType::connected,
                                      "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiMeasurementCounter",
                                      {}},
                       MemberDataInfo{"SigHeader.uiCycleCounter",
                                      SignalType::connected,
                                      "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiCycleCounter",
                                      {}},
                       MemberDataInfo{"SigHeader.sSigState", SignalType::fixed_values, "", {0, 1, 2, 3, 4, 4, 6, 1}}}});

  ref_control_data.contol_data_urls.push_back(TriggerDataInfo{
      "some_component",
      "runSequencer_response2",
      {MemberDataInfo{
           "SigHeader.uiTimeStamp", SignalType::connected, "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp", {}},
       MemberDataInfo{"SigHeader.uiMeasurementCounter",
                      SignalType::connected,
                      "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiMeasurementCounter",
                      {}},
       MemberDataInfo{"SigHeader.sSigState", SignalType::fixed_values, "", {0, 1, 2, 3, 4, 4, 6, 1}}}});

  auto control_data = BaseDataReadOutConfigTester::CreateInfoFromFlowId("flow1");

  EXPECT_TRUE(ref_control_data == control_data);
}

TEST_F(BaseControlConfigReaderFixture, create_fallback_data_by_flow_id) {

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  TriggerDataInfo ref_fallback_info;
  ref_fallback_info.component_name = "FallbackComponent";
  ref_fallback_info.execution_name = "FallbackExecution";
  ref_fallback_info.info.push_back(MemberDataInfo{
      "SigHeader.uiTimeStamp", SignalType::connected, "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp", {}});
  ref_fallback_info.info.push_back(MemberDataInfo{"SigHeader.uiMeasurementCounter",
                                                  SignalType::connected,
                                                  "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiMeasurementCounter",
                                                  {}});
  ref_fallback_info.info.push_back(
      MemberDataInfo{"SigHeader.sSigState", SignalType::fixed_values, "", {0, 1, 2, 3, 4, 4, 6, 1}});

  auto fallback_info = BaseDataReadOutConfigTester::CreateFallbackTriggerDataInfoFromFlowId("flow1");

  EXPECT_TRUE(ref_fallback_info == fallback_info);
}

TEST_F(BaseControlConfigReaderFixture, create_default_connected_member_info) {

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  MemberDataInfo ref_data_info;
  ref_data_info.control_data_url_name = "SigHeader.uiTimeStamp";
  ref_data_info.type = SignalType::connected;
  ref_data_info.connection_name = "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp";
  ref_data_info.fixed_value_array = {};

  std::string key = "next_player:orchestrator:flow1:meta:control_data_generation:member_data:SigHeader.uiTimeStamp";

  auto member_info = BaseDataReadOutConfigTester::CreateMemberInfo("SigHeader.uiTimeStamp", key);

  EXPECT_TRUE(ref_data_info == member_info);
}

TEST_F(BaseControlConfigReaderFixture, create_default_fixed_member_info) {

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  MemberDataInfo ref_data_info;
  ref_data_info.control_data_url_name = "SigHeader.sSigState";
  ref_data_info.type = SignalType::fixed_values;
  ref_data_info.connection_name = "";
  ref_data_info.fixed_value_array = {0, 1, 2, 3, 4, 4, 6, 1};

  std::string key = "next_player:orchestrator:flow1:meta:control_data_generation:member_data:SigHeader.sSigState";

  auto member_info = BaseDataReadOutConfigTester::CreateMemberInfo("SigHeader.sSigState", key);

  EXPECT_TRUE(ref_data_info == member_info);
}

TEST_F(BaseControlConfigReaderFixture, faulty_key_test) {

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  MemberDataInfo ref_data_info;
  ref_data_info.control_data_url_name = "SigHeader.sSigState";
  ref_data_info.type = SignalType::fixed_values;
  ref_data_info.connection_name = "";
  ref_data_info.fixed_value_array = {0};

  std::string key = "next_player:orchestrator:flow1:meta:control_data_generation:member_data:uiTimestamp";

  auto member_info = BaseDataReadOutConfigTester::CreateMemberInfo("SigHeader.sSigState", key);

  EXPECT_TRUE(ref_data_info == member_info);
}

TEST_F(BaseControlConfigReaderFixture, merge_fallback_into_trigger_data) {
  TriggerDataInfo ref_trigger_info;

  ref_trigger_info.component_name = "some_component";
  ref_trigger_info.execution_name = "runSequencer_response";
  ref_trigger_info.info.push_back(MemberDataInfo{"SigHeader.uiTimeStamp",
                                                 SignalType::connected,
                                                 "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiTimeStamp",
                                                 {}});
  ref_trigger_info.info.push_back(
      MemberDataInfo{"SigHeader.uiMeasurementCounter",
                     SignalType::connected,
                     "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiMeasurementCounter",
                     {}});
  ref_trigger_info.info.push_back(MemberDataInfo{"SigHeader.uiCycleCounter",
                                                 SignalType::connected,
                                                 "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.SigHeader.uiCycleCounter",
                                                 {}});
  ref_trigger_info.info.push_back(
      MemberDataInfo{"SigHeader.sSigState", SignalType::fixed_values, "", {0, 1, 2, 3, 4, 4, 6, 1}});

  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  auto fallback_trigger_info = BaseDataReadOutConfigTester::CreateFallbackTriggerDataInfoFromFlowId("flow1");
  auto trigger_info =
      BaseDataReadOutConfigTester::CreateTriggerDataInfo("flow1", "some_component", "runSequencer_response");

  BaseDataReadOutConfigTester::MergeFallbackIntoTriggerDataInfo(fallback_trigger_info, trigger_info);

  EXPECT_TRUE(ref_trigger_info == trigger_info);
}

TEST_F(BaseControlConfigReaderFixture, read_bad_config_no_crash) {
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("bad_cfg_test_control_data.json");
  next::appsupport::ConfigClient::finish_init();

  auto config = BaseDataReadOutConfig::CreateInfoFromFlowId("flow1");

  auto info = config.contol_data_urls[0].info;

  // Check number of members
  ASSERT_TRUE(info.size() == 6);

  // Checking "SigHeader.sSigState" array size
  ASSERT_TRUE(info[2].fixed_value_array.size() == 9);
  // Checking "SigHeader.sSigState" value 1.5 from the array
  ASSERT_TRUE(info[2].fixed_value_array[1] == 1.5);

  // Checking "SigHeader.sSigState2" array size, should be 0 because it's a connected signal
  ASSERT_TRUE(info[3].fixed_value_array.size() == 0);
  // Checking "SigHeader.sSigState2"
  ASSERT_TRUE(info[3].connection_name == "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp");

  // Checking "SigHeader.sSigState3" array size
  ASSERT_TRUE(info[4].fixed_value_array.size() == 1);
  // Checking "SigHeader.sSigState3"
  ASSERT_TRUE(info[4].fixed_value_array[0] == 0.0);

  // Checking "SigHeader.sSigState4" array size
  ASSERT_TRUE(info[5].fixed_value_array.size() == 1);
  // Checking "SigHeader.sSigState4"
  ASSERT_TRUE(info[5].fixed_value_array[0] == 1.0);
}

} // namespace orchestrator
} // namespace control
} // namespace next
