/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     logger_config.cpp
 *  @brief    Testing the configuration of the logger lib
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>

using next::appsupport::ConfigClient;

TEST(LoggerConfig, local_definitions_available) {
  // just call the functions to check that they are available
  std::string comp_name = "test";
  ConfigClient::do_init(comp_name);
  ConfigClient::getConfig()->putCfg("logger_cfg.json");
  ConfigClient::finish_init();

  EXPECT_TRUE(ConfigClient::getConfig()->get_option(
      next::appsupport::configkey::logger::getSinkActiveKey(comp_name, "console"), false));
  EXPECT_TRUE(ConfigClient::getConfig()->get_option(
      next::appsupport::configkey::logger::getSinkActiveKey(comp_name, "file"), false));
  EXPECT_TRUE(ConfigClient::getConfig()->get_option(
      next::appsupport::configkey::logger::getSinkActiveKey(comp_name, "global"), false));

  EXPECT_EQ(ConfigClient::getConfig()->get_string(
                next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "console"), ""),
            "info");
  EXPECT_EQ(ConfigClient::getConfig()->get_string(
                next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "file"), ""),
            "debug");
  EXPECT_EQ(ConfigClient::getConfig()->get_string(
                next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "global"), ""),
            "warn");

  EXPECT_EQ(ConfigClient::getConfig()->get_int(next::appsupport::configkey::logger::getSendFrequencyKey(comp_name), 0),
            200);
  EXPECT_EQ(
      ConfigClient::getConfig()->get_string(next::appsupport::configkey::logger::getLocalLogfileKey(comp_name), ""),
      "test.log");

  ConfigClient::do_reset();
}

TEST(LoggerConfig, global_definitions_available) {
  // just call the functions to check that they are available
  std::string comp_name = "next_controlbridge";
  ConfigClient::do_init(comp_name);
  ConfigClient::getConfig()->putCfg("logger_cfg.json");
  ConfigClient::finish_init();

  EXPECT_EQ(ConfigClient::getConfig()->get_string(next::appsupport::configkey::logger::getGlobalLogfileKey(), ""),
            "global.log");

  ConfigClient::do_reset();
}
