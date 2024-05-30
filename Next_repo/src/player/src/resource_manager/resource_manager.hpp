/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     resource_manager.hpp
 * @brief    Provides a set of common resources nedded by the player state machine
 *
 **/

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include "buffer_queue.hpp"
#include "resource_manager_interface.hpp"
#include "udex_service.hpp"

class ResourceManager : public IResourceManager, public std::enable_shared_from_this<ResourceManager> {
public:
  ResourceManager();
  virtual void Initialize() override;
  virtual std::shared_ptr<next::player::BufferQueue> GetDataBuffer() override;
  virtual std::shared_ptr<UdexService> GetUdexService() override;
  virtual std::shared_ptr<replay_statistics> GetReplayStatistics() override;
  virtual std::shared_ptr<next::player::CycleManager> GetCycleManager() override;
  virtual void ResetStatistics() override;
  virtual void Shutdown() override;

private:
  std::shared_ptr<next::player::BufferQueue> package_buffer_ptr_;
  std::shared_ptr<UdexService> udex_service_ptr_;
  std::shared_ptr<replay_statistics> replay_statistics_ptr_;
  std::shared_ptr<next::player::CycleManager> cycle_manager_ptr_;
};

#endif // RESOURCE_MANAGER_H_
