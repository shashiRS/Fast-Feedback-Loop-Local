/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next-player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     resource_manager_interface.hpp
 * @brief    Provides a set of common resources nedded by the player state machine
 *
 **/

#ifndef INTERFACE_RESOURCE_MANAGER_
#define INTERFACE_RESOURCE_MANAGER_

#include "buffer_queue.hpp"
#include "cycle_manager.hpp"
#include "udex_service.hpp"

struct replay_statistics {
  replay_statistics() { resetValues(); }

  void resetValues() {
    package_count = 0;
    published_package_count = 0;
    published_bytes = 0;
    publish_fail_count = 0;
    last_publish_time = std::chrono::steady_clock::now();
    replay_start_time = std::chrono::steady_clock::now();
  }

  uint64_t package_count;
  uint64_t published_package_count;
  uint64_t published_bytes;
  uint64_t publish_fail_count;
  std::chrono::steady_clock::time_point last_publish_time;
  std::chrono::steady_clock::time_point replay_start_time;
};

class IResourceManager {
public:
  IResourceManager() = default;
  virtual void Initialize() = 0;
  virtual std::shared_ptr<next::player::BufferQueue> GetDataBuffer() = 0;
  virtual std::shared_ptr<UdexService> GetUdexService() = 0;
  virtual std::shared_ptr<replay_statistics> GetReplayStatistics() = 0;
  virtual std::shared_ptr<next::player::CycleManager> GetCycleManager() = 0;
  virtual void ResetStatistics() = 0;
  virtual void Shutdown() = 0;
};

#endif
