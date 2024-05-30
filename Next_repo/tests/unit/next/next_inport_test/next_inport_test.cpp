#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/sdk.hpp>

#include "next_inport.hpp"

#include <next/udex/publisher/data_publisher.hpp>

namespace next {
namespace inport {
using config = next::appsupport::ConfigClient;

typedef unsigned char uint8;       /* [PLATFORM013] */
typedef unsigned short uint16;     /* [PLATFORM014] */
typedef unsigned long long uint64; /* 0 .. 18446744073709551615   */
typedef unsigned long uint32;      /* [PLATFORM015] */

typedef uint16 AlgoCycleCounter_t;
typedef uint32 AlgoInterfaceVersionNumber_t;
typedef uint64 AlgoDataTimeStamp_t;

struct SignalHeader_t {
  AlgoDataTimeStamp_t uiTimeStamp;
  AlgoCycleCounter_t uiMeasurementCounter;
  AlgoCycleCounter_t uiCycleCounter;
  uint8 eSigStatus;
};

struct TestType {
  AlgoInterfaceVersionNumber_t u_VersionNumber;
  SignalHeader_t sSigHeader;
  uint64_t payload;
};

struct TestTypeHeader {
  AlgoInterfaceVersionNumber_t u_VersionNumber;
  SignalHeader_t sSigHeader;
};

struct SyncRef_t {
  AlgoInterfaceVersionNumber_t u_VersionNumber;
  SignalHeader_t sSigHeader;
  TestTypeHeader testType;
};

inline std::string GetSyncRefPayloadPortName() { return "SyncRef_Root.test_type"; }
inline std::string GetSyncRefPortName() { return "SyncRef_Root.syncref"; }

class NextInportTester : public NextInport {
public:
  NextInportTester() : NextInport("test_component") {}
  std::unordered_map<std::string, std::shared_ptr<next::udex::extractor::DataExtractor>> GetExtractor() {
    return map_of_extractors_;
  }
  std::shared_ptr<next::udex::subscriber::DataSubscriber> getSubscriber() { return data_subscriber_; }
  SyncManager getSyncManager() { return sync_manager_; }

  void WriteTestConfig(const std::string &port_name, const std::string &sync_algo, const std::string &sync_mode) {
    std::string sync_ref_url = "SIM VFB.SyncRef_Root.syncref.sSigHeader.uiTimeStamp";
    std::string sync_ref_url_timestamp_key = "test_type.uiTimeStamp";
    std::string sync_ref_member_name_timestamp_key = "sSigHeader.uiTimeStamp";
    std::string sync_ref_selection_mode = "Timestamp";
    next::appsupport::ConfigClient::do_init(component_name_);
    auto config = next::appsupport::ConfigClient::getConfig();
    config->put("test", "test_value");
    std::string composition_url = "SIM VFB.SyncRef_Root.test_type";
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getCompositionPortUrlKey(component_name_, port_name), composition_url);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncAlgoKey(component_name_, port_name), sync_algo);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncModeKey(component_name_, port_name), sync_mode);
    // setup syncref
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncRefUrlKey(component_name_, port_name), sync_ref_url);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncMemberUrlTimestampKey(component_name_, port_name),
        sync_ref_url_timestamp_key);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncMemberNameTimestampKey(component_name_, port_name),
        sync_ref_member_name_timestamp_key);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncRefSelectionMode(component_name_, port_name), sync_ref_selection_mode);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncRefUrl(component_name_, port_name), sync_ref_url);

    next::appsupport::ConfigClient::finish_init();
  }

  void WriteTestConfigV1(const std::string &port_name, const std::string &sync_algo, const std::string &sync_mode) {
    std::string sync_ref_url = "SIM VFB.SyncRef_Root.syncref.sSigHeader.uiTimeStamp";
    std::string sync_ref_url_timestamp_key = "test_type.uiTimeStamp";
    std::string sync_ref_member_name_timestamp_key = "sSigHeader.uiTimeStamp";
    std::string composition_url = "SIM VFB.SyncRef_Root.test_type";

    next::appsupport::ConfigClient::do_init(component_name_);
    auto config = next::appsupport::ConfigClient::getConfig();
    config->put("test", "test_value");

    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getCompositionPortUrlKey(component_name_, port_name), composition_url);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getTimestampSyncrefValueMemberName(component_name_, port_name),
        sync_ref_member_name_timestamp_key);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getTimestampSyncrefPortMemberUrl(component_name_, port_name),
        sync_ref_url_timestamp_key);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getRefValueMode(component_name_, port_name), sync_mode);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSelectionAlgo(component_name_, port_name), sync_algo);

    if (sync_mode == "timestamp") {
      next::appsupport::ConfigClient::getConfig()->put(
          next::appsupport::configkey::getSyncValueTimestampUrl(component_name_, port_name), sync_ref_url);
    } else if (sync_mode == "sync_ref_timestamp") {
      next::appsupport::ConfigClient::getConfig()->put(
          next::appsupport::configkey::getSyncValueSyncrefBaseControlUrl(component_name_, port_name), sync_ref_url);
      next::appsupport::ConfigClient::getConfig()->put(
          next::appsupport::configkey::getSyncValueSyncrefSyncHeaderUrl(component_name_, port_name), sync_ref_url);
    }

    next::appsupport::ConfigClient::finish_init();
  }
};

