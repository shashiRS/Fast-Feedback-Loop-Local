#ifndef SYNC_CONFIG_READER_HPP
#define SYNC_CONFIG_READER_HPP

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/next.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/udex/data_types.hpp>

using config = next::appsupport::ConfigClient;

namespace next {

enum SyncMode { SYNC_UNSUPPORTED, SYNC_SYNC_REF_TIMESTAMP, SYNC_SYNC_REF_SYNCID, SYNC_TIMESTAMP, SYNC_SYNCID };

struct SyncPortMemberDetails {
  std::string sync_ref_sync_value_member_name;
  std::string port_member_data_url;
};

struct SyncValueUrls {
  std::string timestamp_url;
  std::string sync_id_url;
  std::string syncref_base_control_url;
  std::string syncref_sync_header_url;
};

struct SyncConfig {
  SyncPortMemberDetails sync_id;
  SyncPortMemberDetails timestamp;
  SyncMode mode;
  SyncValueUrls sync_value;
  next::udex::SyncAlgoCfg algo_port_level{};
};

class SyncConfigReader {
public:
  void ReadSyncConfig(const std::string &component_name, const ComponentInputPortConfig &it_input_port,
                      SyncConfig &sync_config) {
    SetV0ValuesToConfig(component_name, it_input_port);
    ConvertConfigV0ToV1AndUpdateDatabase(component_name, it_input_port.port_name);
    ReadV1Config(component_name, it_input_port.port_name, sync_config);
  }

private:
  void SetV0ValuesToConfig(const std::string &component_name, const ComponentInputPortConfig &it_input_port) {
    std::string syncMemberNameKey = config::getConfig()->get_string(
        appsupport::configkey::getSyncMemberNameSyncIdKey(component_name, it_input_port.port_name), "", true);
    if (syncMemberNameKey == "") {
      config::getConfig()->put(
          appsupport::configkey::getSyncMemberNameSyncIdKey(component_name, it_input_port.port_name),
          it_input_port.sync_mapping.signal_url_syncid.synchronized_port_member_name);
    }
    std::string syncMemberKey = config::getConfig()->get_string(
        appsupport::configkey::getSyncMemberUrlSyncIdKey(component_name, it_input_port.port_name), "", true);
    if (syncMemberKey == "") {
      config::getConfig()->put(
          appsupport::configkey::getSyncMemberUrlSyncIdKey(component_name, it_input_port.port_name),
          it_input_port.sync_mapping.signal_url_syncid.syncref_signal_url);
    }
    std::string syncMemberTimestampKey = config::getConfig()->get_string(
        appsupport::configkey::getSyncMemberNameTimestampKey(component_name, it_input_port.port_name), "", true);
    if (syncMemberTimestampKey == "") {
      config::getConfig()->put(
          appsupport::configkey::getSyncMemberNameTimestampKey(component_name, it_input_port.port_name),
          it_input_port.sync_mapping.signal_url_timestamp.synchronized_port_member_name);
    }
    std::string syncMemberTimestampName = config::getConfig()->get_string(
        appsupport::configkey::getSyncMemberUrlTimestampKey(component_name, it_input_port.port_name), "", true);
    if (syncMemberTimestampName == "") {
      config::getConfig()->put(
          appsupport::configkey::getSyncMemberUrlTimestampKey(component_name, it_input_port.port_name),
          it_input_port.sync_mapping.signal_url_timestamp.syncref_signal_url);
    }

    // Default Sync Mode and Algo if not set
    std::string sync_mode = config::getConfig()->get_string(
        appsupport::configkey::getSyncModeKey(component_name, it_input_port.port_name), "", true);
    if (sync_mode == "") {
      config::getConfig()->put(appsupport::configkey::getSyncModeKey(component_name, it_input_port.port_name),
                               "Timestamp");
    }
    std::string sync_algo = config::getConfig()->get_string(
        appsupport::configkey::getSyncAlgoKey(component_name, it_input_port.port_name), "", true);
    if (sync_algo == "") {
      config::getConfig()->put(appsupport::configkey::getSyncAlgoKey(component_name, it_input_port.port_name),
                               "GET_LAST_ELEMENT");
    }
    // Default SyncRef mode and url if not set
    std::string syncRef_mode = config::getConfig()->get_string(
        appsupport::configkey::getSyncRefSelectionMode(component_name, it_input_port.port_name), "", true);
    if (syncRef_mode == "") {
      config::getConfig()->put(appsupport::configkey::getSyncRefSelectionMode(component_name, it_input_port.port_name),
                               "undefined");
    }
    std::string syncRef_url = config::getConfig()->get_string(
        appsupport::configkey::getSyncRefUrl(component_name, it_input_port.port_name), "", true);
    if (syncRef_url == "") {
      config::getConfig()->put(appsupport::configkey::getSyncRefUrl(component_name, it_input_port.port_name),
                               "undefined");
    }
  }

