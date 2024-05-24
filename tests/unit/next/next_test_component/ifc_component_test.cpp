#include <cip_test_support/gtest_reporting.h>

#include <iostream>

#include <ecal/ecal.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/next.hpp>

#include <next/udex/publisher/data_publisher.hpp>

namespace cem {
typedef unsigned char uint8;       /* [PLATFORM013] */
typedef unsigned short uint16;     /* [PLATFORM014] */
typedef unsigned long long uint64; /* 0 .. 18446744073709551615   */

typedef unsigned long uint32; /* [PLATFORM015] */
typedef uint16 AlgoCycleCounter_t;
typedef uint32 AlgoInterfaceVersionNumber_t;
typedef uint64 AlgoDataTimeStamp_t;

struct SignalHeader_t {

  /// Time stamp of the signal (for SIP: T0 timestamp of input signal
  ///, otherwise: T7 timestamp to which signal has been adjusted)
  AlgoDataTimeStamp_t uiTimeStamp;

  /// Deprecated, not used in CEM. Should be set to 0.
  AlgoCycleCounter_t uiMeasurementCounter;

  /// Rolling counter of source component. Will be incremented in every
  /// call to the exec method of the component.
  AlgoCycleCounter_t uiCycleCounter;
};

struct EMF_GlobalTimestamp_t {

  /// Interface version number
  ///(increased when changed)
  AlgoInterfaceVersionNumber_t u_VersionNumber;

  /// Contains time stamp, frame stamp, etc.
  SignalHeader_t SigHeader;
};
} // namespace cem

namespace next {

struct SignalHeader_t {
  uint64_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
};

inline std::string GetSyncPortName() { return "array_root.simple_float_arrays"; }

class NextComponentInterfaceTester : public next::NextComponentInterface {
public:
  enum class TestCase { FalseInputNoCrash = 0, InputSync = 1 };

public:
  NextComponentInterfaceTester(const std::string component_name = "EmfNodeTriggering",
                               TestCase test_case = TestCase::FalseInputNoCrash)
      : NextComponentInterface(component_name), test_case_(test_case), component_name_(component_name){};
  int trigger_counter = 0;
  TestCase test_case_;
  std::string component_name_;

  std::string sync_port_name = GetSyncPortName();
  size_t sync_port_size = 80;
  char outport[80];

  void OnTriggerCallbackTester(next::control::orchestrator::OrchestratorErrors &err, std::string &err_msg,
                               const std::string &method_name, const triggerCallback &client_callback) {
    OnTriggerCallback(err, err_msg, method_name, client_callback);
  }

