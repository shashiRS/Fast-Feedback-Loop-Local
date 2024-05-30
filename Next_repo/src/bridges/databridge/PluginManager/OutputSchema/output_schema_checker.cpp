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

#include "output_schema_checker.h"

#include <boost/filesystem.hpp>

#include <flatbuffers/idl.h>

#include "version.h"

namespace next {
namespace databridge {
namespace plugin_manager {

std::optional<std::string> OutputSchemaChecker::GetAttribute(std::string path_to_fbs, std::string data_class_name,
                                                             std::string attribute) const {
  // load FlatBuffer schema (.fbs) from disk
  std::string schemafile;
  bool ok = flatbuffers::LoadFile(path_to_fbs.c_str(), false, &schemafile);
  if (!ok) {
    printf("couldn't load the file!\n");
    return std::nullopt;
  }
  // parse schema first, so we can use it to parse the data after
  flatbuffers::Parser parser;

  boost::filesystem::path file_path(path_to_fbs);
  boost::filesystem::path file_dir = file_path.parent_path();
  std::string file_dir_string = file_dir.string();

  const char *include_directories[] = {file_dir_string.c_str(), nullptr};

  ok = parser.Parse(schemafile.c_str(), include_directories);
  if (!ok) {
    printf("The file has a wrong struncture!\n");
    return std::nullopt;
  }

  auto flatbuffers_struct = parser.LookupStruct(data_class_name);
  if (flatbuffers_struct != nullptr) {
    if (auto flatbuffers_vers_attr = flatbuffers_struct->attributes.Lookup(attribute)) {
      return flatbuffers_vers_attr->constant;
    }
  }
  return std::nullopt;
}

std::vector<std::string> OutputSchemaChecker::ExtendDataClasses(std::string gui_data_class_name) const {
  std::vector<std::string> return_list;
  return_list.push_back(gui_data_class_name);

  if (gui_data_class_name == "trafficParticipants") {
    return_list.push_back("GuiSchema.TrafficParticipant");
  }

  if (gui_data_class_name == "groundLines") {
    return_list.push_back("GuiSchema.GroundLine");
  }

  if (gui_data_class_name == "pointClouds") {
    return_list.push_back("GuiSchema.PointCloud");
  }

  if (gui_data_class_name == "primitives") {
    return_list.push_back("GuiSchema.PrimitiveList");
    return_list.push_back("General3dView");
    return_list.push_back("GuiSchema.FreeGeometry");
  }

  if (gui_data_class_name == "egoVehicle") {
    return_list.push_back("GuiSchema.EgoVehicle");
  }

  if (gui_data_class_name == "CpuProfiler") {
    return_list.push_back("GuiSchema.CpuProfiler");
  }

  if (gui_data_class_name == "freeGeometry") {
    return_list.push_back("GuiSchema.FreeGeometry");
  }

  return return_list;
}

bool OutputSchemaChecker::CheckFlatbufferSchema(std::string file_name, std::string data_class_name_gui,
                                                std::string version, VersionCheck check_type) const {

  auto extended_class_names = OutputSchemaChecker::ExtendDataClasses(data_class_name_gui);

  for (auto data_class_name : extended_class_names) {
    auto response = GetAttribute(file_name, data_class_name, "version");
    if (response.has_value()) {
      if (check_type == VersionCheck::e_equal && response.value() == version) {
        return true;
      } else if (check_type == VersionCheck::e_more) {
        return Version(response.value()) > Version(version);
      } else if (check_type == VersionCheck::e_more_or_equal) {
        return Version(response.value()) >= Version(version);
      } else if (check_type == VersionCheck::e_less) {
        return Version(response.value()) < Version(version);
      } else if (check_type == VersionCheck::e_less_or_equal) {
        return Version(response.value()) <= Version(version);
      }
    }
  }
  return false;
}

} // namespace plugin_manager
} // namespace databridge
} // namespace next
