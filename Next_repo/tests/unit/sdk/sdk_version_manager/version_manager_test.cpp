/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     version_manager_test.cpp
 *  @brief    testing the version manager
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/version_manager/version_manager.hpp>

#include <next/sdk/sdk.hpp>

#define VERSION1 L"3"
#define VERSION2 L"0.1.2"
#define VERSION3 L"v10.3.5.6.7.8"
#define VERSION4 L"v1.21.300-rc5-test3 - asdf123458chdk"
#define VERSION5 L"test prealpha 200.3465.95-patch3-do-not-use for testing only!"
#define VERSION6 L"beta-3.4"

TEST(VersionManagerTest, makeVersionString_validInput_correctOutput) {
  const std::string version1_exp("3");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION1), version1_exp);

  const std::string version2_exp("0.1.2");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION2), version2_exp);

  const std::string version3_exp("10.3.5.6.7.8");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION3), version3_exp);

  const std::string version4_exp("1.21.300-rc5-test3");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION4), version4_exp);

  const std::string version5_exp("200.3465.95-patch3-do-not-use");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION5), version5_exp);

  const std::string version6_exp("3.4");
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSION6), version6_exp);
}

#define VERSIONINVALID1 L"asdf"                   // no numbers
#define VERSIONINVALID2 L"string without numbers" // no numbers
#define VERSIONINVALID3 L""                       // empty string

TEST(VersionManagerTest, makeVersionString_invalidInput_emptyOutput) {
  const std::string empty{""};
  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSIONINVALID1), empty);

  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSIONINVALID2), empty);

  EXPECT_EQ(next::sdk::version_manager::MakeVersionString(VERSIONINVALID3), empty);
}

TEST(VersionManagerTest, initTest_charValues) {
  next::sdk::version_manager::register_to_init_chain();

  next::sdk::InitChain::ConfigMap initmap;

  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), "VersionManagerTest");
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), "link");
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), "0.0.0");

  EXPECT_TRUE(next::sdk::InitChain::Run(initmap));

  next::sdk::InitChain::Reset();
}

TEST(VersionManagerTest, initTest_stringValues) {
  next::sdk::version_manager::register_to_init_chain();

  next::sdk::InitChain::ConfigMap initmap;

  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(),
                  (std::string) "VersionManagerTest");
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), (std::string) "link");
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), (std::string) "0.0.0");

  EXPECT_TRUE(next::sdk::InitChain::Run(initmap));

  next::sdk::InitChain::Reset();
}
