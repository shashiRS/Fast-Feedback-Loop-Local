// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_CORRELATIONS_H_
#define FDP_BASE_CEM_TP_T_OBJ_CORRELATIONS_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Correlations of the kinematic state
struct CEM_TP_t_ObjCorrelations {
  /**
  @range{-1,1}

  Correlation between X and Y

  */
  float32 fDistXDistY;
  /**
  @range{-1,1}

  Correlation between X and VX

  */
  float32 fDistXVx;
  /**
  @range{-1,1}

  Correlation between X and VY

  */
  float32 fDistXVy;
  /**
  @range{-1,1}

  Correlation between X and AX

  */
  float32 fDistXAx;
  /**
  @range{-1,1}

  Correlation between X and AY

  */
  float32 fDistXAy;
  /**
  @range{-1,1}

  Correlation between Y and VX

  */
  float32 fDistYVx;
  /**
  @range{-1,1}

  Correlation between Y and VY

  */
  float32 fDistYVy;
  /**
  @range{-1,1}

  Correlation between Y and AX

  */
  float32 fDistYAx;
  /**
  @range{-1,1}

  Correlation between Y and AY

  */
  float32 fDistYAy;
  /**
  @range{-1,1}

  Correlation between VX and VY

  */
  float32 fVxVy;
  /**
  @range{-1,1}

  Correlation between VX and AX

  */
  float32 fVxAx;
  /**
  @range{-1,1}

  Correlation between VX and AY

  */
  float32 fVxAy;
  /**
  @range{-1,1}

  Correlation between VY and AX

  */
  float32 fVyAx;
  /**
  @range{-1,1}

  Correlation between VY and AY

  */
  float32 fVyAy;
  /**
  @range{-1,1}

  Correlation between AX and AY

  */
  float32 fAxAy;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_CORRELATIONS_H_