class NextInputPortFixture : public next::sdk::testing::TestUsingEcal {
public:
  NextInputPortFixture() { this->instance_name_ = "NextInputPortFixture"; }
  void SetUp() override {
    TestUsingEcal::SetUp();
    config::do_init("test_component");
    config::finish_init();

    port_dummy_data_ = new char[dummy_large_port_size];
    ASSERT_TRUE(port_dummy_data_) << "Dummy data allocation to the heap failed";
  }

  void TearDown() override {
    config::getConfig()->do_reset();
    TestUsingEcal::TearDown();
    if (port_dummy_data_ != nullptr) {
      delete[] port_dummy_data_;
    }
  }
  const size_t dummy_large_port_size = 500000;
  char *port_dummy_data_;
  std::string generateUniqueName(const std::string &in) {
    return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::steady_clock::now().time_since_epoch())
                                   .count());
  }

  void PublishTestPayload(next::udex::publisher::DataPublisher &pub, const AlgoDataTimeStamp_t uiTimestamp,
                          const AlgoCycleCounter_t uiMeasurementCounter, const uint64_t payload) {
    TestType test_syncref_payload;
    test_syncref_payload.u_VersionNumber = 1;
    test_syncref_payload.sSigHeader.uiTimeStamp = uiTimestamp;
    test_syncref_payload.sSigHeader.uiMeasurementCounter = uiMeasurementCounter;
    test_syncref_payload.sSigHeader.eSigStatus = 1;
    test_syncref_payload.payload = payload;
    pub.publishData("SIM VFB." + GetSyncRefPayloadPortName(), (void *)&test_syncref_payload, sizeof(TestType));
  }

  void PublishSyncref(next::udex::publisher::DataPublisher &pub, const AlgoDataTimeStamp_t uiTimestamp,
                      const AlgoCycleCounter_t uiMeasurementCounter, const AlgoDataTimeStamp_t ttuiTimestamp,
                      const AlgoCycleCounter_t ttuiMeasurementCounter, const uint64_t header_timestamp) {
    SyncRef_t sync_ref_data;
    sync_ref_data.u_VersionNumber = 1;
    sync_ref_data.sSigHeader.uiTimeStamp = uiTimestamp;
    sync_ref_data.sSigHeader.uiMeasurementCounter = uiMeasurementCounter;
    sync_ref_data.sSigHeader.eSigStatus = 1;
    sync_ref_data.testType.u_VersionNumber = 1;
    sync_ref_data.testType.sSigHeader.uiTimeStamp = ttuiTimestamp;
    sync_ref_data.testType.sSigHeader.uiMeasurementCounter = ttuiMeasurementCounter;
    sync_ref_data.testType.sSigHeader.eSigStatus = 1;
    pub.publishData(GetSyncRefPortName(), (void *)&sync_ref_data, sizeof(SyncRef_t), true, header_timestamp);
  }
};

