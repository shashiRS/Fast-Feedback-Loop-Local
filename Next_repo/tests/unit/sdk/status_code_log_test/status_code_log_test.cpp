#include <cip_test_support/gtest_reporting.h>

#include "status_code_loglevel_parser.hpp"

TEST(STATUS_CODE_TO_LOG_LEVEL, StatusToLog_Success) {
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::INFO, next::appsupport::loglevel_parser::LogLevelFromStatusCode(
                                                   next::appsupport::StatusCode::SUCC_COMP_CONFIG_FINISHED));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::ERR, next::appsupport::loglevel_parser::LogLevelFromStatusCode(
                                                  next::appsupport::StatusCode::ERR_COMP_INIT_FAILED));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::ERR, next::appsupport::loglevel_parser::LogLevelFromStatusCode(
                                                  next::appsupport::StatusCode::ERR_EXPORTER_NO_SUBSCRIPTION));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::WARN, next::appsupport::loglevel_parser::LogLevelFromStatusCode(
                                                   next::appsupport::StatusCode::WARN_PLUGIN_NOT_FOUND));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::ERR,
            next::appsupport::loglevel_parser::LogLevelFromStatusCode(
                next::appsupport::StatusCode::ERR_EXPORTER_FILE_FORMAT_NOT_SUPPORTED));
}

TEST(STATUS_CODE_TO_LOG_LEVEL, StatusCodeDefaultToDebug_Success) {
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::DEBUG,
            next::appsupport::loglevel_parser::LogLevelFromStatusCode((next::appsupport::StatusCode)600));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::DEBUG,
            next::appsupport::loglevel_parser::LogLevelFromStatusCode((next::appsupport::StatusCode)-600));
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::DEBUG,
            next::appsupport::loglevel_parser::LogLevelFromStatusCode((next::appsupport::StatusCode)100));
}
