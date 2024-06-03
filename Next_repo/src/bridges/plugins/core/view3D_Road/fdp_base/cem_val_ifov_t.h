// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VAL_IFOV_T_H_
#define FDP_BASE_CEM_VAL_IFOV_T_H_

#include "Platform_Types.h"
#include "fdp_base/cem_val_output_polygon_format_t.h"

namespace fdp_base {

/// Structure represents the ideal field of view of the sensor setup attached to the ego-vehicle
struct CEM_VAL_IFOV_t {
  /**
  describe how many polygons are in the outputted structure.

  */
  uint16 u_TotalPolygons;
  /**
  Actual polygons representing the IFOV

  */
  CEM_VAL_OutputPolygonFormat_t VAL_IFOVPolygons[100];
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VAL_IFOV_T_H_