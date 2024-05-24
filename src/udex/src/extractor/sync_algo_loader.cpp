/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "sync_algo_loader.h"

namespace next {
namespace udex {

SyncAlgoLoader::SyncAlgoLoader() {
  SyncAlgoCfg default_sync_cfg;
  SignalInfo signal_info;

  ConfigureSyncAlgo(default_sync_cfg, signal_info);
}

SyncAlgoLoader::~SyncAlgoLoader() {}

std::shared_ptr<iSyncAlgo> SyncAlgoLoader::CreateSyncAlgo(SyncAlgoType sync_algorithm) {
  switch (sync_algorithm) {
  case GET_LAST_ELEMENT:
    return std::make_shared<SyncAlgo_LastElement>();

  case FIND_EXACT_VALUE:
    return std::make_shared<SyncAlgo_ExactValue>();

  case FIND_NEAREST_VALUE:
    return std::make_shared<SyncAlgo_NearestValue>();

  case FIND_EXACT_HEADER_TIMESTAMP:
    return std::make_shared<SyncAlgo_ExactHeaderTimestamp>();

  case WRONG_SYNC_ALGO_SPECIFIER:
    return nullptr;

  default:
    return std::make_shared<SyncAlgo_LastElement>();
  }
}

void SyncAlgoLoader::SetSyncAlgo(std::shared_ptr<iSyncAlgo> sync_func) {
  if (isync_algo_ != nullptr) {
    isync_algo_.reset();
  }

  isync_algo_ = sync_func;
}

bool SyncAlgoLoader::ConfigureSyncAlgo(SyncAlgoCfg sync_cfg, SignalInfo signal_info) {
  std::shared_ptr sync_algo = CreateSyncAlgo(sync_cfg.algorythm_type);
  if (sync_algo != nullptr) {
    SetSyncAlgo(sync_algo);
    sync_algo_cfg_ = sync_cfg;
    signal_info_ = signal_info;

    return true;
  }

  return false;
}

SyncAlgoCfg SyncAlgoLoader::GetSyncAlgoCfg() { return sync_algo_cfg_; }

TimestampedDataPacket SyncAlgoLoader::CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                                    bool &extrac_succesfull, const std::vector<SyncValue> &sync_values,
                                                    size_t &nr_of_elements_to_remove) {
  if (isync_algo_ != nullptr) {
    return isync_algo_->CallSyncAlogo(local_list, extrac_succesfull, sync_algo_cfg_.composition_sync_signal_url,
                                      sync_values, signal_info_, nr_of_elements_to_remove);
  }

  return {nullptr, 0, 0};
}

} // namespace udex
} // namespace next
