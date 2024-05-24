/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_cpu_profiler_impl.h
 * @brief    Profiler in Next to get CPU usage per prozess
 *
 * This profiler gets a name of a process as input and calculates
        the CPU Usage of this process on Windows
 *
 **/
#ifndef NEXT_CPU_PROFILER_IMPL_PLUGIN_H_
#define NEXT_CPU_PROFILER_IMPL_PLUGIN_H_

#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
// include Windows.h first
#include <Windows.h>
// then TlHelp32.h
#include <TlHelp32.h>

#include <next/performanceviewer/next_cpu_profiler.h>

namespace next {
namespace plugins {
namespace performanceviewer {

struct childThread {
  THREADENTRY32 thread_entry;
  FILETIME past_thread_kernel_time;
  FILETIME past_thread_user_time;
  FILETIME current_thread_kernel_time;
  FILETIME current_thread_user_time;
  float thread_cpu_usage;

  friend bool operator==(const childThread &lhs, const childThread &rhs) {
    return lhs.thread_entry.th32ThreadID == rhs.thread_entry.th32ThreadID;
  }
};

class NextCpuProfiler : public INextCpuProfiler {
public:
  // interface functions
  NextCpuProfiler();
  virtual ~NextCpuProfiler();
  bool init(const std::string &processName) override;
  std::string getProcessName() override;
  unsigned long getProcessID() override;
  bool calcCpuUsage() override;
  bool calcThreadCpuUsage() override;
  float getCpuUsage() override;
  std::vector<threadInformation> getThreadInformation() override;

protected:
  // internal functions
  void setProcessName(const std::string &processName);

  /*!
   * @brief Detect the processID of the process given by Name
   * Updates the value NextCpuProfiler::process_id
   * @return 0 on successfull execution, -1 on failed snapshot, -2 when failed to get first Process
   */
  int detectProcessID();

  /*!
   * @brief Detect all child threads of the process, adds them if not already available
   * updates the vector list of childThread objects
   * @return 0 on successfull execution, -1 on failed snapshot, -2 when failed to get first thread
   */
  int detectChildThreads();

  /*!
   * @brief    Calculates the CPU uasge based on the process and system times
   * Updates the value
   *		NextCpuProfiler::cpu_usage
   */
  void updateCpuUsage();

  /*!
   * @brief    Calculates the CPU uasge for all threads based on the thread and system times
   * Updates the value
   *		NextCpuProfiler::process_threads_.thread_cpu_usage
   */
  void updateThreadCpuUsage(struct childThread &thread);

  /*!
   * @brief    Updating Current Timestamps for comparison
   * Updates the values
   *		NextCpuProfiler::current_process_kernel_time_
   *		NextCpuProfiler::current_process_user_time_
   *		NextCpuProfiler::current_system_kernel_time_
   *		NextCpuProfiler::current_system_user_time_
   * @return 0 on successfull execution, -1 on failed OpenProcess
   */
  int updateCurrentTimestamps();

  /*!
   * @brief    Updating Current Timestamps for threads
   *
   * @return 0 on successfull execution, -1 on failed OpenThread
   */
  int updateCurrentThreadTimestamps(struct childThread &thread);

  /*!
   * @brief    Updating Past Timestamps for comparison
   * Updates the values
   *		NextCpuProfiler::past_process_kernel_time_
   *		NextCpuProfiler::past_process_user_time_
   *		NextCpuProfiler::past_system_kernel_time_
   *		NextCpuProfiler::past_system_user_time_
   */
  void updatePastTimestamps();

  /*!
   * @brief    Updating Past Timestamps for comparison
   */
  void updatePastThreadTimestamps(struct childThread &thread);

  /*!
   * @brief    Subtracts two FILETIME variables
   * @param[in]	currentTime		Minuend of the subtraction. Should be current Time to get positive difference
   * @param[in]	pastTime		Subtrahend of the subtraction. Should be past Time to get positive difference.
   * @return time difference between the input values
   */
  ULONGLONG subtractTimes(const FILETIME &currentTime, const FILETIME &pastTime);

  // internal variables
  std::string process_name_;
  unsigned long process_id_{0};
  float cpu_usage_{0.0f};

  std::vector<childThread> process_threads_;

  FILETIME past_process_kernel_time_{0, 0};
  FILETIME past_process_user_time_{0, 0};
  FILETIME current_process_kernel_time_{0, 0};
  FILETIME current_process_user_time_{0, 0};

  FILETIME past_system_kernel_time_{0, 0};
  FILETIME past_system_user_time_{0, 0};
  FILETIME current_system_kernel_time_{0, 0};
  FILETIME current_system_user_time_{0, 0};

}; // class NextCpuProfiler

} // namespace performanceviewer
} // namespace plugins
} // namespace next

#endif // NEXT_CPU_PROFILER_IMPL_PLUGIN_H_
