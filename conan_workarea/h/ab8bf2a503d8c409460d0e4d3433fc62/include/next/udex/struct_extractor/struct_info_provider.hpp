/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     signal_info_provider.hpp
 * @brief    class to provide the information for using struct_extractor. Can be user implmeneted as well
 */

#ifndef NEXT_UDEX_STRUCT_INFO_PROVIDER_HPP
#define NEXT_UDEX_STRUCT_INFO_PROVIDER_HPP

#include <string>

#include <next/udex/data_types.hpp>

struct memoryBlock {
  char *address; //!< address of data
  size_t size;   //!< size of available data
};

class ISignalInfoProvider {
public:
  ISignalInfoProvider() = default;

  /*! @brief computes offset in memory layout for specific url
   * returns byte-offset to the requested signal url within the flat cpp-memory space
   *
   * @param url URL of requested signal (i.E. device.view.group.subgroup)
   * @return returns offset in bytes
   */
  virtual size_t getOffsetByUrl(const std::string &url) = 0;

  /*! @brief provdes signal info for a specific url
   *
   * @param url URL of requested signal (i.E. device.view.group.subgroup)
   * @return returns signal information to requested url
   */
  virtual next::udex::SignalInfo getSignalInfo(const std::string &url) = 0;

  /*! return if the SignalInfoProvider needs some post processin deserializaion outside NEXT-UDex (i.E. protobuf)
   *
   * @return deserialization needed (default is false which needs to using just the offeset and root memory)
   */
  virtual bool deserializationRequired() = 0;

  /*! @brief provides deserialized memory if root memory layout is still not valid
   * can be used to work with protobuf deserialization or data not defined within NEXT-UDex
   *
   * @param [in,out] deserialized_memory flat memory layout to cast values with respect to the signal info form
   * getSignalInfo and the byte offset requested by getOffsetByUrl
   * @return bool deserialization was successfull
   */
  virtual bool getDeserializedMemory(memoryBlock &deserialized_memory) = 0;
};
#endif // NEXT_UDEX_STRUCT_INFO_PROVIDER_HPP
