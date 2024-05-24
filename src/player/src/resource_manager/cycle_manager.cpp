/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_manager.cpp
 * @brief    Cycle Manager
 *
 **/

#include "cycle_manager.hpp"

namespace next {
namespace player {

CycleManager::CycleManager() : cycle_information_event_(std::string("PlayerCycleEventInformation")) {}

void CycleManager::CreateCycleEventPublisher(
    const std::vector<next::player::plugin::ReaderInterface::CycleInformation> &cycles) {
  cycles_ = cycles;
  next::sdk::events::CycleInformation cycle_information;
  for (const auto &elem : cycles_) {
    cycle_information.push_back({elem.device_name, elem.id});
    auto p = cycle_event_.emplace(elem.id, std::string("PlayerCycleEvent_") + elem.device_name);
    if (p.second == true) {
      if (!p.first->second.initialize()) {
      }
    }
  }
  cycle_information_event_.publish(cycle_information);
}

void CycleManager::PublishCycle(const size_t timestamp, const uint32_t cycle_id, const uint8_t cycle_state) {
  (void)timestamp;
  if (auto it = cycle_event_.find(cycle_id); cycle_event_.end() != it) {
    it->second.publish(next::sdk::events::Cycle(cycle_id, static_cast<next::sdk::events::CycleState>(cycle_state)));
  }
}

void CycleManager::Reset() {
  cycle_event_.clear();
  cycles_.clear();
  next::sdk::events::CycleInformation cycle_information;
  cycle_information_event_.publish(cycle_information);
}

} // namespace player
} // namespace next
