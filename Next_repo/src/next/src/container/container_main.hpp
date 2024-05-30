/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     container_main.hpp
 * @brief    handles the main function for the next_container
 */

#ifndef NEXT_CONTAINER_MAIN_HPP_
#define NEXT_CONTAINER_MAIN_HPP_

#include <iostream>
#include <thread>

#include <next/appsupport/startup.hpp>
#include <next/sdk/sdk.hpp>

#include "argument_parser.hpp"
#include "dll_loader.hpp"

#ifdef WIN32
#include <windows.h>

#include <minwindef.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void SetThreadName(std::thread &thread_temp, const std::string &name) {

  DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread_temp.native_handle()));
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = name.c_str();
  info.dwThreadID = threadId;
  info.dwFlags = 0;

  __try {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}
#else
void SetThreadName(std::thread &, const std::string &) {}
#endif

namespace next {
namespace container {

std::vector<std::thread> startDllThreads(int argc_extended, char **argv_extended,
                                         const std::vector<next::container::NextDll> &next_dlls) {
  std::vector<std::thread> main_threads;
  for (auto &dll_temp : next_dlls) {
    info(__FILE__, "Starting lib: {0}", dll_temp.name);
    auto thread_temp = std::thread(dll_temp.dll_main_func, argc_extended, argv_extended);
    SetThreadName(thread_temp, dll_temp.name);
    main_threads.push_back(std::move(thread_temp));
  }
  return main_threads;
}

int containerMain(int argc, char *argv[]) {

  auto parser = next::container::ArgumentParserNextContainer();
  std::vector<std::string> node_names;
  std::vector<std::string> args_extended;
  auto instance_name = parser.parseArguments(argc, argv, args_extended, node_names);
  std::vector<char *> args_char_array = parser.convertArguments(args_extended);

  next::appsupport::NextStartUp::InitializeNext(instance_name, args_char_array, true);

  std::vector<next::container::NextDll> next_dlls;
  if (!next::container::DllLoader::loadLibrariesInWorkingDirectory(next_dlls)) {
    error(__FILE__, "[NEXT Container] loading libraries failed");
  }
  if (!next::container::DllLoader::filterLibraries(next_dlls, node_names)) {
    warn(__FILE__, "[NEXT Container] not all libraries found");
    for (auto &name : node_names) {
      for (auto &dll : next_dlls) {
        if (name == dll.name) {
          continue;
        }
      }
      warn(__FILE__, "[NEXT Container] {0} as a node name given but missing in dlls", name);
    }
  }
  auto dll_threads = startDllThreads((int)args_char_array.size() - 1, args_char_array.data(), next_dlls);

  if (next_dlls.size() == 0) {
    warn(__FILE__, "[NEXT Container] no dlls found. Shutting down");
    return 0;
  }
  next::sdk::shutdown_handler::register_shutdown_handler();
  while (!next::sdk::shutdown_handler::shutdown_requested()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
  return 0;
}

} // namespace container
} // namespace next

#endif // NEXT_CONTAINER_MAIN_HPP_
