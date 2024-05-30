/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     output_schema_checker_test.cpp
 *  @brief    OutputSchemaChecker unit tests
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <next/databridge/plugin_manager.h>
#include <output_schema_checker.h>

TEST(OUTPUT_SCHEMA_CHECKER, check_for_trafficparticipants) {
  auto output_checker = next::databridge::plugin_manager::OutputSchemaChecker();
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "trafficparticipant.fbs";
  EXPECT_TRUE(output_checker.CheckFlatbufferSchema(path_to_fbs.string(), "GuiSchema.TrafficParticipant", "0.0.1"));
}

TEST(OUTPUT_SCHEMA_CHECKER, check_for_groundLines) {
  auto output_checker = next::databridge::plugin_manager::OutputSchemaChecker();
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "groundline.fbs";
  EXPECT_TRUE(output_checker.CheckFlatbufferSchema(path_to_fbs.string(), "GuiSchema.GroundLine", "0.0.1"));
}
