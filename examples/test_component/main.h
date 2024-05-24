/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#ifndef MAIN_H
#define MAIN_H

/// Copied from actual ECO generated node
/*
 * When building a shared libary the public symbols must be marked as "exported"
 * when building the library, and "imported" when using the library on Windows.
 *
 * In case of other usage (e.g. static library or source files within the
 * object) the define will be an empty string.
 *
 * DEFINE ECO_EXPORTS in your dynamic library build
 *
 */

#if defined(_WIN32)
#if defined(ECO_EXPORTS)
#define DLL_FUNCTION_EXPORT extern "C" __declspec(dllexport)
#endif
#endif

// define empty if linux or else
#ifndef DLL_FUNCTION_EXPORT
#define DLL_FUNCTION_EXPORT
#endif

DLL_FUNCTION_EXPORT const char *nextDllName();
DLL_FUNCTION_EXPORT int nextDllMain(int argc, const char *argv[]);
DLL_FUNCTION_EXPORT int nextDllVersion(void);

#endif // MAIN_H
