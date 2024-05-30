#include <map>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include "next/sdk/logger/logger.hpp"
#include "status_code_loglevel_parser.hpp"

namespace next {
namespace appsupport {

const std::map<std::string, next::sdk::events::StatusOrigin> kNodeNameToEventStatusOrigin = {
    {"next_controlbridge", next::sdk::events::StatusOrigin::NEXT_CONTROLBRIDGE},
    {"next_databridge", next::sdk::events::StatusOrigin::NEXT_DATABRIDGE},
    {"next_player", next::sdk::events::StatusOrigin::NEXT_PLAYER},
    {"sim_node", next::sdk::events::StatusOrigin::SIM_NODE},
    {"default", next::sdk::events::StatusOrigin::DEFAULT},
};

std::optional<next::sdk::events::StatusCodeMessage>
NextStatusCode::PublishStatusCode(next::appsupport::StatusCode next_status_code, const std::string &status_code_msg) {

  std::string component_name = GetComponentNameFromConfig("default");
  // use StatusOrigin::DEFAULT as default origin
  next::sdk::events::StatusOrigin origin = GetOrigin(component_name, sdk::events::StatusOrigin::DEFAULT);

  return SendStatusCode(origin, component_name, next_status_code, status_code_msg);
}

std::optional<next::sdk::events::StatusCodeMessage>
NextStatusCode::PublishStatusCode(std::string component_name, next::appsupport::StatusCode next_status_code,
                                  const std::string &status_code_msg) {
  // Hint: This call is expected to be used to publish status codes for sim nodes
  // All sim nodes have the general component name "sim_node". The general component name is needed to get the origin
  // type afterward.
  std::string general_component_name = GetComponentNameFromConfig(component_name);
  // use SIM_NODE as default origin
  next::sdk::events::StatusOrigin origin = GetOrigin(general_component_name, sdk::events::StatusOrigin::SIM_NODE);
  return SendStatusCode(origin, component_name, next_status_code, status_code_msg);
}

std::string NextStatusCode::GetComponentNameFromConfig(const std::string &default_component_name) {
  if (next::appsupport::ConfigClient::getConfig()) {
    return next::appsupport::ConfigClient::getConfig()->getRootName();
  }
  return default_component_name;
}

sdk::events::StatusOrigin NextStatusCode::GetOrigin(const std::string &component_name,
                                                    const sdk::events::StatusOrigin &default_origin) {
  sdk::events::StatusOrigin origin = default_origin;
  if (kNodeNameToEventStatusOrigin.find(component_name) != kNodeNameToEventStatusOrigin.end()) {
    origin = kNodeNameToEventStatusOrigin.at(component_name);
  }
  return origin;
}

const std::optional<next::sdk::events::StatusCodeMessage>
NextStatusCode::SendStatusCode(const next::sdk::events::StatusOrigin &origin, const std::string &component_name,
                               const next::appsupport::StatusCode &next_status_code,
                               const std::string &status_code_msg) {

  next::sdk::events::StatusCodeMessage status_code((int)next_status_code);

  status_code.origin = origin;
  status_code.component_name = component_name;
  status_code.message = status_code_msg;

  std::string log_message = "NEXT_STATUS_CODE: [Component: " + status_code.component_name +
                            "][StatusCode: " + std::to_string(status_code.toInt()) + "][Text: " + status_code.message +
                            "]";

  switch (next::appsupport::loglevel_parser::LogLevelFromStatusCode(next_status_code)) {
  case sdk::logger::LOGLEVEL::INFO:
    info(component_name, log_message);
    break;
  case sdk::logger::LOGLEVEL::DEBUG:
    info(component_name, log_message);
    break;
  case sdk::logger::LOGLEVEL::WARN:
    warn(component_name, log_message);
    break;
  case sdk::logger::LOGLEVEL::ERR:
    error(component_name, log_message);
    break;
  default:
    debug(component_name, log_message);
    break;
  }

  static next::sdk::events::StatusCodeEvent status_code_event;
  status_code_event.publish(status_code);

  return status_code;
}

} // namespace appsupport
} // namespace next