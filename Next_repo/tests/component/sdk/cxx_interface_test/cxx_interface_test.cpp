/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     cxx_interface_test.cpp
 *  @brief    Testing the Next SDK interface C++ standard (compatibility to C++14)
 *
 * This file is using the Next SDK and some of the public interfaces. By linking against the SDK it is inherited to
 * compile with C++14.
 *
 */

#include <cip_test_support/gtest_reporting.h>
#include <next/sdk/event_base/base_event.h>
#include <next/sdk/event_base/base_event_binary.h>
#include <next/sdk/event_base/base_event_binary_blocking.h>
#include <next/sdk/event_base/base_event_binary_impl.h>
#include <next/sdk/events/lifecycle_state.h>
#include <next/sdk/events/lifecycle_trigger.h>
#include <next/sdk/events/request_config_from_client.h>
#include <next/sdk/events/request_config_from_server.h>
#include <next/sdk/events/send_config_to_client.h>
#include <next/sdk/events/send_config_to_server.h>
#include <next/sdk/events/version_info.h>
#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_base.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/config/config_key_generator_player.hpp>
#include <next/appsupport/config/config_server.hpp>
#include <next/appsupport/lifecycle/lifecycle_base.hpp>
#include <next/appsupport/lifecycle/lifecycle_statemachine_enum.hpp>
#include <next/sdk/sdk_config.hpp>
#include <next/sdk/shutdown_handler/shutdown_handler.hpp>
#include <next/sdk/string_helper/strings.hpp>

// this test only checks that the application was compiled with C++14 (inherited by SDK)
TEST(CxxInterfaceTest, cxx14used) {
  const long Cxx14{201402L};

  EXPECT_EQ(__cplusplus, Cxx14);
}
