// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_VAL_POLYGON_FORMAT_T_H_
#define FDP_BASE_VAL_POLYGON_FORMAT_T_H_

#include "Platform_Types.h"
#include "fdp_base/cem_val_point2_d_t.h"

namespace fdp_base {

/// Descripton of the polygon format to be used between Validity and Sensor Plugin's
struct VAL_PolygonFormat_t {
  /**
  Total number of points used to create the polygon

  */
  uint16 u_TotalPolygonPoints;
  /**
  Vertices of the polygon

  */
  CEM_VAL_Point2D_t polygonVertices[100];
};

} // namespace fdp_base

#endif // FDP_BASE_VAL_POLYGON_FORMAT_T_H_
