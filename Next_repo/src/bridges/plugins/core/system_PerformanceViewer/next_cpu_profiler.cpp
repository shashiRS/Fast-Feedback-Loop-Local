/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_cpu_profiler.cpp
 * @brief    Profiler in Next for CPU profiling. Used by Performance Viewer Plugin
 *
 * See next_cpu_profiler.h
 *
 **/
#include <algorithm>

#include <json/json.h>

#include <next/performanceviewer/next_cpu_profiler.h>
#include <next/sdk/logger/logger.hpp>

#include "next_cpu_profiler_impl.h"

namespace next {
namespace plugins {
namespace performanceviewer {

std::shared_ptr<INextCpuProfiler> CreateNextCpuProfiler() { return std::make_shared<NextCpuProfiler>(); }

NextCpuProfiler::NextCpuProfiler() {}
NextCpuProfiler::~NextCpuProfiler(){};

//! Initializing the name, ID, timestamps and current child threads of the process object
bool NextCpuProfiler::init(const std::string &processName) {
  setProcessName(processName);
  if (detectProcessID() != 0) {
    error(__FILE__, "Error occured during detectProcessID!");
    return false;
  }

  if (detectChildThreads() != 0) {
    error(__FILE__, "Error occured during detectChildThreads!");
    return false;
  }
  GetSystemTimes(NULL, &current_process_kernel_time_, &current_process_user_time_);
  GetSystemTimes(NULL, &current_system_kernel_time_, &current_system_user_time_);
  return true;
}

void NextCpuProfiler::setProcessName(const std::string &processName) { process_name_ = processName; }

std::string NextCpuProfiler::getProcessName() { return process_name_; }

//! Detects the processID of the process given by Name
int NextCpuProfiler::detectProcessID() {
  // process_info will get the information from each process while going through the snaphot
  PROCESSENTRY32 process_info;
  process_info.dwSize = sizeof(PROCESSENTRY32);
  HANDLE processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (processes_snapshot == INVALID_HANDLE_VALUE) {
    error(__FILE__, "Processes Snapshot returned 'INVALID_HANDLE_VALUE'");
    return -1;
  }
  if (Process32First(processes_snapshot, &process_info) == false) {
    error(__FILE__, "Issue getting the first Process in the Snapshot!");
    return -2;
  }

  do {
    if (process_name_.compare(process_info.szExeFile) == 0) {
      process_id_ = process_info.th32ProcessID;
      break;
    }
  } while (Process32Next(processes_snapshot, &process_info));
  CloseHandle(processes_snapshot);
  return 0;
}

//! Detects all child threads of the process, adds them if not already available
int NextCpuProfiler::detectChildThreads() {
  THREADENTRY32 thread_info;
  thread_info.dwSize = sizeof(THREADENTRY32);
  // FILETIME dummy = {0};
  HANDLE thread_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

  if (thread_snapshot == INVALID_HANDLE_VALUE) {
    error(__FILE__, "Thread Snapshot returned 'INVALID_HANDLE_VALUE'");
    return -1;
  }
  if (Thread32First(thread_snapshot, &thread_info) == false) {
    error(__FILE__, "Issue getting the first Thread in the Snapshot!");
    return -2;
  }

  do {
    if (thread_info.th32OwnerProcessID == process_id_) {
      childThread thread = {thread_info, {0}};
      GetSystemTimes(NULL, &thread.past_thread_kernel_time, &thread.past_thread_user_time);
      GetSystemTimes(NULL, &thread.current_thread_kernel_time, &thread.current_thread_user_time);
      if (std::find(process_threads_.begin(), process_threads_.end(), thread) != process_threads_.end()) {
        // Thread already exists in vector
        continue;
      } else {
        process_threads_.push_back(thread);
      }
    }
  } while (Thread32Next(thread_snapshot, &thread_info));
  CloseHandle(thread_snapshot);
  return 0;
}

unsigned long NextCpuProfiler::getProcessID() { return process_id_; }

//! Setting past timestamps for comparison
void NextCpuProfiler::updatePastTimestamps() {
  // Write current system and process time into past system and process time
  past_process_kernel_time_ = current_process_kernel_time_;
  past_process_user_time_ = current_process_user_time_;
  past_system_kernel_time_ = current_system_kernel_time_;
  past_system_user_time_ = current_system_user_time_;
}

//! Setting past thread timestamps for comparison
void NextCpuProfiler::updatePastThreadTimestamps(childThread &thread) {
  thread.past_thread_kernel_time = thread.current_thread_kernel_time;
  thread.past_thread_user_time = thread.current_thread_user_time;
}

//! Updating current timestamps for comparison
int NextCpuProfiler::updateCurrentTimestamps() {
  HANDLE process_handle_by_id;
  FILETIME dummy_time1{0, 0};
  FILETIME dummy_time2{0, 0};

  process_handle_by_id = OpenProcess(PROCESS_ALL_ACCESS, false, this->process_id_);
  if (process_handle_by_id == NULL) {
    error(__FILE__, "Opening process with id {0} was not successul!", process_id_);
    return -1;
  }

  // Get current system and process time
  GetProcessTimes(process_handle_by_id, &dummy_time1, &dummy_time2, &current_process_kernel_time_,
                  &current_process_user_time_);
  GetSystemTimes(&dummy_time1, &current_system_kernel_time_, &current_system_user_time_);

  CloseHandle(process_handle_by_id);
  return 0;
}

//! Updating current thread timestamps for comparison
int NextCpuProfiler::updateCurrentThreadTimestamps(childThread &thread) {
  HANDLE thread_handle_by_id;
  FILETIME dummy_time1{0, 0};
  FILETIME dummy_time2{0, 0};

  thread_handle_by_id = OpenThread(THREAD_ALL_ACCESS, false, thread.thread_entry.th32ThreadID);
  if (thread_handle_by_id == NULL) {
    return -1;
  }

  // Get current system and process time
  GetThreadTimes(thread_handle_by_id, &dummy_time1, &dummy_time2, &thread.current_thread_kernel_time,
                 &thread.current_thread_user_time);
  GetSystemTimes(&dummy_time1, &current_system_kernel_time_, &current_system_user_time_);

  CloseHandle(thread_handle_by_id);
  return 0;
}

//! Subtracts two FILETIME variables
ULONGLONG NextCpuProfiler::subtractTimes(const FILETIME &currentTime, const FILETIME &pastTime) {
  LARGE_INTEGER current, past;
  current.LowPart = currentTime.dwLowDateTime;
  current.HighPart = currentTime.dwHighDateTime;

  past.LowPart = pastTime.dwLowDateTime;
  past.HighPart = pastTime.dwHighDateTime;

  return current.QuadPart - past.QuadPart;
}

//! Triggers timing updates and triggers CPU calculation afterwards
bool NextCpuProfiler::calcCpuUsage() {
  updatePastTimestamps();
  if (updateCurrentTimestamps() != 0) {
    error(__FILE__, "Error occured during updateCurrentTimestamp!");
    return false;
  };
  updateCpuUsage();
  return true;
}

//! Triggers thread timing updates and thread CPU calculation afterwards
bool NextCpuProfiler::calcThreadCpuUsage() {
  // see if new threads need to be added
  if (detectChildThreads() != 0) {
    error(__FILE__, "Error occured during detectChildThreads!");
    return false;
  }

  for (std::vector<childThread>::iterator it = process_threads_.begin(); it != process_threads_.end();) {
    updatePastThreadTimestamps(*it);
    if (updateCurrentThreadTimestamps(*it) != 0) {
      // Remove thread from vector
      it = process_threads_.erase(it);
    } else {
      updateThreadCpuUsage(*it);
      it++;
    }
  }
  return true;
}

//! Calculates the CPU usage based on the process and system times
void NextCpuProfiler::updateCpuUsage() {
  ULONGLONG total_process_kernel_time = subtractTimes(current_process_kernel_time_, past_process_kernel_time_);
  ULONGLONG total_process_user_time = subtractTimes(current_process_user_time_, past_process_user_time_);

  ULONGLONG total_system_kernel_time = subtractTimes(current_system_kernel_time_, past_system_kernel_time_);
  ULONGLONG total_system_user_time = subtractTimes(current_system_user_time_, past_system_user_time_);

  ULONGLONG total_process_time = total_process_kernel_time + total_process_user_time;
  ULONGLONG total_system_time = total_system_kernel_time + total_system_user_time;

  cpu_usage_ = static_cast<float>(total_process_time) / static_cast<float>(total_system_time);
}

//! Calculates the thread CPU usage based on the thread and system times
void NextCpuProfiler::updateThreadCpuUsage(childThread &thread) {
  ULONGLONG total_thread_kernel_time = subtractTimes(thread.current_thread_kernel_time, thread.past_thread_kernel_time);
  ULONGLONG total_thread_user_time = subtractTimes(thread.current_thread_user_time, thread.past_thread_user_time);

  ULONGLONG total_system_kernel_time = subtractTimes(current_system_kernel_time_, past_system_kernel_time_);
  ULONGLONG total_system_user_time = subtractTimes(current_system_user_time_, past_system_user_time_);

  ULONGLONG total_thread_time = total_thread_kernel_time + total_thread_user_time;
  ULONGLONG total_system_time = total_system_kernel_time + total_system_user_time;

  thread.thread_cpu_usage = static_cast<float>(total_thread_time) / static_cast<float>(total_system_time);
}

float NextCpuProfiler::getCpuUsage() { return cpu_usage_; }

//! Returns the needed information from all child threads
std::vector<threadInformation> NextCpuProfiler::getThreadInformation() {
  std::vector<threadInformation> all_threads_information;
  for (auto &thread : process_threads_) {
    threadInformation process_thread_info = {thread.thread_entry.th32ThreadID, thread.thread_entry.th32OwnerProcessID,
                                             thread.thread_cpu_usage};
    all_threads_information.push_back(process_thread_info);
  }
  return all_threads_information;
}
} // namespace performanceviewer
} // namespace plugins
} // namespace next
