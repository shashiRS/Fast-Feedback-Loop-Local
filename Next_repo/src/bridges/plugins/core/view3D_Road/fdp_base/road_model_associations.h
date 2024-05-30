// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_MODEL_ASSOCIATIONS_H_
#define FDP_BASE_ROAD_MODEL_ASSOCIATIONS_H_

#include "Platform_Types.h"
#include "boundary_continuity.h"
#include "curvature_association.h"
#include "lane_count_association.h"
#include "sensor_association.h"
#include "slope_association.h"

namespace fdp_base {

struct RoadModelAssociations {
  uint8 numCurvatures;
  uint8 numSlopes;
  uint8 numSensorAssociations;
  uint8 numBoundaryContinuity;
  uint8 numLaneCounts;
  LaneCountAssociation laneCounts[20];
  CurvatureAssociation curvatures[20];
  SlopeAssociation slopes[20];
  SensorAssociation sensorAssociations[20];
  BoundaryContinuity boundaryContinuity[40];
};

} // namespace fdp_base

#endif // FDP_BASE_ROAD_MODEL_ASSOCIATIONS_H_
