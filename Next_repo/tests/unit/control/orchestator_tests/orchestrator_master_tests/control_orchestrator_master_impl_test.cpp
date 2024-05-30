#include <gtest/gtest.h>
#include <list>

#include <next/sdk/sdk.hpp>

#include <next/control/orchestrator/flow_control_config.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include "orchestrator_master_impl.h"

namespace next {
namespace control {
namespace orchestrator {

class OrchestratorMasterImplTester : public next::control::orchestrator::OrchestratorMasterImpl {
public:
  OrchestratorMasterImplTester(){};
  ~OrchestratorMasterImplTester(){};
  FlowLeaf createFlow(const std::string &flow_id) {
    return next::control::orchestrator::OrchestratorMasterImpl::createFlow(flow_id);
  }
  const std::multimap<std::string, FlowLeaf> &getDataPkgMap() { return multi_map_data_triggering_; }
  const std::multimap<std::string, FlowLeaf> &getDataCycleMap() { return multi_map_cycle_triggering_; }
  const std::multimap<std::string, FlowLeaf> &getTimeBasedPkgMap() { return multi_map_timebased_triggering_; }

  const std::unordered_map<std::string, std::shared_ptr<ControlDataGenerator>> &getControlDataGenerator() {
    return ctrl_data_generators_;
  }
};

class TestOrchestratorMasterImplFixture : public next::sdk::testing::TestUsingEcal {
public:
  TestOrchestratorMasterImplFixture() { instance_name_ = "TestOrchestratorMasterImplFixture"; }
  void TearDown() {
    next::appsupport::ConfigClient::do_reset();
    TestUsingEcal::TearDown();
  }
};

TEST_F(TestOrchestratorMasterImplFixture, loadFlowFromConfig_Success) {
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("test_player_config.json");
  next::appsupport::ConfigClient::finish_init();
  auto flows =
      next::appsupport::ConfigClient::getConfig()->getChildren(next::appsupport::configkey::orchestrator::getFlows());
  ASSERT_EQ(flows.size(), 4);
  ASSERT_EQ(flows[0], "flow1");
  ASSERT_EQ(flows[1], "anotherFlow");
  ASSERT_EQ(flows[2], "flowTimeBased");
  ASSERT_EQ(flows[3], "invalidClients");

  OrchestratorMasterImplTester test_orchestrator;
  auto flow_leaf = test_orchestrator.createFlow(flows[0]);
  ASSERT_EQ(flow_leaf.id, flows[0]);
  ASSERT_EQ(flow_leaf.meta_data_.delay, 0u);
  ASSERT_EQ(flow_leaf.meta_data_.isSync, true);
  ASSERT_EQ(flow_leaf.trigger_.type, TriggerType::SIGNAL_TRIGGER);
  ASSERT_EQ(flow_leaf.trigger_.val.signal_name, "ADC4xx.EMF.EmfGlobalTimestamp");
  ASSERT_EQ(flow_leaf.trigger_ports_[0].first, "EmfNodeTriggering");
  ASSERT_EQ(flow_leaf.trigger_ports_[0].second, "runSequencer_response");

  flow_leaf = test_orchestrator.createFlow(flows[1]);
  ASSERT_EQ(flow_leaf.id, flows[1]);
  ASSERT_EQ(flow_leaf.meta_data_.delay, 100u);
  ASSERT_EQ(flow_leaf.meta_data_.isSync, false);
  ASSERT_EQ(flow_leaf.trigger_.type, TriggerType::CYCLE_START_TRIGGER);
  ASSERT_EQ(flow_leaf.trigger_.val.cycle_start, 123);
  ASSERT_EQ(flow_leaf.trigger_ports_.size(), 3);
  ASSERT_EQ(flow_leaf.trigger_ports_[0].first, "client1");
  ASSERT_EQ(flow_leaf.trigger_ports_[0].second, "port1");
  ASSERT_EQ(flow_leaf.trigger_ports_[1].first, "client2");
  ASSERT_EQ(flow_leaf.trigger_ports_[1].second, "port2");
  ASSERT_EQ(flow_leaf.trigger_ports_[2].first, "client3");
  ASSERT_EQ(flow_leaf.trigger_ports_[2].second, "port3");

  flow_leaf = test_orchestrator.createFlow(flows[2]);
  ASSERT_EQ(flow_leaf.id, flows[2]);
  ASSERT_EQ(flow_leaf.meta_data_.delay, 0u);
  ASSERT_EQ(flow_leaf.meta_data_.isSync, true);
  ASSERT_EQ(flow_leaf.trigger_.type, TriggerType::TIME_TRIGGER);
  ASSERT_EQ(flow_leaf.trigger_.val.time, 40000);
  ASSERT_EQ(flow_leaf.trigger_.val.last_trigger_time, 0);
  ASSERT_EQ(flow_leaf.trigger_ports_.size(), 1);
  ASSERT_EQ(flow_leaf.trigger_ports_[0].first, "test_timebased_0");
  ASSERT_EQ(flow_leaf.trigger_ports_[0].second, "Test_timebased_0_m_event_trigger");

  flow_leaf = test_orchestrator.createFlow(flows[3]);
  ASSERT_EQ(flow_leaf.trigger_ports_.size(), 1);
  ASSERT_EQ(flow_leaf.trigger_ports_[0].first, "client2");
  ASSERT_EQ(flow_leaf.trigger_ports_[0].second, "port2");
  ASSERT_EQ(flow_leaf.meta_data_.delay, 100u);
  ASSERT_EQ(flow_leaf.meta_data_.isSync, false);
  ASSERT_EQ(flow_leaf.trigger_.type, TriggerType::CYCLE_START_TRIGGER);
  ASSERT_EQ(flow_leaf.trigger_.val.cycle_start, 123);
}

TEST_F(TestOrchestratorMasterImplFixture, test_loadConfigurationFromExternalFile_Success) {
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("test_control_data_with_file_link.json");
  next::appsupport::ConfigClient::finish_init();
  OrchestratorMasterImplTester test_orchestrator;
  test_orchestrator.loadConfiguration();

  auto test_control_generator = test_orchestrator.getControlDataGenerator();
  EXPECT_EQ(test_control_generator.size(), 2);
  ASSERT_TRUE(test_control_generator.find("anotherFlow") != test_control_generator.end());
  auto flows = test_orchestrator.getDataCycleMap();
  EXPECT_EQ(flows.size(), 1);
  auto flow_details = flows.find("123");
  ASSERT_NE(flow_details, flows.end());

  auto &clients = flow_details->second.trigger_ports_;
  ASSERT_EQ(clients.size(), 3);
  ASSERT_EQ(clients[0].first, "client1");
  ASSERT_EQ(clients[0].second, "port1");
  ASSERT_EQ(clients[1].first, "client2");
  ASSERT_EQ(clients[1].second, "port2");
  ASSERT_EQ(clients[2].first, "client3");
  ASSERT_EQ(clients[2].second, "port3");
}

TEST(TestOrchestratorMasterImpl, default_values_flow) {

  FlowLeaf flow_leaf_default;
  EXPECT_EQ(flow_leaf_default.dependency_leaves_.size(), 0);
  EXPECT_EQ(flow_leaf_default.id, "");
  EXPECT_EQ(flow_leaf_default.meta_data_.delay, 0u);
  EXPECT_EQ(flow_leaf_default.meta_data_.isSync, true);
  EXPECT_EQ(flow_leaf_default.synch_.synch_id_url, "");
  EXPECT_EQ(flow_leaf_default.synch_.synch_timestamp_url, "");
  EXPECT_EQ(flow_leaf_default.trigger_.type, TriggerType::NONE);
  EXPECT_EQ(flow_leaf_default.trigger_.val.cycle_end, 0);
  EXPECT_EQ(flow_leaf_default.trigger_.val.cycle_start, 0);
  EXPECT_EQ(flow_leaf_default.trigger_.val.last_trigger_time, 0);
  EXPECT_EQ(flow_leaf_default.trigger_.val.signal_name, "");
  EXPECT_EQ(flow_leaf_default.trigger_.val.time, 0);
  EXPECT_EQ(flow_leaf_default.trigger_ports_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, registerFlow_dataBased) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_dataBased;
  flow_leaf_dataBased.trigger_.type = TriggerType::SIGNAL_TRIGGER;
  std::string topic_url = "TopicUrl";
  flow_leaf_dataBased.trigger_.val.signal_name = topic_url;
  test_orchestrator.registerFlow(flow_leaf_dataBased);

  std::multimap<std::string, FlowLeaf> multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 1);
  EXPECT_EQ(multi_map_data_triggering.count(topic_url), 1);
  EXPECT_EQ(multi_map_data_triggering.begin()->second.trigger_.val.signal_name, topic_url);

