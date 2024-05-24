/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     cmd_options.hpp
 * @brief    Command line options handling.
 *
 * Include this file if you want to use the Next command line parameter handling.
 * This is depending on the Config_Base.
 *
 *
 */

#ifndef CMDOPTIONS_H_
#define CMDOPTIONS_H_

#include <string>

#include <next/appsupport/appsupport_config.hpp>

namespace next {
namespace appsupport {

class CmdOptions_Impl;

/*!
 * @brief Interface to the command line parsing functions
 * @brief Used for parsing the command line options given at runtime.
 *
 * There are two default options with fixed implementations:
 * - Help, which shows the help message of the application
 * - Version, which shows the name of the application and the current version of the binary
 *
 * Another option is prepared (but could be overridden):
 * - config-file, which allows to give a list of configuration files, to be parsed at the startup.
 */
class DECLSPEC_appsupport CmdOptions {
public:
  //! creates a CmdOptions with the given caption.
  /*!
   * @param caption           The text that will be shown in the help screen
   * @param config_file_key   The long,short parameter tag for the config files
   */
  CmdOptions(const std::string &caption, const std::string &config_file_key = "config-files,c");

  //! standard destructor for DataPublisher
  virtual ~CmdOptions();

  //! Parses the commandline
  /*!
   * Parses the commandline for all given parameters and stores the data into the config
   *
   * @param argc			Number of parameters in the parameter string (as given by main function)
   * @param argv            The parameter string (as given by main function)
   *
   * @return                true if execution should continue, false if execution should be stopped (e.g. help, version)
   */
  bool parseCmd(int argc, const char *argv[]);

  //! Check if option is used
  /*!
   * Returns true if the option was provided as command line argument and it is used
   *
   * @param option      Option to be checked
   *
   * @return            true if option was provided as input, false otherwise
   */
  bool OptionUsed(const std::string &option);

  //! Output of helper String
  /*!
   * Forward the helper string to show it inside other parsers (e.g. Cobolt)
   *
   * @return    The helper string that would be printed by --help -h
   */
  std::string getHelperString();

  //! Set specific commandline option
  /*!
   * Add a commandline option to the parser, that will accept an integer value. The given value for this option will be
   * stored into the config
   *
   * @param key			The long,short parameter tag for the parameter
   * @param description The text that will be shown in the help screen for this parameter
   * @param configKey   The key in the config, the value will be stored under
   */
  void addParameterInt(const std::string &key, const std::string &description, const std::string &configKey);

  //! Set specific commandline option
  /*!
   * Add a commandline option to the parser, that will accept a float value. The given value for this option will be
   * stored into the config
   *
   * @param key			The long,short parameter tag for the parameter
   * @param description The text that will be shown in the help screen for this parameter
   * @param configKey   The key in the config, the value will be stored under
   */
  void addParameterFloat(const std::string &key, const std::string &description, const std::string &configKey);

  //! Set specific commandline option
  /*!
   * Add a commandline option to the parser, that will accept a string value. The given value for this option will be
   * stored into the config
   *
   * @param key			The long,short parameter tag for the parameter
   * @param description The text that will be shown in the help screen for this parameter
   * @param configKey   The key in the config, the value will be stored under
   */
  void addParameterString(const std::string &key, const std::string &description, const std::string &configKey);

  //! Set specific commandline option
  /*!
   * Add a commandline option to the parser, that will accept a bool value. The given value for this option will be
   * stored into the config
   *
   * @param key			The long,short parameter tag for the parameter
   * @param description The text that will be shown in the help screen for this parameter
   * @param configKey   The key in the config, the value will be stored under
   */
  void addParameterBool(const std::string &key, const std::string &description, const std::string &configKey);

  // TODO add function for a parameter that accepts multiple strings
private:
  CmdOptions_Impl *impl_ = nullptr;
};

} // namespace appsupport
} // namespace next

#endif // CMDOPTIONS_H_
