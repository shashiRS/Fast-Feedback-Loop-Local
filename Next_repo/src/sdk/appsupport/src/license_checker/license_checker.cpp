/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     license_checker.cpp
 * @brief    Checks the license for telemetry
 *
 * Implementation of the functions for checking the licence for telemetry.
 *
 */

#include "license_checker.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <next/sdk/logger/logger.hpp>
#include "json/json.h"

namespace next {
namespace appsupport {
namespace license {
LicenseChecker::LicenseChecker() {}

std::string LicenseChecker::OpenJson(const std::string license_path) {
  std::ifstream ifs;
  std::ostringstream content;

  if (!boost::filesystem::exists(license_path)) {
    // empty string
    return "";
  }

  ifs.open(license_path);

  std::string line;
  while (std::getline(ifs, line)) {
    content << line;
  }

  ifs.close();
  valid_licenses_path_ = license_path;

  return content.str();
}

bool LicenseChecker::ParseJson(const std::string &json_content) {
  Json::Value root;
  Json::Reader reader;

  if (json_content == "") {
    // no info or warning specified if the file does not exist
    return false;
  }
  if (!reader.parse(json_content, root)) {
    warn(__FILE__, "Invalid Json format of {}", valid_licenses_path_);
    return false;
  }

  if (root["TelemetryEnabled"].empty()) {
    warn(__FILE__, "Invalid member in file {}", valid_licenses_path_);
    return false;
  }

  if (root["TelemetryEnabled"].asBool()) {
    return true;
  } else {
    info(__FILE__, "Telemetry is disabled in file {}", valid_licenses_path_);
    return false;
  }
}

} // namespace license
} // namespace appsupport
} // namespace next
