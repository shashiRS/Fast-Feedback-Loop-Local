/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     initchain_test.cpp
 *  @brief    Testing the init chain
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/init_chain/simple_init_chain.hpp>

#include "random_generator.hpp"

#include <array>
#include <iostream>

TEST(RandomNumbers, getNumbers_numbersAreCorrect) {
  next::sdk::InitChain::ConfigMap config;
  config[RandomGenerator::getSeedCfgKey()] = 100lu;
  const unsigned int min = 10u;
  config[RandomGenerator::getMinCfgKey()] = min;
  config[RandomGenerator::getMaxCfgKey()] = 19u;
  const size_t nRolls = 10000;

  next::sdk::InitChain::El rand(next::sdk::InitPriority::USER_LEVEL_BASE + 10, RandomGenerator::init,
                                RandomGenerator::reset);
  next::sdk::InitChain::Run(config);

  std::array<int, 10u> rolls = {};

  for (size_t i = 0u; i < nRolls; ++i) {
    const unsigned int roll = RandomGenerator::get();
    ++rolls[roll - min];
  }

  for (size_t i = 0; i < rolls.size(); ++i) {
    EXPECT_TRUE(950 < rolls[i] && rolls[i] < 1050)
        << "value " << i << " should be close to 1000, but it is " << rolls[i];
  }

  next::sdk::InitChain::Reset();

  // now try to get exactly the same numbers again

  next::sdk::InitChain::Run(config);

  std::array<int, 10u> rolls_2ndtry = {};
  static_assert(rolls.size() == rolls_2ndtry.size());

  for (size_t i = 0u; i < nRolls; ++i) {
    const unsigned int roll = RandomGenerator::get();
    ++rolls_2ndtry[roll - min];
  }

  for (size_t i = 0u; i < rolls.size(); ++i) {
    EXPECT_EQ(rolls[i], rolls_2ndtry[i]) << "values on position " << i << " are not equal";
  }

  next::sdk::InitChain::Reset();
}
