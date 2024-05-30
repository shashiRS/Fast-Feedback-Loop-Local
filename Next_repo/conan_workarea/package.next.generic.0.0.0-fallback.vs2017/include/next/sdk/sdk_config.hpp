/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     sdk_config.hpp
 * @brief    configuration of the Next SDK
 *
 **/

#ifndef NEXTSDK_CONFIG_H_
#define NEXTSDK_CONFIG_H_

/*
 * When building a shared libary the public symbols must be marked as "exported" when building the library,
 * and "imported" when using the library on Windows.
 *
 * In case of other usage (e.g. static library or source files within the object) the define will be an empty string.
 *
 * CMake is defining "next_sdk_EXPORTS" by itself when building the shared library (the CMake target is named
 * "next_sdk"). This can be used here, without defining something new.
 */

#ifndef NEXT_SDK_LIBRARY_STATIC
#if defined(_WIN32)
#if defined(next_sdk_EXPORTS)
#define DECLSPEC_nextsdk __declspec(dllexport)
#else
#define DECLSPEC_nextsdk __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_nextsdk
#endif
#endif

#ifndef DECLSPEC_nextsdk
#define DECLSPEC_nextsdk
#endif

#endif // NEXTSDK_CONFIG_H_
