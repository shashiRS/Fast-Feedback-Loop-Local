#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk_macros.h>

#include <array>
#include <fstream>
#include <streambuf>
#include <string>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/package.hpp>
#include <mts/introspection/xtypes/dynamic_type.hpp>
#include <mts/introspection/xtypes/generator/sdl.hpp>
#include <mts/runtime/memory_allocator.hpp>
#include <mts/runtime/offline/reader_proxy.hpp>
#include <mts/runtime/service_container.hpp>
#include <mts/runtime/udex/extractor_proxy.hpp>
#include <mts/runtime/xport/recording_exporter_proxy.hpp>
#include <mts/toolbox/ecu/ecu.hpp>
#include <mts/toolbox/swc/packager.hpp> // For SWC generation
NEXT_RESTORE_WARNINGS

#include <boost/filesystem.hpp>

#include "pack_memory_to_corepackage.h"

/* Structure of the data in the exported recording */
struct export_test_data {
  uint32_t version;
  uint32_t counter;
  double a;
  float b;
  int32_t c;
  int16_t d;
  char e;
};

std::vector<uint8_t> GetSwcBuffer(const std::string &sdl_content) {
  // The software container manifest is a mandatory part of the container
  // Create and fill a manifest
  mts::toolbox::swc::manifest manifest;
  manifest.set_version_major(3U);
  manifest.set_version_minor(21U);
  manifest.set_version_patch_level(0U);
  manifest.set_build_type(2U);
  manifest.set_project_id("ARS540BW11");
  manifest.set_crc(0U);
  // Create a SWC packager object
  mts::toolbox::swc::packager packager;
  // Build an SWC in the memory
  // At least the manifest and the list of SDLs have to be specified
  // The result is a byte array representing the SWC (in the memory)
  auto swc_buffer = packager.build(manifest, {sdl_content});

  return swc_buffer;
}

TEST(RecExportTest, write_dummy_rec_using_cdl) {
  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  // Create an exporter proxy
  mts::runtime::xport::recording_exporter_proxy exporter_proxy(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy.get_export_format("REC");

  if (!format.has_value()) {
    FAIL() << "Test failed, missing export format";
  }

  // Where to export
  std::string export_file_path = "dummy_rec_using_cdl.rec";

  // The lifetime of the above objects must extend the lifetime of the exporter
  // This is why we use a {} scope to illustrate this
  {
    try {
      // Start exporting
      exporter_proxy.start_export(format.value().name, export_file_path);

      // Create the XTypes dynamic data instance representing the data source we will export
      auto export_data_source_type = mts::extensibility::xport::get_export_data_source_type();
      mts::introspection::xtypes::dynamic_data dynamic_data_source(*export_data_source_type);
      dynamic_data_source["source_id"] = mts::extensibility::measurement_object_simulation;
      dynamic_data_source["instance_number"] = 0U;
      dynamic_data_source["format_identifier"] = mts::toolbox::ecu::sensor_software_package_format;
      dynamic_data_source["source_name"] = "SIM VFB";
      // Create a data description for this data source, we will store a CDL describing the `export_test_data` structure
      // used in this test
      auto data_description_with_content_type =
          mts::extensibility::xport::get_export_data_description_with_content_type();
      mts::introspection::xtypes::dynamic_data dynamic_data_description(*data_description_with_content_type);
      dynamic_data_description["file_name"] = "test.cdl";
      dynamic_data_description["type"] = "cdl";
      // Read the content of the CDL file
      std::ifstream ifs("test.cdl", std::ios::binary);
      std::string cdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
      // XTypes sequences are modeling std::vector-like containers
      // Resize the sequence which holds the data description content
      dynamic_data_description["content"].resize(cdl_content.size());
      // ... and copy the data description content into the sequence (similarly how one could do with copying starting
      // at std::vector::at(0))
      std::memcpy(const_cast<uint8_t *>(dynamic_data_description["content"][size_t(0)].raw_instance()),
                  cdl_content.data(), cdl_content.size());
      // Append to the data source's collection
      dynamic_data_source["data_descriptions"].push(dynamic_data_description);

      // Export to the data source channel
      exporter_proxy.export_source(dynamic_data_source);

      // The size of each generic package payload: the ECU software header + low resolution timestamp (2 bytes) +
      // padding length (2 bytes) + the measurement data itself
      constexpr auto package_payload_size =
          sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(export_test_data);
      std::array<uint8_t, package_payload_size> payload{};

      // We will export 10 packages with a counter
      for (uint32_t counter = 1U; counter <= 10U; counter++) {
        // Start with the ECU software header
        auto *header = reinterpret_cast<mts::toolbox::ecu::software_header *>(payload.data());
        header->task_id = 3U;                                  // For demonstration purposes we use Cycle Id = 3
        header->task_counter = static_cast<uint16_t>(counter); // Create a cycle counter from our counter
        header->address = 0x24000000U; // For demonstration purposes we use Virtual Address 0x24000000U
        header->control_length.length = sizeof(export_test_data); // Length of the actual measurement data
        header->control_length.function_id = 0U;
        header->control_length.packet_counter = 0U;
        header->control_length.high_resolution_ts = 0U;
        header->control_length.absolute_ts = 0U;
        header->control_length.checksum = 0U;
        header->control_length.reserved = 0U;

        // Skip ECU software header, low resolution timestamp and padding length fields
        auto *test_data = reinterpret_cast<export_test_data *>(
            payload.data() + sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t));
        // For demonstration purposes set the counter member
        test_data->counter = counter;

        // Create the MTS generic package from the current payload
        mts::extensibility::package package(mts::runtime::get_default_memory_allocator(),
                                            mts::toolbox::ecu::sensor_software_package_format, payload.data(),
                                            payload.size());
        package->source_id = mts::extensibility::measurement_object_simulation;
        package->source_location = 0U;
        package->instance_number = 0U;
        package->cycle_id = header->task_id;           // From ECU software header
        package->cycle_counter = header->task_counter; // From ECU software header
        // Export a complete cycle (the exporter will create separate cycle start and cycle end marker packages)
        package->cycle_state =
            mts::extensibility::cycle_start | mts::extensibility::cycle_body | mts::extensibility::cycle_end;
        // Use a non-zero timestamp for MTS 2.6 compatibility!
        package->reference_timestamp = counter * 100000;
        // No hardware timestamp available
        package->hardware_timestamp.id = ::mts_time_spec_invalid_timebase_id;
        package->hardware_timestamp.time = ::mts_time_spec_invalid_time;

        // Export to the package channel
        exporter_proxy.export_package(package);
      }

      // Finished exporting
      exporter_proxy.end_export();
    } catch (const std::exception &error) {
      FAIL() << "Caught exception, test will exit: '" << error.what() << "'" << std::endl;
    }
  }

  ASSERT_TRUE(boost::filesystem::exists(export_file_path));
  EXPECT_NE(boost::filesystem::file_size(export_file_path), 0);
}

