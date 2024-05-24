/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_CORE_LIB_READER_IMPL_HPP
#define NEXT_CORE_LIB_READER_IMPL_HPP

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/offline/replay_proxy.hpp>
#include <mts/runtime/processor/data_processor_proxy.hpp>
#include <mts/runtime/udex/extractor_proxy.hpp>
#include "core_lib_package.h"
NEXT_RESTORE_WARNINGS

#include <next/sdk/types/package_hash.hpp>

#include <next/player/plugin/reader_interface.hpp>

namespace next {
namespace player {
namespace plugin {
namespace corelibreader {

class NextCoreLibReaderImpl {
public:
  friend class NextCoreLibReaderImplTester;

public:
  NextCoreLibReaderImpl() { processors_ = mts::runtime::processor::get_data_processors(); }
  virtual ~NextCoreLibReaderImpl() = default;
  std::shared_ptr<mts::runtime::offline::replay_proxy> replay_ptr_;
  std::vector<std::unique_ptr<mts::runtime::processor::data_processor_proxy>> processors_;

  std::unordered_map<size_t, uint32_t> hash_to_size_map_;
  std::unordered_map<size_t, uint64_t> hash_to_next_vaddr_map_;
  EthernetPackage::serialization_info_map ethernet_service_info_;
  std::unordered_map<size_t, CoreLibProcessor> processor_info_;

  next::sdk::types::DataDescription
  CreateDescriptionWithBinary(std::shared_ptr<mts::runtime::data_source_provider> data_source_provider,
                              const mts::extensibility::data_source &data_source,
                              const mts::extensibility::data_description &mts_data_description) {

    next::sdk::types::DataDescription description_temp;
    description_temp.device_name = data_source.get_sanitized_source_name();
    description_temp.instance_number = data_source.get_instance_number();
    description_temp.source_id = data_source.get_source_id();

    description_temp.filename = mts_data_description.get_file_name();
    description_temp.description_format_type =
        next::sdk::types::getDataDescriptionFormatEnum(mts_data_description.get_type());
    description_temp.device_format_type =
        next::sdk::types::getPackageFormatTypeEnum(data_source.get_format_identifier());
    if (description_temp.device_format_type != next::sdk::types::PACKAGE_FORMAT_TYPE_RTRAGE) {
      auto data_description_content =
          data_source_provider->get_data_description_content(data_source, mts_data_description).get();
      FillBinaryDescription(description_temp, data_description_content);
    }

    if (description_temp.device_format_type == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA ||
        description_temp.device_format_type == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW ||
        description_temp.device_format_type == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW ||
        description_temp.device_format_type == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      SetupSpecialPackageHandling(description_temp);
    }

    return description_temp;
  }

  next::sdk::types::DataDescription CreateEmptyDescription(const mts::extensibility::data_source &data_source) const {
    next::sdk::types::DataDescription description_temp;

    description_temp.device_name = data_source.get_sanitized_source_name();
    description_temp.instance_number = data_source.get_instance_number();
    description_temp.source_id = data_source.get_source_id();
    description_temp.filename = "";
    description_temp.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_UNKNOWN;
    description_temp.device_format_type =
        next::sdk::types::getPackageFormatTypeEnum(data_source.get_format_identifier());
    return description_temp;
  }

private:
  void AddProcessorInfoIfMatching(const next::sdk::types::DataDescription &description) {

    try {
      // Iterate through the list of processors
      for (const auto &processor : processors_) {
        // Retrieve all input formats handled by the processor
        auto supported_formats = processor->get_supported_formats();
        // Iterate through the list of supported formats
        for (const auto &format : supported_formats) {
          // Compute the source identity supported by this input format
          auto processor_source_id = mts::extensibility::make_source_id(
              static_cast<mts::extensibility::source_family>(format.family), format.model);
          // Get the corresponding enumeration (CsZ: kind of losing flexibility here)
          auto processor_package_format_type = next::sdk::types::getPackageFormatTypeEnum(format.format_identifier);
          // Check if the processor can handle this data source
          if (description.source_id == processor_source_id) {
            debug(__FILE__, "adding processor for s_id:{0}, vaddr:{1}, instance_number{2}, cycle_id{3} formatType_{4},",
                  processor_source_id, format.virtual_address, description.instance_number, format.cycle_id,
                  format.format_identifier);

            next::sdk::types::IPackage::PackageMetaInfo info;
            info.cycle_id = format.cycle_id;
            info.virtual_address = format.virtual_address;
            info.source_id = description.source_id;
            info.instance_number = description.instance_number;
            size_t processor_hash = next::sdk::types::PackageHash::hashMetaInfo(info);

            std::vector<std::string> port_name_urls;
            // Retrieve all 'ports' output by the processor
            auto supported_ports = processor->get_supported_ports();
            // Create full port UDEx URLs by concatenating source name with port name
            for (const auto &supported_port : supported_ports) {
              if (supported_port.input_cycle_id == info.cycle_id &&
                  supported_port.input_virtual_address == info.virtual_address
                  // && supported_port.port_type == mts::runtime::processor::port_type_rte
              ) {
                // CsZ: We should decide if we restrict only to "RTE" type ports or advertise all ports
                // (processor ports that are never used by a simulation)
                port_name_urls.emplace_back(description.device_name + "." + supported_port.name);
              }
            }
            processor_info_[processor_hash] = {port_name_urls, processor_package_format_type};
          }
        }
      }
    } catch (std::exception &e) {
      next::sdk::logger::error(__FILE__, "Processor discovery failed {}", description.device_name, e.what());
    }
  }

