// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_TOPOLOGY_H_
#define FDP_BASE_ROAD_TOPOLOGY_H_

#include "Platform_Types.h"
#include "road_connection.h"
#include "road_segment.h"

namespace fdp_base {

struct RoadTopology {
  uint8 egoRoadIndex;
  float32 egoPosition;
  uint8 numRoadSegments;
  uint8 numConnections;
  RoadSegment roadSegments[10];
  RoadConnection connections[9];
};

} // namespace fdp_base

#endif // FDP_BASE_ROAD_TOPOLOGY_H_
