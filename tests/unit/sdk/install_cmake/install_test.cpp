/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     install_test.cpp
 *  @brief    Testing the install functions of 3rd party componentes
 */

#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal.h>

//! Testing initialization and finalization of eCAL
TEST(eCAL, findInit) {
  int ret = eCAL::Initialize();
  ASSERT_EQ(ret, 0);

  ret = eCAL::Finalize();
  ASSERT_EQ(ret, 0);
}