  void SetupSpecialPackageHandling(const next::sdk::types::DataDescription &description) {

    AddProcessorInfoIfMatching(description);

    std::string k_default_storage = "file:signal_storage?mode=memory&cache=shared";
    try {
      auto udex_extractor_ptr = std::make_shared<mts::runtime::udex::extractor_proxy>(k_default_storage);
      auto udex_signal_storage = std::make_shared<mts::runtime::udex::signal_storage_proxy>(k_default_storage);

      // register source with file format and get data description signature
      auto signatures = udex_extractor_ptr->register_source_with_data_description(
          description.source_id, description.instance_number, description.device_name, description.filename,
          next::sdk::types::getDataDescriptionFormatString(description.description_format_type),
          (uint8_t *)description.binary_description.data(), description.binary_description.size());

      if (signatures.empty()) {
        return;
      }

      for (const auto &signature : signatures) {
        auto data_description_id = udex_signal_storage->get_data_description_id(signature);
        auto view_nodes_sequence = udex_signal_storage->get_root_nodes(data_description_id);

        auto *view_node_p = view_nodes_sequence->data;
        for (unsigned int i = 0; i < view_nodes_sequence->size; i++) {
          std::size_t hash = 0;

          if (description.device_format_type == sdk::types::PACKAGE_FORMAT_TYPE_MTA ||
              description.device_format_type == sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW ||
              description.device_format_type == sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
            GenerateMtsSwGroupInfoFromNodes(udex_signal_storage, view_node_p, view_node_p->view.cycle_id,
                                            description.source_id, description.instance_number, hash);
          }
          if (description.device_format_type == sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
            GenerateEthernetInfoFromNodes(udex_signal_storage, view_node_p, view_node_p->view.interface_version);
          }
          view_node_p++;
        }
      }
    } catch (std::exception &e) {
      next::sdk::logger::error(__FILE__, "Failed to register source in CoreLibReader {}", description.device_name,
                               e.what());
      return;
    }
  }

  void
  FillBinaryDescription(next::sdk::types::DataDescription &description_temp,
                        const std::tuple<std::unique_ptr<uint8_t[]>, std::size_t> &data_description_content) const {
    size_t content_size = std::get<1>(data_description_content);
    if (std::get<0>(data_description_content) == nullptr) {
      return;
    }
    if (content_size == 0) {
      return;
    }
    description_temp.binary_description.resize(content_size);
    std::copy_n(reinterpret_cast<const char *>(std::get<0>(data_description_content).get()), content_size,
                description_temp.binary_description.data());
  }

