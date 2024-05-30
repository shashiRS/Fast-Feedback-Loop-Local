#ifndef NEXT_CONFIG_KEY_GENERATOR_H_
#define NEXT_CONFIG_KEY_GENERATOR_H_

#include <string>

#include "config_key_generator_databridge.hpp"
#include "config_key_generator_logger.hpp"
#include "config_key_generator_orchestrator.hpp"
#include "config_key_generator_player.hpp"

namespace next {
namespace appsupport {
namespace configkey {
/*
profiling
  level
Inport
  Port
    uint32_t port_version_number;
    size_t port_size;
  Data
    std::string data_type_name;
  GIA
    ADATpter DLL
  Sync
    SyncMode
    SyncAlgo
    SyncRef URL
    SyncSignalPortMapping signal_url_timestamp;
      std::string syncref_signal_url;
      std::string synchronized_port_member_name;
    SyncSignalPortMapping signal_url_syncid;
      std::string syncref_signal_url;
      std::string synchronized_port_member_name;

  Outport
    Port
      uint32_t port_version_number;
      size_t port_size;
    Data
      SDL File
  system_services
      timestamp
              mode
                  signal_url
                  signal_unit
  Composition
    mode
    meas
      url
      alternatives
    sim
      url
*/
// Framework Nodes
// profiling
// level

inline std::string getConfigInputTimeoutKey(const std::string &component_name) {
  return component_name + ":lifecycle:config_input_timeout";
}

inline std::string getProfilingLevelKey(const std::string &component_name) {
  return component_name + ":profiling:level";
}

inline std::string getUdexSizeDiffThreshold(const std::string &component_name) {
  return component_name + ":udex:size_diff_threshold";
}

inline std::string getUdexUpdateRate(const std::string &component_name) { return component_name + ":udex:update_rate"; }

inline std::string getUdexUpdatePeriodMs(const std::string &component_name) {
  return component_name + ":udex:update_period_ms";
}

/*!
 * @brief Generate key to access the parameters for the specific instances
 * @return key generated key.
 */
inline std::string getProcessorInstances(const std::string &component_name) {
  auto key{component_name + ":udex:package_processor:instances"};
  return key;
}

/*!
 * @brief Generate key for the cycle Id information
 * @return key generated key.
 */
inline std::string getProcessorInstanceCycleId(const std::string &component_name, const std::string &processor_name) {
  auto key{component_name + ":udex:package_processor:instances:" + processor_name + ":cycle_id"};
  return key;
}

/*!
 * @brief Generate key for the cycle Id information
 * @return key generated key.
 */
inline std::string getProcessorInstanceInstanceNumber(const std::string &component_name,
                                                      const std::string &processor_name) {
  auto key{component_name + ":udex:package_processor:instances:" + processor_name + ":instance_number"};
  return key;
}

/*!
 * @brief Generate key for the cycle Id information
 * @return key generated key.
 */
inline std::string getProcessorInstanceSourceId(const std::string &component_name, const std::string &processor_name) {
  auto key{component_name + ":udex:package_processor:instances:" + processor_name + ":source_id"};
  return key;
}

/*!
 * @brief Generate key for the cycle Id information
 * @return key generated key.
 */
inline std::string getProcessorInstanceVirtualAddress(const std::string &component_name,
                                                      const std::string &processor_name) {
  auto key{component_name + ":udex:package_processor:instances:" + processor_name + ":v_address"};
  return key;
}

inline std::string getDataAvilabilityMode(const std::string &component_name) {
  return component_name + ":dataAvailabilityMode";
}

inline std::string getSyncRefUrlComponent(const std::string &component_name) {
  return component_name + ":sync:syncref_url";
}
// Inport Port
// Version
inline std::string getInportVersionKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":port:version";
  return key;
}

// Inport Port
// Size
inline std::string getInportSizeKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":port:size";
  return key;
}

// Inport Data
// DataTypeName
inline std::string getInportDataTypeKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":data:type";
  return key;
}

inline std::string getUdexMaximumSize(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":port:size_buffer";
  return key;
}

inline std::string getUdexMaximumSizeMb(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":port:size_buffer_mb";
  return key;
}

// Inport GIA
// Adapter_DLL
inline std::string getAdapterDllKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":gia:adapter_dll";
  return key;
}

// Inport Sync
// Sync Mode
inline std::string getSyncModeKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:mode";
  return key;
}
// Inport Sync
// Sync Algo
inline std::string getSyncAlgoKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:algo";
  return key;
}

// Inport Sync
// SyncRef URL
inline std::string getSyncRefUrlKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:syncref_url";
  return key;
}

