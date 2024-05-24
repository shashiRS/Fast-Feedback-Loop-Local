#ifndef NEXT_PLUGINS_DATA_BRIDGE_COMMON_H_
#define NEXT_PLUGINS_DATA_BRIDGE_COMMON_H_

//===-- data_bridge_common.hpp - common constants and variable for DataBridgeBackendNode --*- C++ -*-===//
///
/// \file
/// This file contains the definition of common constants and variable and
/// data structures need in the DataBridgeBackendNode
///
//===----------------------------------------------------------------------===//

#include <map>

///  Map traffic participants classification to a color to be displayed in GUI
const std::map<std::string, std::string> sensor_color_map{
    {"Truck", "#5a0159"},          {"Car", "#434cf2"},       {"Motorcycle", "#4a02ad"},   {"Bicycle", "#b502fe"},
    {"Pedestrian", "#ae6c43"},     {"Point", "#ffdd00"},     {"Wide", "#00976a"},         {"Mirror", "#9d6193"},
    {"Multiple", "#ff6a00"},       {"Other", "#ff6a00"},     {"Unclassified", "#ff6a00"}, {"Unknown", "#ff6a00"},
    {"Max_Diff_Types", "#ff6a00"}, {"Undecided", "#ff6a00"}, {"Undefined", "#990000"}};

/*! \brief  Map the unified reference point based on reference point.
 *
 * Determine the unified reference point based on reference point
 * | Sensor frame | point location | unified reference frame |
 * | point index  |                | point index             |
 * |--------------|----------------|-------------------------|
 * | 0            | front center   | 6                       |
 * | 1            | rear center    | 2                       |
 * | 2            | rear left      | 4                       |
 * | 3            | rear right     | 8                       |
 *
 * Based on the information given in:
 * \see {  }
 */
const std::map<int, int> kunified_point_map{{0, 6}, {1, 2}, {2, 4}, {3, 8}};

/*! \brief Map traffic participants based on classification value.
 *
 *  Determine the classification of objects based on the classification value:
 * | ID           | 0       | 1     | 2   | 3          | 4       | 5          | 6         |
 * | object class | Unknown | Truck | Car | Motorcycle | Bicycle | Pedestrian | Undecided |
 *
 * Based on the information given in:
 * \see { }
 */
const std::map<int, std::string> kclassification_map{
    {0, "Unknown"}, {1, "Truck"}, {2, "Car"}, {3, "Motorcycle"}, {4, "Bicycle"}, {5, "Pedestrian"}, {6, "Undecided"}};

const std::map<int, std::string> kclassification_gen_map{{0, "Point"},      {1, "Car"},        {2, "Truck"},
                                                         {3, "Pedestrian"}, {4, "Motorcycle"}, {5, "Bicycle"},
                                                         {6, "Wide"},       {7, "Undefined"}};

const int kTrafficParticipantsCount = 100; ///< Maximum number of traffic Participants
const int kArsObjectPair = 20;             ///< Maximum number of object Pairs for ARS Signal
const int kSrrObjectPair = 80;             ///< Maximum number of object Pairs for SRR Signal
const int kValeoObjectPair = 12;           ///< Maximum number of object Pairs for Valeo Signal
const int kArsPair = 2;                    ///< Number of pairs in a signal in ARS
const int kSrrPair = 2;                    ///< Number of pairs in a signal in SRR

/*! \brief  Map the unified reference point based on reference point for ARS Signal.
 *
 * Determine the unified reference point based on reference point
 * | Sensor frame | point location | unified reference frame |
 * | point index  |                | point index             |
 * |--------------|----------------|-------------------------|
 * | 0            | front center   | 6                       |
 * | 1            | rear center    | 2                       |
 * | 2            | rear left      | 4                       |
 * | 3            | rear right     | 8                       |
 *
 * Based on the information given in:
 * \see {
 * https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
 * }
 */
const std::map<int, int> kunified_point_map_Ars{{0, 6}, {1, 2}, {2, 4}, {3, 8}};

// clang-format off
/*! \brief Map traffic participants based on classification value for ars510 Signal.
 *
 *  Determine the classification of objects based on the classification value:
 * ID           | 0     | 1   | 2     | 3          | 4          | 5       | 6    | 7      | 8     | 9        | 10           |
 * object class | Point | Car | Truck | Pedestrian | Motorcycle | Bicycle | Wide | Mirror | Other | Multiple | Unclassified |
 *
 * Based on the information given in:
 * \see {
 * https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
 * }
 */
// clang-format on
const std::map<int, std::string> kclassification_map_Ars{
    {0, "Point"}, {1, "Car"},    {2, "Truck"}, {3, "Pedestrian"}, {4, "Motorcycle"},   {5, "Bicycle"},
    {6, "Wide"},  {7, "Mirror"}, {8, "Other"}, {9, "Multiple"},   {10, "Unclassified"}};

/*! \brief Map the unified reference point based on reference point for SRR Signal.
 *
 * Determine the unified reference point based on reference point
 * | Sensor frame | point location | unified reference frame |
 * | point index  |                | point index             |
 * |--------------|----------------|-------------------------|
 * | 0            |                | 5                       |
 * | 1            |                | 7                       |
 * | 2            |                | 3                       |
 * | 3            |                | 1                       |
 *
 * Based on the information given in:
 * \see {  }
 */
const std::map<int, int> kunified_point_map_Srr{{0, 5}, {1, 7}, {2, 3}, {3, 1}};

// clang-format off
/*! \brief Map traffic participants based on classification value for SRR Signal.
 *
 *  Determine the classification of objects based on the classification value:
 * ID           | 0     | 1   | 2     | 3          | 4          | 5       | 6    | 7            | 8     | 9              |
 * object class | Point | Car | Truck | Pedestrian | Motorcycle | Bicycle | Wide | Unclassified | Other | Max_Diff_Types |
 *
 * Based on the information given in:
 * \see {  }
 */
// clang-format on
const std::map<int, std::string> kclassification_map_Srr{
    {0, "Point"},   {1, "Car"},  {2, "Truck"},        {3, "Pedestrian"}, {4, "Motorcycle"},
    {5, "Bicycle"}, {6, "Wide"}, {7, "Unclassified"}, {8, "Other"},      {9, "Max_Diff_Types"}};

/*! \brief Map traffic participants based on classification value for Valeo signals.
 *
 *  Determine the classification of objects based on the classification value:
 * | ID           | 0       | 1     | 2   | 3          | 4       | 5          | 6         |
 * | object class | Unknown | Truck | Car | Motorcycle | Bicycle | Pedestrian | Undecided |
 *
 * Based on the information given in:
 * \see { }
 */
const std::map<int, std::string> kclassification_map_Valeo{
    {0, "Unknown"}, {1, "Truck"}, {2, "Car"}, {3, "Motorcycle"}, {4, "Bicycle"}, {5, "Pedestrian"}, {6, "Undecided"}};

#endif // NEXT_PLUGINS_DATA_BRIDGE_COMMON_H_
