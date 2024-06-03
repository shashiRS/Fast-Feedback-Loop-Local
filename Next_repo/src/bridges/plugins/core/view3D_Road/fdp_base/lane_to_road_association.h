// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LANE_TO_ROAD_ASSOCIATION_H_
#define FDP_BASE_LANE_TO_ROAD_ASSOCIATION_H_

#include "Platform_Types.h"

namespace fdp_base {

struct LaneToRoadAssociation {
  uint8 roadSegment;
  float32 start;
  float32 end;
};

} // namespace fdp_base

#endif // FDP_BASE_LANE_TO_ROAD_ASSOCIATION_H_