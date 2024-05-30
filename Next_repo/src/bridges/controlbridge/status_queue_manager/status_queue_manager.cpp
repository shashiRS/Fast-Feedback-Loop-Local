#include "status_queue_manager.hpp"

namespace next {
namespace controlbridge {

void StatusQueueManager::RegisterCondition(next::controlbridge::Condition cond) {
  condition_handler_.RegisterCondition(cond);
}

std::vector<next::controlbridge::Condition> StatusQueueManager::getConditions() {
  return condition_handler_.GetConditions();
}

bool StatusQueueManager::ParseStatusCodeMsg(const next::sdk::events::StatusCodeMessage &msg) {
  auto ids = condition_handler_.GetConditionsIdsForMessage(msg);
  if (ids.size() == 0) {
    return false;
  }

  for (auto id : ids) {
    if (auto found_queue = StatusQueues_.find(id); found_queue == StatusQueues_.end()) {
      StatusQueues_[id] = StatusCodeQueue();
    }
    StatusQueues_[id].status_codes_.push_back(msg);
  }

  return true;
}

StatusCodeQueue StatusQueueManager::getQueueById(const std::size_t &id) {
  if (auto found_queue = StatusQueues_.find(id); found_queue != StatusQueues_.end()) {
    return found_queue->second;
  }

  return {};
}
} // namespace controlbridge
} // namespace next