TEST_F(NextInputPortFixture, checkConfigValuesSet) {
  InportConfig::SetValuesToConfig({}, "test_component");
  EXPECT_EQ(config::getConfig()->get_int(appsupport::configkey::getInportVersionKey("test_component", ""), 0), 0);
}

TEST_F(NextInputPortFixture, checkIsDataAvailableAlwaysTrue_success) {
  auto tester = next::inport::NextInportTester();
  EXPECT_FALSE(tester.isDataAvailable(""));
  tester.setDataAvailabilityMode(ALWAYS_TRUE);
  EXPECT_TRUE(tester.isDataAvailable(""));
  EXPECT_TRUE(tester.isDataAvailable("asdf"));
}

TEST_F(NextInputPortFixture, single_port_no_config_success) {
  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 15;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();
  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);
}

TEST_F(NextInputPortFixture, single_port_with_config_syncref_getPublish) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("syncref.sdl", true);
  eCAL::Util::EnableLoopback(true);
  std::string port_name = "testport_syncref", port_type = "test_type";
  size_t port_size = sizeof(TestType);
  uint32_t port_version = 11;
  SyncMapping sync_mapping = {{"test_type.uiTimeStamp", "sSigHeader.uiTimeStamp"},
                              {"test_type.uiMeasurementCounter", "sSigHeader.uiMeasurementCounter"}};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = port_name_mod;
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync_mapping};

  auto tester = next::inport::NextInportTester();

  tester.WriteTestConfig(port_name_mod_1, "FIND_EXACT_VALUE", "SyncRef");
  std::vector<std::string> testRet = next::appsupport::ConfigClient::getConfig()->getChildren("test_component:in_port");

  // loading player config from file for sync reference values
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("config_sync_ref.json");

  tester.SetupInput("", {test_inport1});
  tester.SetupSavedInputConfig();

  SyncManager manager = tester.getSyncManager();

  manager.isUpdated(port_name_mod_1);
  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod_1];

  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();
  std::vector<std::string> topics_to_check;

  for (auto sub_info : info) {
    topics_to_check.push_back(sub_info.second.topic);
  }

  ASSERT_TRUE(CheckConnectionShared(sub, topics_to_check, 5000)) << "Some topics are not connected.";

  // (publisher, timestamp, measurement counter, payload)
  PublishTestPayload(pub, 100000, 1, 1);
  PublishTestPayload(pub, 100002, 2, 2);
  PublishTestPayload(pub, 100003, 3, 3);

  uint64_t header_timestamp = 10002105;
  // (pub, timestamp, measurement  counter, test type timestamp, test type measurement counter, header_timestamp)
  PublishSyncref(pub, 100500, 5, 100000, 1, header_timestamp);

  uint64_t header_timestamp2 = 10002110;
  PublishSyncref(pub, 100505, 6, 100003, 3, header_timestamp2);

  // std::this_thread::sleep_for(std::chrono::seconds(10));

  next::control::events::OrchestratorTrigger trigger{std::to_string(header_timestamp), "flow1"};
  tester.FillReferenceValueAndData(trigger);
  auto package = tester.getSyncedPacket(test_inport1.port_name).data;
  auto result = *reinterpret_cast<TestType *>(package);
  EXPECT_TRUE(result.payload == 1);

  next::control::events::OrchestratorTrigger trigger2{std::to_string(header_timestamp2), "flow1"};
  tester.FillReferenceValueAndData(trigger2);
  package = tester.getSyncedPacket(test_inport1.port_name).data;
  result = *reinterpret_cast<TestType *>(package);
  EXPECT_TRUE(result.payload == 3);
}