TEST(RecExportTest, write_dummy_rec_using_sdl) {
  using namespace mts::introspection::xtypes;

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  std::ifstream ifs("test.sdl", std::ios::binary);
  std::string sdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

  auto swc_buffer = GetSwcBuffer(sdl_content);

  // Create an exporter proxy
  mts::runtime::xport::recording_exporter_proxy exporter_proxy(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy.get_export_format("REC");

  if (!format.has_value()) {
    FAIL() << "Test failed, missing export format";
  }

  // Where to export
  std::string export_file_path = "dummy_rec_using_sdl.rec";

  {
    try {
      // Start exporting
      exporter_proxy.start_export(format.value().name, export_file_path);

      // Prepare a data source object to export
      mts::extensibility::data_source source{mts::extensibility::make_source_id(source_family::lr_radar, 500),
                                             0U,
                                             0,
                                             1,
                                             mts::toolbox::ecu::sensor_software_package_format,
                                             "ARS540 Device",
                                             ""};

      // Export to the data source channel
      // Pass the SWC buffer from the previous steps as data description content
      exporter_proxy.export_source(source, "swc", "test.swc", reinterpret_cast<const uint8_t *>(swc_buffer.data()),
                                   swc_buffer.size());

      // The size of each generic package payload: the ECU software header + low resolution timestamp (2 bytes) +
      // padding length (2 bytes) + the measurement data itself
      constexpr auto package_payload_size =
          sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(export_test_data);
      std::array<uint8_t, package_payload_size> payload{};

      // We will export 10 packages with a counter
      for (uint32_t counter = 1U; counter <= 10U; counter++) {
        // Start with the ECU software header
        auto *header = reinterpret_cast<mts::toolbox::ecu::software_header *>(payload.data());
        header->task_id = 3U;                                  // For demonstration purposes we use Cycle Id = 3
        header->task_counter = static_cast<uint16_t>(counter); // Create a cycle counter from our counter
        header->address = 0x24000000U; // For demonstration purposes we use Virtual Address 0x24000000U
        header->control_length.length = sizeof(export_test_data); // Length of the actual measurement data
        header->control_length.function_id = 0U;
        header->control_length.packet_counter = 0U;
        header->control_length.high_resolution_ts = 0U;
        header->control_length.absolute_ts = 0U;
        header->control_length.checksum = 0U;
        header->control_length.reserved = 0U;

        // Skip ECU software header, low resolution timestamp and padding length fields
        auto *test_data = reinterpret_cast<export_test_data *>(
            payload.data() + sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t));
        // For demonstration purposes set the counter member
        test_data->counter = counter;

        // Create the MTS generic package from the current payload
        mts::extensibility::package package(mts::runtime::get_default_memory_allocator(),
                                            mts::toolbox::ecu::sensor_software_package_format, payload.data(),
                                            payload.size());
        package->source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500);
        package->source_location = 0U;
        package->instance_number = 0U;
        package->cycle_id = header->task_id;           // From ECU software header
        package->cycle_counter = header->task_counter; // From ECU software header
        // Export a complete cycle (the exporter will create separate cycle start and cycle end marker packages)
        package->cycle_state =
            mts::extensibility::cycle_start | mts::extensibility::cycle_body | mts::extensibility::cycle_end;
        // Use a non-zero timestamp for MTS 2.6 compatibility!
        package->reference_timestamp = counter * 100000;
        // No hardware timestamp available
        package->hardware_timestamp.id = ::mts_time_spec_invalid_timebase_id;
        package->hardware_timestamp.time = ::mts_time_spec_invalid_time;

        // Export to the package channel
        exporter_proxy.export_package(package);
      }

      // Finished exporting
      exporter_proxy.end_export();
    } catch (const std::exception &error) {
      FAIL() << "Caught exception, test will exit: '" << error.what() << "'" << std::endl;
    }
  }

  ASSERT_TRUE(boost::filesystem::exists(export_file_path));
  EXPECT_NE(boost::filesystem::file_size(export_file_path), 0);
}

