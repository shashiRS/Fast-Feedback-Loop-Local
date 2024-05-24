/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_explorer.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_SIGNALEXTRACTOR_H_
#define NEXT_BRIDGESDK_PLUGIN_SIGNALEXTRACTOR_H_

#include <array>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include <next/bridgesdk/plugin.h>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

constexpr size_t ktypematrixSize = 13;

class PluginSignalExtractorImpl;

enum class SignalType : std::uint8_t {
  SIGNAL_TYPE_BOOL,
  SIGNAL_TYPE_CHAR,
  SIGNAL_TYPE_UINT8,
  SIGNAL_TYPE_INT8,
  SIGNAL_TYPE_UINT16,
  SIGNAL_TYPE_INT16,
  SIGNAL_TYPE_UINT32,
  SIGNAL_TYPE_INT32,
  SIGNAL_TYPE_UINT64,
  SIGNAL_TYPE_INT64,
  SIGNAL_TYPE_FLOAT,
  SIGNAL_TYPE_DOUBLE,
  SIGNAL_TYPE_STRUCT
};

typedef std::variant<bool, char, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float,
                     double>
    dataType;

struct SignalInfo {
  SignalType signal_type;
  size_t signal_size;
  size_t array_size;
  size_t offset;
};

constexpr int N_BYTES_CAN_DATA = 8;
struct RawSignalInfo {
  size_t signal_offset;
  size_t payload_size_;
  SignalType sig_type_;
  size_t array_length_;
};
struct Signals {
  std::unordered_map<std::string, RawSignalInfo> raw_signal_info{};
};

enum FormatType {
  FORMAT_TYPE_UNKNOWN = -1,
  FORMAT_TYPE_TIMEBASE,
  FORMAT_TYPE_NMEA,
  FORMAT_TYPE_CAN,
  FORMAT_TYPE_FLEXRAY,
  FORMAT_TYPE_REFCAM,
  FORMAT_TYPE_ETH,
  FORMAT_TYPE_IBEO,
  FORMAT_TYPE_CT4,
  FORMAT_TYPE_XCP,
  FORMAT_TYPE_RTRAGE,
  FORMAT_TYPE_MTA
};

//! network data package defintion
struct UdexDataPackage {
  std::string PackageUrl; //!< package url which is the top level url of the receive binary blob
  const void *Data;       //!< actual raw memory location of package
  size_t Size;            //!< size of raw memory
  time_t Time;            //!< network time stamp -> does not need to match recording timestamp
  FormatType format_type;
};

enum class TypeConversionInfo : std::uint8_t {
  FIT = 0,
  CHANED_NO_LOSS = 1,
  CHANGED_DATA_LOSS = 2,
  CHANGED_NARROWING = 3,
  DATA_OVERFLOW = 4,
  INVALID = 5
};

// clang-format off
/*
                                                              To signal ->
                                Bool| Char  | Uint8 | Int8 | Uint16 | Int16 | Uint32 | Int 32 | Uint64 | Int64 | Float | Double | Struct 
From desired type       Bool  |  0  |  2    |   2   |  2   |    2   |   2   |   2    |  2     |   2    |   2   |   2   |   2    |   5 
                              -----------------------------------------------------------------------------------------------------------          
                        Char  |  1  |  0    |   0   |  3   |    4   |   4   |   4    |   4    |    4   |   4   |   4   |   4    |   5    
                              -----------------------------------------------------------------------------------------------------------
                        Uint8 |  1  |  0    |   0   |  3   |    4   |   4   |   4    |   4    |    4   |   4   |   4   |   4    |   5    
                              -----------------------------------------------------------------------------------------------------------                                             
                        Int8  |  1  |  2    |   2   |  0   |    4   |   4   |   4    |   4    |    4   |   4   |   4   |   4    |   5        
                              -----------------------------------------------------------------------------------------------------------
                        Uint16|  1  |  0    |   0   |  *3   |    0   |   3   |   4    |   4    |    4   |   4   |   4   |   4   |   5       
                              -----------------------------------------------------------------------------------------------------------
                        Int16 |  1  |  1    |   1   |  0   |    2   |   0   |   4    |   4    |    4   |   4   |   4   |   4    |   5        
                              -----------------------------------------------------------------------------------------------------------
                        Uint32|  1  |  0    |   0   |  3   |    0   |   3   |   0    |   3    |    4   |   4   |   2   |   4    |   5    
                              -----------------------------------------------------------------------------------------------------------    
                        Int32 |  1  |  0    |   0   |  0   |    0   |   0   |   2    |   0    |    4   |   4   |   4   |   4    |   5   
                              -----------------------------------------------------------------------------------------------------------      
                        Uint64|  1  |  1    |   1   |  3   |    1   |   3   |   1    |   3    |    0   |   3   |   2   |   2    |   5  
                              -----------------------------------------------------------------------------------------------------------          
                        Int64 |  1  |  1    |   1   |  1   |    1   |   1   |   1    |   1    |    2   |   2   |   2   |   2    |   5 
                              -----------------------------------------------------------------------------------------------------------          
                        Float |  1  |  1    |   1   |  1   |    1   |   1   |   2    |   1    |    4   |   4   |   0   |   4    |   5        
                              -----------------------------------------------------------------------------------------------------------
                        Double|  1  |  1    |   1   |  1   |    1   |   1   |   1    |   1    |    2   |   1   |   1   |   0    |   5    
                              -----------------------------------------------------------------------------------------------------------
                        Struct|  5  |  5    |   5   |  5   |    5   |   5   |   5    |   5    |    5   |   5   |   5   |   5    |   0
                              -----------------------------------------------------------------------------------------------------------
*/
// clang-format on

