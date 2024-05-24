// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_FREE_ELEMENT_H_
#define FDP_BASE_FREE_ELEMENT_H_

#include "Platform_Types.h"
#include "fdp_base/cem_meas_status.h"
#include "fdp_base/free_property.h"
#include "fdp_base/trace_info.h"

namespace fdp_base {

/// Attributes of one free space element
struct FreeElement {
  /**
  @range{0,65535}

  Unique ID of the free element

  */
  uint16 id;
  /**
  @range{0,2}

  Measurement status of the free element

  */
  CEM_MeasStatus measStatus;
  /**
  @range{0,NUM_FSF_VERTICES}

     First vertex index of the free element
Index value 1200 is invalid as the unmber of vertex is limited to 1200

  */
  uint16 vertexStartIndex;
  /**
  @range{0,300}

  Number of vertices of this free element

  */
  uint16 numVertices;
  /**
  @range{0,100}

  Probability of existence of the obstacle

  */
  uint8 probability;
  /**
  Timestamp of Birth and Contributing sensor list of the free element

  */
  TraceInfo traceInfo;
  /**
  free property of the obstacle

  */
  FreeProperty freeProperty;
};

} // namespace fdp_base

#endif // FDP_BASE_FREE_ELEMENT_H_
