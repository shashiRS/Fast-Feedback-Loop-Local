#ifndef NEXT_CONFIG_KEY_GENERATOR_DATA_BRIDGE_H_
#define NEXT_CONFIG_KEY_GENERATOR_DATA_BRIDGE_H_

#include <string>

namespace next {
namespace appsupport {
namespace configkey {
namespace databridge {
/*
next_databridge
  plugins
      plugin_name
        regex_config
  web_socket_config
      foxglove_ip
      foxglove_port
      data_broadcast_ip
      data_request_ip
      data_broadcast_port
      data_requester_port
*/

/*!
 * @brief Generate key to access regex configuration in plugins
 * @param plugin_name: name of the plugin: e.g. view3D_StructTpObjects
 * @param entry_name: name of the regex field: e.g. num_objects_
 * @return key generated key.
 */
inline std::string getPluginRegexConfigEntryKey(const std::string &plugin_name, const std::string &entry_name) {
  return "next_databridge:plugins:" + plugin_name + ":regex_config:" + entry_name;
}

/*!
 * @brief Generate key to access regex root configuration in plugins
 * @param plugin_name: name of the plugin: e.g. view3D_StructTpObjects
 * @return key generated key.
 */
inline std::string getPluginRegexConfigRootKey(const std::string &plugin_name) {
  return "next_databridge:plugins:" + plugin_name + ":regex_config";
}

inline std::string getFoxgloveIpKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:foxglove_ip";
}
inline std::string getFoxglovePortKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:foxglove_port";
}
inline std::string getDataBroadcastIpKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:data_broadcast_ip";
}

inline std::string getDataRequestIpKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:data_request_ip";
}

inline std::string getDataBroadcastPortKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:data_broadcast_port";
}

inline std::string getDataRequestPortKey(const std::string &instance_name) {
  return instance_name + ":web_socket_config:data_requester_port";
}

} // namespace databridge
} // namespace configkey
} // namespace appsupport
} // namespace next
#endif // NEXT_CONFIG_KEY_GENERATOR_DATA_BRIDGE_H_
