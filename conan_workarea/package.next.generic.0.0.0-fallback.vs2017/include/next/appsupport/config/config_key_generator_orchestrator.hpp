#ifndef NEXT_CONFIG_KEY_GENERATOR_ORCHESTRATOR_H_
#define NEXT_CONFIG_KEY_GENERATOR_ORCHESTRATOR_H_

#include <string>

namespace next {
namespace appsupport {
namespace configkey {
namespace orchestrator {
/*
FLows
  type
  value
  synch_timestamp_url
  synch_id_url
  meta
    delay
    sync
  client
    name
    port
*/

const std::string k_config_file_flow_exclude = "config_file";

/*!
 * @brief Generate key to access the orchestrator configuration file value from the config file.
 * @return key generated key.
 */
inline std::string getOrchestratorConfigFileKey() {
  auto key{"next_player:orchestrator:" + k_config_file_flow_exclude};
  return key;
}

/*!
 * @brief Generate key to access the list of orchestrator flows.
 * @return key generated key.
 */
inline std::string getFlows() {
  auto key{"next_player:orchestrator"};
  return key;
}

/*!
 * @brief Generate key to access the type value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getType(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":type"};
  return key;
}

/*!
 * @brief Generate key to access the value value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getValue(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":value"};
  return key;
}

/*!
 * @brief Generate key to access the timestamp sync url value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getSyncTimestamp(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":synch_timestamp_url"};
  return key;
}

/*!
 * @brief Generate key to access the sync ID value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getSyncId(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":synch_id_url"};
  return key;
}

/*!
 * @brief Generate key to access the delay value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getDelay(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:delay"};
  return key;
}

/*!
 * @brief Generate key to access the sync type value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getSync(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:sync"};
  return key;
}

/*!
 * @brief Generate key to access the clients value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getClients(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":clients"};
  return key;
}

inline std::string getClientMethods(const std::string &flow, const std::string &client) {
  auto key{"next_player:orchestrator:" + flow + ":clients:" + client};
  return key;
}

/*!
 * @brief Generate key to access the name of a client in the flow from the config file.
 * @param index client index.
 * @return key generated key.
 */
inline std::string getClientName(const std::string &flow, int index) {
  auto key{"next_player:orchestrator:" + flow + ":clients" + std::to_string(index) + ":name"};
  return key;
}

/*!
 * @brief Generate key to access the generate control type value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getGenerateControlData(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:control_data_generation"};
  return key;
}

/*!
 * @brief Generate key to access the generate control timestamp value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getGenerateControlDataTimestampUrl(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:generate_timestamp_url"};
  return key;
}

/*!
 * @brief Generate key to access the generate control measurement counter value of a flow from the config file.
 * @return key generated key.
 */
inline std::string getGenerateControlDataMeasUrl(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:generate_meas_url"};
  return key;
}

/*!
 * @brief Generate key to access the port of a client in the flow from the config file.
 * @param index client index.
 * @return key generated key.
 */
inline std::string getClientPort(const std::string &flow, int index) {
  auto key{"next_player:orchestrator:" + flow + ":clients" + std::to_string(index) + ":port"};
  return key;
}

inline std::string getRefValue(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta_info:ref_value"};
  return key;
}

inline std::string getCompareValue(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta_info:compare_value"};
  return key;
}

inline std::string getControlDataMembers(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:control_data_generation:member_data"};
  return key;
}

inline std::string getControlDataMember(const std::string &flow, const std::string &member) {
  auto key{"next_player:orchestrator:" + flow + ":meta:control_data_generation:member_data:" + member};
  return key;
}

inline std::string getControlDataSdl(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":meta:control_data_generation:sdl_file"};
  return key;
}

inline std::string getControlDataComponents(const std::string &flow) {
  auto key{"next_player:orchestrator:" + flow + ":clients"};
  return key;
}

inline std::string getControlDataFallbackMemberData(const std::string &flow, const std::string &member) {
  auto key{"next_player:orchestrator:" + flow + ":meta:control_data_generation:member_data:" + member};
  return key;
}

inline std::string getControlDataMembersForComponent(const std::string &flow, const std::string &component,
                                                     const std::string &method) {
  auto key{"next_player:orchestrator:" + flow + ":clients:" + component + ":" + method + ":" + "member_data"};
  return key;
}

inline std::string getSpecificControlDataMembersForComponent(const std::string &flow, const std::string &component,
                                                             const std::string &method, const std::string &member) {
  auto key{"next_player:orchestrator:" + flow + ":clients:" + component + ":" + method + ":" + "member_data:" + member};
  return key;
}

} // namespace orchestrator
} // namespace configkey
} // namespace appsupport
} // namespace next

#endif // NEXT_CONFIG_KEY_GENERATOR_ORCHESTRATOR_H_
