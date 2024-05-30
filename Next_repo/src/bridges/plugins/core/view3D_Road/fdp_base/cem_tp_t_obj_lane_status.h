// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_LANE_STATUS_H_
#define FDP_BASE_CEM_TP_T_OBJ_LANE_STATUS_H_

#include "cem_percentage_t.h"
#include "cem_tp_t_obj_associated_lane.h"
#include "cem_tp_t_obj_lane_change.h"

namespace fdp_base {

/// Lane specific attributes
struct CEM_TP_t_ObjLaneStatus {
  /**
  @unit{m}

  Mapping of the object to a lane

  */
  CEM_TP_t_ObjAssociatedLane eAssociatedLane;
  /**
  @range{0,100}

  Trust that status of eAssociatedLane is correct

  */
  CEM_percentage_t uiAssociatedLaneConfidence;
  /**
  Object lane change maneuver in respect to the objects associated
lane.

  */
  CEM_TP_t_ObjLaneChange eLaneChangeManeuver;
  /**
  @range{0,100}

  This parameter determines the object relative width in percent
overlapping the associated driving tube.

  */
  CEM_percentage_t uiPercentageOwnDrivingLane;
  /**
  @range{0,100}

  This parameter determines the object relative width in percent
overlapping the lane left to the associated driving tube.

  */
  CEM_percentage_t uiPercentageSideDrivingLaneLeft;
  /**
  @range{0,100}

  This parameter determines the object relative width in percent
overlapping the lane right to the associated driving tube.

  */
  CEM_percentage_t uiPercentageSideDrivingLaneRight;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_LANE_STATUS_H_
