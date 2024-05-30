#include "condition.hpp"

namespace next {
namespace controlbridge {

bool Condition::Check(const next::sdk::events::StatusCodeMessage &msg) const {
  bool res = false;

  if ((std::find(component_name_.begin(), component_name_.end(), msg.component_name) != component_name_.end()) &&
      msg.component_name != "") {

    res |= true;
  }

  int code = msg.getStatusCode();
  if ((std::find(status_codes_.begin(), status_codes_.end(), code) != status_codes_.end()) && code > 0) {
    res |= true;
  }

  if ((std::find(origins_.begin(), origins_.end(), msg.origin) != origins_.end())) {
    res |= true;
  }

  next::sdk::events::StatusSeverity status_severity = msg.severity;
  if ((std::find(severities_.begin(), severities_.end(), status_severity) != severities_.end())) {
    res |= true;
  }

  return res;
}

std::size_t Condition::GetId() { return unique_id_; }

} // namespace controlbridge
} // namespace next
