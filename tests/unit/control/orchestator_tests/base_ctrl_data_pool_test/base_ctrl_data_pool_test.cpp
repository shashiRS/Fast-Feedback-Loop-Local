#include <gtest/gtest.h>

#include <list>

#include <next/sdk/sdk.hpp>
#include <next/udex/publisher/data_publisher.hpp>

#include "base_ctrl_data_generator/base_ctrl_data_pool.hpp"

namespace next {
namespace control {
namespace orchestrator {

class BaseControlDataPoolTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  static const size_t size_veh_dyn = 160;
  char VehDynDummy[size_veh_dyn];
  char VehDyn1Dummy[size_veh_dyn];

public:
  BaseControlDataPoolTestFixture() { instance_name_ = "BaseControlDataPoolTestFixture"; }
  void SetUp() override {
    TestUsingEcal::SetUp();

    publisher = std::make_shared<next::udex::publisher::DataPublisher>("AddTreeExtractorForSignal");
    publisher->RegisterDataDescription("ARS5xx.sdl", true);
  }

  template <typename value_type>
  void SetValueToMemoryGroup1(const value_type &value, const size_t &offset) {
    memcpy(&(VehDynDummy[offset]), &value, sizeof(value_type));
  }

  template <typename value_type>
  void SetValueToMemoryGroup2(const value_type &value, const size_t &offset) {
    memcpy(&(VehDyn1Dummy[offset]), &value, sizeof(value_type));
  }

  const std::string execution_name_dummy = "trigger1";
  const std::string component_name_dummy = "component1";
  TriggerDataInfo TriggerDataInfoThreeConnectionsFromTwoGroups() {
    TriggerDataInfo test_info;

    test_info.execution_name = execution_name_dummy;
    test_info.component_name = component_name_dummy;
    MemberDataInfo member;
    member.type = SignalType::connected;
    member.control_data_url_name = "uiVersionNumber";
    member.connection_name = "SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber";
    test_info.info.push_back(member);

    member.control_data_url_name = "SigHeader.uiTimeStamp1";
    member.connection_name = "SIM VFB.AlgoVehCycle.VehDyn1.sSigHeader.uiTimeStamp";
    test_info.info.push_back(member);

    member.control_data_url_name = "SigHeader.uiTimeStamp";
    member.connection_name = "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp";
    test_info.info.push_back(member);
    return test_info;
  };
  void TearDown() override {
    publisher.reset();
    TestUsingEcal::TearDown();
  }
  std::shared_ptr<next::udex::publisher::DataPublisher> publisher;
};

class BaseControlDataPoolTester : public BaseControlDataPool {
public:
  BaseControlDataPoolTester(const std::string &flow_name) : BaseControlDataPool(flow_name) {}

  std::shared_ptr<next::udex::AnyValue> SearchOrAddTreeExtractorForSignalTest(const std::string &signal_url,
                                                                              DataPoolBackend &backend) {
    return SearchOrAddTreeExtractorForSignal(signal_url, backend);
  }

  const DataPoolBackend &GetDataPoolBackend(const std::string &comp_name, const std::string &exec_name) {
    return all_data_[createHashName(comp_name, exec_name)];
  }

  const std::shared_ptr<next::udex::subscriber::DataSubscriber> &GetSubscriberTest() { return subscriber_; }
  const std::shared_ptr<next::udex::extractor::DataExtractor> &GetExtractorTest() { return extractor_; }
  DataPoolBackend test_backend;
};

TEST_F(BaseControlDataPoolTestFixture, fixed_values_test) {

  TriggerDataInfo test_info_fixed_values;

  test_info_fixed_values.component_name = "some_component";
  test_info_fixed_values.execution_name = "runSequencer_response";

  test_info_fixed_values.info = {
      MemberDataInfo{"SigHeader.sSigState", SignalType::fixed_values, "", {0, 1, 2, 3, 4, 4, 6, 1}}};

  BaseControlDataPool test_data_pool("flow1");

  test_data_pool.SetupInput({test_info_fixed_values});
  bool success;
  auto result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ(result.fixed_signal["SigHeader.sSigState"], 0);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 1);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 2);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 3);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 4);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 4);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 6);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 1);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 1);
  EXPECT_TRUE(success);

  result = test_data_pool.GetNewData("some_component", "runSequencer_response", success);
  EXPECT_EQ((int)result.fixed_signal["SigHeader.sSigState"], 1);
  EXPECT_TRUE(success);
}