  bool ConfigFalseInputNoCrash() {

    std::vector<ComponentInputPortConfig> input_cfg;
    input_cfg.push_back({"asdf", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf1", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf2", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf3", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf4", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf5", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf6", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf7", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf8", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf9", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf0", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf11", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf22", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf66", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf33", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf44", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"asdf55", 1, 1, "tata", {{"dsf", "asdf"}, {"adsf", "adsf"}}});
    input_cfg.push_back({"", 1, 1, "", {{"", ""}, {"", ""}}});
    this->SetupInput("", input_cfg);

    // call with empty input
    this->SetupInput("", {{}});

    // call with empty input port name
    this->SetupInput("", {{"", 0, 0, "", {{"", ""}, {"", ""}}}});

    return true;
  }

  bool ConfigInputSync() {

    std::vector<ComponentInputPortConfig> input_cfg;
    ComponentInputPortConfig sync_timestamp;
    sync_timestamp.port_name = sync_port_name;
    sync_timestamp.port_size = 80;
    sync_timestamp.data_type_name = "simple_float_array";
    sync_timestamp.port_version_number = 1;
    sync_timestamp.sync_mapping = {};

    input_cfg.push_back(sync_timestamp);
    this->SetupInput("", input_cfg);

    memset(outport, 0, sizeof(char) * sync_port_size);
    // call with empty output
    this->SetupOutput({{}}, {{"", 0, 0}});
    return true;
  }

  bool enterConfig() override {
    auto json_str = next::appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();
    // Values for ComponentInputPortConfig

    uint64_t start_time = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::system_clock::now().time_since_epoch())
                              .count();

    std::string trigger_name = "runSequencer_response", error_message;
    std::vector<std::pair<std::string, triggerCallback>> trigger_v;
    auto cb = std::bind(&NextComponentInterfaceTester::ComponentExecCall, this, error_message);
    trigger_v.push_back(std::make_pair(trigger_name, cb));
    this->SetupTriggers(trigger_v);

    switch (test_case_) {
    case TestCase::InputSync:
      ConfigInputSync();
      break;
    case TestCase::FalseInputNoCrash:
    default:
      ConfigFalseInputNoCrash();
      break;
    }

    // call with empty output
    this->SetupOutput({{}}, {{"", 0, 0}});

    uint64_t end_time = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
    EXPECT_LE(end_time - start_time, 10000);
    reached_Config_ = true;
    return true;
  }

  int ComponentExecCall([[maybe_unused]] std::string &error_message) {
    auto result = this->getTriggerAsyncPush(sync_port_name);
    size_t copy_size = std::get<1>(result) > sync_port_size ? sync_port_size : std::get<1>(result);
    memcpy(outport, std::get<0>(result), copy_size);
    return 0;
  }

  bool enterInit() override {
    reached_Init_ = true;
    return true;
  }

  bool enterShutdown() override {
    reached_Shutdown_ = true;
    return true;
  }

  bool enterReset() override {
    auto json_str = next::appsupport::ConfigClient::getConfig()->getAllConfigurationsAsJson();
    std::cout << "Enter Reset reached" << std::endl;
    reached_Reset_ = true;
    return true;
  }

  bool onError(std::string error, std::exception *e = nullptr) override {
    reached_onError_ = true;
    std::cout << error << std::endl;
    if (e != nullptr)
      return false;

    return true;
  }

  bool onConfigChanged([[maybe_unused]] std::string changed_config_key) override {
    // Please notify me if changed_config_key gets an update
    return true;
  }

  bool reached_Config_ = false;
  bool reached_Init_ = false;
  bool reached_Shutdown_ = false;
  bool reached_Reset_ = false;
  bool reached_onError_ = false;

  uint64_t counter_ = 0;
};

class NextTestComponentFixture : public ::testing::Test {
public:
  void TearDown() override { appsupport::ConfigClient::do_reset(); }
};

TEST_F(NextTestComponentFixture, let_test_component_run_falty_input) {
  std::string component_name = "EmfNodeTriggering";
  auto component = std::make_shared<NextComponentInterfaceTester>(component_name);
  std::atomic<bool> shutdown = false;
  std::thread mythread;
  try {
    mythread = std::thread([component, &shutdown] { component->run(); });
    while (component->getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    component->shutdown();

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    EXPECT_TRUE(false) << "exception caught";
  }

  std::cout << "Shutdown successfull start thread join" << std::endl;

  EXPECT_NO_THROW(mythread.join()) << "shutdown failed";
}

TEST_F(NextTestComponentFixture, test_inport_sync) {
  std::string component_name = "EmfNodeTriggering";

  appsupport::ConfigClient::do_init(component_name);
  appsupport::ConfigClient::finish_init();
  auto client = appsupport::ConfigClient::getConfig();
  client->put(appsupport::configkey::getCompositionPortUrlKey(component_name, GetSyncPortName()) + "[0]",
              "SIM VFB." + GetSyncPortName());

  auto component =
      std::make_shared<NextComponentInterfaceTester>(component_name, NextComponentInterfaceTester::TestCase::InputSync);
  eCAL::Util::EnableLoopback(true);

  next::udex::publisher::DataPublisher pub("tester");
  pub.RegisterDataDescription("arrays.sdl", true);

  std::atomic<bool> shutdown = false;
  std::thread mythread;
  try {
    mythread = std::thread([component, &shutdown] { component->run(); });

    while (component->getCurrentState() != next::appsupport::lifecycleStatemachine::EState::READY_STATE) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    EXPECT_TRUE(false) << "exception caught";
  }

  // wait for ecal to get connected
  std::this_thread::sleep_for(std::chrono::seconds(2));

  char memory[80];
  memset(memory, 1, 80);
  pub.publishData("SIM VFB." + GetSyncPortName(), memory, 80);
  std::string err;
  next::control::orchestrator::OrchestratorErrors orchestratorError;
  component->OnTriggerCallbackTester(orchestratorError, err, "runSequencer_response",
                                     [&](std::string &) { return component->ComponentExecCall(err); });
  EXPECT_EQ(component->outport[0], 1);
  component->shutdown();
  std::cout << "Shutdown successfull start thread join" << std::endl;

  EXPECT_NO_THROW(mythread.join()) << "shutdown failed";
}

} // namespace next
