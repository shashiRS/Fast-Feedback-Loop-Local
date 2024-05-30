/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     performance_viewer_test.cpp
 *  @brief    Unit tests for next_cpu_profiler
 */

#include <cip_test_support/gtest_reporting.h>

#include <chrono>
#include <thread>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>
#include <ecal/msg/string/subscriber.h>
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
// disable warning of protobuf
// the code `if (sizeof(int32) == sizeof(long))` produces
//    warning C4127: conditional expression is constant
//    note: consider using 'if constexpr' statement instead
// TODO: enable warning C4127 once protobuf was updated
#pragma warning(disable : 4127)
#endif
#include <ecal/core/pb/monitoring.pb.h>
#if _MSC_VER
#pragma warning(pop)
#endif

#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>
#include <next/performanceviewer/next_cpu_profiler.h>

#include "next_cpu_profiler_impl.h"
#include "performance_viewer.h"

namespace next {
namespace plugins {

class TestCpuProfiler : public performanceviewer::NextCpuProfiler {
public:
  void setDummyProcessName(std::string &process_name) { process_name_ = process_name; }
  void setDummyProcessID(int &process_id) { process_id_ = process_id; }
  void setDummyCpuUsage(float &cpu_usage) { cpu_usage_ = cpu_usage; }
  void setDummyThreads(const std::vector<performanceviewer::childThread> &threads) {
    for (const struct performanceviewer::childThread &thread : threads) {
      process_threads_.push_back(thread);
    }
  }
};

class PerformanceViewerTest : public PerformanceViewer {
public:
  using PerformanceViewer::PerformanceViewer;
  std::string getPath() { return path_; }
  void setBackgorundRunning(bool running) { background_running_ = running; }
  bool getBackgroundRunning() { return background_running_; }
  void enableBackgroundRunning() { return PerformanceViewer::enableBackgroundRunning(); }
  void disableBackgroundRunning() { return PerformanceViewer::disableBackgroundRunning(); }
  void sendFlatbufferData(const std::string &path_to_fbs, const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                          const std::function<std::optional<Json::Value>(Json::Value)> &,
                          const std::pair<std::string, std::string> &structure_version_pair) {
    bridgesdk::plugin::SensorInfoForFbs sensor_info;
    sensor_info.data_view = sensor_info_.stream;
    return plugin_publisher_->sendFlatbufferData(path_to_fbs, flatbuffer, sensor_info, structure_version_pair);
  }

  void updateCpuProfilerInformationVector(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process) {
    PerformanceViewer::updateCpuProfilerInformationVector(process);
  }

  bridgesdk::plugin::FlatbufferPackage createFlatbufferPointer() {
    return PerformanceViewer::createFlatbufferPointer();
  }

  std::vector<std::string> getNewECalProcesses() { return PerformanceViewer::getNewECalProcesses(); }

  std::vector<std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler>> getCpuProfiler() {
    return available_cpu_profiler_;
  }

