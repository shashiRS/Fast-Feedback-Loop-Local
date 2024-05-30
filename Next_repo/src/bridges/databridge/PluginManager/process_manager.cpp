/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_process_watcher.cpp
 *  @brief    see plugin_process_watcher.h
 */
#include <future>

#include "process_manager.h"

#include <next/appsupport/next_status_code/next_status_code.hpp>

namespace next {
namespace databridge {
namespace plugin_util {

constexpr const std::chrono::seconds kReportingTime = std::chrono::seconds(10);

using namespace std::chrono_literals;

ProcessManager::ProcessManager() {
  handle_fut_ = std::async(std::launch::async, &ProcessManager::handleDeferredCall, this);
}

ProcessManager::~ProcessManager() { stop_thread_ = true; }

void ProcessManager::handleDeferredCall() {
  while (!stop_thread_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    std::lock_guard<std::mutex> lck{call_protection_};

    bool process_running = false;

    auto it_process = process_call_properties_.begin();
    while (it_process != process_call_properties_.end()) {

      // check if function calls are in the queue
      if (it_process->second.size() > 0) {
        // check if new function calls are in the queue
        if (process_manager_state_ == RunningState::Idle) {
          process_manager_state_ = RunningState::Processing;
          debug(__FILE__, "Function calls in queue.");
        }
        process_running = true;
      }

      // Checking in the list of queued function calls of a process if any has finished and return iterator to that
      auto it_property_finished_process =
          find_if(it_process->second.begin(), it_process->second.end(), [&it_process](ProcessProperties &elm) {
            // send out log message with elapsed time of running function call
            if (elm.is_running_) {
              auto current_time = std::chrono::steady_clock::now();
              std::chrono::duration<float> elapsed_seconds_last_report = current_time - elm.last_reporting_time_;
              if (elapsed_seconds_last_report > kReportingTime) {
                std::chrono::duration<float> runtime_function_call = current_time - elm.start_time_;

                debug(__FILE__, "... function call of '{0}' is running since {1:.0f}s", it_process->first,
                      runtime_function_call.count());

                elm.last_reporting_time_ = current_time;
              }
            }
            // check if function call is finished
            return (elm.is_running_ && (elm.thread_future_.wait_for(0s) == std::future_status::ready));
          });

      auto call_deferred_function = [this, it_process]() {
        auto new_process_property = it_process->second.begin();
        new_process_property->thread_future_ = std::async(
            std::launch::async, [new_process_property]() -> void { new_process_property->invoke_function_call_(); });
        new_process_property->start_time_ = std::chrono::steady_clock::now();
        new_process_property->last_reporting_time_ = new_process_property->start_time_;
        new_process_property->is_running_ = true;
      };

      if (it_property_finished_process != std::end(it_process->second)) {
        std::chrono::duration<float> runtime_function_call =
            std::chrono::steady_clock::now() - it_property_finished_process->start_time_;
        debug(__FILE__, "Function call of '{0}' finished in less than {1:.2f}s", it_process->first,
              runtime_function_call.count());

        // Remove already finished process from list.
        it_process->second.erase(it_property_finished_process);

        if (it_process->second.size() != 0) {
          call_deferred_function();
        }
      }
      // remove entries which have no running processes anymore
      if (it_process->second.size() == 0) {
        it_process = process_call_properties_.erase(it_process);
      } else {
        ++it_process;
      }
    }

    if (!process_running && RunningState::Processing == process_manager_state_) {
      process_manager_state_ = RunningState::Idle;
      debug(__FILE__, "Finished processing of all function calls.");
    }
  }
}

bool ProcessManager::invokeCall(std::function<void(void)> f_call, const std::string &process_name) {

  std::lock_guard<std::mutex> lck{call_protection_};
  auto it_process = process_call_properties_.find(process_name);
  std::future<void> result_fut{};

  if (it_process == std::end(process_call_properties_) || (it_process->second.size() == 0)) {
    callAndRemember(f_call, process_name);
  } else {
    auto current_time = std::chrono::steady_clock::now();
    it_process->second.push_back({std::move(result_fut), current_time, current_time, false, f_call});
  }

  return true;
}

void ProcessManager::callAndRemember(std::function<void(void)> f_call, const std::string &process_name) {

  std::future<void> result_fut{};
  result_fut = std::async(std::launch::async, [=]() -> void { f_call(); });
  std::vector<ProcessProperties> tmp_process;
  auto current_time = std::chrono::steady_clock::now();
  tmp_process.push_back({std::move(result_fut), current_time, current_time, true, {}});
  process_call_properties_[process_name] = std::move(tmp_process);
}

bool ProcessManager::isProcessActive(const std::string &process_name) {
  std::lock_guard<std::mutex> lck{call_protection_};
  const auto it_process = process_call_properties_.find(process_name);
  // check if process is added to the processing queue
  if (it_process == std::end(process_call_properties_)) {
    return false;
  }
  // check if process has function calls in the queue
  if (it_process->second.size() == 0) {
    return false;
  } else {
    return true;
  }
}

} // namespace plugin_util
} // namespace databridge
} // namespace next
