/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     trigger_handler_test.cpp
 *  @brief    Testing the TriggerStatusHandler for player
 */
#include "trigger_handler.hpp"
#include <next/control/orchestrator/flow_control_config.h>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

class TriggerStatusHandlerFixture : public testing::Test {
public:
  using TriggerStatus = next::control::orchestrator::TriggerStatus;
  void SetUp() {}

  void TearDown() {}
};

TEST_F(TriggerStatusHandlerFixture, testPublishOccurence) {
  TriggerStatusHandler trigger_handler;
  trigger_handler.current_status_map_ = {{"client1", TriggerStatus::SUCCESS},
                                         {"client2", TriggerStatus::FAILURE},
                                         {"client3", TriggerStatus::NO_RESPONSE},
                                         {"client4", TriggerStatus::MISSING_DATA},
                                         {"client5", TriggerStatus::UNKNOWN}};

  unsigned int counter_success{0}, counter_failed{0}, counter_empty{0};

  unsigned int iterations = 3;
  do {
    iterations--;

    for (const auto &[client, status] : trigger_handler.current_status_map_) {
      const auto code = trigger_handler.GetStatus(client, status);
      if (code == TriggerStatus::SUCCESS) {
        counter_success++;
      } else if (code == TriggerStatus::FAILURE) {
        counter_failed++;
      } else if (code == TriggerStatus::NO_RESPONSE) {
        counter_empty++;
      }
    }
  } while (iterations != 0);

  EXPECT_EQ(counter_success, 1u) << "Number of SUCCESS events is different than expected.";
  EXPECT_EQ(counter_failed, 3u) << "Number of FAILED events different than expected.";
  EXPECT_EQ(counter_empty, 1u) << "Number of NO_RESPONSE events different than expected.";
}

TEST_F(TriggerStatusHandlerFixture, testPublishAndClear) {
  TriggerStatusHandler trigger_handler;
  trigger_handler.current_status_map_ = {{"client1", TriggerStatus::SUCCESS},
                                         {"client2", TriggerStatus::FAILURE},
                                         {"client3", TriggerStatus::NO_RESPONSE}};

  trigger_handler.PublishStatusCurrentPackage();

  EXPECT_TRUE(trigger_handler.current_status_map_.empty())
      << "Map for current package was not cleared after publishing";
}

TEST_F(TriggerStatusHandlerFixture, testResetFunction) {
  TriggerStatusHandler trigger_handler;
  trigger_handler.current_status_map_ = {{"client1", TriggerStatus::SUCCESS},
                                         {"client2", TriggerStatus::FAILURE},
                                         {"client3", TriggerStatus::NO_RESPONSE}};

  trigger_handler.Reset();

  EXPECT_TRUE(trigger_handler.current_status_map_.empty()) << "Reset not working";
}

TEST_F(TriggerStatusHandlerFixture, testRepeatedChecks) {
  TriggerStatusHandler trigger_handler;

  std::vector<std::map<std::string, TriggerStatus>> packages;
  packages.push_back({{"client1", TriggerStatus::SUCCESS}, {"client2", TriggerStatus::FAILURE}});
  packages.push_back({{"client1", TriggerStatus::SUCCESS}, {"client2", TriggerStatus::FAILURE}});
  packages.push_back({{"client1", TriggerStatus::NO_RESPONSE}, {"client2", TriggerStatus::SUCCESS}});
  packages.push_back({{"client1", TriggerStatus::FAILURE}, {"client2", TriggerStatus::NO_RESPONSE}});
  packages.push_back({{"client1", TriggerStatus::FAILURE}, {"client2", TriggerStatus::NO_RESPONSE}});

  unsigned int counter_success_client1{0}, counter_no_resp_client1{0}, counter_failed_client1{0};
  unsigned int counter_success_client2{0}, counter_no_resp_client2{0}, counter_failed_client2{0};

  // consume packages
  for (const auto &package : packages) {
    for (const auto &[client, status] : package) {
      const auto code = trigger_handler.GetStatus(client, status);
      if (client == "client1") {
        if (code == TriggerStatus::SUCCESS) {
          counter_success_client1++;
        } else if (code == TriggerStatus::FAILURE) {
          counter_failed_client1++;
        } else if (code == TriggerStatus::NO_RESPONSE) {
          counter_no_resp_client1++;
        }
      } else if (client == "client2") {
        if (code == TriggerStatus::SUCCESS) {
          counter_success_client2++;
        } else if (code == TriggerStatus::FAILURE) {
          counter_failed_client2++;
        } else if (code == TriggerStatus::NO_RESPONSE) {
          counter_no_resp_client2++;
        }
      }
    }
  }

  EXPECT_EQ(counter_success_client1, 1u);
  EXPECT_EQ(counter_failed_client1, 2u);
  EXPECT_EQ(counter_no_resp_client1, 1u);
  EXPECT_EQ(counter_success_client2, 1u);
  EXPECT_EQ(counter_failed_client2, 2u);
  EXPECT_EQ(counter_no_resp_client2, 1u);
}