/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     profiler.cpp
 * @brief    wrapper for tracy lib
 */
#define TRACY_NO_BROADCAST
#include <atomic>

#include <next/sdk/profiler/profile_tags.hpp>
#include <next/sdk/profiler/profiler.hpp>

static constexpr const char *kProfileLevelConfigKey = "tracy-profile-level";

namespace next {
namespace sdk {
namespace profiler {

static std::atomic<ProfilingLevel> profiling_level_{ProfilingLevel::OFF};

void setProfilingLevel(ProfilingLevel profiling_level) { profiling_level_ = profiling_level; }

ProfilingLevel getProfilingLevel(void) { return profiling_level_; }

const char *getProfileLevelCfgKey() { return kProfileLevelConfigKey; }

static bool init_already_done = false;

bool init(const InitChain::ConfigMap &config, int) {
  if (false == init_already_done) {
    auto cit = config.find(kProfileLevelConfigKey);
    if (config.end() != cit) {
      if (cit->second.type() == typeid(ProfilingLevel)) {
        profiling_level_ = std::any_cast<ProfilingLevel>(cit->second);
      } else {
        return false;
      }
    }
    init_already_done = true;
  }
  return true;
}

void close(const InitChain::ConfigMap &, int) {
  init_already_done = false;
  profiling_level_ = ProfilingLevel::OFF;
}

void register_to_init_chain() { static InitChain::El init_el(InitPriority::PROFILER, init, close); }

} // namespace profiler
} // namespace sdk
} // namespace next