TEST(RecExportTest, write_dummy_rec_using_dynamic_type) {
  using namespace mts::introspection::xtypes;
  using namespace mts::runtime::udex;

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  dynamic_type::ptr type;
  // This code is here only to register a data source with a CDL (crypted SDL) so we can get an XTypes schema based on
  // an actual SDL information
  {
    std::string k_default_storage = "file:signal_storage?mode=memory&cache=shared";
    // UDEx extractor used to register a data source and get an XTypes schema
    mts::runtime::udex::extractor_proxy extractor(k_default_storage, 0ms, service_container_ptr->get_service_locator());

    // Read the content of the CDL file
    std::ifstream ifs("test.cdl", std::ios::binary);
    std::string cdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    extractor.register_source_with_data_description(
        mts::extensibility::make_source_id(source_family::lr_radar, 500), 0U, "ARS540 Device", "test.cdl", "cdl",
        reinterpret_cast<const uint8_t *>(cdl_content.data()), cdl_content.size());

    // Get type schema with UDEx URL and ***request annotation***!
    // View = "ALGO_01", Cycle = 3, Address = 603979776
    type = extractor.get_type("ARS540 Device.ALGO_01.SMyStruct_t", false, true);
  }

  // We can generate and SDL from an annotated XTypes schema
  auto sdl_content = mts::introspection::xtypes::generator::print_sdl(*type);

  auto swc_buffer = GetSwcBuffer(sdl_content);

  // Create an exporter proxy
  mts::runtime::xport::recording_exporter_proxy exporter_proxy(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy.get_export_format("REC");

  if (!format.has_value()) {
    FAIL() << "Test failed, missing export format";
  }

  // Where to export
  std::string export_file_path = "dummy_rec_using_dynamic_type.rec";

  {
    try {
      // Start exporting
      exporter_proxy.start_export(format.value().name, export_file_path);

      // Prepare a data source object to export
      mts::extensibility::data_source source{mts::extensibility::make_source_id(source_family::lr_radar, 500),
                                             0U,
                                             0,
                                             1,
                                             mts::toolbox::ecu::sensor_software_package_format,
                                             "ARS540 Device",
                                             ""};

      // Export to the data source channel
      // Pass the SWC buffer from the previous steps as data description content
      exporter_proxy.export_source(source, "swc", "test.swc", reinterpret_cast<const uint8_t *>(swc_buffer.data()),
                                   swc_buffer.size());

      // The size of each generic package payload: the ECU software header + low resolution timestamp (2 bytes) +
      // padding length (2 bytes) + the measurement data itself
      constexpr auto package_payload_size =
          sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(export_test_data);
      std::array<uint8_t, package_payload_size> payload{};

      // We will export 10 packages with a counter
      for (uint32_t counter = 1U; counter <= 10U; counter++) {
        // Start with the ECU software header
        auto *header = reinterpret_cast<mts::toolbox::ecu::software_header *>(payload.data());
        header->task_id = 3U;                                  // For demonstration purposes we use Cycle Id = 3
        header->task_counter = static_cast<uint16_t>(counter); // Create a cycle counter from our counter
        header->address = 0x24000000U; // For demonstration purposes we use Virtual Address 0x24000000U
        header->control_length.length = sizeof(export_test_data); // Length of the actual measurement data
        header->control_length.function_id = 0U;
        header->control_length.packet_counter = 0U;
        header->control_length.high_resolution_ts = 0U;
        header->control_length.absolute_ts = 0U;
        header->control_length.checksum = 0U;
        header->control_length.reserved = 0U;

        // Skip ECU software header, low resolution timestamp and padding length fields
        auto *test_data = reinterpret_cast<export_test_data *>(
            payload.data() + sizeof(mts::toolbox::ecu::software_header) + sizeof(uint16_t) + sizeof(uint16_t));
        // For demonstration purposes set the counter member
        test_data->a = double(counter + 1);
        test_data->counter = counter;

        // Create the MTS generic package from the current payload
        mts::extensibility::package package(mts::runtime::get_default_memory_allocator(),
                                            mts::toolbox::ecu::sensor_software_package_format, payload.data(),
                                            payload.size());
        package->source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500);
        package->source_location = 0U;
        package->instance_number = 0U;
        package->cycle_id = header->task_id;           // From ECU software header
        package->cycle_counter = header->task_counter; // From ECU software header
        // Export a complete cycle (the exporter will create separate cycle start and cycle end marker packages)
        package->cycle_state =
            mts::extensibility::cycle_start | mts::extensibility::cycle_body | mts::extensibility::cycle_end;
        // Use a non-zero timestamp for MTS 2.6 compatibility!
        package->reference_timestamp = counter * 100000;
        // No hardware timestamp available
        package->hardware_timestamp.id = ::mts_time_spec_invalid_timebase_id;
        package->hardware_timestamp.time = ::mts_time_spec_invalid_time;

        // Export to the package channel
        exporter_proxy.export_package(package);
      }

      // Finished exporting
      exporter_proxy.end_export();
    } catch (const std::exception &error) {
      FAIL() << "Caught exception, test will exit: '" << error.what() << "'" << std::endl;
    }
  }

  ASSERT_TRUE(boost::filesystem::exists(export_file_path));
  EXPECT_NE(boost::filesystem::file_size(export_file_path), 0);
}

