/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     resource_manager.hpp
 * @brief    Provides a set of common resources nedded by the player state machine
 *
 **/

#ifndef RESOURCE_MANAGER_
#define RESOURCE_MANAGER_

// include the udex service moc first, it overwrites the real udex service which is included by
// resource_manager_interface.hpp
#include "udex_service_moc.hpp"

#include "./resource_manager/buffer_queue.hpp"
#include "./resource_manager/resource_manager_interface.hpp"

class ResourceManagerMoc : public IResourceManager {
public:
  ResourceManagerMoc();
  void Initialize();
  std::shared_ptr<next::player::BufferQueue> GetDataBuffer();
  std::shared_ptr<UdexService> GetUdexService();
  std::shared_ptr<replay_statistics> GetReplayStatistics();
  void ResetStatistics();
  void Shutdown();

private:
  std::shared_ptr<next::player::BufferQueue> package_buffer_ptr_;
  std::shared_ptr<UdexService> udex_service_ptr_;
  std::shared_ptr<replay_statistics> replay_statistics_ptr_;
};
#endif
