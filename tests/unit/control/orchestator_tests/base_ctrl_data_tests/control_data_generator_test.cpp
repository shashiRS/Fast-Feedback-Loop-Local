#include "base_ctrl_data_generator/control_data_generator.hpp"
#include <gtest/gtest.h>
#include <list>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/sdk.hpp>

namespace next {
namespace control {
namespace orchestrator {

class ControlDataGeneratorTest : public next::sdk::testing::TestUsingEcal {
public:
  ControlDataGeneratorTest() { this->instance_name_ = "ControlDataGeneratorTest"; };
  void SetUp() override {
    TestUsingEcal::SetUp();
    publisher_ = std::make_shared<next::udex::publisher::DataPublisher>("ControlDataGeneratorTest");
    publisher_->RegisterDataDescription("ARS5xx.sdl", true);
    extractor_ = std::make_shared<next::udex::extractor::DataExtractor>();
    extractor_->SetBlockingExtraction(true);
    subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>("ControlDataGeneratorTest");
    subscriber_->Subscribe(GetSubscriptionGroupUrl(component1_name_test), extractor_);
    subscriber_->Subscribe(GetSubscriptionGroupUrl(component2_name_test), extractor_);
  };

  std::string GetSubscriptionGroupUrl(const std::string component_name) {
    return "Orchestrator." + flow_name_test + "_" + component_name + ".SigHeader";
  }
  static const size_t size_veh_dyn = 160;
  char VehDynDummy[size_veh_dyn];
  void publishValues() {
    EXPECT_TRUE(publisher_->publishData("SIM VFB.AlgoVehCycle.VehDyn", VehDynDummy, size_veh_dyn, false, 1))
        << "publishing failed";
  }

  template <typename value_type>
  void SetValueToMemory(const value_type &value, const size_t &offset) {
    memcpy(&(VehDynDummy[offset]), &value, sizeof(value_type));
  }

  void TearDown() override {
    publisher_.reset();
    extractor_.reset();
    subscriber_.reset();
    TestUsingEcal::TearDown();
  }
  std::shared_ptr<next::udex::publisher::DataPublisher> publisher_;
  std::shared_ptr<next::udex::extractor::DataExtractor> extractor_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> subscriber_;

  std::string component1_name_test = "some_component1";
  std::string component2_name_test = "some_component2";
  std::string execution_name_test = "runSequencer_response";
  std::string flow_name_test = "flow1";

  ~ControlDataGeneratorTest() { next::appsupport::ConfigClient::do_reset(); };
};

TEST_F(ControlDataGeneratorTest, testGeneralMemberConfig_fixedValues) {
  // add in the configuration some members and see if they are read
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::getConfig()->putCfg("test_control_data_config.json");
  next::appsupport::ConfigClient::finish_init();
  ControlDataGenerator generator(flow_name_test);
  generator.initControlDataGenerator();

  bool connected = false;
  for (auto i = 0; i < 10; i++) {
    connected = generator.isConnected();
    if (connected) {
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
  }

  ASSERT_TRUE(connected) << "Subscribers not connected!";

  CheckConnectionShared(subscriber_,
                        {GetSubscriptionGroupUrl(component1_name_test), GetSubscriptionGroupUrl(component2_name_test)},
                        5000);

  SetValueToMemory<uint32_t>(123, 0);
  SetValueToMemory<uint32_t>(42, 4);
  publishValues();

  ASSERT_TRUE(generator.generateAndSendControlData(1, component1_name_test, execution_name_test));
  next::udex::dataType return_value;

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  uint64_t test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 1u);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiTimeStamp",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 42u);

  // Try getting connect data

  // no update on connction see if vlaues stay the same for connected values and change fixed
  generator.generateAndSendControlData(2, component1_name_test, execution_name_test);

  // fixed values will change from 1->2 because it's an array of elements
  // everytime control data is sent, the next element from the array will be picked
  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 2u);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiTimeStamp",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 42u);

  // Publsih again
  SetValueToMemory<uint32_t>(219, 4);
  publishValues();

  generator.generateAndSendControlData(3, component1_name_test, execution_name_test);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 3u);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiTimeStamp",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 219u);

  generator.generateAndSendControlData(4, component1_name_test, execution_name_test);

  // when last array value is reached, it should be repeated
  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 3u);

  generator.generateAndSendControlData(4, component1_name_test, execution_name_test);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 3u);

  // set value to component2, values for component1 should remain unchanged
  SetValueToMemory<uint32_t>(9, 10);
  publishValues();

  generator.generateAndSendControlData(5, component1_name_test, execution_name_test);
  generator.generateAndSendControlData(5, component2_name_test, execution_name_test);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 3u);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component1_name_test) + ".uiTimeStamp",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 219u);

  ASSERT_TRUE(extractor_->GetValue(GetSubscriptionGroupUrl(component2_name_test) + ".uiCycleCounter",
                                   next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
  test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 9u);
}

} // namespace orchestrator
} // namespace control
} // namespace next