  // other trigger flows shall be emtpy
  std::multimap<std::string, FlowLeaf> multi_map_cycle_triggering = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering.size(), 0);

  test_orchestrator.unRegisterFlow(flow_leaf_dataBased);

  // all trigger flows shall be emtpy
  multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 0);
  multi_map_cycle_triggering = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering.size(), 0);
  multi_map_timebase_triggering = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering.size(), 0);
}

TEST(TestOrchestratorMasterImpl, registerFlow_cycleBasedStartTrigger) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_cycleBasedStart;
  flow_leaf_cycleBasedStart.trigger_.type = TriggerType::CYCLE_START_TRIGGER;
  int cycleStart = 1;
  flow_leaf_cycleBasedStart.trigger_.val.cycle_start = cycleStart;
  test_orchestrator.registerFlow(flow_leaf_cycleBasedStart);

  std::multimap<std::string, FlowLeaf> multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 1);
  EXPECT_EQ(multi_map_cycle_triggering_.count(std::to_string(cycleStart)), 1);
  EXPECT_EQ(multi_map_cycle_triggering_.begin()->second.trigger_.val.cycle_start, cycleStart);
  EXPECT_EQ(multi_map_cycle_triggering_.begin()->second.trigger_.val.cycle_end, 0);

  // other trigger flows shall be emtpy
  std::multimap<std::string, FlowLeaf> multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);

  test_orchestrator.unRegisterFlow(flow_leaf_cycleBasedStart);

  // all trigger flows shall be emtpy
  multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);
  multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, registerFlow_cycleBasedEndTrigger) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_cycleBasedEnd;
  flow_leaf_cycleBasedEnd.trigger_.type = TriggerType::CYCLE_END_TRIGGER;
  int cycleEnd = 1;
  flow_leaf_cycleBasedEnd.trigger_.val.cycle_end = cycleEnd;
  test_orchestrator.registerFlow(flow_leaf_cycleBasedEnd);

  std::multimap<std::string, FlowLeaf> multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 1);
  EXPECT_EQ(multi_map_cycle_triggering_.count(std::to_string(cycleEnd)), 1);
  EXPECT_EQ(multi_map_cycle_triggering_.begin()->second.trigger_.val.cycle_start, 0);
  EXPECT_EQ(multi_map_cycle_triggering_.begin()->second.trigger_.val.cycle_end, cycleEnd);

  // other trigger flows shall be emtpy
  std::multimap<std::string, FlowLeaf> multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);

  test_orchestrator.unRegisterFlow(flow_leaf_cycleBasedEnd);

  // all trigger flows shall be emtpy
  multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);
  multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, registerFlow_timeBasedTrigger) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_timeBased;
  flow_leaf_timeBased.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time = 10000;
  flow_leaf_timeBased.trigger_.val.time = cycle_time;

  test_orchestrator.registerFlow(flow_leaf_timeBased);

  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 1);
  EXPECT_EQ(multi_map_timebase_triggering_.count(std::to_string(cycle_time)), 1);
  EXPECT_EQ(multi_map_timebase_triggering_.begin()->second.trigger_.val.time, cycle_time);
  EXPECT_EQ(multi_map_timebase_triggering_.begin()->second.trigger_.val.last_trigger_time, 0);

  // other trigger flows shall be emtpy
  std::multimap<std::string, FlowLeaf> multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);

  test_orchestrator.unRegisterFlow(flow_leaf_timeBased);

  // all trigger flows shall be emtpy
  multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);
  multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, registerFlow_Unkown) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_unkown;
  flow_leaf_unkown.trigger_.type = TriggerType::NONE;

  // register "none" flow -> shall be ignored
  test_orchestrator.registerFlow(flow_leaf_unkown);

  // all trigger flows shall be emtpy
  std::multimap<std::string, FlowLeaf> multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);
  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);

  // unregister "none" flow
  test_orchestrator.unRegisterFlow(flow_leaf_unkown);

  // all trigger flows shall be emtpy
  multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
  multi_map_cycle_triggering_ = test_orchestrator.getDataCycleMap();
  ASSERT_EQ(multi_map_cycle_triggering_.size(), 0);
  multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, unregister_not_existing_flow) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_dataBased;
  flow_leaf_dataBased.trigger_.type = TriggerType::SIGNAL_TRIGGER;

  test_orchestrator.unRegisterFlow(flow_leaf_dataBased);

  std::multimap<std::string, FlowLeaf> multi_map_data_triggering_ = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering_.size(), 0);
}

