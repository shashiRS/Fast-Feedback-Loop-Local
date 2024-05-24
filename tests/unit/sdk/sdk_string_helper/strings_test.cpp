/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     strings_test.cpp
 *  @brief    Testing the string helper functions
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/string_helper/strings.hpp>

using next::sdk::string::split;
using next::sdk::string::strip;

//! Testing the string split function
TEST(StringSplit, CorrectSplits) {
  const std::string hello("hello");
  const std::string world("world");
  const std::string hw(hello + "." + world);

  auto vec = split(hw, ".");

  ASSERT_EQ(vec.size(), 2);
  EXPECT_EQ(vec[0], hello) << "split_string('hello.world', '.')[0] should give 'hello'";
  EXPECT_EQ(vec[1], world) << "split_string('hello.world', '.')[0] should give 'world'";
  vec.clear();

  const std::string empty("");
  vec = split(empty, ".");
  ASSERT_EQ(vec.size(), 1) << "splitting an empty string should give a vector with one empty element";
  EXPECT_EQ(vec[0].size(), 0) << "splitting an empty string should give a vector with one empty element";

  const std::string simple("this is only a sentence without any dot");
  vec = split(simple, ".");
  ASSERT_EQ(vec.size(), 1) << "splitting a string by dot without a dot in the string should give one string back";
  EXPECT_EQ(vec[0], simple) << "splitting a string by dot without a dot in the string should give the original string";

  const std::string doubleDot(hello + ".." + world);
  vec = split(doubleDot, ".");
  ASSERT_EQ(vec.size(), 3) << "splitting a string with two dots should give 3 strings back";
  EXPECT_EQ(vec[0], hello);
  EXPECT_TRUE(vec[1].empty()) << "splitting a string by dot with '..' in give a empty string";
  EXPECT_EQ(vec[2], world);

  EXPECT_THROW(split(hello, ""), std::invalid_argument)
      << "For an empty delimiter a invalid argument exception shall be thrown";

  const std::string trailingDelim(world + "/" + hello + "//");
  vec = split(trailingDelim, "/");
  ASSERT_EQ(vec.size(), 4) << "for having double trailing delimiter it should give two empty strings at the end";
  ASSERT_EQ(vec[0], world);
  ASSERT_EQ(vec[1], hello);
  ASSERT_TRUE(vec[2].empty()) << "for having double trailing delimiter it should give two empty strings at the end";
  ASSERT_TRUE(vec[3].empty()) << "for having double trailing delimiter it should give two empty strings at the end";
}

TEST(Strip, CorrectStrips) {
  const std::string hw("hello world");
  std::string input1(" " + hw + " ");
  strip(input1);
  EXPECT_EQ(input1, hw);

  std::string input2(" .a" + hw + ".a. ");
  strip(input2, "a. ");
  EXPECT_EQ(input2, hw);

  std::string input3(" " + hw + "\t\n");
  strip(input3, " \n");
  EXPECT_EQ(input3, std::string(hw + "\t"));
  strip(input3, "\t");
  EXPECT_EQ(input3, hw);

  std::string input4("www.example.com");
  strip(input4, "cmowz.");
  EXPECT_EQ(input4, std::string("example"));

  std::string input5("www.examples.com");
  strip(input5, "cemowz.");
  EXPECT_EQ(input5, std::string("xamples"));
}

TEST(StripVector, CorrectStrips) {
  const std::vector<std::string> base{" hello world ", ".hello world.", "www.example.com"};

  std::vector<std::string> vec1(base);
  strip(vec1, " ");
  ASSERT_EQ(vec1.size(), 3);
  EXPECT_EQ(vec1[0], std::string("hello world"));
  EXPECT_EQ(vec1[1], std::string(".hello world."));
  EXPECT_EQ(vec1[2], std::string("www.example.com"));

  std::vector<std::string> vec2(base);
  strip(vec2, "cmow.");
  ASSERT_EQ(vec2.size(), size_t(3));
  EXPECT_EQ(vec2[0], " hello world ");
  EXPECT_EQ(vec2[1], "hello world");
  EXPECT_EQ(vec2[2], "example");
}