// random fail
TEST_F(NextInputPortFixture, single_port_with_config_getPublish) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport_notinconfig", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, port_size, port_type, sync};
  std::string test_url_not_available = "SIM VFB.NotThere";
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays.ten_more_floats";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_not_available);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2),
                           test_url_available_subgroup);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport, test_inport1, test_inport2});

  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);
  search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_1];
  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();

  ASSERT_EQ(info.size(), 3);
  EXPECT_EQ(info[1].url, test_url_not_available);
  EXPECT_EQ(info[2].url, test_url_available_group);
  EXPECT_EQ(info[3].topic, test_url_available_group);
  EXPECT_EQ(info[3].url, test_url_available_group);

  ASSERT_TRUE(CheckConnectionShared(sub, {test_url_available_group}, 5000));

  size_t offset_change = 0x30;
  size_t offset_subport = 0x28;
  const size_t size_blob = 0x80;
  char memory[size_blob];
  memset(memory, 2, sizeof(char) * size_blob);
  memory[offset_change] = 1;

  auto package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, size_blob);
  package = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, size_blob - offset_subport);

  pub.publishData("SIM VFB.array_root.simple_float_arrays", memory, size_blob, false, 111);

  next::control::events::OrchestratorTrigger trigger{"1", "1"};
  tester.FillReferenceValueAndData(trigger);
  package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 1);
  EXPECT_EQ(package.size, size_blob);
  auto subpackage = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)subpackage.data)[offset_change - offset_subport], 1);
  EXPECT_EQ(subpackage.size, size_blob - offset_subport);
}

TEST_F(NextInputPortFixture, single_SetupInput_single_SetupSavedInputConfig_sequence) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport_notinconfig", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  // set 3 ports and check all, 3 excpected
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;

  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, port_size, port_type, sync};

  std::string test_url_not_available = "SIM VFB.NotThere";
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays.ten_more_floats";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_not_available);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2),
                           test_url_available_subgroup);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport, test_inport1, test_inport2});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_1];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_2);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_2];
  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 3);

  // set another 3 ports and check all, 6 expected
  auto port_name_mod_3 = "3" + port_name_mod;
  auto port_name_mod_4 = "4" + port_name_mod;
  auto port_name_mod_5 = "5" + port_name_mod;
  ComponentInputPortConfig test_inport3{port_name_mod_3, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport4{port_name_mod_4, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport5{port_name_mod_5, port_version, port_size, port_type, sync};

  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_3),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_4),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_5),
                           test_url_available_subgroup);

  tester.SetupInput("", {test_inport3, test_inport4, test_inport5});
  tester.SetupSavedInputConfig();

  created_inports = tester.GetExtractor();

  search = created_inports.find(port_name_mod_3);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_3];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_4);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_4];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_5);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_5];
  EXPECT_FALSE(map_entry == nullptr);

  sub = tester.getSubscriber();
  info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 6);

  // do reset, then expect 0 ports available
  tester.Reset();

  created_inports = tester.GetExtractor();

  sub = tester.getSubscriber();
  info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 0);

  // request another 3 and expect 3
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_3),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_4),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_5),
                           test_url_available_subgroup);

  tester.SetupInput("", {test_inport3, test_inport4, test_inport5});
  tester.SetupSavedInputConfig();

  created_inports = tester.GetExtractor();

  sub = tester.getSubscriber();
  info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 3);
}

