// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_FREE_SPACE_POLYSET_H_
#define FDP_BASE_FREE_SPACE_POLYSET_H_

#include "Platform_Types.h"
#include "fdp_base/cem_vertex.h"
#include "fdp_base/free_element.h"

namespace fdp_base {

/// Description of free space elements
struct FreeSpacePolyset {
  /**
  @range{0,NUM_FSF_ELEMENTS}

  Number of free space elements in the freeElementArray

  */
  uint16 numElements;
  /**
  Array of free space elements

  */
  FreeElement freeElementArray[300U];
  /**
  Array of vertices for building up the free space elements

  */
  CEM_Vertex vertexArray[1200U];
};

} // namespace fdp_base

#endif // FDP_BASE_FREE_SPACE_POLYSET_H_
