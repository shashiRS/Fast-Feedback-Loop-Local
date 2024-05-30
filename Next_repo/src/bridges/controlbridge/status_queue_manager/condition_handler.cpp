#include "condition_handler.hpp"

namespace next {
namespace controlbridge {

void ConditionHandler::RegisterCondition(const next::controlbridge::Condition &cond) { conditions_.push_back(cond); }

std::vector<next::controlbridge::Condition> ConditionHandler::GetConditions() const { return conditions_; }

void ConditionHandler::ClearConditions() { conditions_.clear(); }

std::vector<std::size_t> ConditionHandler::GetConditionsIdsForMessage(const next::sdk::events::StatusCodeMessage &msg) {
  std::vector<std::size_t> ids;
  for (auto &condition : conditions_) {
    if (condition.Check(msg)) {
      ids.push_back(condition.GetId());
    }
  }

  return ids;
}

} // namespace controlbridge
} // namespace next
