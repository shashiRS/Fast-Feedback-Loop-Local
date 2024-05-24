/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     sdk.hpp
 * @brief    the main include file of Next SDK
 *
 * Include this file if you want to use the Next SDK.
 *
 * If only need specific parts are needed, it is sufficient to only use the corresponding include file.
 *
 * Note: On Windows the shutdown handler is including windows.h. This will provide by default the macros min(a, b)
 * and max(a, b). Add the definition NOMINMAX to disable this. See windows.h.
 */

#ifndef NEXTSDK_SDK_H_
#define NEXTSDK_SDK_H_

// as very first the initializer chain must be included
#include "init_chain/simple_init_chain.hpp"

// then the logger shall be included
#include "logger/logger.hpp"
// when NO_USE_LOGGER_NAMESPACE is defined, the logger.hpp will not make make "using next::coresdk::logger::debug;" (and
// info, warn and error).

// remaining stuff is alphabetically sorted
#include "ecal/ecal_singleton.hpp"
#include "lifecycle/lifecycle_interface.hpp"
#include "profiler/profiler.hpp"
#include "sdk_macros.h"
#include "shutdown_handler/shutdown_handler.hpp"
#include "string_helper/strings.hpp"
#include "version_manager/version_manager.hpp"

#ifdef BUILD_TESTING
#include "testing/test_using_ecal.hpp"
#endif

#endif // NEXTSDK_SDK_H_
