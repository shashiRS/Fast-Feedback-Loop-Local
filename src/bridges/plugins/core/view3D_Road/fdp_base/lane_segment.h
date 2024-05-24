// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LANE_SEGMENT_H_
#define FDP_BASE_LANE_SEGMENT_H_

#include "Platform_Types.h"
#include "fdp_base/direction_enum.h"
#include "fdp_base/lane_to_road_association.h"
#include "fdp_base/polyline.h"

namespace fdp_base {

struct LaneSegment {
  Polyline centerline;
  uint32 id;
  directionEnum direction;
  uint8 leftLane;
  uint8 rightLane;
  uint8 numLeftBoundaryParts;
  uint8 leftBoundaryParts[4];
  uint8 numRightBoundaryParts;
  uint8 rightBoundaryParts[4];
  uint8 numAssociatedRoadSegments;
  LaneToRoadAssociation associatedRoadSegments[4];
};

} // namespace fdp_base

#endif // FDP_BASE_LANE_SEGMENT_H_
