/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     status_code_manager_impl.hpp
 * @brief    collects status code events and handles actions of them
 *
 **/

#ifndef NEXT_STATUS_CODE_MANAGER_IMPL_H
#define NEXT_STATUS_CODE_MANAGER_IMPL_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include <spdlog/details/periodic_worker.h>

#include <next/sdk/event_base/event_types.h>
#include <next/bridgesdk/web_server.hpp>
#include <next/sdk/events/log_event.hpp>
#include <next/sdk/events/status_code_event.hpp>

#include "../status_queue_manager/status_queue_manager.hpp"
#include "status_code_evaluator.hpp"

namespace next {
namespace controlbridge {

//! This class to receive log message from LogEvents and combine them in one log
class StatusCodeManager {
public:
  StatusCodeManager(const std::string &component_name);
  virtual ~StatusCodeManager();

  //! startup and register everything needed
  bool Setup();
  bool Shutdown();

  void SetWebserver(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in);

  void CreateJsonMessagePayload(const next::sdk::events::StatusCodeMessage &status_event_data,
                                std::vector<UiAction> &actions, std::vector<uint8_t> &json_payload);

  void SendMessageWithWebserver(std::vector<uint8_t> json_payload);

private:
  next::controlbridge::StatusQueueManager status_queue_manager_;
  std::string component_name_{""};
  next::sdk::events::StatusCodeEvent status_code_event_{""};
  std::mutex mutex_collect_status_code_messages_;
  std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_{nullptr};

  enum class status { INIT, RUNNING };
  status status_code_manager_status_{status::INIT};

  std::vector<std::unique_ptr<StatusCodeEvaluatorInterface>> status_code_evaluators_;

  void InitEvaluators();

  void RegisterConditions();

  std::vector<UiAction> CheckForActions();
  std::vector<UiAction> FilterUniqueActions(const std::vector<UiAction> &actions);

  void CollectStatusCodeMessagesEventHook();
  void ProcessMessageToActions(const next::sdk::events::StatusCodeMessage &message, std::vector<UiAction> &ui_actions);
  std::vector<UiAction> GetUpdatedActions(std::vector<UiAction> &actions);
  void NotificationCallback(std::vector<uint8_t> *memory);

  std::vector<UiAction> actions_history_;

  friend class StatusCodeManagerTester;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_STATUS_CODE_MANAGER_IMPL_H
