/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/*
 * @file     data_description_types.hpp
 * @brief    interfaces for publishing a data description to the next system
 */
#ifndef NEXT_DATA_DESCRIPTION_TYPES_HPP
#define NEXT_DATA_DESCRIPTION_TYPES_HPP
#include <string>

#include "package_data_types.hpp"

namespace next {
namespace sdk {
namespace types {

/*! @brief
 * Data description containing
 *   name
 *   data format type to understand the formats and create package names accordingly
 *   actual data description
 */
struct DataDescription {
  std::string device_name;
  next::sdk::types::PackageFormatType device_format_type;
  uint32_t instance_number;
  uint16_t source_id;

  std::string filename;
  next::sdk::types::DataDescriptionFormatType description_format_type;
  std::vector<char> binary_description;
};

} // namespace types
} // namespace sdk
} // namespace next

#endif // NEXT_DATA_DESCRIPTION_TYPES_HPP
