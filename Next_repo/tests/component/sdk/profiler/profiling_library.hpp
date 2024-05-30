/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * LIBRARY: Next-UDex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     profiling_library.hpp
 * @brief    testing the usage of the profiler within another library
 */

#ifndef PROFILINGLIBRARY_H
#define PROFILINGLIBRARY_H

#if defined(_WIN32)
#if defined(profiler_library_EXPORTS)
#define DECLSPEC_profiler_library __declspec(dllexport)
#else
#define DECLSPEC_profiler_library __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_profiler_library
#endif

double DECLSPEC_profiler_library square(double d);

#endif // PROFILINGLIBRARY_H