TEST_F(NextInputPortFixture, single_SetupInput_single_SetupSavedInputConfig_sequence_with_composition) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport_notinconfig", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  std::vector<std::string> composition_list_1 = {"ARS.test_channel_2", "SIM VFB.array_root.simple_float_arrays"};
  std::vector<std::string> composition_list_2 = {
      "ARS.test_channel_1", "SIM VFB.array_root.simple_float_arrays.ten_more_floats", "ARS.test_channel_2"};
  // set 3 ports and check all, 3 excpected
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;

  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync, composition_list_1};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, port_size, port_type, sync, composition_list_2};

  std::string test_url_not_available = "SIM VFB.NotThere";
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays.ten_more_floats";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_not_available);

  // setting URL first to simulate already existing entries in the config
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           "Device.ARS.test_channel_1");

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport, test_inport1, test_inport2});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_1];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_2);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_2];
  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 3);

  auto portName = config::getConfig()->getStringList(
      appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1), {}, true);

  // check if the composition is available in the config for all set ports
  ASSERT_TRUE(portName[0] == "Device.ARS.test_channel_1");
  ASSERT_TRUE(portName[1] == "SIM VFB.ARS.test_channel_2");
  ASSERT_TRUE(portName[2] == "SIM VFB.array_root.simple_float_arrays");

  auto portName2 = config::getConfig()->getStringList(
      appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2), {}, true);

  ASSERT_TRUE(portName2[0] == "SIM VFB.ARS.test_channel_1");
  ASSERT_TRUE(portName2[1] == "SIM VFB.array_root.simple_float_arrays.ten_more_floats");
  ASSERT_TRUE(portName2[2] == "SIM VFB.ARS.test_channel_2");

  // do reset, then expect 0 ports available
  tester.Reset();

  created_inports = tester.GetExtractor();

  sub = tester.getSubscriber();
  info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 0);
}

TEST_F(NextInputPortFixture, multiple_SetupInput_single_SetupSavedInputConfig) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport_notinconfig", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  // set 3 ports and check all, 3 excpected
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;
  auto port_name_mod_3 = "3" + port_name_mod;
  auto port_name_mod_4 = "4" + port_name_mod;
  auto port_name_mod_5 = "5" + port_name_mod;
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport3{port_name_mod_3, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport4{port_name_mod_4, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport5{port_name_mod_5, port_version, port_size, port_type, sync};

  std::string test_url_not_available = "SIM VFB.NotThere";
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays.ten_more_floats";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_not_available);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_3),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_4),
                           test_url_available_subgroup);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_5),
                           test_url_available_subgroup);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport, test_inport1, test_inport2});
  tester.SetupInput("", {test_inport3, test_inport4, test_inport5});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_1];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_2);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_2];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_3);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_3];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_4);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_4];
  EXPECT_FALSE(map_entry == nullptr);

  search = created_inports.find(port_name_mod_5);
  ASSERT_TRUE(search != created_inports.end());
  map_entry = created_inports[port_name_mod_5];
  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 6);
}

TEST_F(NextInputPortFixture, single_port_invalid_adapter_dll) {
  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 15;
  uint32_t port_version = 12;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();
  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());

  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry.get() == nullptr);
}

TEST_F(NextInputPortFixture, catchEmptyPortName_success) {

  auto tester = next::inport::NextInportTester();
  {
    std::string url_result;
    std::string port_name = ""; // emtpy port
    std::vector<std::string> url_list;
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "undefined");
  }
  {
    std::string url_result;
    std::string port_name = " "; // emtpy port
    std::vector<std::string> url_list;
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "undefined");
  }
  {
    std::string url_result;
    std::string port_name = "noEmptyPort";
    std::vector<std::string> url_list;
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, port_name);
  }
  {
    std::string url_result;
    std::string port_name = ""; // emtpy port
    std::vector<std::string> url_list = {" "};
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "undefined");
  }
  {
    std::string url_result;
    std::string port_name = " "; // emtpy port
    std::vector<std::string> url_list = {" "};
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "undefined");
  }
  {
    std::string url_result;
    std::string port_name;
    std::vector<std::string> url_list = {"asdf"};
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "asdf");
  }
  {
    std::string url_result;
    std::string port_name = "test";
    std::vector<std::string> url_list = {"asdf"};
    auto result = tester.getValidSubscribeURL(url_list, url_result, port_name);
    EXPECT_EQ(result, "asdf");
  }
}