const std::array<std::array<TypeConversionInfo, ktypematrixSize>, ktypematrixSize> KtypeMatrix = {{
    {TypeConversionInfo::FIT, TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT, TypeConversionInfo::FIT,
     TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT, TypeConversionInfo::FIT,
     TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::FIT, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT, TypeConversionInfo::FIT,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::FIT, TypeConversionInfo::CHANGED_NARROWING,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::FIT, TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::FIT,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT, TypeConversionInfo::FIT,
     TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::FIT, TypeConversionInfo::CHANGED_NARROWING,
     TypeConversionInfo::FIT, TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT, TypeConversionInfo::FIT, TypeConversionInfo::FIT,
     TypeConversionInfo::FIT, TypeConversionInfo::FIT, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::FIT,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANGED_NARROWING,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::FIT,
     TypeConversionInfo::CHANGED_NARROWING, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANGED_DATA_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::DATA_OVERFLOW, TypeConversionInfo::FIT, TypeConversionInfo::DATA_OVERFLOW,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANGED_DATA_LOSS,
     TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::CHANED_NO_LOSS, TypeConversionInfo::FIT,
     TypeConversionInfo::INVALID},

    {TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID,
     TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID,
     TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID, TypeConversionInfo::INVALID,
     TypeConversionInfo::FIT},
}};

const std::map<SignalType, std::uint8_t> KdataTypeIndex{
    {SignalType::SIGNAL_TYPE_BOOL, uint8_t(0)},   {SignalType::SIGNAL_TYPE_CHAR, uint8_t(1)},
    {SignalType::SIGNAL_TYPE_UINT8, uint8_t(2)},  {SignalType::SIGNAL_TYPE_INT8, uint8_t(3)},
    {SignalType::SIGNAL_TYPE_UINT16, uint8_t(4)}, {SignalType::SIGNAL_TYPE_INT16, uint8_t(5)},
    {SignalType::SIGNAL_TYPE_UINT32, uint8_t(6)}, {SignalType::SIGNAL_TYPE_INT32, uint8_t(7)},
    {SignalType::SIGNAL_TYPE_UINT64, uint8_t(8)}, {SignalType::SIGNAL_TYPE_INT64, uint8_t(9)},
    {SignalType::SIGNAL_TYPE_FLOAT, uint8_t(10)}, {SignalType::SIGNAL_TYPE_DOUBLE, uint8_t(11)},
    {SignalType::SIGNAL_TYPE_STRUCT, uint8_t(12)}};

class DECLSPEC_plugin_config PluginSignalExtractor {
private:
  PluginSignalExtractor() = delete;

  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::unique_ptr<PluginSignalExtractorImpl> impl_;
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  PluginSignalExtractor(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginSignalExtractor();

  virtual UdexDataPackage GetRawData(const std::string &topic_name);

  virtual bool isDataAvailable(const std::string &topic_name);

  virtual void *GetExtractedData(const std::string &topic_name, size_t &out_size, size_t &time);

  virtual std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>>
  GetExtractedQueue(const std::string &topic_name);

  virtual void ClearExtractedQueue(const std::string &topic_name);

  virtual bool GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                        SignalInfo *signal_info = nullptr);

  virtual TypeConversionInfo GetValueWithTypeChecking(const std::string &signal_url, const SignalType type,
                                                      dataType &return_value, SignalInfo *signal_info = nullptr);

  virtual bool GetVectorValue(const std::string &signal_url, const SignalType type,
                              std::vector<dataType> &return_vector, SignalInfo *signal_info = nullptr);

  virtual bool GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                               std::vector<dataType> &return_vector);

  virtual SignalInfo GetInfo(const std::string &signal_url);

  virtual float getValueAsFloat() { return 0.F; };

}; // PluginSignalExtractor

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_SIGNALEXTRACTOR_H_
