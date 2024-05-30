#include <gtest/gtest.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>

TEST(LOG_CONNECTOR_TEST, check_log_level_after_setLogLevel_isCalled) {
  auto comp_name = "test_comp";
  auto sink_type = "router";
  next::appsupport::ConfigClient::do_init(comp_name);
  // 1st test case
  auto log_level = "DEBUG";
  next::appsupport::ConfigClient::getConfig()->put(
      next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), log_level);
  LogConnector::connectLoggerToSystem(comp_name);

  auto set_log_level = next::appsupport::ConfigClient::getConfig()->get_string(
      next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), "");
  EXPECT_EQ(set_log_level, log_level);

  // 2nd test case
  log_level = "INFO";
  next::appsupport::ConfigClient::getConfig()->put(
      next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), log_level);

  set_log_level = next::appsupport::ConfigClient::getConfig()->get_string(
      next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), "");

  EXPECT_EQ(set_log_level, log_level);
}
