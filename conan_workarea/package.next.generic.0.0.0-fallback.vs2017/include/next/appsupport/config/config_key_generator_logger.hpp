#ifndef NEXT_CONFIG_KEY_GENERATOR_LOGGER_H_
#define NEXT_CONFIG_KEY_GENERATOR_LOGGER_H_

#include <string>

namespace next {
namespace appsupport {
namespace configkey {
namespace logger {
/*
logging
  <log_sink>
    active
        loglevel[id]
  file
    filename
  global
    filename
    sendfrequency
*/

/*!
 * @brief Generate key to access the enabled / disabled state of a sink.
 * @param component_name  name of the component
 * @param sink_name       name of the sink - valid values: console, file, global
 * @return key generated key.
 */
inline std::string getSinkActiveKey(const std::string &component_name, const std::string &sink_name) {
  return component_name + ":logging:" + sink_name + ":active";
}

/*!
 * @brief Generate key to get the loglevels of a sink.
 * @param component_name  name of the component
 * @param sink_name       name of the sink - valid values: console, file, global
 * @return key generated key.
 */
inline std::string getSinkLoglevelKey(const std::string &component_name, const std::string &sink_name) {
  return component_name + ":logging:" + sink_name + ":loglevel";
}

inline std::string getSystemSinkLoglevelKey(const std::string &sink_name) {
  return "next_system:logging:" + sink_name + ":loglevel";
}

/*!
 * @brief Generate key to access the filename of the local logfile.
 * @param component_name  name of the component
 * @return key generated key.
 */
inline std::string getLocalLogfileKey(const std::string &component_name) {
  return component_name + ":logging:file:filename";
}

/*!
 * @brief Generate key to access the frequency of sending data to the log collector.
 * @param component_name  name of the component
 * @return key generated key.
 */
inline std::string getSendFrequencyKey(const std::string &component_name) {
  return component_name + ":logging:global:sendfrequency";
}

/*!
 * @brief Generate key to access the log file of the log collector (for next_controlbridge only).
 * @return key generated key.
 */
inline std::string getGlobalLogfileKey() { return "next_controlbridge:logging:global:filename"; }

} // namespace logger
} // namespace configkey
} // namespace appsupport
} // namespace next
#endif // NEXT_CONFIG_KEY_GENERATOR_LOGGER_H_
