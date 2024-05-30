/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     sdk_macros.h
 * @brief    helpfull macros, e.g. disabling compile warnings
 *
 **/

#ifndef NEXTSDK_MACROS_H_
#define NEXTSDK_MACROS_H_

// clang_format off

#define NEXT_STRINGIFY(a) #a

// clang-format off
#if defined(_MSC_VER)
#define NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(id) \
__pragma(warning(push)) \
__pragma(warning(disable : id))
#define NEXT_DISABLE_SPECIFIC_WARNING_LINUX(warning)
#else
#define NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(id)
#define NEXT_DISABLE_SPECIFIC_WARNING_LINUX(warning) \
_Pragma("GCC diagnostic push") \
_Pragma(NEXT_STRINGIFY(GCC diagnostic ignored warning))
#endif
// clang-format on

// clang format off
//! disable named warnings on Windows and Linux, call NEXT_RESTORE_WARNINGS afterwards
#define NEXT_DISABLE_SPECIFIC_WARNING(win_warning, linux_warning)                                                      \
  NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(win_warning)                                                                   \
  NEXT_DISABLE_SPECIFIC_WARNING_LINUX(linux_warning)
// clang-format on

//! disabling deprecation warnings, call NEXT_RESTORE_WARNINGS afterwards
#define NEXT_DISABLE_DEPRECATED_WARNING NEXT_DISABLE_SPECIFIC_WARNING(4996, "-Wdeprecated-declarations")

//! disable DLL interface warnings (caused by using std::vector or similar in interface like classes), Windows only
#define NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4251)

//! restore pushed warnings
#if defined(_MSC_VER)
#define NEXT_RESTORE_WARNINGS_WINDOWS __pragma(warning(pop))
#define NEXT_RESTORE_WARNINGS_LINUX
#define NEXT_RESTORE_WARNINGS NEXT_RESTORE_WARNINGS_WINDOWS
#else
#define NEXT_RESTORE_WARNINGS_WINDOWS
#define NEXT_RESTORE_WARNINGS_LINUX _Pragma("GCC diagnostic pop")
#define NEXT_RESTORE_WARNINGS NEXT_RESTORE_WARNINGS_LINUX
#endif

//! remove copy and move of class
#define DISABLE_COPY(Class)                                                                                            \
  Class(const Class &) = delete;                                                                                       \
  Class &operator=(const Class &) = delete;
#define DISABLE_MOVE(Class)                                                                                            \
  Class(Class &&) = delete;                                                                                            \
  Class &operator=(Class &&) = delete;
#define DISABLE_COPY_MOVE(Class)                                                                                       \
  DISABLE_COPY(Class)                                                                                                  \
  DISABLE_MOVE(Class)

// clang_format on

#if defined(_MSC_VER)
constexpr const char *kSharedLibFileExtension = ".dll";
#else
constexpr const char *kSharedLibFileExtension = ".so";
#endif

// Every shared library has a special name called the ``soname''. The soname has the prefix ``lib'', the name of the
// library, the phrase ``.so'
constexpr const char *kSharedLibFilePrefix = "lib";

#endif // NEXTSDK_MACROS_H_
