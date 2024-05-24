/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * LIBRARY: Next-UDex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     profiling_library.cpp
 * @brief    library to test the profiling, for details see profiling_library.hpp
 */

#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profile_tags.hpp>

#include "profiling_library.hpp"

namespace logger = next::sdk::logger; // make it more handy

double square(double d) {
  ProfileCore_FLL;
  d *= d;
  logger::debug(__FILE__, "The result is {0}", d);
  return d;
}
