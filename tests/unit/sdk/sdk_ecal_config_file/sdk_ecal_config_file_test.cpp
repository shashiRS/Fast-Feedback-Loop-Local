/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     sdk_ecal_config_file_test.cpp
 * @brief    testing the eCAL configuration file usage
 */

#include <cip_test_support/gtest_reporting.h>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>

class EcalConfigFileTest : public next::sdk::testing::TestUsingEcal {
public:
  EcalConfigFileTest() { this->instance_name_ = "EcalConfigFileTest"; }
};

TEST_F(EcalConfigFileTest, check_ecal_configuration_loading) {
  EXPECT_TRUE(eCAL::Config::Experimental::IsShmMonitoringEnabled()) << " the provided ecal.ini is not being used";
  EXPECT_EQ(eCAL::Config::GetRegistrationTimeoutMs(), 70123) << " the provided ecal.ini is not being used";
  EXPECT_EQ(eCAL::Config::GetMonitoringTimeoutMs(), 9876) << " the provided ecal.ini is not being used";
}
