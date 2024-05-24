#include <cip_test_support/gtest_reporting.h>
#include <list>

#include "argument_parser.hpp"

const std::string exename = "asdfasdf.exe";
const std::string cut_key = "-cut";
const std::string cut_name1 = "sicoregeneric";
const std::string cut_name2 = "sicoregeneric1";
const std::string name_key = "-name";
const std::string name_1 = "container1";
const std::string next_key = "-n";
const std::string random_key = "-asdf";
const std::string random_key_1 = "-dut";

bool findKeyInList(const std::vector<std::string> &args, std::string key) {
  return std::find(args.begin(), args.end(), key) != args.end();
}

TEST(next_container, parse_argument_add_next_key_success) {

  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;
  next::container::ArgumentParserNextContainer parser = next::container::ArgumentParserNextContainer();
  auto comp = parser.parseArguments({exename}, args_extended, node_names);

  ASSERT_EQ(args_extended.size(), 2);
  EXPECT_TRUE(findKeyInList(args_extended, next_key));
  EXPECT_TRUE(findKeyInList(args_extended, exename));
}

TEST(next_container, parse_argument_single_cut_add_next_key_success) {

  next::container::ArgumentParserNextContainer parser = next::container::ArgumentParserNextContainer();
  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;

  auto comp = parser.parseArguments({exename, cut_key, cut_name1}, args_extended, node_names);
  ASSERT_EQ(args_extended.size(), 2);
  EXPECT_TRUE(findKeyInList(args_extended, next_key));
  EXPECT_TRUE(findKeyInList(args_extended, exename));
}

TEST(next_container, parse_argument_multi_cut_and_random_arg) {

  next::container::ArgumentParserNextContainer parser = next::container::ArgumentParserNextContainer();
  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;

  auto comp = parser.parseArguments({exename, next_key, random_key_1, cut_key, cut_name1, cut_name2, random_key},
                                    args_extended, node_names);
  ASSERT_EQ(args_extended.size(), 4);
  EXPECT_TRUE(findKeyInList(args_extended, next_key));
  EXPECT_TRUE(findKeyInList(args_extended, random_key_1));
  EXPECT_TRUE(findKeyInList(args_extended, random_key));
  EXPECT_TRUE(findKeyInList(args_extended, exename));
  EXPECT_TRUE(findKeyInList(node_names, cut_name1));
  EXPECT_TRUE(findKeyInList(node_names, cut_name2));
}

TEST(next_container, parse_name_success) {

  next::container::ArgumentParserNextContainer parser = next::container::ArgumentParserNextContainer();
  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;

  auto comp = parser.parseArguments({exename, name_key, name_1}, args_extended, node_names);
  ASSERT_EQ(args_extended.size(), 2);
  EXPECT_TRUE(findKeyInList(args_extended, next_key));
  EXPECT_TRUE(findKeyInList(args_extended, exename));
  EXPECT_TRUE(comp == name_1);
}

TEST(next_container, parse_argument_multi_cut_and_random_arg_extract_name_success) {

  next::container::ArgumentParserNextContainer parser = next::container::ArgumentParserNextContainer();
  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;

  auto comp = parser.parseArguments(
      {exename, name_key, name_1, next_key, random_key_1, cut_key, cut_name1, cut_name2, random_key}, args_extended,
      node_names);
  EXPECT_TRUE(comp == name_1);

  ASSERT_EQ(args_extended.size(), 4);
  EXPECT_TRUE(findKeyInList(args_extended, next_key));
  EXPECT_TRUE(findKeyInList(args_extended, random_key_1));
  EXPECT_TRUE(findKeyInList(args_extended, random_key));
  EXPECT_TRUE(findKeyInList(args_extended, exename));
  EXPECT_TRUE(findKeyInList(node_names, cut_name1));
  EXPECT_TRUE(findKeyInList(node_names, cut_name2));
}