TEST(TestOrchestratorMasterImpl, register_same_flow_names) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_dataBased1;
  flow_leaf_dataBased1.trigger_.type = TriggerType::SIGNAL_TRIGGER;
  std::string topic_url = "TopicUrl";
  flow_leaf_dataBased1.trigger_.val.signal_name = topic_url;
  test_orchestrator.registerFlow(flow_leaf_dataBased1);

  FlowLeaf flow_leaf_dataBased2;
  flow_leaf_dataBased2.trigger_.type = TriggerType::SIGNAL_TRIGGER;
  flow_leaf_dataBased2.trigger_.val.signal_name = topic_url;
  test_orchestrator.registerFlow(flow_leaf_dataBased2);

  FlowLeaf flow_leaf_dataBased3;
  flow_leaf_dataBased3.trigger_.type = TriggerType::SIGNAL_TRIGGER;
  flow_leaf_dataBased3.trigger_.val.signal_name = topic_url;
  flow_leaf_dataBased3.meta_data_.delay = 100u;
  test_orchestrator.registerFlow(flow_leaf_dataBased3);

  FlowLeaf flow_leaf_dataBased4;
  flow_leaf_dataBased4.trigger_.type = TriggerType::SIGNAL_TRIGGER;
  std::string topic_url4 = "TopicUrl4";
  flow_leaf_dataBased4.trigger_.val.signal_name = topic_url4;
  test_orchestrator.registerFlow(flow_leaf_dataBased4);

  // flow_leaf_dataBased2 is ignored because it's equal to flow_leaf_dataBased1
  std::multimap<std::string, FlowLeaf> multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 4);
  EXPECT_EQ(multi_map_data_triggering.count(topic_url), 3);
  EXPECT_EQ(multi_map_data_triggering.count(topic_url4), 1);

  // remove all elements which are equal to first flow
  test_orchestrator.unRegisterFlow(flow_leaf_dataBased1);
  multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 2);
  EXPECT_EQ(multi_map_data_triggering.equal_range(topic_url).first->second.meta_data_.delay, 100u);
  EXPECT_EQ(multi_map_data_triggering.equal_range(topic_url4).first->second.trigger_.val.signal_name, topic_url4);

  // remove nothing because element was already removed in the previous step
  test_orchestrator.unRegisterFlow(flow_leaf_dataBased2);
  multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 2);

  test_orchestrator.unRegisterFlow(flow_leaf_dataBased3);
  multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 1);
  EXPECT_EQ(multi_map_data_triggering.equal_range(topic_url4).first->second.trigger_.val.signal_name, topic_url4);

  test_orchestrator.unRegisterFlow(flow_leaf_dataBased4);
  multi_map_data_triggering = test_orchestrator.getDataPkgMap();
  ASSERT_EQ(multi_map_data_triggering.size(), 0);
}

