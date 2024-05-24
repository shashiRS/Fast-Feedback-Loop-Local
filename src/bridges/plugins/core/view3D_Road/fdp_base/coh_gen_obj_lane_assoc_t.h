// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_COH_GEN_OBJ_LANE_ASSOC_T_H_
#define FDP_BASE_COH_GEN_OBJ_LANE_ASSOC_T_H_

#include "Platform_Types.h"
#include "fdp_base/cem_percentage_t.h"
#include "fdp_base/coh_lane_assoc_state_t.h"
#include "fdp_base/lane_assignment_t.h"
#include "fdp_base/relative_lane_assignment_probabilities.h"

namespace fdp_base {

/// GenObject to lane association
struct COH_GenObjLaneAssoc_t {
  /**
  State of the lane assignment

  */
  CohLaneAssocState_t state;
  /**
  ID

  */
  uint32 assocId;
  /**
  State of the lane assignment

  */
  LaneAssignment_t laneAssignment;
  /**
  @range{0,100}

  Lane assignment confidence for a TP, in percentage

  */
  CEM_percentage_t laneAssignmentConfidence;
  /**
  Probabilities for lane assignment to different areas on the road

  */
  RelativeLaneAssignmentProbabilities relativeLaneAssignmentProbabilities;
  /**
  index of the GenObjec in the GentObjectList

  */
  uint16 genObjectId;
  /**
  @range{0,10}

  number of associated lanes

  */
  uint16 numAssocLanes;
  /**
  array of the index of the associated lanes

  */
  uint16 laneId[10];
  /**
  @range{0,100}

  array of the percentage of the TP that is associated to that
Lane

  */
  CEM_percentage_t overlap[10];
  /**
  @range{0,100}

  array of the confidence of the TP that is associated to that
Lane, in percentage

  */
  CEM_percentage_t confidence[10];
};

} // namespace fdp_base

#endif // FDP_BASE_COH_GEN_OBJ_LANE_ASSOC_T_H_
