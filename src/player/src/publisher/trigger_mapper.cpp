
#include "trigger_mapper.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>

bool trigger_mapper::Reset() {
  trigger_map_.clear();
  trigger_urls_.clear();
  return true;
}

bool trigger_mapper::initialize() {
  trigger_urls_.clear();

  debug(__FILE__, "No orchestrator config file and no flows defined!");
  bool trigger_found = false;

  try {
    auto children =
        next::appsupport::ConfigClient::getConfig()->getChildren(next::appsupport::configkey::player::getTriggerKey());

    for (unsigned int i = 0; i < children.size(); ++i) {
      const std::string key = children[i];
      const std::string response = next::appsupport::ConfigClient::getConfig()->get_string(
          next::appsupport::configkey::player::getTriggerFunctionKey(key), "");
      const std::string trigger = next::appsupport::ConfigClient::getConfig()->get_string(
          next::appsupport::configkey::player::getTriggerValueKey(key), "");
      trigger_urls_.push_back(std::make_tuple(trigger, response, key));
      trigger_found = true;
    }
  } catch (const std::exception &) {
    warn(__FILE__, "trigger not defined");
    return false;
  }

  if (!trigger_found) {
    warn(__FILE__, "trigger not defined");
    return false;
  }
  return true;
}

bool trigger_mapper::create_trigger_connections() { return false; }

std::vector<std::tuple<std::string, std::string, std::string>> *trigger_mapper::get_trigger_vector() {
  return &trigger_urls_;
}

std::unordered_map<size_t, std::pair<std::string, std::unique_ptr<eCAL::CServiceClient>>> *
trigger_mapper::get_trigger_map() {
  return &trigger_map_;
}