TEST(TestOrchestratorMasterImpl, triggerTimestampBased_emptyTriggers) {
  OrchestratorMasterImplTester test_orchestrator;

  std::map<std::string, TriggerStatus> trigger_output;
  test_orchestrator.triggerTimestampBased(100, trigger_output);
  EXPECT_EQ(trigger_output.size(), 0);
}

TEST(TestOrchestratorMasterImpl, triggering_noTriggerPortsSet) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_timeBased;
  flow_leaf_timeBased.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time = 10000;
  flow_leaf_timeBased.trigger_.val.time = cycle_time;

  test_orchestrator.registerFlow(flow_leaf_timeBased);

  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 1);

  std::map<std::string, TriggerStatus> trigger_output;
  test_orchestrator.triggerTimestampBased(cycle_time, trigger_output);
  EXPECT_EQ(trigger_output.size(), 0);
}

TEST(TestOrchestratorMasterImpl, triggerTimestampBased_trigger) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_timeBased;
  flow_leaf_timeBased.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time = 10000;
  flow_leaf_timeBased.trigger_.val.time = cycle_time;
  std::string trigger_name = "trigger_name";
  std::string trigger_port = "trigger_port";
  flow_leaf_timeBased.trigger_ports_.push_back(std::make_pair(trigger_name, trigger_port));

  test_orchestrator.registerFlow(flow_leaf_timeBased);

  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 1);

  {
    // trigger always for the first cycle
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(1, trigger_output);
    EXPECT_EQ(trigger_output.size(), 1);
    EXPECT_TRUE(trigger_output.find(trigger_name + trigger_port) != trigger_output.end());
  }
  {
    // no triggering because cycle difference is "cycle_time" - 1
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time, trigger_output);
    EXPECT_EQ(trigger_output.size(), 0);
  }
  {
    // triggering
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time + 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 1);
  }
  {
    // no triggering because cycle difference is too small
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time * 2u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 0);
  }
  {
    // triggering
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time * 2u + 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 1);
  }
}

