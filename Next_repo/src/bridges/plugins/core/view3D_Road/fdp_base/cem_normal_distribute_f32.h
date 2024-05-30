// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_NORMAL_DISTRIBUTE_F32_H_
#define FDP_BASE_CEM_NORMAL_DISTRIBUTE_F32_H_

#include "Platform_Types.h"

namespace fdp_base {

/// General type to describe a normal distribute with an expected value and its standard distribution
struct CEM_NormalDistribute_f32 {
  /**
  expected value

  */
  float32 mu;
  /**
  positive standard Deviation of the value
(negative values indicate
, that no distribution information is available and the expected
value is invalid)

  */
  float32 sig;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_NORMAL_DISTRIBUTE_F32_H_
