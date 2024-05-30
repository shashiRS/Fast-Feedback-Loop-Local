/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     status_code_manager_impl.cpp
 * @brief    collects status code events and handles actions of them
 **/

#include "status_code_manager.hpp"
#include <json/json.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace controlbridge {

StatusCodeManager::StatusCodeManager(const std::string &component_name) : component_name_(component_name) {}

StatusCodeManager::~StatusCodeManager() { Shutdown(); }

bool StatusCodeManager::Setup() {
  if (status::RUNNING == status_code_manager_status_) {
    error(__FILE__, "Setup already running without calling shutdown");
    return false;
  }
  status_code_manager_status_ = status::RUNNING;

  auto event_hook = std::bind(&StatusCodeManager::CollectStatusCodeMessagesEventHook, this);
  status_code_event_.subscribe();
  status_code_event_.addEventHook(event_hook);
  info(__FILE__, "Status Code Manager started succesfully");

  InitEvaluators();

  return true;
}

bool StatusCodeManager::Shutdown() { return true; }

void StatusCodeManager::InitEvaluators() {
  status_code_evaluators_.push_back(std::make_unique<CheckAnySimulationNodeHasWarning>());
  status_code_evaluators_.push_back(std::make_unique<CheckExporterHasWarning>());

  RegisterConditions();
}

void StatusCodeManager::SetWebserver(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in) {
  webserver_ = webserver_in;
}

void StatusCodeManager::RegisterConditions() {
  for (auto &evaluator : status_code_evaluators_) {
    auto condition = evaluator->ProvideCondition();
    status_queue_manager_.RegisterCondition(condition);
  }
}

std::vector<UiAction> StatusCodeManager::CheckForActions() {
  std::vector<UiAction> ret_actions;
  for (auto &evaluator : status_code_evaluators_) {
    auto queue = status_queue_manager_.getQueueById(evaluator->ProvideCondition().GetId());
    if (queue.status_codes_.size() > 0) {
      evaluator->CheckQueue(queue);
      auto actions = evaluator->GetActions();
      std::copy(actions.begin(), actions.end(), std::back_inserter(ret_actions));
    }
  }
  return ret_actions;
}

std::vector<UiAction> StatusCodeManager::FilterUniqueActions(const std::vector<UiAction> &actions) {
  std::vector<UiAction> unique_actions;
  for (const auto &action : actions) {
    auto it = std::find_if(unique_actions.begin(), unique_actions.end(),
                           [action](const UiAction &uniqueAction) { return uniqueAction.type == action.type; });

    if (it == unique_actions.end()) {
      unique_actions.push_back({action.type, action.is_requested});
    } else {
      it->is_requested |= action.is_requested;
    }
  }

  return unique_actions;
}

std::vector<UiAction> StatusCodeManager::GetUpdatedActions(std::vector<UiAction> &actions) {
  std::vector<UiAction> updated_actions;
  for (const auto &action : actions) {
    auto it = std::find_if(actions_history_.begin(), actions_history_.end(),
                           [action](const UiAction &uniqueAction) { return uniqueAction.type == action.type; });

    if (it == actions_history_.end()) {
      actions_history_.push_back({action.type, action.is_requested});
      updated_actions.push_back({action.type, action.is_requested});
    } else {
      if (it->is_requested != action.is_requested) {
        it->is_requested = action.is_requested;
        updated_actions.push_back({action.type, action.is_requested});
      }
    }
  }

  return updated_actions;
}

void StatusCodeManager::ProcessMessageToActions(const next::sdk::events::StatusCodeMessage &message,
                                                std::vector<UiAction> &ui_actions) {
  bool condition_fitting = status_queue_manager_.ParseStatusCodeMsg(message);
  if (condition_fitting) {
    auto actions = CheckForActions();
    auto unique_actions = FilterUniqueActions(actions);
    ui_actions = GetUpdatedActions(unique_actions);
  }
}

void StatusCodeManager::CollectStatusCodeMessagesEventHook() {
  next::sdk::events::StatusCodeMessage message = status_code_event_.getEventData();
  std::lock_guard<std::mutex> lock(mutex_collect_status_code_messages_);
  std::vector<UiAction> ui_actions;

  ProcessMessageToActions(message, ui_actions);

  std::vector<uint8_t> json_payload;
  CreateJsonMessagePayload(message, ui_actions, json_payload);

  SendMessageWithWebserver(json_payload);
}

void StatusCodeManager::CreateJsonMessagePayload(const next::sdk::events::StatusCodeMessage &status_event,
                                                 std::vector<UiAction> &actions, std::vector<uint8_t> &json_payload) {
  Json::Value root;
  root["channel"] = "console";
  root["source"] = "NextBridge";
  root["event"] = "backendNotification";

  Json::Value status_event_json;
  status_event_json["status_code"] = status_event.toInt();
  status_event_json["severity"] = status_event.getSeverityAsString();
  status_event_json["status_origin"] = next::sdk::events::StatusCodeEvent::getStatusOriginString(status_event);
  status_event_json["status_component_name"] = status_event.component_name;
  status_event_json["status_message"] = status_event.message;

  for (auto &action : actions) {
    Json::Value action_json;
    action_json["ui_action"] = action.getUiActionTypeString();
    action_json["is_requested"] = action.is_requested;

    status_event_json["ui_action_request"].append(action_json);
  }

  root["payload"]["StatusEventData"].append(status_event_json);

  Json::FastWriter writer;
  std::string response = writer.write(root);

  json_payload.insert(json_payload.end(), response.begin(), response.end());
}

void StatusCodeManager::NotificationCallback(std::vector<uint8_t> *memory) {
  if (memory) {
    delete memory;
  }
}

void StatusCodeManager::SendMessageWithWebserver(std::vector<uint8_t> json_payload) {
  if (!webserver_) {
    error(__FILE__, "Webserver not initialized, event dropped");
    return;
  }

  std::vector<uint8_t> *json_payload_ptr = new std::vector<uint8_t>();
  json_payload_ptr->assign(json_payload.begin(), json_payload.end());
  webserver_->BroadcastMessageWithNotification(
      *json_payload_ptr, 0, {}, std::bind(&StatusCodeManager::NotificationCallback, this, json_payload_ptr));
}

} // namespace controlbridge
} // namespace next
