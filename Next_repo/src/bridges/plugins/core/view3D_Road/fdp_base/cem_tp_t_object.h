// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJECT_H_
#define FDP_BASE_CEM_TP_T_OBJECT_H_

#include "cem_tp_t_add_obj_classification.h"
#include "cem_tp_t_add_obj_kinematics.h"
#include "cem_tp_t_obj_classification.h"
#include "cem_tp_t_obj_correlations.h"
#include "cem_tp_t_obj_dynamic_property.h"
#include "cem_tp_t_obj_general.h"
#include "cem_tp_t_obj_geometry.h"
#include "cem_tp_t_obj_intention.h"
#include "cem_tp_t_obj_kinematics.h"
#include "cem_tp_t_obj_lane_status.h"
#include "cem_tp_t_obj_occlusion.h"
#include "cem_tp_t_obj_qualifiers.h"
#include "cem_tp_t_obj_shape_points.h"
#include "cem_tp_t_obj_split_merge.h"
#include "sensor_bitfield.h"

namespace fdp_base {

/// Additional Object Attributes
struct CEM_TP_t_Object {
  /**
  General information about the object, eg. lifetime, id (TP_OBJ_ATTRIBUTE_GENERAL)

  */
  CEM_TP_t_ObjGeneral general;
  /**
  Object splitting and merging related information. (TP_OBJ_ATTRIBUTE_SPLIT_MERGE)

  */
  CEM_TP_t_ObjSplitMerge splitMerge;
  /**
  In order to have the information of which sensor contributed
to a given output type (sensor or fused detection), a sensor
coding will be used having a compressed version in the form of
a 32-bit-field where each bit represents a sensor

  */
  SensorBitfield contributingSensors;
  /**
  Kinematic information. (TP_OBJ_ATTRIBUTE_KINEMATICS)

  */
  CEM_TP_t_ObjKinematics kinematic;
  /**
  Additional kinematic information. (TP_OBJ_ATTRIBUTE_ADD_KINEMATICS)

  */
  CEM_TP_t_AddObjKinematics addKinematic;
  /**
  Correlations between the absolute state variables. (TP_OBJ_ATTRIBUTE_ABS_CORRELATION)

  */
  CEM_TP_t_ObjCorrelations correlationAbs;
  /**
  Object size related properties. (TP_OBJ_ATTRIBUTE_SHAPE_POINTS)

  */
  CEM_TP_t_ObjShapePoints shapePoints;
  /**
  Object size related properties. (TP_OBJ_ATTRIBUTE_GEOMETRY)

  */
  CEM_TP_t_ObjGeometry geometry;
  /**
  Object probability to be of a specific kind. (TP_OBJ_ATTRIBUTE_CLASSIFICATION)

  */
  CEM_TP_t_ObjClassification classification;
  /**
  Object probability for specific object class. (TP_OBJ_ATTRIBUTE_ADD_CLASSIFICATION)

  */
  CEM_TP_t_AddObjClassification addClassification;
  /**
  Dynamic property of the object (TP_OBJ_ATTRIBUTE_DYN_PROP)

  */
  CEM_TP_t_ObjDynamicProperty dynamicProperty;
  /**
  Specific attributes regarding intension of other drivers. (TP_OBJ_ATTRIBUTE_INTENTION)

  */
  CEM_TP_t_ObjIntention intension;
  /**
  Lane specific attributes. (TP_OBJ_ATTRIBUTE_LANE_STATUS)

  */
  CEM_TP_t_ObjLaneStatus laneStatus;
  /**
  Describes if/how Object is covered by closer objects in sensing
direction. (TP_OBJ_ATTRIBUTE_OCCLUSION)

  */
  CEM_TP_t_ObjOcclusion occlusion;
  /**
  General object qualifiers. (TP_OBJ_ATTRIBUTE_QUALIFIERS)

  */
  CEM_TP_t_ObjQualifiers qualifiers;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJECT_H_
