// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINTS_H_
#define FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINTS_H_

#include "cem_tp_t_obj_shape_point_coord.h"
#include "tp_t_obj_shape_point_state.h"

namespace fdp_base {

/// Object size related properties
struct CEM_TP_t_ObjShapePoints {
  /**
  State of shape point at the same index in the aShapePointCoordinates
array.
INVALID:
Shape point does not exist.
EDGE_MEASURED:
Shapepoint
exists and is measured by the EM. This marks a real corner of
the bounding shape of the object.
EDGE_ASSUMED:
Shape point exists
but is not measured by the EM. The position of the shape point
is generated out of model knowledge. It is not verified by a
measurement.
VISIBILITY_EDGE:
Shape point exists but is not a
real corner of the object. It is just the boundary up to what
an object is visible to the sensor. Typically used when we only
see a part of the object and know it.

  */
  TP_t_ObjShapePointState aShapePointStates[4];
  /**
  Coordinates relative to the fDistX, fDistY point. To form the
shape they should be connected neighbourwise.

  */
  CEM_TP_t_ObjShapePointCoord aShapePointCoordinates[4];
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINTS_H_
