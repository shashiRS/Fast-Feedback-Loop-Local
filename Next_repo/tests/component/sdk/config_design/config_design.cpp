/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_design.cpp
 * @brief    small application for testing and demonstrating the config handling
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include "eml_config_design_test.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/sdk/sdk.hpp>

using next::appsupport::ConfigClient;
class ConfigDesignTests : public next::sdk::testing::TestUsingEcal {
public:
  ConfigDesignTests() { this->instance_name_ = "ConfigDesignTests"; }
};

TEST_F(ConfigDesignTests, basic_functionality) {
  testing::internal::CaptureStdout();
  bool t = ::next::appsupport::MClient<::next::appsupport::EmlConfig>::doInit("test");

  std::string tstTxt =
      ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().get_string("test.version", "");
  EXPECT_EQ(tstTxt, "0.0.0");

  // SETUP
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().addPort("my_special_port", "0x5000",
                                                                                    "0.0.1", 30, "debug_port_1", "");
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().addPort(
      "m_pSefInputIf", "0x9000", "0.0.1", 1234, "m_pSefInputIf", "coh_gia.dll");
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().addPort(
      "m_pEmlInputIf", "0x9100", "0.0.1", 1234, "m_pEmlInputIf", "coh_gia.dll");
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().addSyncToPort("m_pSefInputIf", 35, 1,
                                                                                          "timestamp");
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().addSyncToPort("m_pEmlInputIf", 35, 2,
                                                                                          "timestamp");

  auto res = ::next::appsupport::MClient<::next::appsupport::EmlConfig>::GetInstance().getChildren("test.in_port");

  ASSERT_TRUE(t);
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "") << "Config functions should not create console output";
  ::next::appsupport::MClient<::next::appsupport::EmlConfig>::doReset();
}