TEST_F(NextInputPortFixture, sync_huge_2inport_from_same_data) {

  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport", port_type = "test_type";

  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, dummy_large_port_size, port_type, sync};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, dummy_large_port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2),
                           test_url_available_subgroup);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport1, test_inport2});
  tester.SetupSavedInputConfig();
  auto sub = tester.getSubscriber();
  ASSERT_TRUE(CheckConnectionShared(sub, {test_url_available_group}, 5000));

  size_t offset_change = 0x30;

  next::udex::TimestampedDataPacket package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, dummy_large_port_size);
  package = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, dummy_large_port_size);

  char value_check_first_package = 1;
  char value_check_second_package = 5;
  memset(port_dummy_data_, 2, sizeof(char) * dummy_large_port_size);
  port_dummy_data_[offset_change] = value_check_first_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, dummy_large_port_size, false, 111);

  memset(port_dummy_data_, 3, sizeof(char) * dummy_large_port_size);
  port_dummy_data_[offset_change] = value_check_second_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, dummy_large_port_size, false, 111);

  next::control::events::OrchestratorTrigger trigger{"1", "1"};
  tester.FillReferenceValueAndData(trigger);
  package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], value_check_second_package);
  EXPECT_EQ(package.size, dummy_large_port_size);
  auto subpackage = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)subpackage.data)[offset_change], value_check_second_package);
  EXPECT_EQ(subpackage.size, dummy_large_port_size);
}

// random fail
TEST_F(NextInputPortFixture, sync_manager_same_port_different_sync_algo) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport", port_type = "test_type";

  uint32_t port_version = 11;
  const size_t port_size = 1024;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  auto port_name_mod_2 = "2" + port_name_mod;
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync};
  ComponentInputPortConfig test_inport2{port_name_mod_2, port_version, port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  std::string test_url_available_subgroup = "SIM VFB.array_root.simple_float_arrays";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_2),
                           test_url_available_subgroup);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport1, test_inport2});
  tester.SetupSavedInputConfig();
  auto sub = tester.getSubscriber();
  ASSERT_TRUE(CheckConnectionShared(sub, {test_url_available_group}, 5000));

  std::unordered_map map_of_extractors = tester.GetExtractor();
  next::udex::SyncAlgoCfg sync_cfg1;
  sync_cfg1.algorythm_type = next::udex::getSyncAlgoTypeEnum("FIND_EXACT_HEADER_TIMESTAMP");
  auto it_1 = map_of_extractors.find(port_name_mod_1);
  ASSERT_TRUE(it_1 != map_of_extractors.end());
  ASSERT_TRUE(it_1->second);
  auto extractor1_ptr = it_1->second;
  extractor1_ptr->SetSyncAlgo(it_1->first, sync_cfg1);
  next::udex::SyncAlgoCfg sync_cfg2;
  sync_cfg2.algorythm_type = next::udex::getSyncAlgoTypeEnum("GET_LAST_ELEMENT");
  auto it_2 = map_of_extractors.find(port_name_mod_2);
  if (it_2 != map_of_extractors.end()) {
    it_2->second->SetSyncAlgo(it_2->first, sync_cfg2);
  }

  size_t offset_change = 0x30;

  next::udex::TimestampedDataPacket package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, port_size);

  package = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], 0);
  EXPECT_EQ(package.size, port_size);

  const uint64_t trigger_timestamp = 111;
  char value_check_first_package = 1;
  memset(port_dummy_data_, 2, sizeof(char) * port_size);
  port_dummy_data_[offset_change] = value_check_first_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, port_size, false, trigger_timestamp);

  next::control::events::OrchestratorTrigger trigger{"1", "1"};
  tester.FillReferenceValueAndData(trigger);

  next::udex::SyncValue value{next::udex::SignalType::SIGNAL_TYPE_UINT64, trigger_timestamp};
  std::vector<next::udex::SyncValue> sync_values;
  sync_values.push_back(value);
  next::udex::TimestampedDataPacket synced_package =
      extractor1_ptr->GetSyncExtractedData(test_url_available_group, sync_values);
  EXPECT_EQ(((char *)synced_package.data)[offset_change], value_check_first_package);
  EXPECT_EQ(synced_package.size, port_size);

  char value_check_second_package = 5;
  memset(port_dummy_data_, 3, sizeof(char) * port_size);
  port_dummy_data_[offset_change] = value_check_second_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, port_size, false, 222);

  next::control::events::OrchestratorTrigger trigger2{"1", "1"};
  tester.FillReferenceValueAndData(trigger2);

  next::udex::TimestampedDataPacket subpackage2 = tester.getSyncedPacket(test_inport2.port_name);
  EXPECT_EQ(((char *)subpackage2.data)[offset_change], value_check_second_package);
  EXPECT_EQ(subpackage2.size, port_size);
}

