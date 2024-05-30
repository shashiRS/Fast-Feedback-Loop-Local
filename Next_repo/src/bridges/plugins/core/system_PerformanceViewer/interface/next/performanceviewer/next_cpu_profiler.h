/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_cpu_profiler.h
 * @brief    Profiler in Next to get Windows CPU usage per prozess. Used by Performance Viewer Plugin.
 *
 * This profiler gets a name of a process as input and calculates
        the CPU Usage of this process
 *
 **/
#ifndef NEXT_PERFORMANCEVIEWER_NEXT_CPU_PROFILER_H_
#define NEXT_PERFORMANCEVIEWER_NEXT_CPU_PROFILER_H_

#include <memory>
#include <string>
#include <vector>

namespace next {
namespace plugins {
namespace performanceviewer {

struct threadInformation {
  unsigned long thread_id;
  unsigned long parent_id;
  float thread_cpu_usage;
};

class INextCpuProfiler {
public:
  INextCpuProfiler() = default;
  virtual ~INextCpuProfiler() = default;

  //! Init the Next CPU Profiler
  /*!
   * Initializing the name, ID , timestamps and current child threads of the process object
   * @return true/false on success/fail
   */
  virtual bool init(const std::string &processName) = 0;

  //! Return the process name
  /*!
   * @return std::string process_name
   */
  virtual std::string getProcessName() = 0;

  //! Returns the Process ID
  /*!
   * @return process_id
   */
  virtual unsigned long getProcessID() = 0;

  //! Returns the CPU Usage
  /*!
   * @return float cpu_usage
   */
  virtual float getCpuUsage() = 0;

  //! Returns the thread information for all child threads
  /*!
   * @return struct threadInformation (thread_id, parent_id, cpu_usage)
   */
  virtual std::vector<threadInformation> getThreadInformation() = 0;

  //! Calculates the CPU uasge based on the process and system times
  /*!
   * Updates the Timestamps of the Process and Kernel Times
   * Calculates the difference between the Timestamps
   * Calculates the Usage by dividing totalProcessTime / totalSystemTime
   * Sets cpu_usage_ to the calculated Value
   * Logs the CPU usage of the given process as DEBUG message
   * @return true/false on success/fail
   */
  virtual bool calcCpuUsage() = 0;
  virtual bool calcThreadCpuUsage() = 0;
}; // class NextCpuProfiler

std::shared_ptr<INextCpuProfiler> CreateNextCpuProfiler();

} // namespace performanceviewer
} // namespace plugins
} // namespace next

#endif // NEXT_PERFORMANCEVIEWER_NEXT_CPU_PROFILER_H_
