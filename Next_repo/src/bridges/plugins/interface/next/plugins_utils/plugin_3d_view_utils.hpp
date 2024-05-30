/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_3d_view_utils.hpp
 * @brief    general functionality used within 3D view plugins
 **/

#ifndef NEXT_PLUGINS_UTILS_PLUGIN_3D_VIEW_UTILS_HPP
#define NEXT_PLUGINS_UTILS_PLUGIN_3D_VIEW_UTILS_HPP

#include <map>
#include <tuple>

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>

namespace next {
namespace plugins_utils {
namespace tpobjects {

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
const std::map<int, std::tuple<std::string, GuiSchema::TrafficParticipantClass>> kclassification_map{
    {0, std::make_tuple("Unknown", GuiSchema::TrafficParticipantClass::None)},
    {1, std::make_tuple("Truck", GuiSchema::TrafficParticipantClass::truck)},
    {2, std::make_tuple("Car", GuiSchema::TrafficParticipantClass::car)},
    {3, std::make_tuple("Motorcycle", GuiSchema::TrafficParticipantClass::motorcycle)},
    {4, std::make_tuple("Bicycle", GuiSchema::TrafficParticipantClass::bicycle)},
    {5, std::make_tuple("Pedestrian", GuiSchema::TrafficParticipantClass::pedestrian)},
    {6, std::make_tuple("Undecided", GuiSchema::TrafficParticipantClass::None)}};

const std::map<int, std::tuple<std::string, GuiSchema::TrafficParticipantClass>> kclassification_gen_map{
    {0, std::make_tuple("Point", GuiSchema::TrafficParticipantClass::None)},
    {1, std::make_tuple("Car", GuiSchema::TrafficParticipantClass::car)},
    {2, std::make_tuple("Truck", GuiSchema::TrafficParticipantClass::truck)},
    {3, std::make_tuple("Pedestrian", GuiSchema::TrafficParticipantClass::pedestrian)},
    {4, std::make_tuple("Motorcycle", GuiSchema::TrafficParticipantClass::motorcycle)},
    {5, std::make_tuple("Bicycle", GuiSchema::TrafficParticipantClass::bicycle)},
    {6, std::make_tuple("Wide", GuiSchema::TrafficParticipantClass::None)},
    {7, std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None)}};

const unsigned int kTrafficParticipantsCount = 100; ///< Maximum number of traffic Participants
const unsigned int kArsObjectPair = 20;             ///< Maximum number of object Pairs for ARS Signal
const unsigned int kSrrObjectPair = 80;             ///< Maximum number of object Pairs for SRR Signal
const unsigned int kValeoObjectPair = 12;           ///< Maximum number of object Pairs for Valeo Signal
const unsigned int kArsPair = 2;                    ///< Number of pairs in a signal in ARS
const unsigned int kSrrPair = 2;                    ///< Number of pairs in a signal in SRR

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
// clang format on
const std::map<int, std::tuple<std::string, GuiSchema::TrafficParticipantClass>> kclassification_map_Ars{
    {0, std::make_tuple("Point", GuiSchema::TrafficParticipantClass::None)},
    {1, std::make_tuple("Car", GuiSchema::TrafficParticipantClass::car)},
    {2, std::make_tuple("Truck", GuiSchema::TrafficParticipantClass::truck)},
    {3, std::make_tuple("Pedestrian", GuiSchema::TrafficParticipantClass::pedestrian)},
    {4, std::make_tuple("Motorcycle", GuiSchema::TrafficParticipantClass::motorcycle)},
    {5, std::make_tuple("Bicycle", GuiSchema::TrafficParticipantClass::bicycle)},
    {6, std::make_tuple("Wide", GuiSchema::TrafficParticipantClass::None)},
    {7, std::make_tuple("Mirror", GuiSchema::TrafficParticipantClass::None)},
    {8, std::make_tuple("Other", GuiSchema::TrafficParticipantClass::None)},
    {9, std::make_tuple("Multiple", GuiSchema::TrafficParticipantClass::None)},
    {10, std::make_tuple("Unclassified", GuiSchema::TrafficParticipantClass::None)},
};

// clang-format off
/*! \brief Map traffic participants based on classification value for PARKING Signal.
 *
 *  Determine the classification of objects based on the classification value:
 * ID           | 0     | 1          | 2       | 3        | 4   | 5     | 6             |
 * object class | Point | Pedestrian | Bicycle | Motocyle | Car | Truck | Unclassified  |
 *
 * Based on the information given in:
 * \see {
 * https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
 * }
 */
// clang format on
const std::map<int, std::tuple<std::string, GuiSchema::TrafficParticipantClass>> kclassification_map_parking{
    {0, std::make_tuple("Point", GuiSchema::TrafficParticipantClass::None)},
    {1, std::make_tuple("Pedestrian", GuiSchema::TrafficParticipantClass::pedestrian)},
    {2, std::make_tuple("Bicycle", GuiSchema::TrafficParticipantClass::bicycle)},
    {3, std::make_tuple("Motocyle", GuiSchema::TrafficParticipantClass::motorcycle)},
    {4, std::make_tuple("Car", GuiSchema::TrafficParticipantClass::car)},
    {5, std::make_tuple("Truck", GuiSchema::TrafficParticipantClass::truck)},
    {6, std::make_tuple("Unclassified", GuiSchema::TrafficParticipantClass::None)},
};

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

/*! \brief Map traffic participants based on classification value for SRR Signal.
 *
 *  Determine the classification of objects based on the classification value:
 * ID           | 0     | 1   | 2     | 3          | 4          | 5       | 6    | 7            | 8     | 9              |
 * object class | Point | Car | Truck | Pedestrian | Motorcycle | Bicycle | Wide | Unclassified | Other | Max_Diff_Types |
 *
 * Based on the information given in:
 * \see {  }
 */
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

} // namespace tpobjects
} // namespace plugins_utils
} // namespace next
#endif // NEXT_PLUGINS_UTILS_PLUGIN_3D_VIEW_UTILS_HPP
