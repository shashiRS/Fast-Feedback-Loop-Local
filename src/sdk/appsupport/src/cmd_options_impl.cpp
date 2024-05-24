/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cmd_options_impl.cpp
 * @brief    Implementation of the command options parsing.
 *
 **/

#include "cmd_options_impl.hpp"
#include <next/appsupport/config/config_base.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include "config.h"

#include <iostream>
#include <type_traits>
#include <vector>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace next {
namespace appsupport {

CmdOptions_Impl::CmdOptions_Impl(const std::string &caption, const std::string &config_file_key /*= ""*/)
    : desc_(caption), config_file_key_(config_file_key) {
  // clang-format off
  desc_.add_options()
	  ("help,h", "produce help message")
	  ("version,v", "produce version message");
  // clang-format on
  if (const std::size_t pos = config_file_key_.rfind(","); pos != std::string::npos) {
    config_file_key_ = config_file_key_.substr(0, pos);
  }
  if (config_file_key_.length() != 0) {
    desc_.add_options()(config_file_key.c_str(),
                        boost::program_options::value<std::vector<std::string>>()->multitoken(),
                        "set one or multiple configuration files");
    auto lambda = [this]() {
      if (vm_.count(config_file_key_)) {
        const auto fl = vm_[config_file_key_].as<std::vector<std::string>>();
        // TODO: adapt with ticket SIMEN-9682
        // components which are different to current components might be ignored during putCfg
        // shall be changed when the config communication is able to filter out all components
        // which are not from this one when it's sending information to the server
        // bool filter_status = ConfigClient::getConfig()->getCurrentFilterByComponentStatus();
        //// deactive filtering during insert of config file
        // ConfigClient::getConfig()->setCurrentFilterByComponentStatus(false);

        for (const std::string &f : fl) {
          ConfigClient::getConfig()->putCfg(f, ConfigSource::CmdOptionsConfig);
        }
        // TODO: adapt with ticket SIMEN-9682
        // components which are different to current components might be ignored during putCfg
        // shall be changed when the config communication is able to filter out all components
        // which are not from this one when it's sending information to the server

        // reset filtering to previous state
        /*ConfigClient::getConfig()->setCurrentFilterByComponentStatus(filter_status);*/

        return true;
      }
      return false;
    };

    optList_[config_file_key_] = lambda;
  }
}

CmdOptions_Impl::~CmdOptions_Impl() {}

bool CmdOptions_Impl::parseCmd(int argc, const char *argv[]) {
  bool retVal = true;
  vm_ = po::variables_map();
  try {
    // allow also unknown options, they will be ignored
    auto parsedOptions = po::command_line_parser(argc, argv).options(desc_).allow_unregistered().run();
    store(parsedOptions, vm_);
    // check for unregistered options
    auto unregistered = po::collect_unrecognized(parsedOptions.options, po::include_positional);
    for (auto opt : unregistered) {
      // check if dash was used wrong
      if (opt.size() > 2) {
        if (opt[0] == '-' && opt[1] != '-') {
          // could be a long name used with single dash - check if it exists
          std::string dashlessOption = opt.substr(1, opt.size());
          if (std::find(argumentKeys_.begin(), argumentKeys_.end(), dashlessOption) != argumentKeys_.end()) {
            std::cerr << "[CmdOptions] please use double dash [--] for command: [" << dashlessOption << "]"
                      << std::endl;
          }
        }
      }
      std::cerr << "[CmdOptions] Unrecognized argument: " << opt << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << "[CmdOptions] Exception detected parsing command: " << e.what() << std::endl;
    return false;
  }
  // check for help or version
  if (vm_.count("help")) {
    helpOption();
    return false;
  } else if (vm_.count("version")) {
    versionOption();
    return false;
  }

  // TODO config file loading has to be done before the specific parameters
  //  the execution order should be - load config from global, load from file, set specifics

  notify(vm_);
  // TODO maybe not needed, we have our own notify functions

  for (auto opt : optList_) {
    if (vm_.count(opt.first))
      if (!(opt.second)())
        retVal = false;
  }

  return retVal;
}

bool CmdOptions_Impl::OptionUsed(const std::string &option) {
  bool exists = vm_.find(option) != vm_.end();
  return exists;
}

std::string CmdOptions_Impl::getHelperString() {
  std::ostringstream out;
  desc_.print(out);
  return out.str();
}

template <typename T>
void CmdOptions_Impl::addParameter(const std::string &key, const std::string &description, const std::string &configKey,
                                   po::options_description &desc_, const po::variables_map &vm_,
                                   std::map<std::string, std::function<bool()>> &optList_,
                                   std::vector<std::string> &argumentKeys_) {
  std::cout << key << " : " << description << " -> " << configKey << "\n";

  if (key.length() == 0) {
    return;
  }

  std::string shortened_key{key};
  std::string long_key{key};
  if (const std::size_t pos = key.rfind(","); pos != std::string::npos) {
    shortened_key = key.substr(0, pos);
    long_key = key.substr(pos + 1, key.size());
  }

  if (shortened_key.empty()) {
    return;
  }
  if (std::find(argumentKeys_.begin(), argumentKeys_.end(), shortened_key) != argumentKeys_.end()) {
    std::cerr << "[CmdOptions] Key [" << shortened_key << "] is already defined; it will be ignored." << std::endl;
    return;
  }
  if (std::find(argumentKeys_.begin(), argumentKeys_.end(), long_key) != argumentKeys_.end()) {
    std::cerr << "[CmdOptions] Key [" << long_key << "] is already defined; it will be ignored." << std::endl;
    return;
  }
  argumentKeys_.push_back(shortened_key);
  argumentKeys_.push_back(long_key);

  if constexpr (std::is_same_v<T, bool>) {
    desc_.add_options()(key.c_str(), description.c_str());
  } else {
    desc_.add_options()(key.c_str(), po::value<T>(), description.c_str());
  }

  auto lambda = [&vm_, shortened_key, configKey]() {
    auto static_ptr = ConfigClient::getConfig();
    if (!static_ptr) {
      return false;
    }
    if constexpr (std::is_same_v<T, bool>) {
      const bool value = vm_.count(shortened_key);
      static_ptr->put_option(configKey, value, ConfigSource::CmdOptionsParameters);
    } else {
      const auto value = vm_[shortened_key].as<T>();
      static_ptr->put(configKey, value, ConfigSource::CmdOptionsParameters);
    }
    return true;
  };

  optList_[shortened_key] = lambda;
}

void CmdOptions_Impl::addParameterInt(const std::string &key, const std::string &description,
                                      const std::string &configKey) {
  addParameter<int>(key, description, configKey, desc_, vm_, optList_, argumentKeys_);
}

void CmdOptions_Impl::addParameterFloat(const std::string &key, const std::string &description,
                                        const std::string &configKey) {
  addParameter<float>(key, description, configKey, desc_, vm_, optList_, argumentKeys_);
}
void CmdOptions_Impl::addParameterString(const std::string &key, const std::string &description,
                                         const std::string &configKey) {
  addParameter<std::string>(key, description, configKey, desc_, vm_, optList_, argumentKeys_);
}

void CmdOptions_Impl::addParameterBool(const std::string &key, const std::string &description,
                                       const std::string &configKey) {
  addParameter<bool>(key, description, configKey, desc_, vm_, optList_, argumentKeys_);
}

bool CmdOptions_Impl::helpOption() {
  std::cout << desc_ << '\n';
  return false;
}
bool CmdOptions_Impl::versionOption() {
  // todo fix this
  auto componentNameList = ConfigClient::getConfig()->componentName();
  for (auto comp_name : componentNameList) {
    auto major_v = ConfigClient::getConfig()->get_string(configkey::getMajorVersionKey(comp_name), "0");
    auto minor_v = ConfigClient::getConfig()->get_string(configkey::getMinorVersionKey(comp_name), "0");
    auto patch_v = ConfigClient::getConfig()->get_string(configkey::getPatchVersionKey(comp_name), "0");
    std::cout << comp_name << " v" << major_v << "." << minor_v << "." << patch_v << '\n';
  }
  return false;
}

bool CmdOptions_Impl::cfgOption() {
  if (vm_.count(config_file_key_)) {
    const auto fl = vm_[config_file_key_].as<std::vector<std::string>>();
    for (const std::string &f : fl) {
      ConfigClient::getConfig()->putCfg(f, ConfigSource::CmdOptionsConfig);
    }
    return true;
  }
  return false;
}

} // namespace appsupport
} // namespace next