TEST(TestOrchestratorMasterImpl, triggerTimestampBased_multiTriggers) {
  OrchestratorMasterImplTester test_orchestrator;

  FlowLeaf flow_leaf_timeBased;
  flow_leaf_timeBased.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time = 10000;
  std::string trigger_name = "trigger_name";
  std::string trigger_port = "trigger_port";
  flow_leaf_timeBased.trigger_.val.time = cycle_time;
  flow_leaf_timeBased.trigger_ports_.push_back(std::make_pair(trigger_name, trigger_port));

  FlowLeaf flow_leaf_timeBased2;
  flow_leaf_timeBased2.trigger_.type = TriggerType::TIME_TRIGGER;
  std::string trigger_name2 = "trigger_name2";
  std::string trigger_port2 = "trigger_port2";
  flow_leaf_timeBased2.trigger_.val.time = cycle_time;
  flow_leaf_timeBased2.trigger_ports_.push_back(std::make_pair(trigger_name2, trigger_port2));

  FlowLeaf flow_leaf_timeBased3;
  flow_leaf_timeBased3.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time3 = 100000;
  std::string trigger_name3 = "trigger_name3";
  std::string trigger_port3 = "trigger_port3";
  flow_leaf_timeBased3.trigger_.val.time = cycle_time3;
  flow_leaf_timeBased3.trigger_ports_.push_back(std::make_pair(trigger_name3, trigger_port3));

  test_orchestrator.registerFlow(flow_leaf_timeBased);
  test_orchestrator.registerFlow(flow_leaf_timeBased2);
  test_orchestrator.registerFlow(flow_leaf_timeBased3);

  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 3);

  {
    // initial triggering -> all flows are triggered
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(1, trigger_output);
    EXPECT_EQ(trigger_output.size(), 3);
    EXPECT_TRUE(trigger_output.find(trigger_name + trigger_port) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name2 + trigger_port2) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name3 + trigger_port3) != trigger_output.end());
  }
  {
    // no triggering because cycle difference is "cycle_time" - 1
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time, trigger_output);
    EXPECT_EQ(trigger_output.size(), 0);
  }
  {
    // triggering flows with "cycle_time"
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time + 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 2);
    EXPECT_TRUE(trigger_output.find(trigger_name + trigger_port) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name2 + trigger_port2) != trigger_output.end());
  }
  {
    // no triggering because cycle difference is too small
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time * 2u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 0);
  }
  {
    // triggering flows with "cycle_time" again
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time * 2u + 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 2);
    EXPECT_TRUE(trigger_output.find(trigger_name + trigger_port) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name2 + trigger_port2) != trigger_output.end());
  }
  {
    // triggering all flows
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time3 + 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 3);
    EXPECT_TRUE(trigger_output.find(trigger_name + trigger_port) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name2 + trigger_port2) != trigger_output.end());
    EXPECT_TRUE(trigger_output.find(trigger_name3 + trigger_port3) != trigger_output.end());
  }
}

TEST(TestOrchestratorMasterImpl, triggerTimestampBased_nonIncreasingTimestamps) {
  OrchestratorMasterImplTester test_orchestrator;

  std::string trigger_name = "trigger_name";
  std::string trigger_port = "trigger_port";

  FlowLeaf flow_leaf_timeBased;
  flow_leaf_timeBased.trigger_.type = TriggerType::TIME_TRIGGER;
  unsigned int cycle_time = 10000;
  flow_leaf_timeBased.trigger_.val.time = cycle_time;
  flow_leaf_timeBased.trigger_ports_.push_back(std::make_pair(trigger_name, trigger_port));

  test_orchestrator.registerFlow(flow_leaf_timeBased);

  std::multimap<std::string, FlowLeaf> multi_map_timebase_triggering_ = test_orchestrator.getTimeBasedPkgMap();
  ASSERT_EQ(multi_map_timebase_triggering_.size(), 1);

  {
    // triggering
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time, trigger_output);
    EXPECT_EQ(trigger_output.size(), 1);
  }
  {
    // no triggering -> timestamp not increasing
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time - 1u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 0);
  }
  {
    // triggering
    std::map<std::string, TriggerStatus> trigger_output;
    test_orchestrator.triggerTimestampBased(cycle_time * 2u, trigger_output);
    EXPECT_EQ(trigger_output.size(), 1);
  }
}

} // namespace orchestrator
} // namespace control
} // namespace next
