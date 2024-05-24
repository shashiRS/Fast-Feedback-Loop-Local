// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_GENERAL_H_
#define FDP_BASE_CEM_TP_T_OBJ_GENERAL_H_

#include "Platform_Types.h"
#include "cem_algo_cycle_counter_t.h"
#include "cem_algo_data_time_stamp_t.h"
#include "cem_tp_t_obj_maintenance_states.h"

namespace fdp_base {

/// General information about an object
struct CEM_TP_t_ObjGeneral {
  /**
  Timestamp of objects last measurement update. Compare to ObjectList::sSigHeader::uiTimeStamp

  */
  CEM_AlgoDataTimeStamp_t uiLastMeasuredTimeStamp;
  /**
  @unit{s}

  Time period in seconds since object creation min:0

  */
  float32 fLifeTime;
  /**
  Object list cycle counts since object creation

  */
  CEM_AlgoCycleCounter_t uiLifeCycles;
  /**
  Cycle number of objects last measurement update. Compare to ObjectList::sSigHeader::uiCycleCounter

  */
  CEM_AlgoCycleCounter_t uiLastMeasuredCycle;
  /**
  Indicate the adminstration state of object

  */
  CEM_TP_t_ObjMaintenanceStates eMaintenanceState;
  /**
  @range{0,100}

  Object id of object

  */
  uint8 uiID;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_GENERAL_H_
