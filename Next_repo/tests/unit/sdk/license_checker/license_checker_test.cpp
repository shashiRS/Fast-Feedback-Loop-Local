
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include "license_checker.hpp"

TEST(LICENSE_CHECKER_TEST, check_if_json_file_is_valid) {
  next::appsupport::license::LicenseChecker checker;

  auto stream = checker.OpenJson("license_tester_file.json");
  auto res = checker.ParseJson(stream);
  EXPECT_TRUE(res);
}

TEST(LICENSE_CHECKER_TEST, check_if_json_file_is_invalid) {

  next::appsupport::license::LicenseChecker checker;

  auto stream = checker.OpenJson("invalid.json");
  EXPECT_EQ(stream, "");
}

TEST(LICENSE_CHECKER_TEST, check_result_no_content_to_parse) {

  next::appsupport::license::LicenseChecker checker;

  auto stream = "";
  auto res = checker.ParseJson(stream);
  EXPECT_FALSE(res);
}

TEST(LICENSE_CHECKER_TEST, check_result_invalid_json_format) {

  next::appsupport::license::LicenseChecker checker;

  auto stream = "{   \"TelemetryEnabled\" ----> true }";
  auto res = checker.ParseJson(stream);
  EXPECT_FALSE(res);
}

TEST(LICENSE_CHECKER_TEST, check_result_invalid_member) {

  next::appsupport::license::LicenseChecker checker;

  // auto stream = checker.OpenJson("license_tester_file.json");
  auto stream = "{   \"TelemetryEna\" : true }";
  auto res = checker.ParseJson(stream);
  EXPECT_FALSE(res);
}

TEST(LICENSE_CHECKER_TEST, check_result_telemetry_disabled) {

  next::appsupport::license::LicenseChecker checker;

  // auto stream = checker.OpenJson("license_tester_file.json");
  auto stream = "{   \"TelemetryEnabled\" : false }";
  auto res = checker.ParseJson(stream);
  EXPECT_FALSE(res);
}
