/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     appsupport_config.hpp
 * @brief    configuration of appsupport
 *
 **/

#ifndef APPSUPPORT_CONFIG_H_
#define APPSUPPORT_CONFIG_H_

#if defined(_WIN32)
#if defined(next_appsupport_EXPORTS)
#define DECLSPEC_appsupport __declspec(dllexport)
#else
#define DECLSPEC_appsupport __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_appsupport
#endif

#endif // APPSUPPORT_CONFIG_H_
