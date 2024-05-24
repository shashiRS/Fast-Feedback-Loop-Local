/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file    profiler_test.cpp
 *  @brief   Testing the profiler
 */

#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;

class ProfilerInitialization : public ::testing::Test {
protected:
  void SetUp() override { next::sdk::profiler::register_to_init_chain(); }

  void TearDown() override { InitChain::Reset(); }

  bool init_success_ = false;
  InitChain::ConfigMap initmap_;
};

TEST_F(ProfilerInitialization, NoInstanceNameGiven_fails) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
}

TEST_F(ProfilerInitialization, check_enum_overview) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OVERVIEW);
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
  EXPECT_EQ(next_profiling::getProfilingLevel(), next_profiling::ProfilingLevel::OVERVIEW);
}

TEST_F(ProfilerInitialization, check_enum_detail) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::DETAIL);
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
  EXPECT_EQ(next_profiling::getProfilingLevel(), next_profiling::ProfilingLevel::DETAIL);
}

TEST_F(ProfilerInitialization, check_enum_full) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  initmap_.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::FULL);
  init_success_ = InitChain::Run(initmap_);
  EXPECT_TRUE(init_success_);
  EXPECT_EQ(next_profiling::getProfilingLevel(), next_profiling::ProfilingLevel::FULL);
}

TEST_F(ProfilerInitialization, check_wrong_enum_int) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  const auto default_level = next_profiling::getProfilingLevel();
  initmap_.emplace(next_profiling::getProfileLevelCfgKey(), 6);
  init_success_ = InitChain::Run(initmap_);
  EXPECT_FALSE(init_success_);
  EXPECT_EQ(next_profiling::getProfilingLevel(), default_level);
}

TEST_F(ProfilerInitialization, check_wrong_enum_string) {
  ASSERT_EQ(initmap_.size(), 0);
  ASSERT_FALSE(init_success_) << "variable is not correctly initialized";

  const auto default_level = next_profiling::getProfilingLevel();
  initmap_.emplace(next_profiling::getProfileLevelCfgKey(), "dfjlksdfkj");
  init_success_ = InitChain::Run(initmap_);
  EXPECT_FALSE(init_success_);
  EXPECT_EQ(next_profiling::getProfilingLevel(), default_level);
}
