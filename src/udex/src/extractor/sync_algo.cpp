/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "sync_algo.h"

namespace next {
namespace udex {

SyncAlgo_LastElement::SyncAlgo_LastElement() {}

SyncAlgo_LastElement::~SyncAlgo_LastElement() {}

TimestampedDataPacket SyncAlgo_LastElement::CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                                          bool &extrac_succesfull,
                                                          [[maybe_unused]] const std::string &url,
                                                          [[maybe_unused]] const std::vector<SyncValue> &sync_values,
                                                          [[maybe_unused]] const SignalInfo &signal_info,
                                                          size_t &nr_of_elements_to_remove) {
  if (local_list.size() > 0) {
    std::shared_ptr<TimestampedDataPacket> obj = local_list.back();
    extrac_succesfull = true;
    nr_of_elements_to_remove = local_list.size() - 1; // last element was provided, so all the list can be discarded
    return *obj;
  }
  extrac_succesfull = false;
  nr_of_elements_to_remove = 0; // nothing to discard
  return {nullptr, 0, 0};
}

SyncAlgo_ExactValue::SyncAlgo_ExactValue() {}

SyncAlgo_ExactValue::~SyncAlgo_ExactValue() {}

TimestampedDataPacket SyncAlgo_ExactValue::CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                                         bool &extrac_succesfull,
                                                         [[maybe_unused]] const std::string &url,
                                                         const std::vector<SyncValue> &sync_values,
                                                         const SignalInfo &signal_info,
                                                         size_t &nr_of_elements_to_remove) {
  // sync_values[0].signal_type; // use this in combination with the get
  uint64_t value = std::get<uint64_t>(sync_values[0].signal_value);
  uint64_t sync_value = 0;
  for (auto element : local_list) {
    memcpy(&sync_value, ((char *)(element->data)) + (signal_info.offset), signal_info.signal_size);
    if (sync_value == (uint64_t)value) {
      extrac_succesfull = true;
      return *element;
    }
    nr_of_elements_to_remove++; // only remove if package is not needed and it has been itterated through
  }
  extrac_succesfull = false;
  nr_of_elements_to_remove = 0; // nothing to discard
  return {nullptr, 0, 0};
}

SyncAlgo_NearestValue::SyncAlgo_NearestValue() {}

SyncAlgo_NearestValue::~SyncAlgo_NearestValue() {}

TimestampedDataPacket
SyncAlgo_NearestValue::CallSyncAlogo(std::list<std::shared_ptr<TimestampedDataPacket>> &local_list,
                                     bool &extrac_succesfull, [[maybe_unused]] const std::string &url,
                                     const std::vector<SyncValue> &sync_values, const SignalInfo &signal_info,
                                     size_t &nr_of_elements_to_remove) {
  // sync_values[0].signal_type; // use this in combination with the get
  uint64_t value = std::get<uint64_t>(sync_values[0].signal_value);
  uint64_t sync_value = 0;
  TimestampedDataPacket previous_element{nullptr, 0, 0};
  extrac_succesfull = false;
  for (auto element : local_list) {
    memcpy(&sync_value, ((char *)(element->data)) + signal_info.offset, signal_info.signal_size);
    if ((uint64_t)value > sync_value) {
      previous_element = *element;
    } else {
      if (previous_element.data != nullptr) {
        extrac_succesfull = true;
        nr_of_elements_to_remove--;
        return previous_element;
      } else {
        /*Requested for the Element less than first element in the que, return first element*/
        extrac_succesfull = true;
        nr_of_elements_to_remove = 1;
        return **local_list.begin();
      }
    }
    nr_of_elements_to_remove++; // only remove if package is not needed and it has been itterated through
  }
  extrac_succesfull = false;
  nr_of_elements_to_remove = 0; // nothing to discard
  return {nullptr, 0, 0};
}

SyncAlgo_ExactHeaderTimestamp::SyncAlgo_ExactHeaderTimestamp() {}

SyncAlgo_ExactHeaderTimestamp::~SyncAlgo_ExactHeaderTimestamp() {}

TimestampedDataPacket SyncAlgo_ExactHeaderTimestamp::CallSyncAlogo(
    std::list<std::shared_ptr<TimestampedDataPacket>> &local_list, bool &extrac_succesfull,
    [[maybe_unused]] const std::string &url, const std::vector<SyncValue> &sync_values,
    [[maybe_unused]] const SignalInfo &signal_info, size_t &nr_of_elements_to_remove) {
  // sync_values[0].signal_type; // use this in combination with the get
  uint64_t value = std::get<uint64_t>(sync_values[0].signal_value);
  uint64_t sync_value = 0;
  for (auto element : local_list) {
    sync_value = element->timestamp;
    if (sync_value == value) {
      extrac_succesfull = true;
      return *element;
    }
    nr_of_elements_to_remove++; // only remove if package is not needed and it has been itterated through
  }
  extrac_succesfull = false;
  nr_of_elements_to_remove = 0; // nothing to discard
  return TimestampedDataPacket{nullptr, 0, 0};
}

} // namespace udex
} // namespace next
