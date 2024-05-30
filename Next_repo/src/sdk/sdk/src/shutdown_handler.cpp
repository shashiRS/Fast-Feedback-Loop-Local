/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     shutdown_handler.cpp
 * @brief    handling shutdown requests
 */

#include <next/sdk/profiler/profile_tags.hpp>
#include <next/sdk/shutdown_handler/shutdown_handler.hpp>

#if defined(_WIN32) && defined(_MSC_VER)
#include "windows.h"
#elif defined(__linux__) && defined(__GNUG__)
#include "signal.h"
#else
#error not supported OS (either Windows (MSVC) or Linux (GCC))
#endif

#include <chrono>
#include <thread>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace shutdown_handler {

// whether a shutdown event was already received
volatile bool shutdown_event_received = false;
bool shutdown_requested() {
  ProfileCore_OV;
  return shutdown_event_received;
}

// time the shtudown request was received
std::chrono::time_point<std::chrono::high_resolution_clock> shutdown_event_time;

// the function we have to call if the shutdown request was received
shutdown_callback callback = nullptr;

//! Handling the CTRL+C event
/**
 * Setting a variable that a shutdown request (CTRL+C, SIG INT ...) was received.
 *
 */
void handle_shutdown_event() {
  if (false == shutdown_event_received) {
    shutdown_event_received = true;
    shutdown_event_time = std::chrono::high_resolution_clock::now();
    if (callback) {
      callback();
    }
  } else {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                              shutdown_event_time)
            .count() > 3000) {
      exit(EXIT_FAILURE);
    }
  }
  // we have to wait here in case of terminal closing, giving the application a chance to close itself
  // as soon as we return from here, the application will be terminated
  std::this_thread::sleep_for(std::chrono::seconds(2));
}

#ifdef _WIN32
/**
 * @brief Handling of shutdown requests in the command line program, the Windows way
 *
 * By default CTRL+C would call ExitProcess, we will close the application in a nice way.
 * Also the signals of system shutdown, user logoff and close event are handled.
 *
 * Note: PHANDLER_ROUTINE returns BOOL (typedef of int), don't use bool.
 *
 * @param signal the Signal
 * @return BOOL whether the signal was handled by this handler
 */
BOOL WINAPI signal_handler(DWORD signal) {
  switch (signal) {
  case CTRL_C_EVENT:
    logger::debug(__FILE__, "CTRL+C pressed");
    handle_shutdown_event();
    break;
  case CTRL_SHUTDOWN_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_CLOSE_EVENT:
    logger::debug(__FILE__, "Close event received");
    handle_shutdown_event();
    break;
  default:
    // not handled here, let it be handled by the default handler
    return 0;
  }
  return 1;
}
#else
/**
 * @brief Handling of SIG INT on Linux
 *
 * This function should only be registered as a handler for SIGINT and SIGHUP. Other signals are ignored.
 *
 * SIGINT: e.g. CTRL+C
 * SIGHUP: console is closed
 *
 * @param signal the signal (e.g. SIGINT)
 */
void signal_handler(int signal) {
  switch (signal) {
  case SIGHUP:
  case SIGINT:
    logger::debug(__FILE__, "SIGINT received");
    handle_shutdown_event();
    break;
  }
}
#endif

void register_shutdown_handler() {
  ProfileCore_DTL;
#ifdef _WIN32
  SetConsoleCtrlHandler(signal_handler, true);
#else
  signal(SIGINT, signal_handler);
#endif
}

void register_shutdown_handler(shutdown_callback f) {
  ProfileCore_DTL;
  callback = f;
  register_shutdown_handler();
}

} // namespace shutdown_handler
} // namespace sdk
} // namespace next
