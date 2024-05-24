// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VERTEX_H_
#define FDP_BASE_CEM_VERTEX_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Attributes of one static element vertex
struct CEM_Vertex {
  /**
  @unit{m}

  @range{-400,400}

  Coordinate in dimension x

  */
  float32 fX;
  /**
  @unit{m}

  @range{-400,400}

  Coordinate in dimension y

  */
  float32 fY;
  /**
  @unit{m}

  @range{-400,400}

  Coordinate in dimension z

  */
  float32 fZ;
  /**
  @unit{m}

  @range{-1,400}

  Stantard Deviation in dimension x

  */
  float32 stdX;
  /**
  @unit{m}

  @range{-1,400}

  Stantard Deviation in dimension y

  */
  float32 stdY;
  /**
  @unit{m}

  @range{-1,400}

  Stantard Deviation in dimension z

  */
  float32 stdZ;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VERTEX_H_
