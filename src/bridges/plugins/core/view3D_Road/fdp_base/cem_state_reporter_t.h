// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_STATE_REPORTER_T_H_
#define FDP_BASE_CEM_STATE_REPORTER_T_H_

#include "Platform_Types.h"
#include "fdp_base/cem_state_t.h"
#include "fdp_base/failure_state_t.h"

namespace fdp_base {

struct CEM_StateReporter_t {
  /**
  State machine for Cem states
CEM_STATE_OFF:Cem state is in off
state
INIT:Cem state is in init state
CEM_STATE_NORMAL:Cem state
is in normal state
CEM_STATE_FAILURE:Cem state is in failure
state

  */
  CemState_t CemState;
  /**
  Failure information in case of failure state
CEM_STATE_FAILURE_NONE:There
is no failure
CEM_STATE_FAILURE_TEMPORARY:Its is a temporary
failure
CEM_STATE_FAILURE_PERMANENT:It is a permanent failure

  */
  FailureState_t FailureState;
  /**
  two bytes padding for a four byte alignment

  */
  uint16 padding;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_STATE_REPORTER_T_H_
