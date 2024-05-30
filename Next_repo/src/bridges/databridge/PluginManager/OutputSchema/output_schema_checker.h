/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     output_schema_checker.h
 * @brief    parses flatbuffer schemas to check for data classses
 **/

#ifndef NEXT_DATABRIDGE_PLUGINMANAGER_OUTPUT_SCHEMA_CHECKER_H
#define NEXT_DATABRIDGE_PLUGINMANAGER_OUTPUT_SCHEMA_CHECKER_H

#include <optional>
#include <string>
#include <vector>

namespace next {
namespace databridge {
namespace plugin_manager {
enum class VersionCheck { e_equal, e_more, e_more_or_equal, e_less, e_less_or_equal };

// TODO this can be a simple function, no need for a class
class OutputSchemaChecker {

public:
  //! check whether the schema contains the following version of the following class
  bool CheckFlatbufferSchema(std::string file_name, std::string data_class_name, std::string version,
                             VersionCheck check_type = VersionCheck::e_equal) const;

private:
  //! check whether the schema contains the following attribute of the following class
  std::optional<std::string> GetAttribute(std::string path_to_fbs, std::string data_class_name,
                                          std::string attribute) const;

  std::vector<std::string> ExtendDataClasses(std::string gui_data_class_name) const;
};

} // namespace plugin_manager
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_PLUGINMANAGER_OUTPUT_SCHEMA_CHECKER_H
