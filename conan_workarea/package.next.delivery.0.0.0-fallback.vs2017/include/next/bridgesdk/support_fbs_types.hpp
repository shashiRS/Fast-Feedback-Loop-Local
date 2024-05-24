/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     support_fbs_types.hpp
 * @brief    provides all supported fbs for direct transfer as flatbuffer
 *
 **/
#ifndef NEXT_SUPPORT_FBS_TYPES_HPP
#define NEXT_SUPPORT_FBS_TYPES_HPP

#include <map>
#include <string>

class SupportedFbsTypes {
public:
  enum class supportedFBS {
    UNIDENTIFIED,
    CPU_Profiler,
    EgoVehicle,
    FOV,
    FreeGeometry,
    GroundLines,
    NetworkGraph,
    Path,
    PointCloud,
    Primitive,
    RoadSign,
    TrafficParticipant,
    Tunnel,
    SignalList,
    Custom, //< if no matching string is found we send out the fbs per default to "dataStream" with respective
            // parameters
  };

  static supportedFBS MapSchemaNameToUiType(const std::string &schema_root_name) {
    std::map<std::string, supportedFBS> fbs_table_name_to_enum{
        {"GuiSchema.EgoVehicleList", supportedFBS::EgoVehicle},
        {"GuiSchema.GroundLineList", supportedFBS::GroundLines},
        {"GuiSchema.FreeGeometryList", supportedFBS::FreeGeometry},
        {"GuiSchema.PointCloudList", supportedFBS::PointCloud},
        {"GuiSchema.PrimitiveList", supportedFBS::Primitive},
        {"GuiSchema.TrafficParticipantList", supportedFBS::TrafficParticipant},
    };

    auto index = fbs_table_name_to_enum.find(schema_root_name);
    if (index == fbs_table_name_to_enum.end()) {
      // we did not find the supported fbs -> return custom if any is given
      if (schema_root_name != "") {
        return supportedFBS::Custom;
      }
      // no structure given at all -> not supported format for g -> refuse
      return supportedFBS::UNIDENTIFIED;
    } else {
      return fbs_table_name_to_enum[schema_root_name];
    }
  }
};

#endif // NEXT_SUPPORT_FBS_TYPES_HPP
