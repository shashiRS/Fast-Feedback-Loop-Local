/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_HASH_MANAGER
#define UDEX_HASH_MANAGER

#include <mts/extensibility/type_node.h>
#include <mts/toolbox/can/can_types.h>
#include <mts/toolbox/ecu/ecu_types.h>
#include <mts/extensibility/sources.hpp>
#include <mts/runtime/udex/extractor_proxy.hpp>
#include <mts/toolbox/ethernet/ethernet.hpp>
#include <mts/toolbox/gps/gps.hpp>

#ifndef NO_USE_LOGGER_NAMESPACE
#define UNDEF_NO_USE_LOGGER_NAMESPACE
#endif
#define NO_USE_LOGGER_NAMESPACE
#include <mts/runtime/processor/data_processor_proxy.hpp>
#include <next/sdk/logger/logger.hpp>
#ifdef UNDEF_NO_USE_LOGGER_NAMESPACE
#undef NO_USE_LOGGER_NAMESPACE
#undef UNDEF_NO_USE_LOGGER_NAMESPACE
#endif

#include "next/sdk/types/package_data_types.hpp"
#include "next/sdk/types/package_hash.hpp"

namespace next {
namespace udex {

inline uint32_t get_data_length_from_dlc(uint8_t dlc) {
  static const uint8_t data_length_translation_table[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

  uint32_t data_length = 0;
  if (dlc < sizeof(data_length_translation_table)) {
    data_length = data_length_translation_table[dlc];
  }

  return data_length;
}

const size_t gps_nmea_packet_size[] = {
    60,  // mts_gps_nmea_gga
    40,  // mts_gps_nmea_gsa
    128, // mts_gps_nmea_gsv
    62,  // mts_gps_nmea_rmc
    33,  // mts_gps_nmea_vtg
    31   // mts_gps_nmea_gll
};

class HashManager {
public:
  struct HashTopicInformation {
    size_t hash;
    std::string data_source_name;
    std::string format_type_string;
    uint32_t cycle_id;
    uint16_t source_id;
    uint32_t instance_id;
    uint64_t vaddr;
  };

  HashManager() {}

  virtual ~HashManager() {}

  void Initialize(mts::runtime::udex::extractor_proxy::shared_pointer extractor_proxy) {
    extractor_proxy_ = std::move(extractor_proxy);
    processors_ = mts::runtime::processor::get_data_processors();
  }

  void Terminate() {
    for (auto &info : url_to_info_cache_) {
      info.second.Reset();
    }

    vaddr_to_hash_cache_.clear();
    url_to_info_cache_.clear();
    url_to_hash_cache_.clear();
    hash_to_url_map_.clear();

    if (extractor_proxy_) {
      extractor_proxy_.reset();
    }
  }

  void GenerateHashesAndUrls(const std::string &data_source_name, const uint16_t source_id,
                             const uint32_t instance_number, const std::string &format_identifier) {
    if (!extractor_proxy_) {
      return;
    }

    try {
      // Get the list of 'View' nodes associated with the data source
      auto view_nodes_sequence = extractor_proxy_->get_root_nodes(data_source_name);
      // Iterate the nodes
      for (std::size_t i{}; i < view_nodes_sequence->size; i++) {
        const auto *view_node_ptr = &view_nodes_sequence->data[i];
        // Generate hashes and UDEx URLs
        std::size_t hash = 0;
        GenerateHashesAndUrlsForNodes(view_node_ptr, data_source_name, view_node_ptr->view.cycle_id, data_source_name,
                                      source_id, instance_number, format_identifier, hash,
                                      view_node_ptr->view.interface_version);
      }
    } catch (const std::exception &e) {
      next::sdk::logger::warn(__FILE__, "GenerateHashesAndUrls error: {}", e.what());
      return;
    }
  }

  void GenerateHashesAndUrls(const std::string &port_name, const size_t port_size, const std::string &data_source_name,
                             const uint16_t source_id, const uint32_t instance_number,
                             const std::string &format_identifier) {
    url_info_t url_info;
    url_info.data_source_name = data_source_name;
    url_info.source_id = source_id;
    url_info.instance_number = instance_number;
    url_info.format_identifier = format_identifier;

    auto hash_vaddress = next::sdk::types::PackageHash::hashManualPort(port_name, port_size);
    auto hash = next::sdk::types::PackageHash::hashMetaInfo({source_id, instance_number, 0, hash_vaddress, 0, 0});
    url_info.hash = hash;

    url_to_hash_cache_[port_name] = hash;
    HashTopicInformation info = {hash,      data_source_name, format_identifier, 0,
                                 source_id, instance_number,  hash_vaddress};
    topic_to_hash_info_temp.insert(std::make_pair(port_name, info));

    return;
  }

  mts_udex_extractor_subscription_handle_t GetHashByVaddr(const uint64_t vaddr) {
    auto pos = vaddr_to_hash_cache_.find(vaddr);
    if (pos != vaddr_to_hash_cache_.end()) {
      return pos->second;
    }
    return 0;
  }

  mts_udex_extractor_subscription_handle_t GetHashByPortname(const std::string &data_source_name,
                                                             const std::string &port_name, bool append_device) {
    std::string complete_url = "";
    if (append_device) {
      // nodes don't know the device name and provide only the port name until view level
      complete_url = data_source_name;
      complete_url += ".";
      complete_url += port_name;
    } else {
      // we don't know who will use it like this at the moment
      complete_url = port_name;
    }

    auto pos = url_to_hash_cache_.find(complete_url);
    if (pos != url_to_hash_cache_.end()) {
      return pos->second;
    }

    return 0; // should not happen
  }

  const std::unordered_map<std::string, size_t> &GetTopicsAndHashes() { return url_to_hash_cache_; }

  bool IsProcessor(const std::string &port_name) {
    try {
      // Iterate through the list of processors
      for (const auto &processor : processors_) {
        auto supported_ports = processor->get_supported_ports();
        for (auto &port : supported_ports) {
          if (port_name == port.name) {
            return true;
          }
        }
      }
    } catch (std::exception &e) {
      next::sdk::logger::error(__FILE__, "Processor discovery failed {}", port_name, e.what());
    }

    return false;
  }

  bool get_schema(const std::string &url, const uint16_t source_id, std::string &schema) {
    if (!extractor_proxy_) {
      return false;
    }
    try {
      // Only annotate sources that are ECUs, generating SDLs is only supported
      // for XTypes schemas that are annotated with measurement information
      bool annotate_schema = mts::extensibility::is_sensor(source_id);
      // Do not pretty print
      bool pretty_print = false;
      try {
        // First try to access the schema without ignoring schema errors
        schema = extractor_proxy_->get_type_schema(url, false, annotate_schema, pretty_print);
      } catch (const std::invalid_argument &e) {
        // Second try, access the schema with ignoring schema errors
        schema = extractor_proxy_->get_type_schema(url, true, annotate_schema, pretty_print);
        next::sdk::logger::debug(__FILE__, "Failed to get schema for '{0}' {1}, fallback to non-validated schema",
                                 url.c_str(), e.what());
      }
    } catch (const std::exception &e) {
      next::sdk::logger::debug(__FILE__, "Failed to get schema for '{0}' {1}", url.c_str(), e.what());
      return false;
    }

    if (!schema.empty()) {
      schema.erase(std::remove(schema.begin(), schema.end(), ' '), schema.end());
      return true;
    }

    next::sdk::logger::debug(__FILE__, "Failed to get schema for '{0}'", url.c_str());
    return false;
  }

  // function contains only the currently registered topics
  const std::unordered_map<std::string, HashTopicInformation> &GetNewRegisteredTopicsList() {
    return topic_to_hash_info_temp;
  }

  // clear of the curently registered sources to prepare for next RegisterDataDescription
  void ClearNewRegisteredTopicsList() { topic_to_hash_info_temp.clear(); }

private:
  void GenerateHashesAndUrlsForNodes(const mts_type_node_t *node, std::string url, uint32_t cycle_id,
                                     const std::string &data_source_name, const uint16_t source_id,
                                     const uint32_t instance_number, const std::string &format_identifier,
                                     std::size_t &prev_hash, uint8_t file_type = 0) {
    if (!extractor_proxy_) {
      return;
    }

    url += std::string(".") + node->name.data;
    if (node->node_type == mts_type_node_view) {

      auto child_nodes = extractor_proxy_->get_node_children(node->node_id);
      auto *chid_node = child_nodes->data;
      if (chid_node == nullptr) {
        return;
      }

      std::size_t hash = 0;
      for (unsigned int node_index = 0; node_index < node->children_count; node_index++) {
        if (format_identifier == "mts.eth") {
          GenerateHashesAndUrlsForNodes(chid_node, url, node->view.service_id, data_source_name, source_id,
                                        instance_number, format_identifier, hash, file_type);
        } else {
          GenerateHashesAndUrlsForNodes(chid_node, url, node->view.cycle_id, data_source_name, source_id,
                                        instance_number, format_identifier, hash);
        }
        chid_node++;
      }
    } else if (node->node_type == mts_type_node_group) {
      uint64_t vaddr = 0;
      uint16_t service_id = 0;
      uint32_t method_id = 0;
      if (format_identifier == "mts.eth") {
        if (file_type == 1) {
          // FIBEX Data description file
          service_id = (uint16_t)cycle_id; // the cycle_id is actually the service_id of the view.
          method_id = node->group.message_id;
          cycle_id = 0; // Consider only service ID and Method ID for hash calculation.
        } else if (file_type == 0) {
          // ARXML Data description file
          // The message_id has both serice identifier and method identifier
          service_id = static_cast<uint16_t>((node->group.message_id >> 16U));
          method_id = static_cast<uint16_t>((node->group.message_id & 0x0000FFFFU));
          cycle_id = 0; // Consider only service ID and Method ID for hash calculation.
        } else {
          return;
        }
      } else {
        vaddr = node->group.address;
        if (cycle_id == 0 && vaddr == 0) {
          cycle_id = node->group.cycle_id;
        }
      }

      std::size_t hash = 0;

      std::string view_group_url(node->path.data);
      if (IsProcessor(view_group_url)) {
        hash = next::sdk::types::PackageHash::hashMetaInfoWithName(
            {source_id, instance_number, cycle_id, vaddr, service_id, method_id}, view_group_url);
      } else {
        hash = next::sdk::types::PackageHash::hashMetaInfo(
            {source_id, instance_number, cycle_id, vaddr, service_id, method_id});
      }

      url_info_t url_info;
      url_info.data_source_name = data_source_name;
      url_info.source_id = source_id;
      url_info.instance_number = instance_number;
      url_info.format_identifier = format_identifier;
      url_info.hash = hash;

      if (hash_to_url_map_.find(hash) == hash_to_url_map_.end()) {
        hash_to_url_map_.insert(std::make_pair(hash, url));
      } else {
        next::sdk::logger::warn(__FILE__,
                                "[next_udex::GenerateHashesAndUrlsForNodes] same hash for URLs: [{0}] and [{1}] ",
                                hash_to_url_map_[hash], url);
        auto url_info_temp = url_to_info_cache_[hash_to_url_map_[hash]];
        if (hash_to_url_map_[hash] != url) {
          next::sdk::logger::warn(
              __FILE__,
              "[next_udex::GenerateHashesAndUrlsForNodes] vaddr, cyc_id, inst, s_id [{0},{1},{2},{3}] "
              "[{4},{5},{6},{7}]. This is expected only for processor packages",
              vaddr, cycle_id, url_info_temp.instance_number, url_info_temp.source_id, vaddr, cycle_id, instance_number,
              source_id);
        }
      }
      vaddr_to_hash_cache_.insert(std::make_pair(vaddr, hash));
      url_to_info_cache_.insert(std::make_pair(url, url_info));
      url_to_hash_cache_.insert(std::make_pair(url, hash));
      HashTopicInformation info = {
          url_info.hash, url_info.data_source_name, url_info.format_identifier, cycle_id, source_id, instance_number,
          vaddr};
      topic_to_hash_info_temp.insert(std::make_pair(url, info));

      prev_hash = hash;
    }

    return;
  }

private:
  struct url_info_t {
    url_info_t() { Reset(); }

    void Reset() {
      data_source_name = "";
      source_id = 0;
      instance_number = 0;
      format_identifier = "";
      hash = 0;
    }

    std::string data_source_name;
    uint16_t source_id;
    uint32_t instance_number;
    std::string format_identifier;
    std::size_t hash;
  };

  std::vector<std::unique_ptr<mts::runtime::processor::data_processor_proxy>> processors_;
  std::unordered_map<uint64_t, size_t> vaddr_to_hash_cache_;
  std::unordered_map<std::string, url_info_t> url_to_info_cache_;
  std::unordered_map<std::string, size_t> url_to_hash_cache_;
  std::unordered_map<size_t, std::string> hash_to_url_map_;
  std::unordered_map<std::string, HashTopicInformation> topic_to_hash_info_temp;
  mts::runtime::udex::extractor_proxy::shared_pointer extractor_proxy_;
};

} // namespace udex
} // namespace next

#endif
