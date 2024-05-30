/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     cxx_std_test.cpp
 *  @brief    Testing the CXX std setting
 *
 * Within this repository the default C++ standard is set to 17. This applicaiton itself is compiled explicitly with
 * C++14. By linking against the SDK we make sure that it is not propagating C++17 as default setting.
 *
 * for the values of __cplusplus see https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
 *
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/string_helper/strings.hpp>

#include "def_cxx_std.hpp"

// make sure that this test is compiled usind C++14 (explicitly set in the CMakeLists.txt of this test)
TEST(CxxStdTest, cxxStd_shallBe14) {

#if __cplusplus != 201402L
#error "The C++ standard of this executable shall be C++14."
#endif
  // no actual test here, when it compiles it is fine for us
  // for google test it is fine when nothing fails
}

// make sure that the default C++ standard is also set to C++17, use a help library for that
TEST(CxxStdTest, defaultCxxStd_shallBe14) {
  const long Cxxdefault{getDefStd()};

  EXPECT_EQ(201402L, Cxxdefault);
}

// using something of the SDK, must be compatible to C++14
TEST(CxxStdTest, useSdk_shallCompileWithCxx14) {
  const std::string s{"foo.bar"};

  next::sdk::string::split(s, ".");
}