  bool initCpuProfiler(std::vector<std::string> ecal_processes) {
    return PerformanceViewer::initCpuProfiler(ecal_processes);
  }
};

std::atomic<bool> send_ecal_messages = true;

// NEXT CPU all Process Profiler Tests

TEST(PerformanceViewerTestsuite, getECalProcessName) {

  if (!eCAL::IsInitialized()) {
    eCAL::Initialize({}, "TestThread1", eCAL::Init::All);
  }

  auto pub_thread = std::thread([] {
    eCAL::string::CPublisher<std::string> pub("demo_test1");
    std::string send_a = "test";
    eCAL::Util::EnableLoopback(true);

    while (send_ecal_messages) {
      pub.Send(send_a);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  PerformanceViewerTest performance_plugin("test");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::vector<std::string> found_processes = performance_plugin.getNewECalProcesses();
  auto it = find(found_processes.begin(), found_processes.end(), "system_PerformanceViewer_test.exe");
  if (it != found_processes.end()) {
    size_t index = it - found_processes.begin();
    EXPECT_EQ(found_processes[index], "system_PerformanceViewer_test.exe");
  } else {
    ADD_FAILURE();
  }
  send_ecal_messages = false;
  pub_thread.join();
  eCAL::Finalize();
}

TEST(PerformanceViewerTestsuite, runProfiler) {
  PerformanceViewerTest performance_plugin("test");
  EXPECT_NO_THROW(performance_plugin.runProfiler());
  eCAL::Finalize();
}

TEST(PerformanceViewerTestsuite, initCpuProfiler) {
  PerformanceViewerTest performance_plugin("test");
  std::vector<std::string> process_names;
  std::vector<std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler>> available_profiler;
  process_names.push_back("system_PerformanceViewer_test.exe");

  performance_plugin.initCpuProfiler(process_names);
  available_profiler = performance_plugin.getCpuProfiler();

  EXPECT_EQ(available_profiler.size(), 1);
}

TEST(PerformanceViewerTestsuite, enableBackgroundRunning) {
  PerformanceViewerTest performance_plugin("test");
  performance_plugin.setBackgorundRunning(false);
  performance_plugin.enableBackgroundRunning();
  EXPECT_TRUE(performance_plugin.getBackgroundRunning());
  performance_plugin.setBackgorundRunning(false);
}

TEST(PerformanceViewerTestsuite, disableBackgroundRunning) {
  PerformanceViewerTest performance_plugin("test");
  performance_plugin.setBackgorundRunning(true);
  performance_plugin.disableBackgroundRunning();
  EXPECT_FALSE(performance_plugin.getBackgroundRunning());
}

TEST(PerformanceViewerTestsuite, createBuilder) {
  PerformanceViewerTest performance_plugin("test");
  TestCpuProfiler test_profiler;

  std::string process_name = "test_name";
  int process_id = 150;
  float cpu_usage = 10.3f;
  std::vector<performanceviewer::childThread> test_threads;
  performanceviewer::childThread dummy_thread;
  dummy_thread.thread_cpu_usage = 20.5;
  test_threads.push_back(dummy_thread);

  test_profiler.setDummyProcessName(process_name);
  test_profiler.setDummyProcessID(process_id);
  test_profiler.setDummyCpuUsage(cpu_usage);
  test_profiler.setDummyThreads(test_threads);

  auto ptr_profiler = std::make_shared<performanceviewer::NextCpuProfiler>(test_profiler);

  performance_plugin.updateCpuProfilerInformationVector(ptr_profiler);

  auto flatbuffer = performance_plugin.createFlatbufferPointer();
  auto read_cpu_list_flatbuffer = GuiSchema::GetCpuProfilerList(flatbuffer.fbs_binary);
  auto read_cpu_flatbuffer_processes = read_cpu_list_flatbuffer->processes();
  auto read_cpu_flatbuffer = read_cpu_flatbuffer_processes->Get(0);

  auto read_basic_info = read_cpu_flatbuffer->basic();
  auto read_name = read_basic_info->name()->c_str();

  auto read_cpu_usage_list = read_cpu_flatbuffer->cpu();
  auto read_cpu_usage_first = read_cpu_usage_list->Get(0);
  auto read_cpu_usage = read_cpu_usage_first->usage();
  auto read_process_id = read_cpu_flatbuffer->pid();

  auto read_all_thread = read_cpu_flatbuffer->threads();
  auto read_first_thread = read_all_thread->Get(0);
  auto read_thread_usage = read_first_thread->tusage();

  EXPECT_EQ(read_name, process_name);
  EXPECT_EQ(read_cpu_usage, cpu_usage * 100);
  EXPECT_EQ(read_process_id, process_id);
  EXPECT_EQ(read_thread_usage, dummy_thread.thread_cpu_usage);
}

} // namespace plugins
} // namespace next
