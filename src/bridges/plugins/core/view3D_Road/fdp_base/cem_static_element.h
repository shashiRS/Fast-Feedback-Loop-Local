// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_STATIC_ELEMENT_H_
#define FDP_BASE_CEM_STATIC_ELEMENT_H_

#include "Platform_Types.h"
#include "fdp_base/cem_meas_status.h"
#include "fdp_base/static_element_attributes.h"
#include "fdp_base/trace_info.h"

namespace fdp_base {

/// Attributes of one static element
struct CEM_StaticElement {
  /**
  @range{0,500}

  Unique ID of the stationary obstacle

  */
  uint16 id;
  /**
  @range{0,2}

  Measurement status of the static element

  */
  CEM_MeasStatus measStatus;
  /**
  @range{0,1200}

     First vertex index of the static element
Index value 1200 is invalid as the unmber of vertex is limited to 1200

  */
  uint16 vertexStartIndex;
  /**
  @range{0,1200}

  Number of vertices of the static element

  */
  uint16 numVertices;
  /**
  @range{0,100}

  Probability of existence of the static element

  */
  uint8 probabilityOfExistence;
  /**
  Timestamp of Birth and Contributing sensor list of the static element

  */
  TraceInfo traceInfo;
  /**
  Drivability and Classification attributes of the static element

  */
  StaticElementAttributes staticElementAttributes;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_STATIC_ELEMENT_H_
