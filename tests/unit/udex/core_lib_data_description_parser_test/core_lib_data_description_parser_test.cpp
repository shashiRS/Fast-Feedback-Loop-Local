#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include "core_lib_wrap_data_descriptor_parser.hpp"

class CoreLibDatadescriptionParserTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  CoreLibDatadescriptionParserTestFixture() { this->instance_name_ = "CoreLibDatadescriptionParserTestFixture"; }
};

TEST_F(CoreLibDatadescriptionParserTestFixture, getArrayIndexFromString_no_array_success) {
  auto tester = CoreLibWrap::DataDescriptionParser();
  size_t index;
  std::string url_test = "asdf.asdf";
  std::string url = "asdf.asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(url_test, url);

  url_test = "asdf[].asdf";
  url = "asdf[].asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(url_test, url);

  url_test = "[]asdf.asdf";
  url = "[]asdf.asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(url_test, url);

  url_test = "adsfadsf.asdf[123].asdf";
  url = "adsfadsf.asdf[123].asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "adsfadsf[1].asdf[123].asdf";
  url = "adsfadsf[1].asdf[123].asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "[1].asdf[123].asdf";
  url = "[1].asdf[123].asdf";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "[1].asdf[123].asdf[]";
  url = "[1].asdf[123].asdf[]";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "[]";
  url = "[]";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "]";
  url = "]";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "[";
  url = "[";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);
}

TEST_F(CoreLibDatadescriptionParserTestFixture, getArrayIndexFromString_invalid_strings_no_crash) {
  auto tester = CoreLibWrap::DataDescriptionParser();
  size_t index;
  std::string url_test;
  std::string url;

  url_test = "[]";
  url = "[]";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "]";
  url = "]";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);

  url_test = "[";
  url = "[";
  EXPECT_EQ(false, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(url_test, url);
  EXPECT_EQ(index, 0);
}

TEST_F(CoreLibDatadescriptionParserTestFixture, getArrayIndexFromString_find_array_success) {
  auto tester = CoreLibWrap::DataDescriptionParser();
  size_t index;
  std::string url_cut = "asdf.asdf";
  std::string url = "asdf.asdf[012]";
  EXPECT_EQ(true, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 12);
  EXPECT_EQ(url_cut, url);

  url_cut = "asdf[adsf].asdf";
  url = "asdf[adsf].asdf[012]";
  EXPECT_EQ(true, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 12);
  EXPECT_EQ(url_cut, url);

  url_cut = "asdf[adsf].asdf";
  url = "asdf[adsf].asdf[0]";
  EXPECT_EQ(true, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(url_cut, url);

  url_cut = "";
  url = "[0]";
  EXPECT_EQ(true, tester.ExtractNameAndIndexFromArraySignal(index, url));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(url_cut, url);
}