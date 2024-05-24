/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     resource_manager.cpp
 * @brief    Provides a set of common resources nedded by the player state machine
 *
 **/

#include "resource_manager.hpp"

ResourceManager::ResourceManager()
    : package_buffer_ptr_(new next::player::BufferQueue()), udex_service_ptr_(new UdexService()),
      replay_statistics_ptr_(new replay_statistics()), cycle_manager_ptr_(new next::player::CycleManager()) {}

std::shared_ptr<next::player::BufferQueue> ResourceManager::GetDataBuffer() { return package_buffer_ptr_; }
std::shared_ptr<UdexService> ResourceManager::GetUdexService() { return udex_service_ptr_; }

void ResourceManager::Initialize() {
  udex_service_ptr_->Initialize();
  package_buffer_ptr_->clearBuffer();
  package_buffer_ptr_->startPush();
  ResetStatistics();
}

// void ResourceManager::Reset() { replay_statistics_ptr_->resetValues(); }
std::shared_ptr<replay_statistics> ResourceManager::GetReplayStatistics() { return replay_statistics_ptr_; }

std::shared_ptr<next::player::CycleManager> ResourceManager::GetCycleManager() { return cycle_manager_ptr_; }

void ResourceManager::ResetStatistics() { replay_statistics_ptr_->resetValues(); }
void ResourceManager::Shutdown() {
  package_buffer_ptr_->shutdown();
  cycle_manager_ptr_->Reset();
}
