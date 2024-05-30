/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     ecal_singleton_initialization_test.cpp
 * @brief    testing the eCAL singleton initialization
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class EcalInitialization : public ::testing::Test {
protected:
  void SetUp() override { next::sdk::EcalSingleton::register_to_init_chain(); }

  void TearDown() override {
    if (init_success_) {
      // simulate that eCAL is used
      next::sdk::EcalSingleton::get_instance();
    }
    InitChain::Reset();
  }

  bool init_success_ = false;
  InitChain::ConfigMap initmap_;
};

TEST_F(EcalInitialization, NoInstanceNameGiven_fails) {
  ASSERT_EQ(initmap_.size(), 0) << "config seems incorrect";
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  init_success_ = InitChain::Run(initmap_);
  EXPECT_FALSE(init_success_);
}

TEST_F(EcalInitialization, InstanceNameGivenAsChar_success) {
  ASSERT_EQ(initmap_.size(), 0) << "config seems incorrect";
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "EcalInitializationTest");
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
}

TEST_F(EcalInitialization, InstanceNameGivenAsString_success) {
  ASSERT_EQ(initmap_.size(), 0) << "config seems incorrect";
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), (std::string) "EcalInitializationTest");
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
}

TEST_F(EcalInitialization, InitMultipleTimes_fails) {
  ASSERT_EQ(initmap_.size(), 0) << "config seems incorrect";
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "EcalInitializationTest");
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
  EXPECT_FALSE(InitChain::Run(initmap_));
  EXPECT_FALSE(InitChain::Run());
}
