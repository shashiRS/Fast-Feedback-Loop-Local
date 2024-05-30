/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_CONFIG_H_
#define NEXT_UDEX_CONFIG_H_

#if defined(_MSC_VER) && !defined(_WIN32_WINNT)
// set to using Windows SDK 10 (not supporting Windows 8.1 or older)
#define _WIN32_WINNT 0x0A00
#endif

/*
Preconditions:
Define UDEX_LIBRARY_STATIC when building and using as a static library.
Define UDEX_LIBRARY_BUILD when building the library.
Mark public symbols with UDEX_LIBRARY_EXPORT.
*/

#ifndef UDEX_LIBRARY_STATIC
/* It's a dynamic library.
The public symbols must be marked as "exported" when building the library,
and "imported" when using the library.
*/
#ifdef UDEX_LIBRARY_BUILD
/* Building the library */
#ifdef _WIN32
#define UDEX_LIBRARY_EXPORT __declspec(dllexport)
#elif __GNUC__ >= 4
#define UDEX_LIBRARY_EXPORT __attribute__((visibility("default")))
#else
#define UDEX_LIBRARY_EXPORT
#endif
#else
/* Using the library */
#ifdef _WIN32
#define UDEX_LIBRARY_EXPORT __declspec(dllimport)
#else
#define UDEX_LIBRARY_EXPORT
#endif
#endif
#endif

#ifndef UDEX_LIBRARY_EXPORT
/* It's a static library, no need to import/export anything */
#define UDEX_LIBRARY_EXPORT
#endif

#endif // NEXT_UDEX_CONFIG_H_