// random fail
TEST_F(NextInputPortFixture, sync_manager_same_timestamp_different_package) {

  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "testport", port_type = "test_type";

  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = "1" + port_name_mod;
  const size_t port_size = 1024;
  const uint64_t trigger_timestamp = 111;
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod_1),
                           test_url_available_group);

  auto tester = next::inport::NextInportTester();
  tester.SetupInput("", {test_inport1});
  tester.SetupSavedInputConfig();
  auto sub = tester.getSubscriber();
  ASSERT_TRUE(CheckConnectionShared(sub, {test_url_available_group}, 5000));

  size_t offset_change = 0x30;
  char value_check_first_package = 1;
  memset(port_dummy_data_, 2, sizeof(char) * port_size);
  port_dummy_data_[offset_change] = value_check_first_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, port_size, false, trigger_timestamp);

  next::control::events::OrchestratorTrigger trigger{"1", "1"};
  tester.FillReferenceValueAndData(trigger);

  next::udex::TimestampedDataPacket package = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)package.data)[offset_change], value_check_first_package);
  EXPECT_EQ(package.size, port_size);

  // second package same timestamp, weak_shadow_buffer should update
  char value_check_second_package = 5;
  memset(port_dummy_data_, 3, sizeof(char) * port_size);
  port_dummy_data_[offset_change] = value_check_second_package;
  pub.publishData("SIM VFB.array_root.simple_float_arrays", port_dummy_data_, port_size, false, trigger_timestamp);

  next::control::events::OrchestratorTrigger trigger2{"1", "1"};
  tester.FillReferenceValueAndData(trigger2);

  auto subpackage = tester.getSyncedPacket(test_inport1.port_name);
  EXPECT_EQ(((char *)subpackage.data)[offset_change], value_check_second_package);
  EXPECT_EQ(subpackage.size, port_size);
}

TEST_F(NextInputPortFixture, single_port_check_udex_size_limit) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  int buffer_max_size = 30;
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getUdexMaximumSize("test_component", port_name_mod), buffer_max_size);

  auto tester = next::inport::NextInportTester();

  tester.SetupInput("", {test_inport});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();
  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);
  EXPECT_EQ(map_entry->GetExtractionQueueMaxSize(), buffer_max_size);
}

TEST_F(NextInputPortFixture, single_port_check_udex_size_limit_mb) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  unsigned short buffer_max_size = 10;
  float temp_bufinB = (float)(port_size * buffer_max_size);
  float buffer_max_size_mb = temp_bufinB * powf(2, -20);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getUdexMaximumSizeMb("test_component", port_name_mod),
                           buffer_max_size_mb);

  auto tester = next::inport::NextInportTester();

  tester.SetupInput("", {test_inport});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();
  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);
  EXPECT_EQ(map_entry->GetExtractionQueueMaxSize(), buffer_max_size);
}

TEST_F(NextInputPortFixture, single_port_check_udex_size_limit_mb_not_round_value) {
  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 0x80;
  uint32_t port_version = 11;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  std::string test_url_available_group = "SIM VFB.array_root.simple_float_arrays";
  unsigned short buffer_max_size = 10;
  float buffer_max_size_mb = (float)(port_size * buffer_max_size + 1) * powf(2, -20);
  config::getConfig()->put(appsupport::configkey::getCompositionPortUrlKey("test_component", port_name_mod),
                           test_url_available_group);
  config::getConfig()->put(appsupport::configkey::getUdexMaximumSizeMb("test_component", port_name_mod),
                           buffer_max_size_mb);

  auto tester = next::inport::NextInportTester();

  tester.SetupInput("", {test_inport});
  tester.SetupSavedInputConfig();

  auto created_inports = tester.GetExtractor();
  auto search = created_inports.find(port_name_mod);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod];
  EXPECT_FALSE(map_entry == nullptr);
  EXPECT_EQ(map_entry->GetExtractionQueueMaxSize(), buffer_max_size);
}

