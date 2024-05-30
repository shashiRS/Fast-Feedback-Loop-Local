// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_ASSOCIATIONS_H_
#define FDP_BASE_CEM_ASSOCIATIONS_H_

#include "Platform_Types.h"
#include "fdp_base/cem_algo_interface_version_number_t.h"
#include "fdp_base/cem_signal_header_t.h"
#include "fdp_base/coh_gen_obj_lane_assoc_t.h"

namespace fdp_base {

/// contains all COH Associations
struct CEM_Associations {
  /**
  Interface version number
(increased when changed)

  */
  CEM_AlgoInterfaceVersionNumber_t u_VersionNumber;
  /**
  Contains time stamp, frame stamp, etc.

  */
  CEM_SignalHeader_t SigHeader;
  /**
  Signal Header of the corresponding GenObjecList

  */
  CEM_SignalHeader_t SigUsedGenObjec;
  /**
  Signal Header of the corresponding RmfRoadModel

  */
  CEM_SignalHeader_t SigUsedRMF;
  /**
  @range{0,100}

  number of GenObject to lane associations

  */
  uint16 u_NumGenObjLaneAssoc;
  /**
  array of GenObject to sLane associations

  */
  COH_GenObjLaneAssoc_t a_GenObjLaneAssoc[100];
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_ASSOCIATIONS_H_
