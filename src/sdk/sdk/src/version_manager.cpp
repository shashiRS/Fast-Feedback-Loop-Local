/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     version_manager.cpp
 * @brief    implementation of version manager function
 */

#include <next/sdk/version_manager/version_manager.hpp>

#include <next/sdk/events/version_info.h>
#include <next/sdk/init_chain/simple_init_chain.hpp>

static constexpr const char *kVersionManagerComponentNameConfigKey = "version-manager-name";
static constexpr const char *kVersionManagerDocumentationConfigKey = "version-manager-docu";
static constexpr const char *kVersionManagerVersionStringConfigKey = "version-manager-vers";

namespace next {
namespace sdk {
namespace version_manager {

const char *getVersionManagerComponentNameCfgKey() { return kVersionManagerComponentNameConfigKey; }

const char *getVersionManagerDocumentationCfgKey() { return kVersionManagerDocumentationConfigKey; }

const char *getVersionManagerVersionStringCfgKey() { return kVersionManagerVersionStringConfigKey; }

bool get_string_value(std::string &s, InitChain::ConfigMap::const_iterator element) {
  if (element->second.type() == typeid(std::string)) {
    // const std::string is provided as std::string, no special case needed
    s = std::any_cast<std::string>(element->second);
  } else if (element->second.type() == typeid(char *) || element->second.type() == typeid(const char *)) {
    // treating a char* as const char* is fine
    s = std::any_cast<const char *>(element->second);
  } else {
    return false;
  }
  return true;
}

std::shared_ptr<next::sdk::events::VersionInfo> version_info_event = nullptr;
static bool init_already_done = false;
static next::sdk::events::VersionInfoPackage version_info_package;

bool init(const InitChain::ConfigMap &map, int) {
  static std::string name = "name_not_set";
  static std::string docu = "docu_not_available";
  static std::string vers = "0.0.0.0";

  if (false == init_already_done) {
    auto str = map.find(kVersionManagerComponentNameConfigKey);
    if (map.end() != str) {
      if (!get_string_value(name, str)) {
        return false;
      }
    }

    str = map.find(kVersionManagerDocumentationConfigKey);
    if (map.end() != str) {
      if (!get_string_value(docu, str)) {
        return false;
      }
    }

    str = map.find(kVersionManagerVersionStringConfigKey);
    if (map.end() != str) {
      if (!get_string_value(vers, str)) {
        return false;
      }
    }
    init_already_done = true;
  }

  version_info_event = std::make_shared<next::sdk::events::VersionInfo>("");

  version_info_package.component_name = name;
  version_info_package.documentation_link = docu;
  version_info_package.component_version = vers;

  version_info_event->publish(version_info_package);

  return true;
}

void close(const InitChain::ConfigMap &, int) {
  init_already_done = false;
  version_info_event.reset();
  version_info_package.component_name = "";
  version_info_package.documentation_link = "";
  version_info_package.component_version = "";
}

void register_to_init_chain() { static InitChain::El init_el(InitPriority::VERSION_MANAGER, init, close); }

/*!
 * @brief Generate the component version in string format, to send to GUI.
 *
 * from the VersionInfo macro "VERSIONINFO_PROJECTVERSION_STR" which is having some extra info.
 * Extract only the version info, excluding the v in the beginning and all the remaining info after the release tag.
 * A sample VERSIONINFO_PROJECTVERSION_STR : L"v0.10.800-rc1 - 507d18212e4583455feeede3624feccd925b603e"
 * @return generated version string
 */
std::string MakeVersionString(const wchar_t *project_version) {
  std::wstring ws(project_version);
  std::string project_version_string(ws.begin(), ws.end());
  const size_t start = project_version_string.find_first_of("0123456789");
  const auto end = project_version_string.find(" ", start);
  if (start == std::string::npos)
    return std::string();
  std::string version_string = project_version_string.substr(start, end - start);
  return version_string;
}

/*!
 * @brief Generate link to the documentation. Documentation is saved on a webserver.
 * use hard coded documentation link of 'next' repo for all components right now.
 *
 * TO DO: generate documentation link based on the verison info.
 * @return generated documentation link
 */
std::string CreateDocumentationLink(ComponentName component_name) {
  (void)component_name;
  return "https://github-am.geo.conti.de/pages/ADAS/next/";
}

} // namespace version_manager
} // namespace sdk
} // namespace next
