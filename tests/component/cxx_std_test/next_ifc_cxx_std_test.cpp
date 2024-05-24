/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ifc_cxx_std_test.cpp
 *  @brief    Testing the CXX standard setting of the Next Interface
 *
 * This file is for checking the C++ standard. It shall be C++14. For that it links against next (and by that against
 * SDK, uDEX...). So it inherits the public settings of the other libraries. They shall not set to some newer standard.
 *
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <memory>

#include <next/next.hpp>

// following includes are already included by next.hpp, but to have tested that they are really working, they are added
// again
#include <next/next_config.h>
#include <next/data_types.hpp>
#include <next/next_callbacks.hpp>

TEST(CxxStdTest, defaultIsCxx14) {
  // from https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
  // C++11 201103L
  // C++14 201402L
  // C++17 201703L
  // C++20 202002L
  const long cxx14 = 201402L;
  EXPECT_EQ(cxx14, __cplusplus) << "C++ standard should be set to C++14 (set in CMakeLists.txt)";
}

class MyTestClass : public next::NextComponentInterface {
public:
  MyTestClass(const std::string component_name) : NextComponentInterface(component_name) {}
  virtual bool enterConfig() { return true; }
  virtual bool enterReset() { return true; }
  virtual bool enterInit() { return true; }
  virtual bool enterShutdown() { return true; }
  virtual bool onError(std::string error, std::exception *e = nullptr) {
    (void)error;
    (void)e;
    return true;
  }
};

TEST(InterfaceCxxStdTest, shallCompileWithCxx14) {
  const std::string component_name{"InterfaceCxxStdTest"};
  std::shared_ptr<MyTestClass> interface{nullptr};
  EXPECT_NO_THROW(interface = std::make_shared<MyTestClass>(component_name));
  EXPECT_NO_THROW(interface.reset());
}
