// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LANE_TOPOLOGY_H_
#define FDP_BASE_LANE_TOPOLOGY_H_

#include "Platform_Types.h"
#include "lane_connection.h"
#include "lane_segment.h"

namespace fdp_base {

struct LaneTopology {
  uint8 egoLane;
  float32 egoPosition;
  uint8 numLaneSegments;
  uint8 numConnections;
  LaneSegment laneSegments[5];
  LaneConnection connections[9];
};

} // namespace fdp_base

#endif // FDP_BASE_LANE_TOPOLOGY_H_
