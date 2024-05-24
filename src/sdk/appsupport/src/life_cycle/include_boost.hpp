/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     include_boost.h
 * @brief    this is a helper file to surpress compiler warnings from boost lib
 *
 *
 *
 */

#ifdef __linux__
#pragma GCC system_header // disable all warnings from boost lib for gcc++
#endif

#ifdef _WIN32
#pragma warning(push, 0) // disable all warnings from boost lib for vc++
#endif

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#ifdef _WIN32
#pragma warning(pop)
#endif
