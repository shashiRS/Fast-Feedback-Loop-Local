/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     performance_viewer.cpp
 * @brief    Performance Viewer Plugin
 *
 * See performance_viewer.h
 *
 **/

#include "performance_viewer.h"

#include <time.h>
#include <chrono>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
// include Windows.h first
#include <Windows.h>
// then TlHelp32.h
#include <TlHelp32.h>

#include <next/sdk/sdk_macros.h>

#include <ecal/ecal.h>
NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
// disable warning of protobuf
// the code `if (sizeof(int32) == sizeof(long))` produces
//    warning C4127: conditional expression is constant
//    note: consider using 'if constexpr' statement instead
// TODO: enable warning C4127 once protobuf was updated
NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4127)
#include <ecal/core/pb/monitoring.pb.h>
NEXT_RESTORE_WARNINGS_WINDOWS
NEXT_RESTORE_WARNINGS_WINDOWS

#include <json/json.h>

#include <next/bridgesdk/schema/profiler/basic_info_generated.h>
#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>
#include <next/performanceviewer/next_cpu_profiler.h>
#include <next/plugins_utils/helper_functions.h>

namespace next {
namespace plugins {
using namespace next::plugins_utils;

PerformanceViewer::PerformanceViewer(const char *path) : next::bridgesdk::plugin::Plugin(path), builder_(1024) {
  debug(__FILE__, "Hello system_PerformanceViewer plugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("system_PerformanceViewer"));
  path_ = std::string(path);
}

PerformanceViewer::~PerformanceViewer() {
  if (background_running_ == true) {
    disableBackgroundRunning();
    background_thread_->join();
  }
}

bool PerformanceViewer::init() {
  // TODO: Add profiler init here
  // TODO: remove once widget is implemented
  sensor_info_.stream = "profiler";
  update_rate_ = 1000;

  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  return true;
}

bool PerformanceViewer::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"Update Rate", {next::bridgesdk::ConfigType::EDITABLE, "1000"}});
  config_fields.insert({"Measure Threads", {next::bridgesdk::ConfigType::SELECTABLE, "on"}});
  config_fields.insert({"Measure Threads", {next::bridgesdk::ConfigType::SELECTABLE, "off"}});
  return true;
}

bool PerformanceViewer::removeConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) {
  debug(__FILE__, "Stopping Performance Measurement");

  disableBackgroundRunning();
  if (background_thread_) {
    background_thread_->join();
    background_thread_.reset();
  }
  return true;
}

bool PerformanceViewer::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool PerformanceViewer::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {

  // add update rate
  std::string update_rate, measure_threads;

  config_ = config;

  if (!GetValueFromPluginConfig(config, "Update Rate", update_rate)) {
    error(__FILE__, " Performance Viewer missing config value: Update Rate");
    return false;
  }
  update_rate_ = stoi(update_rate);

  if (!GetValueFromPluginConfig(config, "DATA_VIEW", sensor_info_.stream)) {
    error(__FILE__, " Performance Viewer missing config value: DATA_VIEW");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "Measure Threads", measure_threads)) {
    error(__FILE__, " Performance Viewer missing config value: Measure Threads");
    return false;
  }
  if (measure_threads == "on") {
    measure_threads_ = true;
  } else if (measure_threads == "off") {
    measure_threads_ = false;
  } else {
    error(__FILE__, "Invalid Value for \"Measure Threads\"!");
    return false;
  }

  debug(__FILE__, "Starting Performance Measurement in Background");
  enableBackgroundRunning();
  if (!background_thread_) {
    background_thread_ = std::make_shared<std::thread>(std::bind(&PerformanceViewer::runProfiler, this));
  }

  return true;
}

bool PerformanceViewer::enterReset() {
  // so far we are doing nothing here
  return true;
}

bool PerformanceViewer::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

void PerformanceViewer::enableBackgroundRunning() { this->background_running_ = true; }

void PerformanceViewer::disableBackgroundRunning() { this->background_running_ = false; }

