#include "resource_manager_moc.hpp"

ResourceManagerMoc::ResourceManagerMoc()
    : package_buffer_ptr_(new next::player::BufferQueue()), udex_service_ptr_(new UdexService()),
      replay_statistics_ptr_(new replay_statistics()) {}

std::shared_ptr<next::player::BufferQueue> ResourceManagerMoc::GetDataBuffer() { return package_buffer_ptr_; }
std::shared_ptr<UdexService> ResourceManagerMoc::GetUdexService() { return udex_service_ptr_; }

void ResourceManagerMoc::Initialize() {
  udex_service_ptr_->initialize();
  package_buffer_ptr_->clearBuffer();
  package_buffer_ptr_->startPush();
  ResetStatistics();
}
// void ResourceManager::Reset() { replay_statistics_ptr_->resetValues(); }
std::shared_ptr<replay_statistics> ResourceManagerMoc::GetReplayStatistics() { return replay_statistics_ptr_; }
void ResourceManagerMoc::ResetStatistics() { replay_statistics_ptr_->resetValues(); }
void ResourceManagerMoc::Shutdown() { package_buffer_ptr_->shutdown(); }
