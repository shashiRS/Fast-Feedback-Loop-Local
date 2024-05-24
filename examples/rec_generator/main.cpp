/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

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

#include "pack_memory_to_corepackage.h"
#include "signal_types.h"

struct Package {
  uint64_t timestamp;
  uint16_t sigheader_timestamp;
  uint16_t sigheader_counter;
  std::string name;
};

std::map<std::string, uint64_t> name_to_virtual_address{
    {"DataPackage_A", 0x80200000}, {"DataPackage_B", 0x80300000}, {"Trigger_A", 0x80400000}, {"Trigger_B", 0x80500000}};

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

void readInput(const std::string &filepath, std::vector<Package> &packages) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open the file." << std::endl;
    return;
  }

  std::string line;
  std::getline(file, line); // skip the header
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    Package pkg;
    char comma;

    if (!(iss >> pkg.timestamp >> comma >> pkg.sigheader_timestamp >> comma >> pkg.sigheader_counter >> comma >>
          pkg.name)) {
      std::cerr << "Error: Unable to read data from file." << std::endl;
      return;
    }

    if (!pkg.name.empty()) {
      packages.push_back(pkg);
    }
  }

  // Close the file
  file.close();
}

int main(int argc, const char **argv) {
  (void)argc;
  (void)argv;

  using namespace mts::introspection::xtypes;

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  std::ifstream ifs("generated_rec.sdl", std::ios::binary);
  std::string sdl_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

  auto swc_buffer = GetSwcBuffer(sdl_content);

  // Create an exporter proxy
  std::unique_ptr<mts::runtime::xport::recording_exporter_proxy> exporter_proxy =
      std::make_unique<mts::runtime::xport::recording_exporter_proxy>(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy->get_export_format("REC");

  if (!format.has_value()) {
    std::cerr << "Error: Invalid export format, exiting." << std::endl;
    return 1;
  }

  std::string export_file_path = "generated.rec";
  std::string input_packages_path = "packages.txt";
  std::vector<Package> packages;

  readInput(input_packages_path, packages);

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

    for (auto &package : packages) {
      next::bridgesdk::plugin_addons::PluginSignalDescription basic_info;
      basic_info.source_id = mts::extensibility::make_source_id(source_family::lr_radar, 500); // @ line 450
      basic_info.instance_id = 0;                                                              // @ line 451
      basic_info.cycle_id = 2;                                                                 // from SDL view
      basic_info.vaddr = name_to_virtual_address[package.name];                                // from SDL group

      next::examples::signal_types::generic_signal_64 test_data;
      test_data.uiVersionNumber = 1;
      test_data.sSigHeader.uiTimeStamp = package.sigheader_timestamp;
      test_data.sSigHeader.uiMeasurementCounter = package.sigheader_counter;
      test_data.sSigHeader.uiCycleCounter = package.sigheader_counter;
      test_data.sSigHeader.eSigStatus = 1;

      auto mts_package = next::plugins::rec_exporter::TransferCoreLibPackage::CreatePackageFromData(
          basic_info, &test_data, sizeof(test_data), 100 + package.timestamp);

      next::plugins::rec_exporter::TransferCoreLibPackage::WritePackage(exporter_proxy, mts_package);

      next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();
    }

    // Finished exporting
    exporter_proxy->end_export();
  } catch (const std::exception &error) {
    std::cout << error.what();
    return 1;
  }

  return 0;
}
