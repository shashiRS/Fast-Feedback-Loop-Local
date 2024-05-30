/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     control_cxx_std_test.cpp
 *  @brief    Testing the control interface to be compatible to C++14
 *
 * This file is using the orchestrator_client which is part of control and is compiled with C++17, and we are testing if
 * we can compile test file with the control library in C++14.
 */

#include "gtest/gtest.h"

#include <next/control/orchestrator/orchestrator_client.h>
#include <next/control/orchestrator/orchestrator_master.h>

TEST(CxxStdTest, dummyTest) {
  const long Cxx14{201402L};
  EXPECT_EQ(Cxx14, __cplusplus);
}
