/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     cmd_options_impl.hpp
 * @brief    Implementation of the command options parsing.
 *
 * Implementation of the functions for command line option handling.
 *
 */

#ifndef CMDOPTIONS_IMPL_H_
#define CMDOPTIONS_IMPL_H_

#include <functional>
#include <map>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace next {
namespace appsupport {

class CmdOptions_Impl {
public:
  CmdOptions_Impl(const std::string &caption, const std::string &config_file_key = "");
  virtual ~CmdOptions_Impl();

  bool parseCmd(int argc, const char *argv[]);

  bool OptionUsed(const std::string &option);

  void addParameterInt(const std::string &key, const std::string &description, const std::string &configKey);

  std::string getHelperString();

  template <typename T>
  void addParameter(const std::string &key, const std::string &description, const std::string &configKey,
                    po::options_description &desc_, const po::variables_map &vm_,
                    std::map<std::string, std::function<bool()>> &optList_, std::vector<std::string> &argumentKeys_);

  void addParameterFloat(const std::string &key, const std::string &description, const std::string &configKey);

  void addParameterString(const std::string &key, const std::string &description, const std::string &configKey);

  void addParameterBool(const std::string &key, const std::string &description, const std::string &configKey);

private:
  bool helpOption();
  bool versionOption();
  bool cfgOption();

protected:
  po::options_description desc_;
  // actual options provided as input, after parsing
  po::variables_map vm_;
  // possible options that could be provided as arguments, set using addParameterString
  std::map<std::string, std::function<bool()>> optList_;
  std::vector<std::string> argumentKeys_;
  std::string config_file_key_;
};

} // namespace appsupport
} // namespace next

#endif // CMDOPTIONS_IMPL_H_