TEST(RecExportTest, test_corepackage_nofail) {
  using namespace mts::introspection::xtypes;

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  std::ifstream ifs("test.sdl", std::ios::binary);
  std::string sdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

  auto swc_buffer = GetSwcBuffer(sdl_content);

  // Create an exporter proxy
  std::unique_ptr<mts::runtime::xport::recording_exporter_proxy> exporter_proxy =
      std::make_unique<mts::runtime::xport::recording_exporter_proxy>(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy->get_export_format("REC");

  if (!format.has_value()) {
    FAIL() << "Test failed, missing export format";
  }

  // Where to export
  std::string export_file_path = "test_corepackage_nofail.rec";

  {
    try {
      // Start exporting
      exporter_proxy->start_export(format.value().name, export_file_path);

      // Prepare a data source object to export
      mts::extensibility::data_source source{mts::extensibility::make_source_id(source_family::lr_radar, 500),
                                             0U,
                                             0,
                                             1,
                                             mts::toolbox::ecu::sensor_software_package_format,
                                             "ARS540 Device",
                                             ""};

      // Export to the data source channel
      // Pass the SWC buffer from the previous steps as data description content
      exporter_proxy->export_source(source, "swc", "test.swc", reinterpret_cast<const uint8_t *>(swc_buffer.data()),
                                    swc_buffer.size());

      // We will export 10 packages with a counter
      for (uint32_t counter = 1U; counter <= 10U; counter++) {
        next::bridgesdk::plugin_addons::PluginSignalDescription basic_info;
        basic_info.source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500); // @ line 450
        basic_info.instance_id = 0;                                                              // @ line 451
        basic_info.cycle_id = 3;                                                                 // from SDL view
        basic_info.vaddr = 0x24000000U;                                                          // from SDL group

        export_test_data test_data;
        test_data.counter = counter;
        auto package = next::plugins::rec_exporter::TransferCoreLibPackage::CreatePackageFromData(
            basic_info, &test_data, sizeof(test_data), counter * 100000);

        next::plugins::rec_exporter::TransferCoreLibPackage::WritePackage(exporter_proxy, package);

        next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();
      }

      // Finished exporting
      exporter_proxy->end_export();
    } catch (const std::exception &error) {
      FAIL() << "Caught exception, test will exit: '" << error.what() << "'" << std::endl;
    }
  }

  ASSERT_TRUE(boost::filesystem::exists(export_file_path));
  EXPECT_NE(boost::filesystem::file_size(export_file_path), 0);
}

