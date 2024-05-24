/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     get_def_std.hpp
 *  @brief    Get the default C++ standard
 */

#ifndef GET_DEF_STD_H_
#define GET_DEF_STD_H_

#if defined(_WIN32)
#if defined(sdk_def_cxx_std_EXPORTS)
#define DECLSPEC_sdk_def_cxx_std __declspec(dllexport)
#else
#define DECLSPEC_sdk_def_cxx_std __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_sdk_def_cxx_std
#endif

#ifndef DECLSPEC_sdk_def_cxx_std
#define DECLSPEC_sdk_def_cxx_std
#endif

long DECLSPEC_sdk_def_cxx_std getDefStd();

#endif // GET_DEF_STD_H_
