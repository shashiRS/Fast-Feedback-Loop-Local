// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VAL_OUTPUT_POLYGON_FORMAT_T_H_
#define FDP_BASE_CEM_VAL_OUTPUT_POLYGON_FORMAT_T_H_

#include "Platform_Types.h"
#include "fdp_base/e_polygon_type_t.h"
#include "fdp_base/val_polygon_format_t.h"

namespace fdp_base {

/// Actual polygons representing the IFOV
struct CEM_VAL_OutputPolygonFormat_t {
  /**
  ID of the polygon

  */
  uint16 u_PolygonID;
  /**
  Type of the polygon representing whether the polygon is filled
or a hole

  */
  e_PolygonType_t e_PolygonType;
  /**
  Every polygon represents a part of the overall IFOV

  */
  VAL_PolygonFormat_t VAL_OutputPolygon;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VAL_OUTPUT_POLYGON_FORMAT_T_H_
