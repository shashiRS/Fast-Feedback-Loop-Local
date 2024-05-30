#include <next/control/orchestrator/flow_control_config.h>
#include <next/appsupport/next_status_code/next_status_code.hpp>

class TriggerStatusHandler {
public:
  using TriggerStatus = next::control::orchestrator::TriggerStatus;

  std::map<std::string, TriggerStatus> current_status_map_;

  void Reset() {
    current_status_map_.clear();
    status_storage_map_.clear();
  }

  void PublishStatusCurrentPackage() {
    for (const auto &[client_name, status] : current_status_map_) {
      auto status_code = GetStatus(client_name, status);

      switch (status_code) {
      case TriggerStatus::FAILURE:
        next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_PLAYER_TRIGGER_FINISHED,
                                                            "Trigger failure for client: " + client_name);
        break;
      case TriggerStatus::SUCCESS:
        next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_PLAYER_TRIGGER_FINISHED,
                                                            "Trigger successful for client: " + client_name);
        break;
      case TriggerStatus::EXEC_WARN:
        next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::WARN_PLAYER_TRIGGER_EXEC_WARN,
                                                            "Trigger warning occured for client: " + client_name);
        break;
      case TriggerStatus::NO_RESPONSE:
        next::appsupport::NextStatusCode::PublishStatusCode(
            next::appsupport::StatusCode::WARN_PLAYER_TRIGGER_NO_RECEIVER_FOUND,
            "Trigger empty for client: " + client_name);
        break;
      default:
        break;
      }
    }
    current_status_map_.clear(); // clear map for next package
  }

  TriggerStatus GetStatus(std::string client_name, TriggerStatus status) {
    auto &count_status = status_storage_map_[client_name][status];
    count_status++;

    switch (status) {
    case TriggerStatus::FAILURE:
      /*if error response received, publish always*/
      return status;
    case TriggerStatus::SUCCESS:
      // publish only the first occurence;
      if (count_status == 1) {
        return status;
      }
      break;
    case TriggerStatus::EXEC_WARN:
      if (count_status == 1) {
        return status;
      }
      break;
    case TriggerStatus::NO_RESPONSE:
      // publish only the first occurence
      if (count_status == 1) {
        return status;
      }
      break;
    case TriggerStatus::MISSING_DATA:
      /*not handled for now; do nothing*/
      break;
    case TriggerStatus::UNKNOWN:
      /*unknown trigger response, we do nothing here*/
      break;
    default:
      break;
    }

    return TriggerStatus::UNKNOWN;
  }

private:
  std::map<std::string, std::map<TriggerStatus, unsigned int>> status_storage_map_;
};
