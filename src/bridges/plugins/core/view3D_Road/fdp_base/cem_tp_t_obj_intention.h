// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_INTENTION_H_
#define FDP_BASE_CEM_TP_T_OBJ_INTENTION_H_

#include "cem_percentage_t.h"
#include "cem_tp_t_obj_status_brake_light.h"
#include "cem_tp_t_obj_status_turn_indicator.h"

namespace fdp_base {

/// Specific attributes regarding intention of other drivers
struct CEM_TP_t_ObjIntention {
  /**
  Status of brake light detection

  */
  CEM_TP_t_ObjStatusBrakeLight eStatusBrakeLight;
  /**
  @range{0,100}

  Trust that status of eStatusBrakeLight is correct

  */
  CEM_percentage_t uiBrakeLightConfidence;
  /**
  Status of turn indicator detection

  */
  CEM_TP_t_ObjStatusTurnIndicator eStatusTurnIndicator;
  /**
  @range{0,100}

  Trust that status of eStatusTurnIndicator is correct

  */
  CEM_percentage_t uiTurnIndicatorConfidence;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_INTENTION_H_
