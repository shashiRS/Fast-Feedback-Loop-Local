/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_config.hpp
 * @brief    configuration of Next
 *
 **/

#ifndef NEXT_CONFIG_H_
#define NEXT_CONFIG_H_

#ifndef NEXT_LIBRARY_STATIC
#if defined(_WIN32)
#if defined(next_EXPORTS)
#define DECLSPEC_next __declspec(dllexport)
#else
#define DECLSPEC_next __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_next
#endif
#else
#define DECLSPEC_next
#endif

#endif // NEXT_CONFIG_H_
