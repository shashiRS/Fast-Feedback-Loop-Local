// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_FSF_OUTPUT_T_H_
#define FDP_BASE_CEM_FSF_OUTPUT_T_H_

#include "fdp_base/cem_algo_interface_version_number_t.h"
#include "fdp_base/cem_signal_header_t.h"
#include "fdp_base/static_elements_set.h"

namespace fdp_base {

struct CEM_FsfOutput_t {
  /**
  @range{0,4294967295}

  Interface version number

  */
  CEM_AlgoInterfaceVersionNumber_t uiVersionNumber;
  /**
  Contains time stamp, cycle counter and measurement status.

  */
  CEM_SignalHeader_t sSigHeader;
  /**
  Set of static Elements

  */
  StaticElementsSet staticElementsSet;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_FSF_OUTPUT_T_H_
