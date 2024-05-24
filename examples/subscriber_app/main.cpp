/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "next/appsupport/startup.hpp"
#include "next/sdk/logger/logger.hpp"
#include "next/udex/explorer/signal_explorer.hpp"

namespace logger = next::sdk::logger;

int main(int argc, char **argv) {
  int exit_code = 0;

  std::vector<char *> args(argv, argv + argc);
  exit_code = next::appsupport::NextStartUp::InitializeNext("next_subscriber_app", {});

  next::udex::explorer::SignalExplorer explorer;

  while (true) {
    auto devices = explorer.GetChildByUrl("");

    logger::info(__FILE__, "Available devices:");
    for (const auto &device : devices) {
      logger::info(__FILE__, "  {0} -> {1} children", device.name, device.child_count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  return (exit_code);
}