  void GenerateEthernetInfoFromNodes(mts::runtime::udex::signal_storage_proxy::shared_pointer udex_signal_storage,
                                     const mts_type_node_t *node, uint8_t file_type) {
    if (!udex_signal_storage) {
      return;
    }
    if (file_type == 0U) {
      // ARXML data decsription file
      if (node->node_type == mts::extensibility::node_type_view) {
        auto child_nodes = udex_signal_storage->get_node_children(node->node_id);
        auto *child_node = child_nodes->data;
        if (child_node == nullptr) {
          return;
        }
        for (unsigned int node_index = 0; node_index < node->children_count; node_index++) {
          if (child_node->node_type == mts::extensibility::node_type_view ||
              child_node->node_type == mts::extensibility::node_type_group) {
            GenerateEthernetInfoFromNodes(udex_signal_storage, child_node, file_type);
          }
          child_node++;
        }
      } else if (node->node_type == mts::extensibility::node_type_group) {
        auto service_id = static_cast<uint16_t>((node->group.message_id >> 16U));
        if (node->group.pdus.size != 1ULL) {
          return;
        }
        auto pdu_node = node->group.pdus.data[0];
        auto interface_version = static_cast<uint8_t>(pdu_node.header_identifier);
        auto serialization = node->group.serialization;
        auto itr = ethernet_service_info_.find(service_id);
        if (itr != ethernet_service_info_.end()) {
          if (itr->second.serialization_technology_type !=
                  static_cast<serialization_technology>(serialization.serialization_technology) &&
              itr->second.interface_version != interface_version &&
              itr->second.protocol_version != serialization.serialization_technology_version) {
            next::sdk::logger::debug(
                __FILE__,
                "Service ID repeated and serialization information is not matching with previous occurrence. "
                "Data publishing for the topic corresponding to service ID: {0} will not work correctly.",
                service_id);
          }
        } else {
          ethernet_service_info_[service_id] = {
              static_cast<serialization_technology>(serialization.serialization_technology),
              node->view.interface_version, serialization.serialization_technology_version};
        }
      }
    } else if (file_type == 1U) {
      // FIBEX data decsription file
      if (node->node_type == mts::extensibility::node_type_view) {
        auto child_nodes = udex_signal_storage->get_node_children(node->node_id);
        auto *child_node = child_nodes->data;
        if (child_node == nullptr) {
          return;
        }
        auto service_id = (uint16_t)node->view.service_id;
        auto ancestors = udex_signal_storage->get_node_ancestors(node->node_id);
        auto transport_info = ancestors->data[ancestors->size - 1].view.transports;
        auto serialization = ancestors->data[ancestors->size - 1].view.serialization;
        auto itr = ethernet_service_info_.find(service_id);
        if (itr != ethernet_service_info_.end() && transport_info.size > 0) {
          if (itr->second.serialization_technology_type !=
                  static_cast<serialization_technology>(serialization.serialization_technology) &&
              itr->second.interface_version != node->view.interface_version &&
              itr->second.protocol_version != serialization.serialization_technology_version) {
            next::sdk::logger::debug(
                __FILE__,
                "Service ID repeated and serialization information is not matching with previous occurrence. "
                "Data publishing for the topic corresponding to service ID: {0} will not work correctly.",
                service_id);
          }
        } else if (transport_info.size > 0) {
          ethernet_service_info_[service_id] = {
              static_cast<serialization_technology>(serialization.serialization_technology),
              node->view.interface_version, serialization.serialization_technology_version};
        }

        for (unsigned int node_index = 0; node_index < node->children_count; node_index++) {
          if (child_node->node_type == mts::extensibility::node_type_view) {
            GenerateEthernetInfoFromNodes(udex_signal_storage, child_node, file_type);
          }
          child_node++;
        }
      }
    }
    return;
  }

  void GenerateMtsSwGroupInfoFromNodes(mts::runtime::udex::signal_storage_proxy::shared_pointer udex_signal_storage,
                                       const mts_type_node_t *node, uint32_t cycle_id, const uint16_t source_id,
                                       const uint32_t instance_number, std::size_t &prev_hash) {
    if (!udex_signal_storage) {
      return;
    }

    if (node->node_type == mts::extensibility::node_type_view) {

      auto child_nodes = udex_signal_storage->get_node_children(node->node_id);
      auto *child_node = child_nodes->data;
      if (child_node == nullptr) {
        return;
      }

      std::size_t hash = 0;
      for (unsigned int node_index = 0; node_index < node->children_count; node_index++) {
        GenerateMtsSwGroupInfoFromNodes(udex_signal_storage, child_node, node->view.cycle_id, source_id,
                                        instance_number, hash);
        child_node++;
      }
    } else if (node->node_type == mts::extensibility::node_type_group) {
      uint64_t vaddr = node->group.address;
      size_t hash = next::sdk::types::PackageHash::hashMetaInfo({source_id, instance_number, cycle_id, vaddr, 0, 0});
      if (vaddr != 0) {
        hash_to_size_map_.insert(std::make_pair(hash, node->group.size * node->group.array_length));
        hash_to_next_vaddr_map_.insert(std::make_pair(prev_hash, vaddr));
      }
      prev_hash = hash;
    }
    return;
  }
};

} // namespace corelibreader
} // namespace plugin
} // namespace player
} // namespace next
#endif // NEXT_CORE_LIB_READER_IMPL_HPP
