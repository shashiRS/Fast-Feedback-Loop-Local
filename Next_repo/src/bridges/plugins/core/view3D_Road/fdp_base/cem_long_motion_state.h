// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_LONG_MOTION_STATE_H_
#define FDP_BASE_CEM_LONG_MOTION_STATE_H_

#include "Platform_Types.h"
#include "fdp_base/cem200_long_mot_states.h"

namespace fdp_base {

struct CEM_LongMotionState {
  /**
  Moving (forward or reverse)
Moving forward
Moving reverse
Stand
still

  */
  Cem200LongMotStates longMotStateOverall;
  /**
  @unit{%}

  Probability to move forward

  */
  float32 fwdProbability;
  /**
  @unit{%}

  Probability to move forward

  */
  float32 revProbability;
  /**
  @unit{%}

  Probability to be in stand still

  */
  float32 ssProbability;
  /**
  @unit{%}

  Confidence level of the Motion State

  */
  sint32 confidence;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_LONG_MOTION_STATE_H_
