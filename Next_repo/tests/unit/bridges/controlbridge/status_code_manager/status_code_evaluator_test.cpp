
#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal.h>
#include <gtest/gtest.h>

#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>

#include "status_code_evaluator.hpp"

namespace next {
namespace controlbridge {

class CheckAnySimulationNodeHasWarningTest : public ::testing::Test {
public:
  CheckAnySimulationNodeHasWarningTest() {}

  StatusCodeQueue status_code_queue_;
  CheckAnySimulationNodeHasWarning warning_checker_;
};

TEST_F(CheckAnySimulationNodeHasWarningTest, no_action) {
  auto action = warning_checker_.GetActions();
  EXPECT_EQ(action.size(), 0);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, success_play_warning_false) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5211, "SIM_NODE_component", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, warning_play_warning_true) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5311, "SIM_NODE_component", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, error_play_warning_true) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5411, "SIM_NODE_component", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, mixed_case_play_warning_false) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, 2nodes_2ok_play_warning_false) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckAnySimulationNodeHasWarningTest, 2nodes_1ok_1warning_play_warning_true) {
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

class CheckExporterHasWarningTest : public ::testing::Test {
public:
  CheckExporterHasWarningTest() {}

  StatusCodeQueue status_code_queue_;
  CheckExporterHasWarning warning_checker_;
};

TEST_F(CheckExporterHasWarningTest, no_action) {
  auto action = warning_checker_.GetActions();
  EXPECT_EQ(action.size(), 0);
}

TEST_F(CheckExporterHasWarningTest, success_exporter_warning_false) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, warning_exporter_warning_true) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, error_exporter_warning_true) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{441, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, mixed_case_exporter_warning_false) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{441, "exporter", "test_message"});
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, mixed_case_exporter_warning_true) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{441, "exporter", "test_message"});
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, 2exporters_2ok_exporter_warning_false) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter1", "test_message"});
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter1", "test_message"});
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST_F(CheckExporterHasWarningTest, 2exporters_1ok_1warning_exporter_warning_true) {
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter1", "test_message"});
  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{341, "exporter2", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  auto actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);

  status_code_queue_.status_codes_.push_back(next::sdk::events::StatusCodeMessage{241, "exporter1", "test_message"});

  warning_checker_.CheckQueue(status_code_queue_);
  actions = warning_checker_.GetActions();
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);
}

} // namespace controlbridge
} // namespace next
