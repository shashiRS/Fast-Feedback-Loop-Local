/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     condition_handler.hpp
 * @brief    registers conditions and returns ids based on status code
 *
 **/

#ifndef NEXT_CONDITION_HANDLER_H
#define NEXT_CONDITION_HANDLER_H

#include <list>
#include <next/sdk/events/status_code_event.hpp>
#include <vector>
#include "condition.hpp"

namespace next {
namespace controlbridge {

class ConditionHandler {
public:
  void RegisterCondition(const next::controlbridge::Condition &cond);
  std::vector<next::controlbridge::Condition> GetConditions() const;
  void ClearConditions();
  std::vector<std::size_t> GetConditionsIdsForMessage(const next::sdk::events::StatusCodeMessage &msg);

private:
  std::vector<next::controlbridge::Condition> conditions_;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_CONDITION_HANDLER_H
