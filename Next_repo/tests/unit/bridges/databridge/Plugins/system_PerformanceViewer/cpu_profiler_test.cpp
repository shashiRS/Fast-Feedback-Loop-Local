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
#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <next/performanceviewer/next_cpu_profiler.h>

#include "next_cpu_profiler_impl.h"
#include "performance_viewer.h"

namespace next {
namespace plugins {
namespace performanceviewer {

class NextCpuProfilerTest : public NextCpuProfiler {
public:
  void setPastProcessKernelTime(const LARGE_INTEGER &pastProcessKernelTime) {
    past_process_kernel_time_.dwHighDateTime = pastProcessKernelTime.HighPart;
    past_process_kernel_time_.dwLowDateTime = pastProcessKernelTime.LowPart;
  };
  void setPastProcessUserTime(const LARGE_INTEGER &pastProcessUserTime) {
    past_process_user_time_.dwHighDateTime = pastProcessUserTime.HighPart;
    past_process_user_time_.dwLowDateTime = pastProcessUserTime.LowPart;
  };
  void setCurrentProcessKernelTime(const LARGE_INTEGER &currentProcessKernelTime) {
    current_process_kernel_time_.dwHighDateTime = currentProcessKernelTime.HighPart;
    current_process_kernel_time_.dwLowDateTime = currentProcessKernelTime.LowPart;
  };
  void setCurrentProcessUserTime(const LARGE_INTEGER &currentProcessUserTime) {
    current_process_user_time_.dwHighDateTime = currentProcessUserTime.HighPart;
    current_process_user_time_.dwLowDateTime = currentProcessUserTime.LowPart;
  };
  void setPastSystemKernelTime(const LARGE_INTEGER &pastSystemKernelTime) {
    past_system_kernel_time_.dwHighDateTime = pastSystemKernelTime.HighPart;
    past_system_kernel_time_.dwLowDateTime = pastSystemKernelTime.LowPart;
  };
  void setPastSystemUserTime(const LARGE_INTEGER &pastSystemUserTime) {
    past_system_user_time_.dwHighDateTime = pastSystemUserTime.HighPart;
    past_system_user_time_.dwLowDateTime = pastSystemUserTime.LowPart;
  };
  void setCurrentSystemKernelTime(const LARGE_INTEGER &currentSystemKernelTime) {
    current_system_kernel_time_.dwHighDateTime = currentSystemKernelTime.HighPart;
    current_system_kernel_time_.dwLowDateTime = currentSystemKernelTime.LowPart;
  };
  void setCurrentSystemUserTime(const LARGE_INTEGER &currentSystemUserTime) {
    current_system_user_time_.dwHighDateTime = currentSystemUserTime.HighPart;
    current_system_user_time_.dwLowDateTime = currentSystemUserTime.LowPart;
  };

  FILETIME getPastProcessKernelTime() { return past_process_kernel_time_; };
  FILETIME getPastProcessUserTime() { return past_process_user_time_; };
  FILETIME getCurrentProcessKernelTime() { return current_process_kernel_time_; };
  FILETIME getCurrentProcessUserTime() { return current_process_user_time_; };
  FILETIME getPastSystemKernelTime() { return past_system_kernel_time_; };
  FILETIME getPastSystemUserTime() { return past_system_user_time_; };
  FILETIME getCurrentSystemKernelTime() { return current_system_kernel_time_; };
  FILETIME getCurrentSystemUserTime() { return current_system_user_time_; };

  void updateCpuUsage() { return NextCpuProfiler::updateCpuUsage(); };
  int updateCurrentTimestamps() { return NextCpuProfiler::updateCurrentTimestamps(); };
  void updatePastTimestamps() { return NextCpuProfiler::updatePastTimestamps(); };
  ULONGLONG subtractTimes(const FILETIME &currentTime, const FILETIME &pastTime) {
    return NextCpuProfiler::subtractTimes(currentTime, pastTime);
  };
};

// NEXT CPU Process Profiler Tests
TEST(NextCpuProfilerInterfaceTest, createObject) {
  std::shared_ptr<next::plugins::performanceviewer::INextCpuProfiler> process =
      next::plugins::performanceviewer::CreateNextCpuProfiler();
  EXPECT_TRUE(process);
}

TEST(NextCpuProfilerInterfaceTest, initName) {
  std::string dummy = "system_PerformanceViewer_test.exe";

  NextCpuProfiler process;
  process.init(dummy);

  EXPECT_EQ(process.getProcessName(), dummy);
}

TEST(NextCpuProfilerInterfaceTest, initID) {
  DWORD current_process_id = GetCurrentProcessId();

  NextCpuProfiler process;
  process.init("system_PerformanceViewer_test.exe");

  EXPECT_EQ(process.getProcessID(), current_process_id);
}

TEST(NextCpuProfilerUnitTest, updateCurrentProcessKernelTime) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");