  void ConvertConfigV0ToV1AndUpdateDatabase(const std::string &component_name, const std::string &port_name) {
    // sync_id:sync_ref_sync_value_member_name
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncMemberNameSyncIdKey(component_name, port_name),
                            appsupport::configkey::getSyncIdSyncrefValueMemberName(component_name, port_name), "");
    // sync_id:port_member_data_url
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncMemberUrlSyncIdKey(component_name, port_name),
                            appsupport::configkey::getSyncIdSyncrefPortMemberUrl(component_name, port_name), "");

    // timestamp:sync_ref_sync_value_member_name
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncMemberNameTimestampKey(component_name, port_name),
                            appsupport::configkey::getTimestampSyncrefValueMemberName(component_name, port_name), "");
    // timestamp:port_member_data_url
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncMemberUrlTimestampKey(component_name, port_name),
                            appsupport::configkey::getTimestampSyncrefPortMemberUrl(component_name, port_name), "");

    // sync_value:timestamp_url
    CheckKeysAndUpdateValue("", appsupport::configkey::getSyncValueTimestampUrl(component_name, port_name));
    // sync_value:sync_id_url
    CheckKeysAndUpdateValue("", appsupport::configkey::getSyncValueSyncIdUrl(component_name, port_name));
    // sync_value:syncref_base_control_url
    // when converting V0 keys into V1 keys, this field will always be "undefined"
    CheckKeysAndUpdateValue("", appsupport::configkey::getSyncValueSyncrefBaseControlUrl(component_name, port_name));
    // sync_value:syncref_sync_header_url
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncRefUrl(component_name, port_name),
                            appsupport::configkey::getSyncValueSyncrefSyncHeaderUrl(component_name, port_name));

    // selection_algo
    CheckKeysAndUpdateValue(appsupport::configkey::getSyncAlgoKey(component_name, port_name),
                            appsupport::configkey::getSelectionAlgo(component_name, port_name));

    SetRefValueModeToConfig(component_name, port_name);
  }

  void CheckKeysAndUpdateValue(const std::string &left_key, const std::string &right_key,
                               const std::string &default_option = "undefined") {
    const std::string &right_value = config::getConfig()->get_string(right_key, "");
    if (right_value == "") {
      const std::string &left_value = config::getConfig()->get_string(left_key, "");
      if (left_value == "") {
        config::getConfig()->put(right_key, default_option);
      } else {
        config::getConfig()->put(right_key, left_value);
      }
    }
  }

  void SetRefValueModeToConfig(const std::string &component_name, const std::string &port_name) {
    const std::string &ref_value_mode_key = appsupport::configkey::getRefValueMode(component_name, port_name);
    std::string ref_value_mode = config::getConfig()->get_string(ref_value_mode_key, "");

    if (ref_value_mode == "") {
      ref_value_mode = "unsupported";

      const std::string &mode =
          config::getConfig()->get_string(appsupport::configkey::getSyncModeKey(component_name, port_name), "");

      if (mode == "Timestamp") {
        ref_value_mode = "timestamp";
      } else if (mode == "CycleId") {
        ref_value_mode = "sync_id";
      } else if (mode == "SyncRef") {
        const auto &sub_mode = next::appsupport::ConfigClient::getConfig()->get_string(
            next::appsupport::configkey::getSyncRefSelectionMode(component_name, port_name), "");

        if (sub_mode == "Timestamp") {
          ref_value_mode = "sync_ref_timestamp";
        } else if (sub_mode == "SyncId") {
          ref_value_mode = "sync_ref_sync_id";
        }
      }

      config::getConfig()->put(ref_value_mode_key, ref_value_mode);
    }
  }

  void ReadV1Config(const std::string &component_name, const std::string &port_name, SyncConfig &sync_config) {
    SetSyncMode(component_name, port_name, sync_config);

    if (sync_config.mode != SYNC_UNSUPPORTED) {
      SetSyncIdPortDetails(component_name, port_name, sync_config);
      SetTimestampPortDetails(component_name, port_name, sync_config);
      SetUrls(component_name, port_name, sync_config);
      SetAlgoPortLevel(component_name, port_name, sync_config);
    } else {
      next::sdk::logger::warn(__FILE__, "Unkown sync mode.");
    }
  }

  void SetSyncMode(const std::string &component_name, const std::string &port_name, SyncConfig &sync_config) {
    const auto &mode = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getRefValueMode(component_name, port_name), "");

    if (mode == "timestamp") {
      sync_config.mode = SYNC_TIMESTAMP;
    } else if (mode == "sync_id") {
      sync_config.mode = SYNC_SYNCID;
    } else if (mode == "sync_ref_timestamp") {
      sync_config.mode = SYNC_SYNC_REF_TIMESTAMP;
    } else if (mode == "sync_ref_sync_id") {
      sync_config.mode = SYNC_SYNC_REF_SYNCID;
    } else {
      sync_config.mode = SYNC_UNSUPPORTED;
    }
  }

  void SetSyncIdPortDetails(const std::string &component_name, const std::string &port_name, SyncConfig &sync_config) {
    sync_config.sync_id.sync_ref_sync_value_member_name = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncIdSyncrefValueMemberName(component_name, port_name), "");

    sync_config.sync_id.port_member_data_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncIdSyncrefPortMemberUrl(component_name, port_name), "");
  }

  void SetTimestampPortDetails(const std::string &component_name, const std::string &port_name,
                               SyncConfig &sync_config) {
    sync_config.timestamp.sync_ref_sync_value_member_name = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getTimestampSyncrefValueMemberName(component_name, port_name), "");

    sync_config.timestamp.port_member_data_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getTimestampSyncrefPortMemberUrl(component_name, port_name), "");
  }

  void SetUrls(const std::string &component_name, const std::string &port_name, SyncConfig &sync_config) {
    sync_config.sync_value.timestamp_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncValueTimestampUrl(component_name, port_name), "");

    sync_config.sync_value.sync_id_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncValueSyncIdUrl(component_name, port_name), "");

    sync_config.sync_value.syncref_base_control_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncValueSyncrefBaseControlUrl(component_name, port_name), "");

    sync_config.sync_value.syncref_sync_header_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSyncValueSyncrefSyncHeaderUrl(component_name, port_name), "");
  }

  void SetAlgoPortLevel(const std::string &component_name, const std::string &port_name, SyncConfig &sync_config) {
    const auto &algo = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getSelectionAlgo(component_name, port_name), "");
    const std::string &composition_url = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::getCompositionPortUrlKey(component_name, port_name), "");

    sync_config.algo_port_level.algorythm_type = next::udex::getSyncAlgoTypeEnum(algo);

    if (sync_config.mode == SYNC_TIMESTAMP) {
      sync_config.algo_port_level.sync_signal_url =
          GetPortLevelUrl(sync_config.sync_value.syncref_sync_header_url) + sync_config.timestamp.port_member_data_url;
      sync_config.algo_port_level.composition_sync_signal_url =
          composition_url + sync_config.timestamp.sync_ref_sync_value_member_name;
    } else if (sync_config.mode == SYNC_SYNCID) {
      sync_config.algo_port_level.sync_signal_url =
          GetPortLevelUrl(sync_config.sync_value.syncref_sync_header_url) + sync_config.sync_id.port_member_data_url;
      sync_config.algo_port_level.composition_sync_signal_url =
          composition_url + sync_config.sync_id.sync_ref_sync_value_member_name;
    } else if (sync_config.mode == SYNC_SYNC_REF_TIMESTAMP) {
      sync_config.algo_port_level.sync_signal_url = GetPortLevelUrl(sync_config.sync_value.syncref_sync_header_url) +
                                                    "." + sync_config.timestamp.port_member_data_url;
      sync_config.algo_port_level.composition_sync_signal_url =
          composition_url + "." + sync_config.timestamp.sync_ref_sync_value_member_name;
    } else if (sync_config.mode == SYNC_SYNC_REF_SYNCID) {
      sync_config.algo_port_level.sync_signal_url = GetPortLevelUrl(sync_config.sync_value.syncref_sync_header_url) +
                                                    "." + sync_config.sync_id.port_member_data_url;
      sync_config.algo_port_level.composition_sync_signal_url =
          composition_url + "." + sync_config.sync_id.sync_ref_sync_value_member_name;
    }
  }

  std::string GetPortLevelUrl(const std::string &url) {
    size_t pos = 0;
    unsigned int dot_counter = 0;
    const unsigned int url_level = 2;

    while (dot_counter < url_level + 1 && pos != std::string::npos) {
      pos = url.find('.', pos + 1);
      if (pos != std::string::npos) {
        dot_counter++;
      } else if (pos == std::string::npos && dot_counter == url_level) {
        // If we don't find enough dots, url is already group level, return the entire string
        return url;
      }
    }

    return pos != std::string::npos ? url.substr(0, pos) : "";
  }
}; // SyncConfigReader

} // namespace next

#endif // SYNC_CONFIG_READER_HPP