TEST_F(NextInputPortFixture, single_port_with_config_format_V1_syncref_getPublish) {
  next::appsupport::ConfigClient::do_reset();

  auto pub = next::udex::publisher::DataPublisher("tester");
  pub.RegisterDataDescription("syncref.sdl", true);
  eCAL::Util::EnableLoopback(true);
  std::string port_name = "testport_syncref", port_type = "test_type";
  size_t port_size = sizeof(TestType);
  uint32_t port_version = 11;
  SyncMapping sync_mapping = {{"test_type.uiTimeStamp", "sSigHeader.uiTimeStamp"},
                              {"test_type.uiMeasurementCounter", "sSigHeader.uiMeasurementCounter"}};
  auto port_name_mod = generateUniqueName(port_name);
  auto port_name_mod_1 = port_name_mod;
  ComponentInputPortConfig test_inport1{port_name_mod_1, port_version, port_size, port_type, sync_mapping};

  auto tester = next::inport::NextInportTester();
  tester.WriteTestConfigV1(port_name_mod_1, "FIND_EXACT_VALUE", "sync_ref_timestamp");

  std::vector<std::string> testRet = next::appsupport::ConfigClient::getConfig()->getChildren("test_component:in_port");

  // loading player config from file for sync reference values
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("config_sync_ref.json");

  auto cfg = next::appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();

  tester.SetupInput("", {test_inport1});

  cfg = next::appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();

  tester.SetupSavedInputConfig();

  SyncManager manager = tester.getSyncManager();

  manager.isUpdated(port_name_mod_1);
  auto created_inports = tester.GetExtractor();

  auto search = created_inports.find(port_name_mod_1);
  ASSERT_TRUE(search != created_inports.end());
  auto map_entry = created_inports[port_name_mod_1];

  EXPECT_FALSE(map_entry == nullptr);

  auto sub = tester.getSubscriber();
  auto info = sub->getSubscriptionsInfo();
  std::vector<std::string> topics_to_check;

  for (auto sub_info : info) {
    topics_to_check.push_back(sub_info.second.topic);
  }

  ASSERT_TRUE(CheckConnectionShared(sub, topics_to_check, 5000)) << "Some topics are not connected.";

  // (publisher, timestamp, measurement counter, payload)
  PublishTestPayload(pub, 100000, 1, 1);
  PublishTestPayload(pub, 100002, 2, 2);
  PublishTestPayload(pub, 100003, 3, 3);

  uint64_t header_timestamp = 10002105;
  // (pub, timestamp, measurement  counter, test type timestamp, test type measurement counter, header_timestamp)
  PublishSyncref(pub, 100500, 5, 100000, 1, header_timestamp);

  uint64_t header_timestamp2 = 10002110;
  PublishSyncref(pub, 100505, 6, 100003, 3, header_timestamp2);

  // std::this_thread::sleep_for(std::chrono::seconds(10));

  next::control::events::OrchestratorTrigger trigger{std::to_string(header_timestamp), "flow1"};
  tester.FillReferenceValueAndData(trigger);
  auto package = tester.getSyncedPacket(test_inport1.port_name).data;
  auto result = *reinterpret_cast<TestType *>(package);
  EXPECT_TRUE(result.payload == 1);

  next::control::events::OrchestratorTrigger trigger2{std::to_string(header_timestamp2), "flow1"};
  tester.FillReferenceValueAndData(trigger2);
  package = tester.getSyncedPacket(test_inport1.port_name).data;
  result = *reinterpret_cast<TestType *>(package);
  EXPECT_TRUE(result.payload == 3);
}

} // namespace inport
} // namespace next