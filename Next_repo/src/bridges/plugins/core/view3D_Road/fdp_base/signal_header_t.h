// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_SIGNAL_HEADER_T_H_
#define FDP_BASE_SIGNAL_HEADER_T_H_

#include "algo_cycle_counter_t.h"
#include "algo_data_time_stamp_t.h"
#include "algo_signal_state_t.h"

namespace fdp_base {

/// Common header for all structured data types
struct SignalHeader_t {
  /**
  Time stamp of the signal (for SIP: T0 timestamp of input signal
, otherwise: T7 timestamp to which signal has been adjusted)

  */
  AlgoDataTimeStamp_t uiTimeStamp;
  /**
  Deprecated, not used in CEM. Should be set to 0.

  */
  AlgoCycleCounter_t uiMeasurementCounter;
  /**
  Rolling counter of source component. Will be incremented in every
call to the exec method of the component.

  */
  AlgoCycleCounter_t uiCycleCounter;
  /**
  Validity status of the signal. If not set to AL_SIG_STATE_OK,
then do not evaluate the content of the associated signal!

  */
  AlgoSignalState_t eSigStatus;
};

} // namespace fdp_base

#endif // FDP_BASE_SIGNAL_HEADER_T_H_
