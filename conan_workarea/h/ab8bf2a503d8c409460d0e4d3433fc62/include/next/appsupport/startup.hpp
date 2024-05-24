/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_appsupport
 * ========================== NEXT Project ==================================
 */
/**
 * @file     appsupport_startup.hpp
 * @brief    class with static startup functions to setup and start a executable in the
 *           NEXT Simulation Framework
 **/

#ifndef APPSUPPORT_STARTUP_HPP_
#define APPSUPPORT_STARTUP_HPP_

#include <string>
#include <vector>

#include <next/appsupport/appsupport_config.hpp>

namespace next {
namespace appsupport {

class DECLSPEC_appsupport NextStartUp {
  NextStartUp() = delete;

public:
  static bool InitializeNext(std::string instance_name, const std::vector<char *> &args,
                             bool interprocess_enabled = false);
};

} // namespace appsupport
} // namespace next

#endif // APPSUPPORT_STARTUP_HPP_
