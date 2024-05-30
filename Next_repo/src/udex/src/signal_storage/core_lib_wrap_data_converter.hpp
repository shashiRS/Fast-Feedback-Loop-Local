/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_CORE_LIB_WRAP_DATA_CONVERTER
#define UDEX_CORE_LIB_WRAP_DATA_CONVERTER

#include <next/sdk/sdk_macros.h>

#include <map>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/sources.h>
#include <mts/extensibility/package.hpp>
#include <mts/introspection/conversion.hpp>
#include <mts/introspection/xtypes/dynamic_type.hpp>
#include <mts/introspection/xtypes/parser/json.hpp>
#include <mts/runtime/memory_allocator.hpp>
#include <mts/runtime/udex/extractor_proxy.hpp>
#include <mts/toolbox/ecu/ecu.hpp>
NEXT_RESTORE_WARNINGS

#include <next/udex/data_types.hpp>

using memory_pointer = std::unique_ptr<std::vector<uint8_t>>;

namespace CoreLibWrap {

struct DynamicTypes {
  mts::introspection::xtypes::dynamic_type::ptr source_dynamic_type;
  mts::introspection::xtypes::dynamic_type::ptr targer_dynamic_type;
};

class DataConverter {
public:
  DataConverter();

  void ConvertData(std::string topic, void *inData, size_t inSize, uint64_t timestamp, std::string &input_schema,
                   const next::udex::PackageInfo &package_info, memory_pointer &output);

private:
  void PrepareConverterInput(void *inData, size_t inSize, void *&converter_input, size_t &converter_input_size,
                             const mts::introspection::xtypes::dynamic_type &dynamic_type);

  void WriteToOutput(memory_pointer &output, const mts::introspection::xtypes::dynamic_type &dynamic_type,
                     const mts::introspection::xtypes::dynamic_data &dynamic_data);

  void ConvertDataWithProcessor(const std::string &topic, void *inData, size_t inSize, uint64_t timestamp,
                                const next::udex::PackageInfo &package_info, memory_pointer &output);

  void SetupCoreLibForPackageDevice(const std::string &topic, const next::udex::PackageInfo &package_info);

  void ConvertPackage(size_t topic_id, const mts::extensibility::package &package, memory_pointer &output);

  mts::extensibility::package CreatePackage(void *inData, size_t inSize, uint64_t timestamp,
                                            const next::udex::PackageInfo &package_info);

  void WriteProcessedPackage(const mts::introspection::xtypes::dynamic_data &dynamic_data, memory_pointer &output);

public:
  inline static const std::string_view kDummyHWSDLContent{R"(
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance" xsd:noNamespaceSchemaLocation="sdl2-after-compiler.xsd" ByteAlignment="1" Version="2.0">
	<View Name="Dummy" CycleID="0">
	</View>
</SdlFile> )"};

private:
  std::map<std::string, DynamicTypes> topic_dynamic_types_;
  mts::introspection::type_conversion_context type_conversion_context_;
  bool converter_input_realocated_ = false;

  std::shared_ptr<mts::runtime::udex::extractor_proxy> udex_extractor_ptr_;
  std::map<std::string, size_t> hw_subscription_map_;
};

} // namespace CoreLibWrap
#endif
