/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     license_checker.hpp
 * @brief    Checks the license for telemetry
 *
 * Implementation of the functions for checking the licence for telemetry.
 *
 */

#ifndef LICENSE_CHECKER_IMPL_H_
#define LICENSE_CHECKER_IMPL_H_

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <thread>

#include <next/sdk/events/session_event.hpp>

namespace next {
namespace appsupport {
namespace license {

class LicenseChecker {
public:
  LicenseChecker();
  std::string OpenJson(const std::string license_path);
  bool ParseJson(const std::string &json_content);

private:
  std::string valid_licenses_path_;
};

} // namespace license
} // namespace appsupport
} // namespace next

#endif // LICENSE_CHECKER_IMPL_H_
