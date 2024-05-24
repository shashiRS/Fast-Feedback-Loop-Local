/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     profiler.hpp
 * @brief    wrapper for tracy lib
 */

#ifndef NEXTSDK_PROFILER_H_
#define NEXTSDK_PROFILER_H_

#include "../init_chain/simple_init_chain.hpp"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace profiler {
/**
 * @brief enum for different available profiler level (OVERVIEW, DETAIL, FULL)
 *
 */
enum class ProfilingLevel { OFF, OVERVIEW, DETAIL, FULL };

const std::map<ProfilingLevel, std::string> kProfilingLevel = {{ProfilingLevel::OFF, "OFF"},
                                                               {ProfilingLevel::OVERVIEW, "OVERVIEW"},
                                                               {ProfilingLevel::DETAIL, "DETAIL"},
                                                               {ProfilingLevel::FULL, "FULL"}};

/**
 * @brief set the profiling level
 *
 * @param ProfilingLevel
 */
void DECLSPEC_nextsdk setProfilingLevel(ProfilingLevel profiling_level);

/**
 * @brief get the profiling level
 *
 * @return ProfilingLevel
 */
ProfilingLevel DECLSPEC_nextsdk getProfilingLevel(void);

/**
 * @brief get the config key for the profile level for the init chain
 *
 * @return const char*
 */
const char DECLSPEC_nextsdk *getProfileLevelCfgKey();

/**
 * @brief initialize the profiler
 *
 * @param config
 * @param level
 * @return true
 * @return false
 */
bool DECLSPEC_nextsdk init(const InitChain::ConfigMap &config, int level);

/**
 * @brief reset the profiler
 *
 * @param config
 * @param level
 */
void DECLSPEC_nextsdk reset(const InitChain::ConfigMap &config, int level);

/**
 * @brief register the next_tracy for initialization chain
 */
void DECLSPEC_nextsdk register_to_init_chain();

} // namespace profiler
} // namespace sdk
} // namespace next

namespace next_profiling = next::sdk::profiler;

#endif // NEXTSDK_PROFILER_H_
