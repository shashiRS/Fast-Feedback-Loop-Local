// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_QUALIFIERS_H_
#define FDP_BASE_CEM_TP_T_OBJ_QUALIFIERS_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Object specific qualifiers
struct CEM_TP_t_ObjQualifiers {
  /**
  @unit{%}

  @range{0,100}

  Likelihood that object is present.
Probability that the object
is a real object.

  */
  uint8 uiProbabilityOfExistence;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_QUALIFIERS_H_
