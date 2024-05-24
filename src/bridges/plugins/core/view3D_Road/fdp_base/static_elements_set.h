// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_STATIC_ELEMENTS_SET_H_
#define FDP_BASE_STATIC_ELEMENTS_SET_H_

#include "Platform_Types.h"
#include "fdp_base/cem_static_element.h"
#include "fdp_base/cem_vertex.h"

namespace fdp_base {

/// Description of static elements
struct StaticElementsSet {
  /**
  @range{0,300}

  Number of static elements in the staticElements array

  */
  uint16 numStaticElements;
  /**
  Array of static elements

  */
  CEM_StaticElement staticElementArray[300U];
  /**
  Array of vertices of all static elements

  */
  CEM_Vertex vertexArray[1200U];
};

} // namespace fdp_base

#endif // FDP_BASE_STATIC_ELEMENTS_SET_H_
