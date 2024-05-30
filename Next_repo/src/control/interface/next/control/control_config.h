/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     control_config.h
 * @brief
 *
 **/

#ifndef NEXT_CONTROL_CONFIG_H_
#define NEXT_CONTROL_CONFIG_H_

/*
Preconditions:
Define CONTROL_LIBSUPPORT_STATIC when building and using as a static library.
Define CONTROL_LIBSUPPORT_BUILD when building the library.
Mark public symbols with CONTROL_LIBRARY_EXPORT.
*/

#ifndef CONTROL_LIBRARY_STATIC
/* It's a dynamic library.
The public symbols must be marked as "exported" when building the library,
and "imported" when using the library.
*/
#ifdef CONTROL_LIBRARY_BUILD
/* Building the library */
#ifdef _WIN32
#define CONTROL_LIBRARY_EXPORT __declspec(dllexport)
#elif __GNUC__ >= 4
#define CONTROL_LIBRARY_EXPORT __attribute__((visibility("default")))
#else
#define CONTROL_LIBRARY_EXPORT
#endif
#else
/* Using the library */
#ifdef _WIN32
#define CONTROL_LIBRARY_EXPORT __declspec(dllimport)
#else
#define CONTROL_LIBRARY_EXPORT
#endif
#endif
#endif

#ifndef CONTROL_LIBRARY_EXPORT
/* It's a static library, no need to import/export anything */
#define CONTROL_LIBRARY_EXPORT
#endif

#endif // NEXT_CONTROL_CONFIG_H_
