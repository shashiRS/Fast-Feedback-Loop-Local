/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_data_datatypes.hpp
 * @brief    Handles the datatypes and const values used in the config database
 *
 * Implementation of the datatypes and const values for data handling in the config database.
 *
 **/
#ifndef CONFIG_DATA_DATATYPES_H
#define CONFIG_DATA_DATATYPES_H

#include <string>
#include <vector>

#include <boost/variant.hpp>

namespace next {
namespace appsupport {

constexpr char kConfigurationKeyDelimiter = ':';

struct ConfigValue {
  boost::variant<boost::blank, bool, int, float, std::string> value;
};

struct ConfigValueList {
  std::vector<ConfigValue> data;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATA_DATATYPES_H
