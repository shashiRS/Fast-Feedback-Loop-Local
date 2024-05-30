/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     process_traffic_participants.hpp
 * @brief    general functionality used to process traffic participant data
 **/

#ifndef NEXT_PLUGINS_UTILS_PROCESS_TRAFFIC_PARTICIPIANTS_HPP
#define NEXT_PLUGINS_UTILS_PROCESS_TRAFFIC_PARTICIPIANTS_HPP

#include <algorithm>
#include <cmath>
#include <map>

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>

#include "plugin_3d_view_utils.hpp"

namespace next {
namespace plugins_utils {
namespace tpobjects {

std::string classification_participant(const int classification) {
  /* Determine the classification of objects based on the classification value
    0: "Unknown", 1: "Truck" , 2:"Car", 3:"Motorcycle", 4: "Bicycle", 5:"Pedestrian", 6: "Undecided"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification;
  try {
    object_classification = kclassification_map.at(classification);
  } catch (const std::exception &e) {
    object_classification = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification);
  }
  return std::get<0>(object_classification);
}

GuiSchema::TrafficParticipantClass flatubffer_classification_participant(const int classification) {
  /* Determine the classification of objects based on the classification value
    0: "Unknown", 1: "Truck" , 2:"Car", 3:"Motorcycle", 4: "Bicycle", 5:"Pedestrian", 6: "Undecided"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification;
  try {
    object_classification = kclassification_map.at(classification);
  } catch (const std::exception &e) {
    object_classification = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification);
  }
  return std::get<1>(object_classification);
}

std::string classification_participant_gen(const int classification) {
  /* Determine the classification of objects based on the classification value
    0: "Unknown", 1: "Truck" , 2:"Car", 3:"Motorcycle", 4: "Bicycle", 5:"Pedestrian", 6: "Undecided"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification;
  try {
    object_classification = kclassification_gen_map.at(classification);
  } catch (const std::exception &e) {
    object_classification = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification);
  }
  return std::get<0>(object_classification);
}

/// Function to classify the participants based on the value from the ars510 message
///
/// Determine the classification value from the map
/// \param [in] classification_ars : classification value from signal
/// \param [out] object_classification_ars : Classfication object from the classification value
///
std::string classification_participant_ars(const int classification_ars) {
  /* Determine the classification of objects based on the classification value
    0: "Point", 1: "Car" , 2:"Truck", 3:"Pedestrian", 4: "Motorcycle", 5:"Bicycle", 6: "Wide"
    7: "Mirror", 8: "Other", 9: "Multiple", 10: "Unclassified"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification_ars;
  try {
    object_classification_ars = kclassification_map_Ars.at(classification_ars);
  } catch (const std::exception &e) {
    object_classification_ars = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification_ars);
  }
  return std::get<0>(object_classification_ars);
}

/// Function to classify the participants based on the value from the PARKING message
///
/// Determine the classification value from the map
/// \param [in] classification_ars : classification value from signal
/// \param [out] object_classification_ars : Classfication object from the classification value
///
GuiSchema::TrafficParticipantClass flatbuffer_classification_participant_parking(const int classification_PRK) {
  /* Determine the classification of objects based on the classification value
    0: "Point", 1: "Pedestrian" , 2:"Bicycle", 3:"Motocyle", 4: "Car", 5:"Truck", 6: "Unclassified"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification_prk;
  try {
    object_classification_prk = kclassification_map_parking.at(classification_PRK);
  } catch (const std::exception &e) {
    object_classification_prk = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification_PRK);
  }
  return std::get<1>(object_classification_prk);
}

/// Function to classify the participants based on the value from the ars510 message
///
/// Determine the classification value from the map
/// \param [in] classification_ars : classification value from signal
/// \param [out] object_classification_ars : Classfication object from the classification value
///
GuiSchema::TrafficParticipantClass flatbuffer_classification_participant_ars(const int classification_ars) {
  /* Determine the classification of objects based on the classification value
    0: "Point", 1: "Car" , 2:"Truck", 3:"Pedestrian", 4: "Motorcycle", 5:"Bicycle", 6: "Wide"
    7: "Mirror", 8: "Other", 9: "Multiple", 10: "Unclassified"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::tuple<std::string, GuiSchema::TrafficParticipantClass> object_classification_ars;
  try {
    object_classification_ars = kclassification_map_Ars.at(classification_ars);
  } catch (const std::exception &e) {
    object_classification_ars = std::make_tuple("Undefined", GuiSchema::TrafficParticipantClass::None);
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification_ars);
  }
  return std::get<1>(object_classification_ars);
}

/// Function to classify the participants based on the value from the srr520 message
///
/// Determine the classification value from the map
/// \param [in] classification_srr : classification value from signal
/// \param [out] object_classification_srr : Classfication object from the classification value
///
std::string classification_participant_srr(const int classification_srr) {
  /* Determine the classification of objects based on the classification value
    0: "Point", 1: "Car" , 2:"Truck", 3:"Pedestrian", 4: "Motorcycle", 5:"Bicycle", 6: "Wide"
    7: "Unclassified", 8: "Other", 9: "Max_Diff_Types"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::string object_classification_srr;
  try {
    object_classification_srr = kclassification_map_Srr.at(classification_srr);
  } catch (const std::exception &e) {
    object_classification_srr = "Undefined";
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification_srr);
  }
  return object_classification_srr;
}

/// Function to classify the participants based on the value from the valeo message
///
/// Determine the classification value from the map
/// \param [in] classification_valeo : classification value from signal
/// \param [out] object_classification_valeo : Classfication object from the classification value
///
std::string classification_participant_valeo(const int classification_valeo) {
  /* Determine the classification of objects based on the classification value
    0: "Unknown", 1: "Truck" , 2:"Car", 3:"Motorcycle", 4: "Bicycle",
    5:"Pedestrian", 6: "Undecided"
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  std::string object_classification_valeo;
  try {
    object_classification_valeo = kclassification_map_Valeo.at(classification_valeo);
  } catch (const std::exception &e) {
    object_classification_valeo = "Undefined";
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", classification_valeo);
  }
  return object_classification_valeo;
}

/// Function to to determine the classification of objects
///
/// Determine the unified reference point corresponding to the map
/// \param [in] reference_point : reference point from the signal
/// \param [out] unified_ref_point : unified reference point determined from the reference point
///
int fdp_21_reference_point_to_unified(const int reference_point) {
  /* Determine the unified reference point based on reference point
    0 → front center→ 6 (unified reference point)
    1 → rear center → 2 (unified reference point)
    2 → rear left → 4 (unified reference point)
    3 → rear right→ 8 (unified reference point)
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  int unified_ref_point;
  try {
    unified_ref_point = kunified_point_map.at(reference_point);
  } catch (const std::exception &e) {
    unified_ref_point = -1;
    warn(__FILE__, e.what());
    warn(__FILE__, "Reference point received: {0}", reference_point);
  }
  return unified_ref_point;
}

/// Function to to determine the classification of objects
///
/// Determine the unified reference point corresponding to the map for ARS Signal
/// \param [in] reference_point_Ars : reference point from the signal
/// \param [out] unified_ref_point_ars : unified reference point determined from the reference point
///
int ars_reference_point_to_unified(const int reference_point_Ars) {
  /* Determine the unified reference point based on reference point
    0 → front center→ 6 (unified reference point)
    1 → rear center → 2 (unified reference point)
    2 → rear left → 4 (unified reference point)
    3 → rear right→ 8 (unified reference point)
    */

  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  int unified_ref_point_ars;
  try {
    unified_ref_point_ars = kunified_point_map_Ars.at(reference_point_Ars);
  } catch (const std::exception &e) {
    unified_ref_point_ars = -1;
    warn(__FILE__, e.what());
    warn(__FILE__, "Reference point received: {0}", reference_point_Ars);
  }
  return unified_ref_point_ars;
}

/// Function to to determine the classification of objects
///
/// Determine the unified reference point corresponding to the map for ARS Signal
/// \param [in] reference_point_Srr : reference point from the signal
/// \param [out] unified_ref_point_srr : unified reference point determined from the reference point
///
int srr_reference_point_to_unified(const int reference_point_Srr) {
  /* Determine the unified reference point based on reference point
    0 → front center→ 5 (unified reference point)
    1 → rear center → 7 (unified reference point)
    2 → rear left → 3 (unified reference point)
    3 → rear right→ 1 (unified reference point)
    */
  /// Based on the information given in:
  /// https://confluence-adas.zone2.agileci.conti.de/pages/viewpage.action?spaceKey=TRC&title=3D+Traffic+Participants+visualization
  int unified_ref_point_srr;
  try {
    unified_ref_point_srr = kunified_point_map_Srr.at(reference_point_Srr);
  } catch (const std::exception &e) {
    unified_ref_point_srr = -1;
    warn(__FILE__, e.what());
    warn(__FILE__, "Classification received: {0}", reference_point_Srr);
  }
  return unified_ref_point_srr;
}

// void calculate_geometry_fdp_base_objects(const fdp_base::CEM_TP_t_Object fdp_base_obj, const float p_x, const float
// p_y,
//                                         float &r_x, float &r_y, float &r_unified_yaw, float &length, float &width) {
//
//  float point1_x = p_x + fdp_base_obj.shapePoints.aShapePointCoordinates[0].fPosX;
//  float point1_y = p_y + fdp_base_obj.shapePoints.aShapePointCoordinates[0].fPosY;
//
//  float point2_x = p_x + fdp_base_obj.shapePoints.aShapePointCoordinates[1].fPosX;
//  float point2_y = p_y + fdp_base_obj.shapePoints.aShapePointCoordinates[1].fPosY;
//
//  float point4_x = p_x + fdp_base_obj.shapePoints.aShapePointCoordinates[3].fPosX;
//  float point4_y = p_y + fdp_base_obj.shapePoints.aShapePointCoordinates[3].fPosY;
//
//  r_x = p_x;
//  r_y = p_y;
//
//  length = std::sqrtf(std::pow((point4_x - point1_x), 2) + std::pow((point4_y - point1_y), 2));
//  width = std::sqrtf(std::pow((point2_x - point1_x), 2) + std::pow((point2_y - point1_y), 2));
//
//  try {
//    float angle = atan2((point4_y - point1_y), (point4_x - point1_x));
//    r_unified_yaw = angle; // Angle is in radians
//  } catch (...) {
//    r_unified_yaw = 0;
//  }
//}

/// calculates the unified reference points.
///
/// Calculation of unified data reference for position, yaw, length and width with respect to reference point
/// \param [in] p_x : longitudinal position
/// \param [in] p_y : lateral position
/// \param [in] yaw : yaw angle of the car
/// \param [in] length : length of the car
/// \param [in] width : width of the car
/// \param [out] r_x : longitudinal position of reference point with respect to center rear axle ego
/// \param [out] r_y : lateral position of reference point with respect to center rear axle ego
/// \param [out] r_unified_yaw : yaw angle of the car with respect to center rear axle ego
/// \param [out] r_length : length of the car with respect to center rear axle ego
/// \param [out] r_width : width of the car with respect to center rear axle ego
///
void calculate_unified_data(const int unified_ref_point, const float p_x, const float p_y, const float yaw,
                            const float length, const float width, float &r_x, float &r_y, float &r_unified_yaw,
                            float &r_length, float &r_width) {
  /* Calculation of unified data reference for position, yaw, length and width with respect to reference point
    p_x = longitudinal position of reference point with respect to center rear axle ego
    p_y = lateral position of reference point with respect to center rear axle ego
    yaw = yaw angle of the car
    length = length of the car
    width = width of the car
    */
  float point1_x = 0;
  float point1_y = 0;
  float point4_x = 0;
  float point4_y = 0;
  float point5_x = 0;
  float point5_y = 0;

  /* Calculate the position, yaw, length and width based on unified reference point*/

  switch (unified_ref_point) {
  case 1:
    point1_x = p_x;
    point1_y = p_y;
    point4_x = p_x + length * cosf(yaw);
    point4_y = p_y + length * sinf(yaw);
    point5_x = p_x + length / 2.f * cosf(yaw) + width / 2.f * sinf(yaw);
    point5_y = p_y + length / 2.f * sinf(yaw) + width / 2.f * cosf(yaw);
    break;
  case 2:
    point1_x = p_x + width / 2.f * sinf(yaw);
    point1_y = p_y - width / 2.f * cosf(yaw);
    point4_x = p_x + length * cosf(yaw) + width / 2.f * sinf(yaw);
    point4_y = p_y + length * sinf(yaw) - width / 2.f * cosf(yaw);
    point5_x = p_x + length / 2.f * cosf(yaw);
    point5_y = p_y + length / 2.f * sinf(yaw);
    break;
  case 3:
    point1_x = p_x + width * sinf(yaw);
    point1_y = p_y - width * cosf(yaw);
    point4_x = p_x + length * cosf(yaw) + width * sinf(yaw);
    point4_y = p_y + length * sinf(yaw) - width * cosf(yaw);
    point5_x = p_x + length / 2.f * cosf(yaw) + width / 2.f * sinf(yaw);
    point5_y = p_y + length / 2.f * sinf(yaw) - width / 2.f * cosf(yaw);
    break;
  case 4:
    point1_x = p_x - length / 2.f * cosf(yaw) + width * sinf(yaw);
    point1_y = p_y - length / 2.f * sinf(yaw) - width * cosf(yaw);
    point4_x = p_x + length / 2.f * cosf(yaw) + width * sinf(yaw);
    point4_y = p_y + length / 2.f * sinf(yaw) - width * cosf(yaw);
    point5_x = p_x + width / 2.f * sinf(yaw);
    point5_y = p_y - width / 2.f * cosf(yaw);
    break;
  case 5:
    point1_x = p_x - length * cosf(yaw) + width * sinf(yaw);
    point1_y = p_y - length * sinf(yaw) - width * cosf(yaw);
    point4_x = p_x + width * sinf(yaw);
    point4_y = p_y - width * cosf(yaw);
    point5_x = p_x - length / 2.f * cosf(yaw) + width / 2.f * sinf(yaw);
    point5_y = p_y - length / 2.f * sinf(yaw) - width / 2.f * cosf(yaw);
    break;
  case 6:
    point1_x = p_x - length * cosf(yaw) + width / 2.f * sinf(yaw);
    point1_y = p_y - width / 2.f * cosf(yaw) - length * sinf(yaw);
    point4_x = p_x + width / 2.f * sinf(yaw);
    point4_y = p_y - width / 2.f * cosf(yaw);
    point5_x = p_x - length / 2.f * cosf(yaw);
    point5_y = p_y - length / 2.f * sinf(yaw);
    break;
  case 7:
    point1_x = p_x - length * cosf(yaw);
    point1_y = p_y - length * sinf(yaw);
    point4_x = p_x;
    point4_y = p_y;
    point5_x = p_x - length / 2.f * cosf(yaw) - width / 2.f * sinf(yaw);
    point5_y = p_y + width / 2.f * cosf(yaw) - length / 2.f * sinf(yaw);
    break;
  case 8:
    point1_x = p_x - length / 2.f * cosf(yaw);
    point1_y = p_y - length / 2.f * sinf(yaw);
    point4_x = p_x + length / 2.f * cosf(yaw);
    point4_y = p_y + length / 2.f * sinf(yaw);
    point5_x = p_x - width / 2.f * sinf(yaw);
    point5_y = p_y + width / 2.f * cosf(yaw);
    break;
  case 9: {
    float ad = 1.f; // assuming the rear axle is 1m away from the bumper
    point1_x = p_x - ad * cosf(yaw) + width / 2.f * sinf(yaw);
    point1_y = p_y - ad * sinf(yaw) - width / 2.f * cosf(yaw);
    point4_x = p_x + (length - ad) * cosf(yaw) + width / 2.f * sinf(yaw);
    point4_y = p_y - width / 2.f * cosf(yaw) + (length - ad) * sinf(yaw);
    point5_x = p_x + (length / 2 - ad) * cosf(yaw);
    point5_y = p_y + (length / 2 - ad) * sinf(yaw);
    break;
  }
  case 10:
    point1_x = p_x - length / 2.f * cosf(yaw) + width / 2.f * sinf(yaw);
    point1_y = p_y - width / 2.f * cosf(yaw) - length / 2.f * sinf(yaw);
    point4_x = p_x + length / 2.f * cosf(yaw) + width / 2.f * sinf(yaw);
    point4_y = p_y - width / 2.f * cosf(yaw) + length / 2.f * sinf(yaw);
    point5_x = p_x;
    point5_y = p_y;
    break;
  case 11: // prop never used
  {
    float ad = 1.f; // assuming the front axle is 1m away from the bumper
    point1_x = p_x - (length - ad) * cosf(yaw) + width / 2.f * sinf(yaw);
    point1_y = p_y - (length - ad) * sinf(yaw) - width / 2.f * cosf(yaw);
    point4_x = p_x + ad * cosf(yaw) + width / 2.f * sinf(yaw);
    point4_y = p_y - width / 2.f * cosf(yaw) + ad * sinf(yaw);
    point5_x = p_x - (length / 2 - ad) * cosf(yaw);
    point5_y = p_y - (length / 2 - ad) * sinf(yaw);
    break;
  }
  default:
    break;
  }
  // If unified_ref_point is not within given parameters
  // then use default value of r_unified_yaw
  if (unified_ref_point >= 1 && unified_ref_point <= 11) {
    try {
      float angle = atan2f((point4_y - point1_y), (point4_x - point1_x));
      r_unified_yaw = angle; // Angle is in radians
    } catch (...) {
      r_unified_yaw = 0;
    }
  } else {
    r_unified_yaw = 0;
  }

  /* The position, yaw, length and width returned */
  r_x = point5_x;
  r_y = point5_y;
  r_length = std::max<float>(length, 0.2f);
  r_width = std::max<float>(width, 0.2f);
}

} // namespace tpobjects
} // namespace plugins_utils
} // namespace next
#endif // NEXT_PLUGINS_UTILS_PROCESS_TRAFFIC_PARTICIPIANTS_HPP