  FILETIME test_process_creation_time_1{ULONG_MAX, ULONG_MAX}, test_process_idle_time_1{ULONG_MAX, ULONG_MAX},
      test_process_kernel_time_1{ULONG_MAX, ULONG_MAX}, test_process_user_time_1{ULONG_MAX, ULONG_MAX};

  GetProcessTimes(GetCurrentProcess(), &test_process_creation_time_1, &test_process_idle_time_1,
                  &test_process_kernel_time_1, &test_process_user_time_1);

  process.updateCurrentTimestamps();
  FILETIME process_time = process.getCurrentProcessKernelTime();

  FILETIME test_process_creation_time_2{ULONG_MAX, ULONG_MAX}, test_process_idle_time_2{ULONG_MAX, ULONG_MAX},
      test_process_kernel_time_2{ULONG_MAX, ULONG_MAX}, test_process_user_time_2{ULONG_MAX, ULONG_MAX};

  GetProcessTimes(GetCurrentProcess(), &test_process_creation_time_2, &test_process_idle_time_2,
                  &test_process_kernel_time_2, &test_process_user_time_2);

  EXPECT_TRUE((test_process_kernel_time_1.dwHighDateTime <= process_time.dwHighDateTime) &&
              (process_time.dwHighDateTime <= test_process_kernel_time_2.dwHighDateTime));

  EXPECT_TRUE((test_process_kernel_time_1.dwLowDateTime <= process_time.dwLowDateTime) &&
              (process_time.dwLowDateTime <= test_process_kernel_time_2.dwLowDateTime));
}
TEST(NextCpuProfilerUnitTest, updateCurrentProcessUserTime) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  FILETIME test_process_creation_time_1{ULONG_MAX, ULONG_MAX}, test_process_idle_time_1{ULONG_MAX, ULONG_MAX},
      test_process_kernel_time_1{ULONG_MAX, ULONG_MAX}, test_process_user_time_1{ULONG_MAX, ULONG_MAX};
  GetProcessTimes(GetCurrentProcess(), &test_process_creation_time_1, &test_process_idle_time_1,
                  &test_process_kernel_time_1, &test_process_user_time_1);
  process.updateCurrentTimestamps();

  FILETIME process_time = process.getCurrentProcessUserTime();

  FILETIME test_process_creation_time_2{ULONG_MAX, ULONG_MAX}, test_process_idle_time_2{ULONG_MAX, ULONG_MAX},
      test_process_kernel_time_2{ULONG_MAX, ULONG_MAX}, test_process_user_time_2{ULONG_MAX, ULONG_MAX};

  GetProcessTimes(GetCurrentProcess(), &test_process_creation_time_2, &test_process_idle_time_2,
                  &test_process_kernel_time_2, &test_process_user_time_2);

  EXPECT_TRUE((test_process_user_time_1.dwHighDateTime <= process_time.dwHighDateTime) &&
              (process_time.dwHighDateTime <= test_process_user_time_2.dwHighDateTime));

  EXPECT_TRUE((test_process_user_time_1.dwLowDateTime <= process_time.dwLowDateTime) &&
              (process_time.dwLowDateTime <= test_process_user_time_2.dwLowDateTime));
}
TEST(NextCpuProfilerUnitTest, updateCurrentSystemKernelTime) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");

  FILETIME test_system_idle_time_1{ULONG_MAX, ULONG_MAX}, test_system_kernel_time_1{ULONG_MAX, ULONG_MAX},
      test_system_user_time_1{ULONG_MAX, ULONG_MAX};

  GetSystemTimes(&test_system_idle_time_1, &test_system_kernel_time_1, &test_system_user_time_1);
  process.updateCurrentTimestamps();

  FILETIME process_time = process.getCurrentSystemKernelTime();

  FILETIME test_system_idle_time_2{ULONG_MAX, ULONG_MAX}, test_system_kernel_time_2{ULONG_MAX, ULONG_MAX},
      test_system_user_time_2{ULONG_MAX, ULONG_MAX};

  GetSystemTimes(&test_system_idle_time_1, &test_system_kernel_time_1, &test_system_user_time_1);

  EXPECT_TRUE((test_system_kernel_time_1.dwHighDateTime <= process_time.dwHighDateTime) &&
              (process_time.dwHighDateTime <= test_system_kernel_time_2.dwHighDateTime));

  EXPECT_TRUE((test_system_kernel_time_1.dwLowDateTime <= process_time.dwLowDateTime) &&
              (process_time.dwLowDateTime <= test_system_kernel_time_2.dwLowDateTime));
}
TEST(NextCpuProfilerUnitTest, updateCurrentSystemUserTime) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  FILETIME test_system_idle_time_1, test_system_kernel_time_1, test_system_user_time_1;

  GetSystemTimes(&test_system_idle_time_1, &test_system_kernel_time_1, &test_system_user_time_1);
  process.updateCurrentTimestamps();

  FILETIME process_time = process.getCurrentSystemUserTime();
  FILETIME test_system_idle_time_2, test_system_kernel_time_2, test_system_user_time_2;

  GetSystemTimes(&test_system_idle_time_2, &test_system_kernel_time_2, &test_system_user_time_2);

  EXPECT_TRUE((test_system_user_time_1.dwHighDateTime <= process_time.dwHighDateTime) &&
              (process_time.dwHighDateTime <= test_system_user_time_2.dwHighDateTime));
  EXPECT_TRUE((test_system_user_time_1.dwLowDateTime <= process_time.dwLowDateTime) &&
              (process_time.dwLowDateTime <= test_system_user_time_2.dwLowDateTime));
}