// Inport Sync
// Timestamp Syncref Signal URL
inline std::string getSyncMemberUrlTimestampKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:timestamp:url";
  return key;
}

// Inport Sync
// Timestamp Port Member Name
inline std::string getSyncMemberNameTimestampKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:timestamp:name";
  return key;
}

// Inport Sync
// Cycle ID Syncref Signal URL
inline std::string getSyncMemberUrlSyncIdKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:sync_id:url"; // <-- TODO Sync ID or Cycle ID?
  return key;
}

// Inport Sync
// Cycle ID Port Member Name
inline std::string getSyncMemberNameSyncIdKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:sync_id:name";
  return key;
}

// SyncRef selection selection
inline std::string getSyncRefSelectionMode(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:syncref:mode";
  return key;
}

// SyncRef url
inline std::string getSyncRefUrl(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":in_port:" + port_name + ":sync:syncref:url";
  return key;
}

// Outport Port
// Version
inline std::string getOutportVersionKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":out_port:" + port_name + ":port:version";
  return key;
}

// Outport Port
// Size
inline std::string getOutportSizeKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":out_port:" + port_name + ":port:size";
  return key;
}

// Outport Data
// SDL File <--TODO SDL File Needed?
inline std::string getOutportSdlKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":out_port:" + port_name + ":sdl_file";
  return key;
}

// System Services
// timestamp mode
inline std::string getTimestampModeKey(const std::string &component_name) {
  std::string key = component_name + ":system_services:timestamp:mode";
  return key;
}

// timestamp url
inline std::string getTimestampUrlKey(const std::string &component_name) {
  std::string key = component_name + ":system_services:timestamp:signal_url";
  return key;
}

// timestamp unit
inline std::string getTimestampUnitKey(const std::string &component_name) {
  std::string key = component_name + ":system_services:timestamp:signal_unit";
  return key;
}

// Composition
// Meas Url
inline std::string getCompositionPortUrlKey(const std::string &component_name, const std::string &port_name) {
  std::string key = component_name + ":composition:" + port_name + ":url";
  return key;
}

// ControlBridge
inline std::string getGlobalConfigurationKey(const std::string &instance_name) {
  auto key{instance_name + ":global_config"};
  return key;
}
inline std::string getControlIpKey(const std::string &instance_name) {
  auto key{instance_name + ":web_socket_config:control_ip"};
  return key;
}

inline std::string getControlPortKey(const std::string &instance_name) {
  auto key{instance_name + ":web_socket_config:control_port"};
  return key;
}

/*!
 * @brief  Major Version key for configurations
 * @param name  Component name e.g control_bridge, data_bridge, player etc
 * @return string combination of component_name plus key token
 */
inline std::string getMajorVersionKey(const std::string &component_name) { return component_name + ":version:major"; }

/*!
 * @brief  Minor Version key for configurations
 * @param name  Component name e.g control_bridge, data_bridge, player etc
 * @return string combination of component_name plus key token
 */
inline std::string getMinorVersionKey(const std::string &component_name) { return component_name + ":version:minor"; }

/*!
 * @brief  Patch Version key for configurations
 * @param name  Component name e.g control_bridge, data_bridge, player etc
 * @return string combination of component_name plus key token
 */
inline std::string getPatchVersionKey(const std::string &component_name) { return component_name + ":version:patch"; }

inline std::string getPluginManagerResetPeriodWait(const std::string &instance_name) {
  return instance_name + ":plugin_manager:reset_period_wait";
}

inline std::string getRefValueMode(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:ref_value_mode";
}

inline std::string getSyncIdSyncrefValueMemberName(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_id:sync_ref_sync_value_member_name";
}

inline std::string getSyncIdSyncrefPortMemberUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_id:port_member_data_url";
}

inline std::string getTimestampSyncrefValueMemberName(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:timestamp:sync_ref_sync_value_member_name";
}

inline std::string getTimestampSyncrefPortMemberUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:timestamp:port_member_data_url";
}

inline std::string getSyncValueTimestampUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_value:timestamp_url";
}

inline std::string getSyncValueSyncIdUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_value:sync_id_url";
}

inline std::string getSyncValueSyncrefBaseControlUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_value:syncref_base_control_url";
}

inline std::string getSyncValueSyncrefSyncHeaderUrl(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:sync_value:syncref_sync_header_url";
}

inline std::string getSelectionAlgo(const std::string &component_name, const std::string &port_name) {
  return component_name + ":in_port:" + port_name + ":sync:selection_algo";
}

} // namespace configkey
} // namespace appsupport
} // namespace next

#endif // NEXT_CONFIG_KEY_GENERATOR_H_