TEST_F(BaseControlDataPoolTestFixture, AddTreeExtractorForSignal_success) {

  BaseControlDataPoolTester test("flow1");
  auto anyvalue =
      test.SearchOrAddTreeExtractorForSignalTest("SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber", test.test_backend);
  ASSERT_FALSE(anyvalue.get() == nullptr);
  EXPECT_EQ(anyvalue->GetFullUrl(), "SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber");
  EXPECT_EQ(test.test_backend.tree_extractors.size(), 1);

  anyvalue = test.SearchOrAddTreeExtractorForSignalTest("SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
                                                        test.test_backend);
  ASSERT_FALSE(anyvalue.get() == nullptr);
  EXPECT_EQ(anyvalue->GetFullUrl(), "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");
  ASSERT_EQ(test.test_backend.tree_extractors.size(), 2);
  EXPECT_EQ(test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp"].use_count(),
            test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber"].use_count());
  EXPECT_EQ(test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp"].get(),
            test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber"].get());

  anyvalue = test.SearchOrAddTreeExtractorForSignalTest("SIM VFB.AlgoVehCycle.VehDyn1.sSigHeader.uiTimeStamp",
                                                        test.test_backend);
  ASSERT_FALSE(anyvalue.get() == nullptr);
  EXPECT_EQ(anyvalue->GetFullUrl(), "SIM VFB.AlgoVehCycle.VehDyn1.sSigHeader.uiTimeStamp");
  ASSERT_EQ(test.test_backend.tree_extractors.size(), 3);
  EXPECT_EQ(test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn1.sSigHeader.uiTimeStamp"].use_count(), 1);
  EXPECT_NE(test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn1.sSigHeader.uiTimeStamp"].get(),
            test.test_backend.tree_extractors["SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber"].get());
}

TEST_F(BaseControlDataPoolTestFixture, SetupDifferentSignals_success) {

  TriggerDataInfo test_info = TriggerDataInfoThreeConnectionsFromTwoGroups();

  BaseControlDataPoolTester test("flow1");

  EXPECT_TRUE(test.SetupInput({test_info})) << "Setup of DataPool failed";
  auto backendpool = test.GetDataPoolBackend(component_name_dummy, execution_name_dummy);
  ASSERT_EQ(backendpool.tree_extractors.size(), 3);
  auto subscriber = test.GetSubscriberTest();

  CheckConnectionShared<next::udex::subscriber::DataSubscriber>(
      subscriber, {"SIM VFB.AlgoVehCycle.VehDyn", "SIM VFB.AlgoVehCycle.VehDyn1"}, 1000);
}

TEST_F(BaseControlDataPoolTestFixture, SetupDifferentSignalsMissingSingal_fail) {

  TriggerDataInfo test_info;

  test_info.execution_name = "trigger1";
  test_info.component_name = "component1";
  MemberDataInfo member;
  member.type = SignalType::connected;
  member.control_data_url_name = "SigHeader.cycleCounter";
  member.connection_name = "SIM VFB.AlgoVehCycle.sSigHeader.uiTimeStamp"; // wrong signal to check error handling
  test_info.info.push_back(member);

  BaseControlDataPoolTester test("flow1");

  EXPECT_TRUE(test.SetupInput({test_info})) << "Setup of DataPool failed";
  auto backendpool = test.GetDataPoolBackend(component_name_dummy, execution_name_dummy);
  ASSERT_EQ(backendpool.tree_extractors.size(), 0);
}

TEST_F(BaseControlDataPoolTestFixture, GetLatestDataDifferentChannels_Success) {
  TriggerDataInfo test_info = TriggerDataInfoThreeConnectionsFromTwoGroups();
  BaseControlDataPoolTester test("flow1");

  EXPECT_TRUE(test.SetupInput({test_info})) << "Setup of DataPool failed";
  auto backendpool = test.GetDataPoolBackend(component_name_dummy, execution_name_dummy);
  ASSERT_EQ(backendpool.tree_extractors.size(), 3);
  // check that the bckendppol is filled -> TODO Create CHECKS
  // backendpool.tree_extractors["SigHeader.uiTimestamp1"];
  // backendpool.tree_extractors["SigHeader.uiTimestamp"];
  // backendpool.tree_extractors["SigHeader.uiVersionNumber"];

  auto connected = CheckConnectionShared<next::udex::subscriber::DataSubscriber>(test.GetSubscriberTest(),
                                                                                 {"SIM VFB.AlgoVehCycle.VehDyn"}, 5000);
  EXPECT_TRUE(connected) << "Not all urls are connected";
  connected = CheckConnectionShared<next::udex::subscriber::DataSubscriber>(test.GetSubscriberTest(),
                                                                            {"SIM VFB.AlgoVehCycle.VehDyn1"}, 5000);
  EXPECT_TRUE(connected) << "Not all urls are connected";

  // Set values for group1, do not set anything for group2
  SetValueToMemoryGroup1<uint32_t>(123, 0);
  SetValueToMemoryGroup1<uint32_t>(42, 4);

  EXPECT_TRUE(publisher->publishData("SIM VFB.AlgoVehCycle.VehDyn", VehDynDummy, size_veh_dyn, false, 1))
      << "publishing failed";

  bool success;
  auto testdata = test.GetNewData(component_name_dummy, execution_name_dummy, success);
  EXPECT_FALSE(success) << "VehDyn1 topic is not available and should return a error";

  EXPECT_EQ(testdata.connected_signal["uiVersionNumber"].value.asUint32, 123u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp"].value.asUint32, 42u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp1"].value.asUint32, 0u) << "Value not correct";

  // Update values for group1, do not set anything for group2
  SetValueToMemoryGroup1<uint32_t>(321, 0);
  SetValueToMemoryGroup1<uint32_t>(42, 4);

  EXPECT_TRUE(publisher->publishData("SIM VFB.AlgoVehCycle.VehDyn", VehDynDummy, size_veh_dyn, false, 1))
      << "publishing failed";

  testdata = test.GetNewData(component_name_dummy, execution_name_dummy, success);
  EXPECT_FALSE(success) << "VehDyn1 topic is not available and should return a error";

  EXPECT_EQ(testdata.connected_signal["uiVersionNumber"].value.asUint32, 321u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp"].value.asUint32, 42u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp1"].value.asUint32, 0u) << "Value not correct";

  // Set values for group2, do not set anything for group1
  SetValueToMemoryGroup2<uint32_t>(219, 4);

  EXPECT_TRUE(publisher->publishData("SIM VFB.AlgoVehCycle.VehDyn1", VehDyn1Dummy, size_veh_dyn, false, 1))
      << "publishing failed";

  testdata = test.GetNewData(component_name_dummy, execution_name_dummy, success);
  EXPECT_TRUE(success);

  EXPECT_EQ(testdata.connected_signal["uiVersionNumber"].value.asUint32, 321u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp"].value.asUint32, 42u) << "Value not correct";
  EXPECT_EQ(testdata.connected_signal["SigHeader.uiTimeStamp1"].value.asUint32, 219u) << "Value not correct";
}

TEST_F(BaseControlDataPoolTestFixture, GetNewDataOnFalsInfoNoCrash) {
  TriggerDataInfo test_info;

  test_info.execution_name = "trigger1";
  test_info.component_name = "component1";
  MemberDataInfo member;
  member.type = SignalType::connected;
  member.control_data_url_name = "SigHeader.cycleCounter";
  member.connection_name = "SIM VFB.AlgoVehCycle.sSigHeader.uiTimeStamp"; // wrong signal to check error handling
  test_info.info.push_back(member);

  BaseControlDataPoolTester test("flow1");
  bool success = false;

  EXPECT_TRUE(test.SetupInput({test_info})) << "Setup of DataPool failed";

  auto data = test.GetNewData(test_info.component_name, test_info.execution_name, success);
  EXPECT_TRUE(success) << "topic is not available and should return a error";

  EXPECT_TRUE(data.connected_signal.size() == 0);
}
} // namespace orchestrator
} // namespace control
} // namespace next