TEST(NextCpuProfilerUnitTest, setPastProcessKernelTimestamps) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  LARGE_INTEGER test_data;
  test_data.QuadPart = 300;

  process.setCurrentProcessKernelTime(test_data);

  process.updatePastTimestamps();

  FILETIME past_time = process.getPastProcessKernelTime();
  FILETIME current_time = process.getCurrentProcessKernelTime();

  EXPECT_EQ(past_time.dwHighDateTime, current_time.dwHighDateTime);
  EXPECT_EQ(past_time.dwLowDateTime, current_time.dwLowDateTime);
}
TEST(NextCpuProfilerUnitTest, setPastProcessUserTimestamps) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  LARGE_INTEGER test_data;
  test_data.QuadPart = 300;

  process.setCurrentProcessUserTime(test_data);

  process.updatePastTimestamps();

  FILETIME past_time = process.getPastProcessUserTime();
  FILETIME current_time = process.getCurrentProcessUserTime();

  EXPECT_EQ(past_time.dwHighDateTime, current_time.dwHighDateTime);
  EXPECT_EQ(past_time.dwLowDateTime, current_time.dwLowDateTime);
}
TEST(NextCpuProfilerUnitTest, setPastSystemKernelTimestamps) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  LARGE_INTEGER test_data;
  test_data.QuadPart = 300;

  process.setCurrentSystemKernelTime(test_data);

  process.updatePastTimestamps();

  FILETIME past_time = process.getPastSystemKernelTime();
  FILETIME current_time = process.getCurrentSystemKernelTime();

  EXPECT_EQ(past_time.dwHighDateTime, current_time.dwHighDateTime);
  EXPECT_EQ(past_time.dwLowDateTime, current_time.dwLowDateTime);
}
TEST(NextCpuProfilerUnitTest, setPastSystemUserTimestamps) {
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");
  LARGE_INTEGER test_data;
  test_data.QuadPart = 300;

  process.setCurrentSystemUserTime(test_data);

  process.updatePastTimestamps();

  FILETIME past_time = process.getPastSystemUserTime();
  FILETIME current_time = process.getCurrentSystemUserTime();

  EXPECT_EQ(past_time.dwHighDateTime, current_time.dwHighDateTime);
  EXPECT_EQ(past_time.dwLowDateTime, current_time.dwLowDateTime);
}

TEST(NextCpuProfilerUnitTest, SubtractTime) {
  NextCpuProfilerTest process;

  LARGE_INTEGER minuend, subtrahend;
  FILETIME currentTime, pastTime;
  minuend.QuadPart = 150;
  subtrahend.QuadPart = 100;

  currentTime.dwHighDateTime = minuend.HighPart;
  currentTime.dwLowDateTime = minuend.LowPart;

  pastTime.dwHighDateTime = subtrahend.HighPart;
  pastTime.dwLowDateTime = subtrahend.LowPart;

  EXPECT_EQ(process.subtractTimes(currentTime, pastTime), 50);
}

TEST(NextCpuProfilerUnitTest, setCpuUsageTest) {
  LARGE_INTEGER current_process_kernel, current_process_user, current_system_kernel, current_system_user;
  LARGE_INTEGER past_process_kernel, past_process_user, past_system_kernel, past_system_user;
  NextCpuProfilerTest process;
  process.init("system_PerformanceViewer_test.exe");

  current_process_kernel.QuadPart = 10;
  past_process_kernel.QuadPart = 2; // total_process_kernel_time = 8
  current_process_user.QuadPart = 3;
  past_process_user.QuadPart = 1; // total_process_user_time = 2
  // total_process_time = 10

  current_system_kernel.QuadPart = 150;
  past_system_kernel.QuadPart = 100; // total_system_kernel_time = 50
  current_system_user.QuadPart = 55;
  past_system_user.QuadPart = 5; // total_system_user_time = 50
  // total_system_time = 100
  // cpu_usage = 10% -> 0.1

  process.setCurrentProcessKernelTime(current_process_kernel);
  process.setCurrentProcessUserTime(current_process_user);

  process.setPastProcessKernelTime(past_process_kernel);
  process.setPastProcessUserTime(past_process_user);

  process.setCurrentSystemKernelTime(current_system_kernel);
  process.setCurrentSystemUserTime(current_system_user);

  process.setPastSystemKernelTime(past_system_kernel);
  process.setPastSystemUserTime(past_system_user);

  process.updateCpuUsage();
  EXPECT_EQ(process.getCpuUsage(), (float)0.1);
}

} // namespace performanceviewer
} // namespace plugins
} // namespace next
