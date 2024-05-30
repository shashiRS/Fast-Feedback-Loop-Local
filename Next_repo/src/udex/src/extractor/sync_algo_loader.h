/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_SYNC_ALGO_LOADER_H_
#define UDEX_SYNC_ALGO_LOADER_H_

#include <next/udex/data_types.hpp>

#include "sync_algo.h"

namespace next {
namespace udex {

class SyncAlgoLoader {
public:
  SyncAlgoLoader();
  virtual ~SyncAlgoLoader();

  bool ConfigureSyncAlgo(SyncAlgoCfg sync_cfg, SignalInfo signal_info);
  SyncAlgoCfg GetSyncAlgoCfg();

  TimestampedDataPacket CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                      bool &extrac_succesfull, const std::vector<SyncValue> &sync_values,
                                      size_t &nr_of_elements_to_remove);

private:
  // std::shared_ptr<iSyncMethod> LoadSyncAlgorithmFromDll(std::string path);

  void SetSyncAlgo(std::shared_ptr<iSyncAlgo> sync_fun);
  std::shared_ptr<iSyncAlgo> CreateSyncAlgo(SyncAlgoType sync_algorithm);

  std::shared_ptr<iSyncAlgo> isync_algo_ = nullptr;

  SyncAlgoCfg sync_algo_cfg_;
  SignalInfo signal_info_;
};

} // namespace udex
} // namespace next

#endif // UDEX_SYNC_ALGO_LOADER_H_
