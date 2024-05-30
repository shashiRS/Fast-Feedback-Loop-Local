//
// @copyright Copyright (c) Elektrobit Automotive GmbH. All rights reserved.
//

#pragma once

namespace cem {
struct AlgoInterfaceVersionNumber_t {
  uint32_t data;
};

struct AlgoDataTimeStamp_t {
  uint64_t data;
};

struct AlgoCycleCounter_t {
  uint16_t data;
};

struct AlgoSignalState_t {
  uint8_t data;
};

struct SignalHeader_t {
  ::cem::AlgoDataTimeStamp_t ui_time_stamp;
  ::cem::AlgoCycleCounter_t ui_measurement_counter;
  ::cem::AlgoCycleCounter_t ui_cycle_counter;
  ::cem::AlgoSignalState_t e_sig_status;
  // uint8_t a_reserve[3];
};

struct EMF_GlobalTimestamp_t {
  ::cem::AlgoInterfaceVersionNumber_t u_VersionNumber;
  ::cem::SignalHeader_t SigHeader;
};

} // namespace cem
