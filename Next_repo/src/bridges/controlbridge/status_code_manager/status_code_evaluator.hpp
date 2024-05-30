/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     status_code_evaluator.hpp
 * @brief    Evaluate a queue of status messages and return actions which should be done
 *
 **/
#ifndef NEXT_STATUS_CODE_EVALUATOR_HPP
#define NEXT_STATUS_CODE_EVALUATOR_HPP

#include <next/appsupport/next_status_code/next_status_code.hpp>
#include "../status_queue_manager/status_queue_manager.hpp"

namespace next {
namespace controlbridge {

enum class UiActionType { PlayWarning = 1, ExporterWarning = 2, VisualNotification = 3 };

const std::map<UiActionType, std::string> UiActionTypeToString = {
    {UiActionType::PlayWarning, "PlayWarning"},
    {UiActionType::ExporterWarning, "ExporterWarning"},
    {UiActionType::VisualNotification, "VisualNotification"}};

struct UiAction {
  UiActionType type;
  bool is_requested;

public:
  std::string getUiActionTypeString() {
    if (UiActionTypeToString.find(type) != UiActionTypeToString.end()) {
      return UiActionTypeToString.at(type);
    }
    return "NoActionToStringMapping";
  }
};

class StatusCodeEvaluatorInterface {
public:
  virtual void Init() = 0; // in the fucture read out config ors stuff
  virtual Condition ProvideCondition() = 0;
  virtual void CheckQueue(const next::controlbridge::StatusCodeQueue &queue) = 0;
  virtual std::vector<UiAction> GetActions() = 0;
};

class CheckExporterHasWarning : public StatusCodeEvaluatorInterface {
public:
  void Init() override { return; }

  Condition ProvideCondition() override {
    return {{},
            {(int)next::appsupport::StatusCode::SUCC_FULL_SUBSCRIPTION,
             (int)next::appsupport::StatusCode::SUCC_EXPORTER_STOPPED,
             (int)next::appsupport::StatusCode::WARN_EXPORTER_PARTIAL_SUBSCRIPTION,
             (int)next::appsupport::StatusCode::ERR_EXPORTER_TRIGGER_NOT_FOUND,
             (int)next::appsupport::StatusCode::ERR_EXPORTER_FILE_FORMAT_NOT_SUPPORTED,
             (int)next::appsupport::StatusCode::ERR_EXPORTER_CONFIGURATION_NOT_SUPPORTED,
             (int)next::appsupport::StatusCode::ERR_EXPORTER_NO_SUBSCRIPTION},
            {},
            {}};
  }

  void CheckQueue(const next::controlbridge::StatusCodeQueue &queue) override {
    std::map<std::string, bool> component_status_list;

    for (const auto &current_status : queue.status_codes_) {
      auto &comp_name = current_status.component_name;
      int severity = (int)current_status.severity;
      component_status_list[comp_name] = (severity == 3) || (severity == 4);
    }

    bool warning_or_error = false;
    for (auto &comp_status : component_status_list) {
      warning_or_error |= comp_status.second;
    }
    if (warning_or_error) {
      actions_ = {{UiActionType::ExporterWarning, true}};
      return;
    }
    actions_ = {{UiActionType::ExporterWarning, false}};
  }

  std::vector<UiAction> GetActions() override { return actions_; }

private:
  std::vector<UiAction> actions_;
};

class CheckAnySimulationNodeHasWarning : public StatusCodeEvaluatorInterface {
public:
  void Init() override { return; }

  Condition ProvideCondition() override {
    return {{},
            {(int)next::appsupport::StatusCode::SUCC_COMP_CONFIG_FINISHED,
             (int)next::appsupport::StatusCode::SUCC_COMP_RESET_FINISHED},
            {},
            {next::sdk::events::StatusOrigin::SIM_NODE}};
  }

  void CheckQueue(const next::controlbridge::StatusCodeQueue &queue) override {
    std::map<std::string, bool> component_status_list;

    for (const auto &current_status : queue.status_codes_) {
      auto &comp_name = current_status.component_name;
      int severity = (int)current_status.severity;
      int status_code = current_status.getStatusCode();
      component_status_list[comp_name] = (severity == 3) || (severity == 4) ||
                                         (status_code == (int)next::appsupport::StatusCode::SUCC_COMP_RESET_FINISHED);
    }

    bool warning_or_error = false;
    for (auto &comp_status : component_status_list) {
      warning_or_error |= comp_status.second;
    }
    if (warning_or_error) {
      actions_ = {{UiActionType::PlayWarning, true}};
      return;
    }
    actions_ = {{UiActionType::PlayWarning, false}};
  }

  std::vector<UiAction> GetActions() override { return actions_; }

private:
  std::vector<UiAction> actions_;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_STATUS_CODE_EVALUATOR_HPP
