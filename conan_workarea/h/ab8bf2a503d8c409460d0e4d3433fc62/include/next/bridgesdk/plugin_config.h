/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_config.h
 * @brief    configuration of plugin
 *
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_CONFIG_H_
#define NEXT_BRIDGESDK_PLUGIN_CONFIG_H_

#if defined(_WIN32)
#if defined(next_plugin_EXPORTS)
#define DECLSPEC_plugin_config __declspec(dllexport)
#else
#define DECLSPEC_plugin_config __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_plugin_config
#endif

#endif // NEXT_BRIDGESDK_PLUGIN_CONFIG_H_
