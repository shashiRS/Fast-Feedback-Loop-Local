/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     status_queue_manager.hpp
 * @brief    sorts status codes in queues based on conditions
 *
 **/

#ifndef NEXT_STATUS_QUEUE_MANAGER_H
#define NEXT_STATUS_QUEUE_MANAGER_H

#include "condition_handler.hpp"

#include <unordered_map>

namespace next {
namespace controlbridge {

class StatusCodeQueue {
public:
  std::string name_;
  std::list<next::sdk::events::StatusCodeMessage> status_codes_;
  bool update_;
};

class StatusQueueManager {
public:
  StatusQueueManager() {}

  void RegisterCondition(next::controlbridge::Condition cond);

  std::vector<next::controlbridge::Condition> getConditions();

  bool ParseStatusCodeMsg(const next::sdk::events::StatusCodeMessage &msg);

  StatusCodeQueue getQueueById(const std::size_t &id);

private:
  std::unordered_map<std::size_t, StatusCodeQueue> StatusQueues_;
  ConditionHandler condition_handler_;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_STATUS_QUEUE_MANAGER_H
