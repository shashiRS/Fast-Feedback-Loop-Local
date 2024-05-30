#ifndef NEXT_CONFIG_KEY_GENERATOR_PLAYER_H_
#define NEXT_CONFIG_KEY_GENERATOR_PLAYER_H_

#include <string>

namespace next {
namespace appsupport {
namespace configkey {
namespace player {
/*
next_player
    orchestrator
        config_file trigger_config.json
trigger
    EmfNodeTriggering
        function runSequencer_response
        triggervalue ADC4xx.FDP_21P.FDP_ObjectData
    GlobalOrchestratorTriggering
        function runSequencer_response
        triggervalue ADC4xx.FDP_21P.FDP_ObjectData
stepping
    time_step
realtime_factor
*/

/*!
 * @brief Generate key to access the player trigger value from the config file.
 * @return key generated key.
 */
inline std::string getTriggerKey() {
  auto key{"next_player:trigger"};
  return key;
}

/*!
 * @brief Generate key to access the trigger function value based on the trigger name from the config file.
 * @param triggername  name of the trigger
 * @return key generated key.
 */
inline std::string getTriggerFunctionKey(const std::string &triggername) {
  auto key{"next_player:trigger:" + triggername + ":function"};
  return key;
}

/*!
 * @brief Generate key to access the trigger value based on the trigger name from the config file.
 * @param triggername  name of the trigger
 * @return key generated key.
 */
inline std::string getTriggerValueKey(const std::string &triggername) {
  auto key{"next_player:trigger:" + triggername + ":triggervalue"};
  return key;
}

/*!
 * @brief Generate key to access the stepping value from the config file.
 * @return key generated key.
 */
inline std::string getSteppingKey() {
  auto key{"next_player:stepping:time_step"};
  return key;
}

/*!
 * @brief Generate key to access the cycle id value from the config file.
 * @return key generated key.
 */
inline std::string getCycleIdKey() {
  auto key{"next_player:stepping:cycle_id"};
  return key;
}

/*!
 * @brief Generate key to access the real time factor value from the config file.
 * @return key generated key.
 */
inline std::string getRealTimeFactorKey() {
  auto key{"next_player:realtime_factor"};
  return key;
}

/*!
 * @brief Generate key to access the recording path value from the config file.
 * @return key generated key.
 */
inline std::string getRecordingPathKey() {
  auto key{"next_player:recording_path"};
  return key;
}

/*!
 * @brief Generate key to access the buffer max read ahead size in MB from the config file.
 * @return key generated key.
 */
inline std::string getBufferMaxReadAheadKey() {
  auto key{"next_player:buffer_size:max_read_ahead_MB"};
  return key;
}

//! ################################### Package Processors #############################################################
/*!
 * @brief Generate key to read out the search path for processors
 * @return key generated key.
 */
inline std::string getProcessorPathListKey() {
  auto key{"next_player:udex:package_processor:search_paths"};
  return key;
}

/*!
 * @brief Generate key to access the filters url from the config file.
 * @return key generated key.
 */
inline std::string getFiltersUrlKey() {
  auto key{"next_player:filters:url"};
  return key;
}

/*!
 * @brief Generate key to access the filters expression from the config file.
 * @return key generated key.
 */
inline std::string getFiltersExpresionKey() {
  auto key{"next_player:filters:expression"};
  return key;
}

} // namespace player
} // namespace configkey
} // namespace appsupport
} // namespace next
#endif // NEXT_CONFIG_KEY_GENERATOR_H_