flatbuffers::Offset<profiler::BasicInformation>
PerformanceViewer::udpateBasicInformation(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process) {
  auto name = builder_.CreateString(process->getProcessName());
  auto status = builder_.CreateString("Running");
  auto timestamp = GetTickCount64() * 1000 - (ULONGLONG)1000;
  auto location = builder_.CreateString("C:");

  return profiler::CreateBasicInformation(builder_, name, status, timestamp, location);
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::CpuUsage>>>
PerformanceViewer::updateCpuInformation(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process) {
  std::vector<flatbuffers::Offset<GuiSchema::CpuUsage>> cpu_usage_all_cores;
  const int FACTOR_FOR_PERCENT = 100;
  uint8_t core = 1u;
  float usage = process->getCpuUsage() * FACTOR_FOR_PERCENT;

  auto cpu_usage_single_core = GuiSchema::CreateCpuUsage(builder_, core, usage);
  cpu_usage_all_cores.push_back(cpu_usage_single_core);

  return builder_.CreateVector(cpu_usage_all_cores);
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::Threads>>>
PerformanceViewer::updateThreadInformation(
    std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process) {
  std::vector<flatbuffers::Offset<GuiSchema::Threads>> threads_vector;
  auto thread_info = process->getThreadInformation();

  for (const struct performanceviewer::threadInformation &thread : thread_info) {
    unsigned long tid = thread.thread_id;
    uint8_t tcore = 1;
    float tusage = thread.thread_cpu_usage;

    auto tmp_thread = GuiSchema::CreateThreads(builder_, tid, tcore, tusage);
    threads_vector.push_back(tmp_thread);
  }

  return builder_.CreateVector(threads_vector);
}

void PerformanceViewer::updateCpuProfilerInformationVector(
    std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process) {
  auto basic_information = udpateBasicInformation(process);
  auto cpu_information = updateCpuInformation(process);
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::Threads>>> thread_information;
  if (measure_threads_) {
    thread_information = updateThreadInformation(process);
  }
  int pid = process->getProcessID();
  // the output expects uint8_t - max count would be 255
  uint8_t thread_count{0};
  if (size_t tmp = process->getThreadInformation().size(); tmp <= std::numeric_limits<decltype(thread_count)>::max()) {
    thread_count = static_cast<decltype(thread_count)>(tmp);
  } else {
    warn(__FILE__, " too many threads for used data type, sending maximum limit");
    thread_count = std::numeric_limits<decltype(thread_count)>::max();
  }

  auto cpu_profiler =
      GuiSchema::CreateCpuProfiler(builder_, basic_information, pid, cpu_information, thread_information, thread_count);
  cpu_profiler_information_.push_back(cpu_profiler);
}

bridgesdk::plugin::FlatbufferPackage PerformanceViewer::createFlatbufferPointer() {
  auto processes = builder_.CreateVector(cpu_profiler_information_);
  auto cpu_profiler_list = GuiSchema::CreateCpuProfilerList(builder_, processes);
  builder_.Finish(cpu_profiler_list);
  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder_.GetBufferPointer();
  flatbuffer.fbs_size = builder_.GetSize();

  return flatbuffer;
}

std::vector<std::string> PerformanceViewer::getNewECalProcesses() {
  NEXT_DISABLE_DEPRECATED_WARNING
  eCAL::pb::Monitoring ecal_monitor;
  std::string monitoring_s;
  std::vector<std::string> process_names;
  bool process_is_available = false;

  // get all current processes
  eCAL::Monitoring::GetMonitoring(monitoring_s);
  ecal_monitor.ParseFromString(monitoring_s);

  for (auto process : ecal_monitor.processes()) {
    process_is_available = false;

    // get only the name without path
    std::size_t found = process.pname().find_last_of("\\");
    std::string process_name = process.pname().substr(found + 1);

    // check if they are already existing
    for (auto cpu_profiler : available_cpu_profiler_) {
      if (cpu_profiler->getProcessName() == process_name) {
        process_is_available = true;
        break;
      }
    }

    if (!process_is_available) {
      // add to return variable
      process_names.push_back(process_name);
    }
  }
  NEXT_RESTORE_WARNINGS
  return process_names;
}

bool PerformanceViewer::initCpuProfiler(const std::vector<std::string> &ecal_processes) {
  for (auto &process_name : ecal_processes) {
    std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> next_cpu_profiler =
        next::plugins::performanceviewer::CreateNextCpuProfiler();
    if (next_cpu_profiler->init(process_name)) {
      available_cpu_profiler_.push_back(next_cpu_profiler);
    } else {
      return false;
    }
  }
  return true;
}

void PerformanceViewer::runProfiler() {
  builder_.ForceDefaults(true);
  std::string path_to_fbs = path_ + "/cpu_profiler.fbs";
  std::pair<std::string, std::string> structure_version_pair = {"CPU_Profiler", "0.0.1"};
  auto correct_json_lambda = [](Json::Value message_json) -> std::optional<Json::Value> { return message_json; };

  while (background_running_) {
    std::vector<std::string> new_ecal_processes = getNewECalProcesses();
    if (!new_ecal_processes.empty()) {
      initCpuProfiler(new_ecal_processes);
    }
    for (auto cpu_profiler : available_cpu_profiler_) {
      if (measure_threads_) {
        if (cpu_profiler->calcCpuUsage() && cpu_profiler->calcThreadCpuUsage()) {
          updateCpuProfilerInformationVector(cpu_profiler);
        } else {
          warn(__FILE__, "Issue while calculating usage for process: {0}", cpu_profiler->getProcessName());
        }
      } else {
        if (cpu_profiler->calcCpuUsage()) {
          updateCpuProfilerInformationVector(cpu_profiler);
        } else {
          warn(__FILE__, "Issue while calculating usage for process: {0}", cpu_profiler->getProcessName());
        }
      }
    }
    auto flatbuffer = createFlatbufferPointer();

    bridgesdk::plugin::SensorInfoForFbs sensor_fbs_info;
    sensor_fbs_info.data_view = sensor_info_.stream;
    plugin_publisher_->sendFlatbufferData(path_to_fbs, flatbuffer, sensor_fbs_info, structure_version_pair);
    cpu_profiler_information_.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(update_rate_));
  }
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::PerformanceViewer(path); }
