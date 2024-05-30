/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     flow_control_config.h
 * @brief    orchestrator for event handling for Next Player Control
 *
 **/

#ifndef NEXT_CONTROL_FLOW_CONTROL_CONFIG_H_
#define NEXT_CONTROL_FLOW_CONTROL_CONFIG_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <next/control/event_types/orchestrator_event_binary.h>

namespace next {
namespace control {
namespace orchestrator {

struct Meta {
  unsigned int delay{0};
  bool isSync{true};
};

enum class TriggerType { NONE, SIGNAL_TRIGGER, TIME_TRIGGER, CYCLE_START_TRIGGER, CYCLE_END_TRIGGER };

struct Trigger {

  TriggerType type = TriggerType::NONE;
  struct Value {
    int cycle_start{0};
    int cycle_end{0};
    std::string signal_name{""};
    uint64_t time{0};
    uint64_t last_trigger_time{0};
  };

  Value val{0, 0, "", 0};

  bool operator==(const Trigger &rhs) const {

    if (type == rhs.type) {
      switch (rhs.type) {
      case TriggerType::SIGNAL_TRIGGER:
        return rhs.val.signal_name == this->val.signal_name;
      case TriggerType::TIME_TRIGGER:
        return rhs.val.time == this->val.time;
      case TriggerType::CYCLE_START_TRIGGER:
        return rhs.val.cycle_start == this->val.cycle_start;
      case TriggerType::CYCLE_END_TRIGGER:
        return rhs.val.cycle_end == this->val.cycle_end;
      case TriggerType::NONE:
        return false;
      }
    }
    return false;
  }
};

struct Synch {
  std::string synch_timestamp_url{""};
  std::string synch_id_url{""};
};

struct FlowLeaf {
  bool operator==(const FlowLeaf &rhs) const {
    return (rhs.meta_data_.delay == meta_data_.delay && rhs.meta_data_.isSync == meta_data_.isSync &&
            rhs.trigger_ == trigger_ && rhs.dependency_leaves_ == dependency_leaves_ &&
            rhs.trigger_ports_ == trigger_ports_ && rhs.id == id);
  }

  std::string id = "";
  Meta meta_data_;
  Trigger trigger_;
  Synch synch_{"", ""};
  std::vector<FlowLeaf> dependency_leaves_;
  std::vector<std::pair<std::string, std::string>> trigger_ports_;
};

enum class OrchestratorErrors { SUCCESS = 0, EXEC_FAILED = 1, SYNC_FAILED = 2, FW_FAILED = 3, UNKNOWN_FAILED = 4 };

constexpr const char *OrchestratorErrorsToString(OrchestratorErrors e) throw() {
  switch (e) {
  case OrchestratorErrors::SUCCESS:
    return "SUCCESS";
  case OrchestratorErrors::EXEC_FAILED:
    return "EXECUTION FAILED";
  case OrchestratorErrors::SYNC_FAILED:
    return "SYNCHRONIZATION FAILED";
  case OrchestratorErrors::FW_FAILED:
    return "FRAMEWORK FAILURE";
  case OrchestratorErrors::UNKNOWN_FAILED:
    return "UNKNOWN FAILURE";
  default:
    return "UNKNOWN";
  }
}

using trigger_hook = std::function<void(OrchestratorErrors &, std::string &)>;

enum class TriggerStatus { SUCCESS = 0, NO_RESPONSE = 1, MISSING_DATA = 2, FAILURE = 3, EXEC_WARN = 4, UNKNOWN = 5 };

} // namespace orchestrator
} // namespace control
} // namespace next

#endif // NEXT_CONTROL_FLOW_CONTROL_CONFIG_H_
