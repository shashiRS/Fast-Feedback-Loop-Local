/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cmd_options.cpp
 * @brief    functions for program option handling
 *
 **/

#include <next/appsupport/cmd_options/cmd_options.hpp>

#include "cmd_options_impl.hpp"

namespace next {
namespace appsupport {

CmdOptions::CmdOptions(const std::string &caption, const std::string &config_file_key /*= "config-files,c"*/) {
  this->impl_ = new CmdOptions_Impl(caption, config_file_key);
}

CmdOptions::~CmdOptions() { delete this->impl_; }

bool CmdOptions::parseCmd(int argc, const char *argv[]) { return this->impl_->parseCmd(argc, argv); }

bool CmdOptions::OptionUsed(const std::string &option) { return this->impl_->OptionUsed(option); }

std::string CmdOptions::getHelperString() { return this->impl_->getHelperString(); }

void CmdOptions::addParameterInt(const std::string &key, const std::string &description, const std::string &configKey) {
  this->impl_->addParameterInt(key, description, configKey);
}

void CmdOptions::addParameterFloat(const std::string &key, const std::string &description,
                                   const std::string &configKey) {
  this->impl_->addParameterFloat(key, description, configKey);
}

void CmdOptions::addParameterString(const std::string &key, const std::string &description,
                                    const std::string &configKey) {
  this->impl_->addParameterString(key, description, configKey);
}

void CmdOptions::addParameterBool(const std::string &key, const std::string &description,
                                  const std::string &configKey) {
  this->impl_->addParameterBool(key, description, configKey);
}

} // namespace appsupport
} // namespace next
