/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "core_lib_wrap_data_converter.hpp"

#include <sstream>
#include <vector>

#ifndef NO_USE_LOGGER_NAMESPACE
#define UNDEF_NO_USE_LOGGER_NAMESPACE
#endif
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/logger/logger.hpp>
#ifdef UNDEF_NO_USE_LOGGER_NAMESPACE
#undef NO_USE_LOGGER_NAMESPACE
#undef UNDEF_NO_USE_LOGGER_NAMESPACE
#endif

namespace CoreLibWrap {

using namespace mts::introspection;
using namespace next::udex;

DataConverter::DataConverter() {
  // create udex_extractor_ptr_ for converting hardware image data
  std::ostringstream ss;

  auto current_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

  ss << std::this_thread::get_id() << current_time.count();
  std::string default_storage_name = "file:" + ss.str() + "?mode=memory";
  udex_extractor_ptr_ = std::make_shared<mts::runtime::udex::extractor_proxy>(default_storage_name);
}

void DataConverter::ConvertData(std::string topic, void *inData, size_t inSize, uint64_t timestamp,
                                std::string &input_schema, const next::udex::PackageInfo &package_info,
                                memory_pointer &output) {

  namespace logger = next::sdk::logger;

  if (package_info.processor_required) {
    ConvertDataWithProcessor(topic, inData, inSize, timestamp, package_info, output);
    return;
  }

  if (input_schema.empty()) {
    output.get()->resize(inSize);
    memcpy(output.get()->data(), inData, inSize);
    return;
  }

  if (topic_dynamic_types_.find(topic) == topic_dynamic_types_.end()) {
    auto context = xtypes::parser::parse(input_schema);
    xtypes::dynamic_type::ptr dynamic_type = get_root_type(context.get_module());
    auto target_dynamic_type = mts::introspection::get_host_platform_type(*dynamic_type, true, true);
    if (!type_conversion_context_.is_source_type_registered(dynamic_type->type_id())) {
      type_conversion_context_.register_types(dynamic_type, target_dynamic_type);
    }

    topic_dynamic_types_.insert(std::make_pair(topic, DynamicTypes{dynamic_type, target_dynamic_type}));
  }

  void *converter_input = nullptr;
  size_t converter_input_size = 0;
  auto topic_dynamic_type = topic_dynamic_types_[topic];

  PrepareConverterInput(inData, inSize, converter_input, converter_input_size, *topic_dynamic_type.source_dynamic_type);

  auto converted_data = mts::introspection::type_converter::convert(
      type_conversion_context_, topic_dynamic_type.source_dynamic_type->type_id(), converter_input,
      converter_input_size, topic_dynamic_type.targer_dynamic_type->type_id());

  WriteToOutput(output, *topic_dynamic_type.targer_dynamic_type, converted_data);

  if (converter_input_realocated_) {
    free(converter_input);
  }
}

void DataConverter::PrepareConverterInput(void *inData, size_t inSize, void *&converter_input,
                                          size_t &converter_input_size, const xtypes::dynamic_type &dynamic_type) {
  converter_input_realocated_ = false;
  auto converter_expected_input_size = dynamic_type.memory_size();

  if (inSize < converter_expected_input_size) {
    std::unique_ptr<uint8_t[]> padding_buffer;
    padding_buffer = std::make_unique<uint8_t[]>(converter_expected_input_size);
    memcpy(padding_buffer.get(), inData, inSize);
    memset(padding_buffer.get() + inSize, 0, converter_expected_input_size - inSize);

    converter_input = padding_buffer.release(); // release pointer here, will be freed later
    converter_input_size = converter_expected_input_size;
    converter_input_realocated_ = true;
  } else if (inSize > converter_input_size) {
    // referece camera, raw image
    converter_input = inData;
    converter_input_size = inSize;
  } else { // equal use case
    converter_input = inData;
    converter_input_size = converter_expected_input_size;
  }
}

void DataConverter::WriteToOutput(memory_pointer &output, const xtypes::dynamic_type &dynamic_type,
                                  const xtypes::dynamic_data &dynamic_data) {
  auto type_size = dynamic_type.memory_size();
  auto data_size = dynamic_data.memory_size();

  if (data_size > type_size) {
    // extra data after the converted data, we must copy that too
    output.get()->resize(data_size);
    memcpy(output.get()->data(), dynamic_data.raw_instance(), data_size);
  } else { // data fits type size
    output.get()->resize(type_size);
    memcpy(output.get()->data(), dynamic_data.raw_instance(), type_size);
  }
}

void DataConverter::ConvertDataWithProcessor(const std::string &topic, void *inData, size_t inSize, uint64_t timestamp,
                                             const next::udex::PackageInfo &package_info, memory_pointer &output) {
  mts::extensibility::package package = CreatePackage(inData, inSize, timestamp, package_info);
  try {
    if (hw_subscription_map_.find(topic) == hw_subscription_map_.end()) {
      SetupCoreLibForPackageDevice(topic, package_info);
    }

    ConvertPackage(hw_subscription_map_.at(topic), package, output);

  } catch (std::exception &e) {
    next::sdk::logger::error(__FILE__, "Hw extraction failed: {0}", e.what());
  }
}

mts::extensibility::package DataConverter::CreatePackage(void *inData, size_t inSize, uint64_t timestamp,
                                                         const PackageInfo &package_info) {
  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();

  auto format_string = getPackageFormatTypeString(package_info.format_type);
  mts::extensibility::package package(memory_allocator_ptr, format_string.c_str(), inData, inSize);

  package->source_id = package_info.source_id;
  package->source_location = 0u;
  package->cycle_state = mts::extensibility::cycle_body;
  package->cycle_id = package_info.cycle_id;
  package->instance_number = package_info.instance_id;
  package->size = inSize;
  package->reference_timestamp = timestamp;
  return package;
}

void DataConverter::SetupCoreLibForPackageDevice(const std::string &topic, const PackageInfo &package_info) {
  std::vector<char> BinaryDescription;
  BinaryDescription.resize(kDummyHWSDLContent.size());
  std::copy_n(reinterpret_cast<const char *>(kDummyHWSDLContent.data()), kDummyHWSDLContent.size(),
              BinaryDescription.data());

  // register source
  auto signatures = udex_extractor_ptr_->register_source_with_data_description(
      package_info.source_id, package_info.instance_id, package_info.data_source_name, "", "sdl",
      (uint8_t *)BinaryDescription.data(), BinaryDescription.size());

  hw_subscription_map_[topic] = udex_extractor_ptr_->subscribe(topic);
}

void DataConverter::ConvertPackage(size_t topic_id, const mts::extensibility::package &package,
                                   memory_pointer &output) {
  try {
    auto sample_tuples = udex_extractor_ptr_->introspect(package);
    if (sample_tuples.size() != 1) {
      next::sdk::logger::warn(__FILE__, "HW image package couldn't be converted. Number packages {0}",
                              sample_tuples.size());
      return;
    }

    for (auto const &sample : sample_tuples) {
      if (std::get<0>(sample) == topic_id) {
        WriteProcessedPackage(std::get<1>(sample), output);
        return;
      }
    }
  } catch (std::exception &e) {
    next::sdk::logger::error(__FILE__, "Package introspectin for processor packages failed: {0}", e.what());
  }
  next::sdk::logger::warn(__FILE__, "No HW image package could be found for topic id {0}.", topic_id);
}

void DataConverter::WriteProcessedPackage(const mts::introspection::xtypes::dynamic_data &dynamic_data,
                                          memory_pointer &output) {
  size_t data_size = dynamic_data.memory_size();
  output.get()->resize(data_size);
  memcpy(output.get()->data(), dynamic_data.raw_instance(), data_size);
}

} // namespace CoreLibWrap
