// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_SEGMENT_H_
#define FDP_BASE_ROAD_SEGMENT_H_

#include "Platform_Types.h"
#include "fdp_base/polyline.h"
#include "fdp_base/road_to_lane_association.h"
#include "fdp_base/road_type_enum.h"

namespace fdp_base {

struct RoadSegment {
  Polyline centerline;
  boolean isMpp;
  roadTypeEnum roadType;
  uint8 numLeftBoundaryParts;
  uint8 leftBoundaryParts[4];
  uint8 numRightBoundaryParts;
  uint8 rightBoundaryParts[4];
  uint8 numAssociatedLaneSegments;
  RoadToLaneAssociation associatedLaneSegments[4];
};

} // namespace fdp_base

#endif // FDP_BASE_ROAD_SEGMENT_H_
