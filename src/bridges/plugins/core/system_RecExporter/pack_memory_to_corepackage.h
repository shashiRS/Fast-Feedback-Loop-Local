#ifndef NEXT_PACK_MEMORY_TO_COREPACKAGE_H
#define NEXT_PACK_MEMORY_TO_COREPACKAGE_H

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/sources.h>
#include <mts/extensibility/package.hpp>
#include <mts/introspection/conversion.hpp>
#include <mts/introspection/xtypes/dynamic_type.hpp>
#include <mts/introspection/xtypes/generator/sdl.hpp>
#include <mts/introspection/xtypes/parser/json.hpp>
#include <mts/runtime/memory_allocator.hpp>
#include <mts/runtime/offline/reader_proxy.hpp>
#include <mts/runtime/service_container.hpp>
#include <mts/runtime/xport/recording_exporter_proxy.hpp>
#include <mts/toolbox/ecu/ecu.hpp>
#include <mts/toolbox/swc/packager.hpp> // For SWC generation
NEXT_RESTORE_WARNINGS

#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
namespace next {
namespace plugins {
namespace rec_exporter {

class TransferCoreLibPackage {
private:
  static inline void *package_payload_ = nullptr;

  static inline std::mutex mem_access_lock_;

public:
  static mts::extensibility::package
  CreatePackageFromData(const next::bridgesdk::plugin_addons::PluginSignalDescription &basic_info, const void *data,
                        size_t size, next::bridgesdk::time_t time) {

    std::lock_guard<std::mutex> lock_guard(mem_access_lock_);

    auto package_payload_size = sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t) + size;

    if (package_payload_ != nullptr) {
      free(package_payload_);
    }

    package_payload_ = malloc(package_payload_size);
    memset(package_payload_, 0, package_payload_size);

    auto *header = reinterpret_cast<mts::toolbox::ecu::software_header *>(package_payload_);
    fillPackageHeader(header, basic_info, size);

    size_t data_payload_offset =
        (size_t)package_payload_ + sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t);
    memcpy((size_t *)data_payload_offset, data, size);

    // Create the MTS generic package from the current payload
    mts::extensibility::package package(mts::runtime::get_default_memory_allocator(),
                                        mts::toolbox::ecu::sensor_software_package_format, package_payload_,
                                        package_payload_size);

    fillMetaData(package, basic_info, header, time);
    return package;
  }

  static void FreePackage() {
    std::lock_guard<std::mutex> lock_guard(mem_access_lock_);

    if (package_payload_ == nullptr) {
      return;
    }

    free(package_payload_);
    package_payload_ = nullptr;
  }

  static void WritePackage(const std::unique_ptr<mts::runtime::xport::recording_exporter_proxy> &exporter_proxy,
                           const mts::extensibility::package &package) {
    std::lock_guard<std::mutex> lock_guard(mem_access_lock_);
    exporter_proxy->export_package(package);
  }

private:
  static void fillMetaData(mts::extensibility::package &package,
                           const next::bridgesdk::plugin_addons::PluginSignalDescription &basic_info,
                           const mts::toolbox::ecu::software_header *header, next::bridgesdk::time_t ref_time) {
    package->source_id = basic_info.source_id;
    package->source_location = 0U;
    package->instance_number = basic_info.instance_id;
    package->cycle_id = basic_info.cycle_id;       // From ECU software header
    package->cycle_counter = header->task_counter; // From ECU software header
    // Export a complete cycle (the exporter will create separate cycle start and cycle end marker packages)
    package->cycle_state =
        mts::extensibility::cycle_start | mts::extensibility::cycle_body | mts::extensibility::cycle_end;
    // Use a non-zero timestamp for MTS 2.6 compatibility!
    package->reference_timestamp = ref_time;
    // No hardware timestamp available
    package->hardware_timestamp.id = ::mts_time_spec_invalid_timebase_id;
    package->hardware_timestamp.time = ::mts_time_spec_invalid_time;
  }

  static void fillPackageHeader(mts::toolbox::ecu::software_header *header,
                                const next::bridgesdk::plugin_addons::PluginSignalDescription &basic_info,
                                size_t data_size) {
    if (data_size > 0x00FFFFFF) {
      error(__FILE__, "Package size larger than uint32_t:24.");
    }

    header->task_id = (uint16_t)basic_info.cycle_id; // task_id type in header is uint16_t
    header->task_counter = static_cast<uint16_t>(0); // Create a cycle counter from our counter
    header->address = (uint32_t)basic_info.vaddr;    // address type in header is uint32_t

    NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wconversion")
    header->control_length.length = (uint32_t)data_size; // Length of the actual measurement data
    NEXT_RESTORE_WARNINGS_LINUX

    header->control_length.function_id = 0U;
    header->control_length.packet_counter = 0U;
    header->control_length.high_resolution_ts = 0U;
    header->control_length.absolute_ts = 0U;
    header->control_length.checksum = 0U;
    header->control_length.reserved = 0U;
  }
};

} // namespace rec_exporter
} // namespace plugins
} // namespace next

#endif // NEXT_PACK_MEMORY_TO_COREPACKAGE_H
