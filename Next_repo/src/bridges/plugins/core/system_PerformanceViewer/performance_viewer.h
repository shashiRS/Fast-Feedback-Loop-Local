/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     performance_viewer.h
 * @brief    Performance Viewer Plugin
 *
 * This plugin gets the performance data from the simulation notes
        and forwards it as a JSON file.
 *
 **/

#ifndef NEXT_PLUGINS_PERFORMANCE_VIEWER_PLUGIN_H_
#define NEXT_PLUGINS_PERFORMANCE_VIEWER_PLUGIN_H_

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <json/json.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/profiler/basic_info_generated.h>
#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>

#include <next/plugins_utils/plugins_types.h>

#include <next/performanceviewer/next_cpu_profiler.h>

namespace next {
namespace plugins {

class PerformanceViewer : public next::bridgesdk::plugin::Plugin {
public:
  PerformanceViewer(const char *path);
  virtual ~PerformanceViewer();

  bool init() override;
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields);
  bool addConfig(const next::bridgesdk::plugin_config_t &config);
  bool removeConfig(const next::bridgesdk::plugin_config_t &config);

  bool enterReset() override;
  bool exitReset() override;

  //! Run the selected profiler
  /*!
   * Runs all needed functions for the selected Profiler
   */
  void runProfiler();

  /*!
   * @brief Get list of new eCal Processes
   *
   * @return std::vector<std::string> Name of all processes running in eCAL
   */
  std::vector<std::string> getNewECalProcesses();

  /*!
   * @brief Initiates CPU Profiler for all detected eCAL processes
   *
   * @param ecal_processes list of all detected eCAL processes
   * @return true All process profiler created
   * @return false Issue during creation of process profiler
   */
  bool initCpuProfiler(const std::vector<std::string> &ecal_processes);

  /*!
   * @brief starts the profiler in the background
   *
   */
  void enableBackgroundRunning();

  /*!
   * @brief stops the profiler
   *
   */
  void disableBackgroundRunning();

protected:
  flatbuffers::FlatBufferBuilder builder_;
  std::vector<flatbuffers::Offset<GuiSchema::CpuProfiler>> cpu_profiler_information_;
  std::vector<std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler>> available_cpu_profiler_;
  std::string path_;
  std::shared_ptr<std::thread> background_thread_;
  std::atomic_bool background_running_ = false;
  std::atomic_bool measure_threads_ = true;
  next::bridgesdk::plugin::SensorInfoForGUI sensor_info_;
  int update_rate_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};
  //! Creates Flatbuffer pointer to Profiler data
  /*!
   * uses the cpu_profiler_information_ vector that gets filled by updateCpuProfilerInformationVector
   *
   * @return returns Flatbuffer Package that can be used to share the data
   */
  bridgesdk::plugin::FlatbufferPackage createFlatbufferPointer();

  /*!
   * @brief Builds together the basic Information for each Process
   *
   * @param process shared pointer to Next Cpu Profiler Object. Values will be taken from this object
   * @return flatbuffers::Offset<profiler::BasicInformation> Offset from Basic Information for overall Flatbuffer
   */
  flatbuffers::Offset<profiler::BasicInformation>
  udpateBasicInformation(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process);

  /*!
   * @brief Builds together the CPU specific Information for each Process
   *
   * @param process shared pointer to Next Cpu Profiler Object. Values will be taken from this object
   * @return flatbuffers::Offset<profiler::CPU> Offset from CPU Information for overall Flatbuffer
   */
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::CpuUsage>>>
  updateCpuInformation(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process);

  /*!
   * @brief Builds together the Thread specific Information for each Process
   *
   * @param builder Flatbuffer builder to create content.
   * @param process shared pointer to Next Cpu Profiler Object. Values will be taken from this object
   * @return flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<profiler::Threads>>> Offset from Thread
   * Information for overall Flatbuffer
   */
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::Threads>>>
  updateThreadInformation(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process);

  /*!
   * @brief Updates the Profiler information for all found processes
   * Takes the data from the different found processes
   * Writes it in the matching FBS structure
   * Updates the Class variable cpu_profiler_information_
   *
   * @param process shared pointer to Next Cpu Profiler Object. Values will be taken from this object
   */
  void updateCpuProfilerInformationVector(std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process);

private:
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_PERFORMANCE_VIEWER_PLUGIN_H_
