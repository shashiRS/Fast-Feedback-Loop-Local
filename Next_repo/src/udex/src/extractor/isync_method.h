/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_ISYNC_METHOD_H_
#define NEXT_UDEX_ISYNC_METHOD_H_

#include <next/udex/data_types.hpp>

namespace next {
namespace udex {

static size_t init_val = 0;

class iSyncAlgo {
public:
  // *nr_of_elements_to_remove permanently removes the entries specified

  virtual TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                              bool &extrac_succesfull, const std::string &url,
                                              const std::vector<SyncValue> &sync_values, const SignalInfo &signal_info,
                                              size_t &nr_of_elements_to_remove = init_val) = 0;
};

// extern "C" std::shared_ptr<iSyncMethod> create_sync_algorithm(const char *path);

} // namespace udex
} // namespace next

#endif // NEXT_UDEX_ISYNC_METHOD_H_
