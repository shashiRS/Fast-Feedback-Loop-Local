/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_handler.h
 *  @brief    stub
 */

#ifndef NEXT_DATABRIDGE_PLUGIN_PROCESS_WATCHER_H_
#define NEXT_DATABRIDGE_PLUGIN_PROCESS_WATCHER_H_

#include <future>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/plugin.h>

namespace next {
namespace databridge {
namespace plugin_util {

struct ProcessProperties {
  std::future<void> thread_future_{};
  std::chrono::time_point<std::chrono::steady_clock> start_time_{};
  std::chrono::time_point<std::chrono::steady_clock> last_reporting_time_{};
  bool is_running_{false};
  std::function<void(void)> invoke_function_call_{};
};

class ProcessManager {
public:
  ProcessManager();
  virtual ~ProcessManager();
  ProcessManager(ProcessManager &) = default;
  ProcessManager(ProcessManager &&) = default;
  bool invokeCall(std::function<void(void)> call, const std::string &process_name);
  bool isProcessActive(const std::string &process_name);

private:
  void handleDeferredCall();
  void callAndRemember(std::function<void(void)> call, const std::string &process_name);

  // map from process name to the list of function calls
  std::unordered_map<std::string, std::vector<ProcessProperties>> process_call_properties_{};
  std::mutex call_protection_{};
  std::atomic<bool> stop_thread_{false};
  std::future<void> handle_fut_{};

  enum class RunningState { Idle, Processing };
  RunningState process_manager_state_{RunningState::Idle};
};

} // namespace plugin_util
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_PLUGIN_PROCESS_WATCHER_H_
