/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_SYNC_ALGO_H
#define NEXT_UDEX_SYNC_ALGO_H

#include "isync_method.h"

namespace next {
namespace udex {

class SyncAlgo_LastElement : public iSyncAlgo {
public:
  SyncAlgo_LastElement();
  virtual ~SyncAlgo_LastElement();

  virtual TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                              bool &extrac_succesfull, const std::string &url,
                                              const std::vector<SyncValue> &sync_values, const SignalInfo &signal_info,
                                              size_t &nr_of_elements_to_remove) override;
};

class SyncAlgo_ExactValue : public iSyncAlgo {
public:
  SyncAlgo_ExactValue();
  virtual ~SyncAlgo_ExactValue();

  virtual TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                              bool &extrac_succesfull, const std::string &url,
                                              const std::vector<SyncValue> &sync_values,
                                              [[maybe_unused]] const SignalInfo &signal_info,
                                              size_t &nr_of_elements_to_remove) override;
};

class SyncAlgo_NearestValue : public iSyncAlgo {
public:
  SyncAlgo_NearestValue();
  virtual ~SyncAlgo_NearestValue();

  virtual TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                              bool &extrac_succesfull, const std::string &url,
                                              const std::vector<SyncValue> &sync_values, const SignalInfo &signal_info,
                                              size_t &nr_of_elements_to_remove) override;
};

class SyncAlgo_ExactHeaderTimestamp : public iSyncAlgo {
public:
  SyncAlgo_ExactHeaderTimestamp();
  virtual ~SyncAlgo_ExactHeaderTimestamp();

  virtual TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                              bool &extrac_succesfull, const std::string &url,
                                              const std::vector<SyncValue> &sync_values, const SignalInfo &signal_info,
                                              size_t &nr_of_elements_to_remove) override;
};

} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SYNC_ALGO_H
