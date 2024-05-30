/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_status_code_test.cpp
 *  @brief    Testing the NextStatusCode class
 */

#include <gtest/gtest.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;
using namespace std::chrono_literals;

class NextStatusCodeTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  NextStatusCodeTestFixture() { this->instance_name_ = "NextStatusCodeTestFixture"; }
};

TEST_F(NextStatusCodeTestFixture, next_status_code_pub_component_next_player) {
  eCAL::Util::EnableLoopback(true);

  std::string component_name = "next_player";
  next::appsupport::ConfigClient::do_init(component_name);

  next::appsupport::StatusCode next_status_code = next::appsupport::StatusCode::ERR_COMP_INIT_FAILED;
  std::optional<next::sdk::events::StatusCodeMessage> ret_status =
      next::appsupport::NextStatusCode::PublishStatusCode(next_status_code, "init has failed");

  next::appsupport::ConfigClient::finish_init();
  next::appsupport::ConfigClient::do_reset();

  EXPECT_EQ(ret_status->component_name, component_name);
  EXPECT_EQ(ret_status->origin, next::sdk::events::StatusOrigin::NEXT_PLAYER);
  EXPECT_EQ(ret_status->toInt(), 1000 * (int)next::sdk::events::StatusOrigin::NEXT_PLAYER + int(next_status_code));
  EXPECT_EQ(ret_status->message, "init has failed");
}

TEST_F(NextStatusCodeTestFixture, next_status_code_pub_component_No_Config_Client_expected_Default) {
  eCAL::Util::EnableLoopback(true);

  std::string component_name = "default";

  next::appsupport::StatusCode next_status_code = next::appsupport::StatusCode::ERR_COMP_INIT_FAILED;
  std::optional<next::sdk::events::StatusCodeMessage> ret_status =
      next::appsupport::NextStatusCode::PublishStatusCode(next_status_code, "init has failed");

  EXPECT_EQ(ret_status->component_name, component_name);
  EXPECT_EQ(ret_status->origin, next::sdk::events::StatusOrigin::DEFAULT);
  EXPECT_EQ(ret_status->toInt(), 1000 * (int)next::sdk::events::StatusOrigin::DEFAULT + int(next_status_code));
  EXPECT_EQ(ret_status->message, "init has failed");
}

TEST_F(NextStatusCodeTestFixture, next_status_code_pub_component_sim_node) {
  eCAL::Util::EnableLoopback(true);

  std::string component_name = "test_node";
  next::appsupport::ConfigClient::do_init("sim_node");

  next::appsupport::StatusCode next_status_code = next::appsupport::StatusCode::ERR_COMP_INIT_FAILED;
  std::optional<next::sdk::events::StatusCodeMessage> ret_status =
      next::appsupport::NextStatusCode::PublishStatusCode(component_name, next_status_code, "init has failed");

  next::appsupport::ConfigClient::finish_init();
  next::appsupport::ConfigClient::do_reset();

  EXPECT_EQ(ret_status->component_name, component_name);
  EXPECT_EQ(ret_status->origin, next::sdk::events::StatusOrigin::SIM_NODE);
  EXPECT_EQ(ret_status->toInt(), 1000 * (int)next::sdk::events::StatusOrigin::SIM_NODE + int(next_status_code));
  EXPECT_EQ(ret_status->message, "init has failed");
}

TEST_F(NextStatusCodeTestFixture, next_status_code_pub_component_no_config_client_Default_Sim_Node) {

  std::string component_name = "test_node";

  eCAL::Util::EnableLoopback(true);

  next::appsupport::StatusCode next_status_code = next::appsupport::StatusCode::ERR_COMP_INIT_FAILED;
  std::optional<next::sdk::events::StatusCodeMessage> ret_status =
      next::appsupport::NextStatusCode::PublishStatusCode(component_name, next_status_code, "init has failed");

  EXPECT_EQ(ret_status->component_name, component_name);
  EXPECT_EQ(ret_status->origin, next::sdk::events::StatusOrigin::SIM_NODE);
  EXPECT_EQ(ret_status->toInt(), 1000 * (int)next::sdk::events::StatusOrigin::SIM_NODE + int(next_status_code));
  EXPECT_EQ(ret_status->message, "init has failed");
}