TEST(RecExportTest, test_corepackage__threads_nofail) {
  using namespace mts::introspection::xtypes;

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  std::ifstream ifs("test.sdl", std::ios::binary);
  std::string sdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

  auto swc_buffer = GetSwcBuffer(sdl_content);

  // Create an exporter proxy
  std::unique_ptr<mts::runtime::xport::recording_exporter_proxy> exporter_proxy =
      std::make_unique<mts::runtime::xport::recording_exporter_proxy>(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy->get_export_format("REC");

  if (!format.has_value()) {
    FAIL() << "Test failed, missing export format";
  }

  // Where to export
  std::string export_file_path = "test_corepackage_threads_nofail.rec";

  {
    try {
      // Start exporting
      exporter_proxy->start_export(format.value().name, export_file_path);

      // Prepare a data source object to export
      mts::extensibility::data_source source{mts::extensibility::make_source_id(source_family::lr_radar, 500),
                                             0U,
                                             0,
                                             1,
                                             mts::toolbox::ecu::sensor_software_package_format,
                                             "ARS540 Device",
                                             ""};

      // Export to the data source channel
      // Pass the SWC buffer from the previous steps as data description content
      exporter_proxy->export_source(source, "swc", "test.swc", reinterpret_cast<const uint8_t *>(swc_buffer.data()),
                                    swc_buffer.size());

      std::thread thread1 = std::thread([&]() {
        // We will export 10 packages with a counter
        for (uint32_t counter = 1U; counter <= 10U; counter++) {
          next::bridgesdk::plugin_addons::PluginSignalDescription basic_info;
          basic_info.source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500); // @ line 450
          basic_info.instance_id = 0;                                                              // @ line 451
          basic_info.cycle_id = 3;                                                                 // from SDL view
          basic_info.vaddr = 0x24000000U;                                                          // from SDL group

          export_test_data test_data;
          test_data.counter = counter;
          auto package = next::plugins::rec_exporter::TransferCoreLibPackage::CreatePackageFromData(
              basic_info, &test_data, sizeof(test_data), counter * 100000);

          next::plugins::rec_exporter::TransferCoreLibPackage::WritePackage(exporter_proxy, package);

          next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();
        }
      });

      std::thread thread2 = std::thread([&]() {
        // We will export 10 packages with a counter
        for (uint32_t counter = 11U; counter <= 20U; counter++) {
          next::bridgesdk::plugin_addons::PluginSignalDescription basic_info;
          basic_info.source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500); // @ line 450
          basic_info.instance_id = 0;                                                              // @ line 451
          basic_info.cycle_id = 3;                                                                 // from SDL view
          basic_info.vaddr = 0x24000000U;                                                          // from SDL group

          export_test_data test_data;
          test_data.counter = counter;
          auto package = next::plugins::rec_exporter::TransferCoreLibPackage::CreatePackageFromData(
              basic_info, &test_data, sizeof(test_data), counter * 100000);

          next::plugins::rec_exporter::TransferCoreLibPackage::WritePackage(exporter_proxy, package);

          next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();
        }
      });

      thread1.join();
      thread2.join();

      // Finished exporting
      exporter_proxy->end_export();
    } catch (const std::exception &error) {
      FAIL() << "Caught exception, test will exit: '" << error.what() << "'" << std::endl;
    }
  }

  ASSERT_TRUE(boost::filesystem::exists(export_file_path));
  EXPECT_NE(boost::filesystem::file_size(export_file_path), 0);
}
