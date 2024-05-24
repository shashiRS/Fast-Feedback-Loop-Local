// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_POINT_H_
#define FDP_BASE_ROAD_POINT_H_

#include "Platform_Types.h"

namespace fdp_base {

struct RoadPoint {
  float32 x;
  float32 y;
  float32 lateralVariance;
  float32 headingYaw;
};

} // namespace fdp_base

#endif // FDP_BASE_ROAD_POINT_H_
