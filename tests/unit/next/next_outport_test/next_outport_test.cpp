#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/sdk.hpp>

#include "next_outport.hpp"

namespace next {
namespace inport {
using config = next::appsupport::ConfigClient;

class NextOutportTester : public NextOutport {
public:
  NextOutportTester() : NextOutport("test_component") {}
  std::shared_ptr<next::udex::publisher::DataPublisher> getPublisher() { return data_publisher_; }
};

class NextOutportPortFixture : public next::sdk::testing::TestUsingEcal {
public:
  NextOutportPortFixture() { this->instance_name_ = "NextOutportPortFixture"; }
  void SetUp() override {
    TestUsingEcal::SetUp();
    component = std::make_shared<NextOutportTester>();
    config::do_init("test_component");
    config::finish_init();
  }

  void TearDown() override {
    config::getConfig()->do_reset();
    TestUsingEcal::TearDown();
  }

  std::string generateUniqueName(const std::string &in) {
    return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::steady_clock::now().time_since_epoch())
                                   .count());
  }

  std::shared_ptr<NextOutportTester> component;
};

TEST_F(NextOutportPortFixture, outport_from_sdl_multiple_register_reset) {
  std::string component_name = "test_component";
  ComponentOutputPortConfig outport = {};
  component->SetupOutput({"test_signal.sdl"}, {outport});
  component->SetupSavedOutputConfig();
  auto publisher = component->getPublisher();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  // expected port comes from test_signal.sdl
  std::string expected_port = "SIM VFB.View1.Group1";
  ASSERT_EQ(publisher->getName(), component_name);
  ASSERT_NE(topicList.find(expected_port), topicList.end());
}

/**
 * @brief This test registers the same SDL 3 times, which results in a single topic/hash.
 */
TEST_F(NextOutportPortFixture, outport_from_sdl_valid) {
  std::string component_name = "test_component";
  ComponentOutputPortConfig outport = {};

  auto publisher = component->getPublisher();
  component->SetupOutput({"test_signal.sdl"}, {outport});
  component->SetupSavedOutputConfig();

  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList.size(), 2);

  component->SetupOutput({"test_signal2.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList1 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList1.size(), 3);

  component->SetupOutput({"test_signal3.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList2 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList2.size(), 4);

  component->Reset();

  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList3 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList3.size(), 0);

  component->SetupOutput({"test_signal.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList4 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList4.size(), 1);

  component->SetupOutput({"test_signal.sdl"}, {outport});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList5 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList5.size(), 2);
}

TEST_F(NextOutportPortFixture, multiple_SetupOutput_single_SetupSavedOutputConfig) {
  std::string component_name = "test_component";
  ComponentOutputPortConfig outport = {};

  auto publisher = component->getPublisher();
  component->SetupOutput({"test_signal.sdl"}, {outport});
  component->SetupOutput({"test_signal2.sdl"}, {outport});
  component->SetupSavedOutputConfig();

  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList.size(), 3);

  component->SetupOutput({"test_signal3.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList2 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList2.size(), 4);

  component->Reset();

  component->SetupOutput({"test_signal.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList3 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList3.size(), 1);
}

TEST_F(NextOutportPortFixture, outport_from_function_valid) {
  std::string component_name = "test_component", port_name = "test_port";
  size_t port_size = 15;
  uint32_t port_version = 13;
  auto port_name_mod = generateUniqueName(port_name);
  ComponentOutputPortConfig outport = {port_name_mod, port_version, port_size};
  component->SetupOutput({"test_signal.sdl"}, {outport});
  component->SetupSavedOutputConfig();
  auto publisher = component->getPublisher();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  ASSERT_EQ(publisher->getName(), component_name);
  ASSERT_NE(topicList.find("SIM VFB." + port_name_mod), topicList.end());
}

TEST_F(NextOutportPortFixture, outport_not_crashing_for_empty_string) {
  std::string component_name = "test_component", port_name = "";
  size_t port_size = 0;
  uint32_t port_version = 0;
  ComponentOutputPortConfig outport = {port_name, port_version, port_size};

  component->SetupOutput({""}, {outport});
  component->SetupSavedOutputConfig();
  auto publisher = component->getPublisher();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS
  ASSERT_EQ(publisher->getName(), component_name);
  ASSERT_EQ(topicList.size(), 1);
}

TEST_F(NextOutportPortFixture, outport_not_crashing_for_empty_sdl_vector) {
  std::string component_name = "test_component", port_name = "";
  size_t port_size = 0;
  uint32_t port_version = 0;
  ComponentOutputPortConfig outport = {port_name, port_version, port_size};

  component->SetupOutput({}, {outport});
  component->SetupOutput({"not there.sdl"}, {outport});
  component->SetupOutput({"not there.sdl"}, {});
  component->SetupSavedOutputConfig();
  auto publisher = component->getPublisher();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS
  ASSERT_EQ(publisher->getName(), component_name);
  ASSERT_EQ(topicList.size(), 1);
}

TEST_F(NextOutportPortFixture, outport_working_with_multiple_sdl) {
  std::string component_name = "test_component";
  ComponentOutputPortConfig outport = {};

  auto publisher = component->getPublisher();
  component->SetupOutput({"test_signal.sdl", "test_signal2.sdl"}, {outport});
  component->SetupSavedOutputConfig();

  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList.size(), 3);

  component->SetupOutput({"test_signal3.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList2 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList2.size(), 4);

  component->Reset();

  component->SetupOutput({"test_signal.sdl"}, {});
  component->SetupSavedOutputConfig();
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topicList3 = publisher->getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS_WINDOWS
  EXPECT_EQ(topicList3.size(), 1);
}
} // namespace inport
} // namespace next
