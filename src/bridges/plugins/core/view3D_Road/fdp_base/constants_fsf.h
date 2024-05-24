// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CONSTANTS_FSF_H_
#define FDP_BASE_CONSTANTS_FSF_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Constants for Static and Free space Elements
struct Constants_Fsf {
  enum { SEF_NR_ELEMENTS = 300U };
  enum { SEF_NR_VERTICES = 1200U };
  enum { NUM_FSF_ELEMENTS = 300U };
  enum { NUM_FSF_VERTICES = 1200U };
  enum { MAX_NUM_CONTRIB_SENSORS = 4U };
};

} // namespace fdp_base

#endif // FDP_BASE_CONSTANTS_FSF_H_
