/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     ecal_gtest_fixture_test.cpp
 *  @brief    Testing the ecal gtest fixture
 */

#include <next/sdk/sdk.hpp>

// to be hones, this is only checking whether it compiles and can be executed

class MyTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  MyTestFixture() { this->instance_name_ = "MyTestFixture"; }
};

TEST_F(MyTestFixture, success) { EXPECT_TRUE(true); }
