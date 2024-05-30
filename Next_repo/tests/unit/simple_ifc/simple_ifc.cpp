
#include <cip_test_support/gtest_reporting.h>
#include <cstddef> //std::byte

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>
#include <next/sdk/testing/test_using_ecal.hpp>

#include "next/next_callbacks.hpp"
#include "next_impl.hpp"

namespace next {
class NextComponentInterfaceTester : public next::NextCallbacks, public next::NextComponentInterfaceImpl {
public:
  NextComponentInterfaceTester(const std::string component_name = "Test")
      : NextCallbacks(component_name), NextComponentInterfaceImpl(component_name) {
    component_name_ = component_name;
  };
  ~NextComponentInterfaceTester() { debug(__FILE__, "Resetting init chain for {}", component_name_); }

  void Reset() { this->userEnterReset(); }

  void WriteTestConfig([[maybe_unused]] const std::string &port_name = "Test",
                       [[maybe_unused]] const std::string &sync_algo = "GET_LAST_ELEMENT",
                       [[maybe_unused]] const std::string &sync_mode = "Timestamp") {
    next::appsupport::ConfigClient::getConfig()->putCfg("node_conf.json");
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncModeKey(component_name_, port_name), sync_mode);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncAlgoKey(component_name_, port_name), sync_algo);
  }

  void SetupSinglePort(ComponentInputPortConfig inport, const std::string &adapter_dll = "") {
    std::vector<ComponentInputPortConfig> inport_vector;
    inport_vector.push_back(inport);
    this->SetupInput(adapter_dll, inport_vector);
    this->userEnterConfigInput();
  }

  void SetupSingleOutport(ComponentOutputPortConfig outport, const std::string &sdl_path = "") {
    std::vector<ComponentOutputPortConfig> outport_vector;
    std::vector<std::string> sdl_vector;
    outport_vector.push_back(outport);
    sdl_vector.push_back(sdl_path);
    this->SetupOutput(sdl_vector, outport_vector);
  }

  void triggerAsyncPush(std::string output_port_name, DataPackage data) {
    return NextComponentInterfaceImpl::triggerAsyncPush(output_port_name, data);
  }

  bool isDataAvailable(const std::string &input_port_name) {
    return NextComponentInterfaceImpl::isDataAvailable(input_port_name);
  }

  // Test Class Variables
  std::string component_name_;
  next::sdk::InitChain::ConfigMap initmap_;
};

class SimpleIfcTests : public ::testing::Test {
public:
  SimpleIfcTests() : ::testing::Test() {}
  void SetUp() override { component = std::make_shared<NextComponentInterfaceTester>("test_component"); }
  void TearDown() override {
    next::appsupport::ConfigClient::do_reset();
    component.reset();
  }

  std::string generateUniqueName(const std::string &in) {
    return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::steady_clock::now().time_since_epoch())
                                   .count());
  }
  std::shared_ptr<NextComponentInterfaceTester> component;
};

TEST_F(SimpleIfcTests, define) { EXPECT_TRUE(component.use_count() > 0); }

TEST_F(SimpleIfcTests, convert_string_to_uint32) {
  auto vers_3bytes = component->ConvertVersionNumber("0.0.2");
  EXPECT_EQ(vers_3bytes, (uint32_t)2);
  auto vers_4bytes = component->ConvertVersionNumber("0.0.3.0");
  EXPECT_EQ(vers_4bytes, (uint32_t)3);
  auto full_vers_4bytes = component->ConvertVersionNumber("1.10.300.0");
  EXPECT_EQ(full_vers_4bytes, (uint32_t)68140);
}

TEST_F(SimpleIfcTests, check_single_inport_data_connections) {
  std::string component_name = "test_component";
  ComponentOutputPortConfig outport = {};
  component->SetupInput("", {{"myPortName", 1, 1, "", {}}});
  component->userEnterConfigInput();
  auto inports = component->getInputPorts();
  ASSERT_EQ(inports.size(), 1);
  EXPECT_EQ(inports[0].port_name, "myPortName");

  auto data = component->getTriggerAsyncPush("myPortName");
  EXPECT_TRUE(std::get<0>(data) != nullptr);
  EXPECT_EQ(std::get<1>(data), 1);
  std::cout << "end of test is reached" << std::endl;
}

TEST_F(SimpleIfcTests, isDataAvailable_no_data_available) {
  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 15;
  uint32_t port_version = 16;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  component->WriteTestConfig(port_name_mod);
  component->SetupSinglePort(test_inport, "invalid_dll");

  ASSERT_FALSE(component->isDataAvailable(port_name));
}

TEST_F(SimpleIfcTests, isDataAvailable_wrong_input_port) {
  std::string port_name = "test_port", port_type = "test_type";
  size_t port_size = 15;
  uint32_t port_version = 17;
  SyncMapping sync = {};
  auto port_name_mod = generateUniqueName(port_name);
  ComponentInputPortConfig test_inport{port_name_mod, port_version, port_size, port_type, sync};
  component->WriteTestConfig(port_name_mod);
  component->SetupSinglePort(test_inport, "invalid_dll");

  ASSERT_FALSE(component->isDataAvailable("unavailable_port_name"));
}

TEST_F(SimpleIfcTests, publish_data_on_outport) {
  std::string port_name = "test_port";
  size_t port_size = 12;
  uint32_t port_version = 18;
  auto port_name_mod = generateUniqueName(port_name);
  ComponentOutputPortConfig test_outport{port_name_mod, port_version, port_size};
  component->WriteTestConfig(port_name_mod);
  component->SetupSingleOutport(test_outport);
  std::string msg = "12345";
  void *msg_p = &msg;
  size_t msg_size = msg.size();
  DataPackage data;
  data = std::make_tuple(msg_p, msg_size);
  component->triggerAsyncPush(test_outport.port_name, data);
}

TEST_F(SimpleIfcTests, trigger_from_component) {
  eCAL::Util::EnableLoopback(true);
  Trigger trigger;
  trigger.methode_name = "test_method";
  trigger.node_name = "test_node";

  auto cb = [](const std::string & /*method_*/, const std::string & /*req_type_*/, const std::string & /*resp_type_*/,
               const std::string & /*request_*/, std::string &response) {
    response = "{\"return_type\":\"EXEC_FAILED\",\"callback_return\":{\"callback_message\":\"test_message\",\"error_"
               "code\":-2}}";
    return 0;
  };

  std::shared_ptr<eCAL::CServiceServer> server = std::make_shared<eCAL::CServiceServer>();
  bool server_created = server->Create("next::control::events::OrchestratorTrigger_test_nodetest_method");
  server->AddMethodCallback(trigger.node_name + trigger.methode_name, "EventRequestType", "EventResponseType", cb);
  EXPECT_EQ(server_created, true);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::list<TriggerReturn> responses = component->invokeTrigger(trigger.node_name, trigger.methode_name);
  EXPECT_EQ(responses.size(), (size_t)1);
  for (auto resp : responses) {
    EXPECT_EQ(resp.error, ErrorCode::EXEC_FAILED);
    EXPECT_EQ(resp.error_message, "test_message");
    EXPECT_EQ(resp.SigState, -2);
  }
}

} // namespace next
