/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     signal_storage_types.h
 * @brief    introduces shared types and definitions for all signal storage
 * applications
 */

#ifndef NEXT_UDEX_SIGNAL_STORAGE_TYPES_H
#define NEXT_UDEX_SIGNAL_STORAGE_TYPES_H

#include <next/sdk/sdk_macros.h>

#include <next/udex/data_types.hpp>

namespace next {
namespace udex {
namespace ecal_util {

// ##################### INTERNAL TYPE INFORMATION FOR NEXT UDEX DATAPACKAGES #####################
/*! Binary blob data representation needed to deserialize DynamicPublisher data
 *
 */
constexpr int topic_header_magic_value = 981836772;

struct TopicHeader {
  int magic_value = magic_value; //< this value is set to recognize a valid package
  MetaInfo meta;
  size_t blob_size;
  size_t blob_offset;
  size_t package_info_size;
  size_t package_info_offset;
};

/*!  Memory layout of binary blob send by the DynamicPublisher
 *
 */
struct InternalMessageStruct {
  TopicHeader header;
  // allow 0 sized array
  NEXT_DISABLE_SPECIFIC_WARNING(4200, "-Wpedantic")
  char blob[0];
  NEXT_RESTORE_WARNINGS
};

} // namespace ecal_util
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_STORAGE_TYPES_H
