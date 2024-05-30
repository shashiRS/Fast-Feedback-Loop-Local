#include <exception>

#include <json/json.h>

#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

const std::map<next::sdk::events::StatusOrigin, std::string> kEventStatusOriginToNodeName = {
    {next::sdk::events::StatusOrigin::NEXT_CONTROLBRIDGE, "next_controlbridge"},
    {next::sdk::events::StatusOrigin::NEXT_DATABRIDGE, "next_databridge"},
    {next::sdk::events::StatusOrigin::NEXT_PLAYER, "next_player"},
    {next::sdk::events::StatusOrigin::SIM_NODE, "sim_node"},
    {next::sdk::events::StatusOrigin::DEFAULT, "default"},
};

payload_type StatusCodeEvent::serializeEventPayload(const StatusCodeMessage &status_code_message) const {
  std::string jsonised_string;
  try {
    Json::Value json_val;
    json_val["status_code"] = std::to_string(status_code_message.toInt());
    json_val["origin"] = std::to_string((int)status_code_message.origin);
    json_val["component_name"] = status_code_message.component_name;
    json_val["status_msg"] = status_code_message.message;

    Json::FastWriter fastWriter;
    jsonised_string = fastWriter.write(json_val);
  } catch (std::exception &e) {
    logger::warn(__FILE__, "StatusCode::deserializeEventPayload :: failed to serializ: {}", e.what());
  }
  return StringToPayload(jsonised_string);
}

StatusCodeMessage StatusCodeEvent::deserializeEventPayload(const payload_type &str_buf) const {
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = PayloadToString(str_buf);
  StatusCodeMessage status_code_message{};

  try {
    if (reader.parse(json_str, json_val)) {
      if (json_val["status_code"].isString() && json_val["origin"].isString() &&
          json_val["component_name"].isString() && json_val["status_msg"].isString()) {

        auto str_status_code = json_val["status_code"].asString();
        if (str_status_code.empty()) {
          throw std::runtime_error("status_code is empty");
        }
        status_code_message = std::stoi(str_status_code);

        auto str_origin = json_val["origin"].asString();
        if (str_origin.empty()) {
          throw std::runtime_error("origin is empty");
        }
        status_code_message.origin = (StatusOrigin)std::stoi(str_origin);

        status_code_message.component_name = json_val["component_name"].asString();

        if (status_code_message.component_name.empty()) {
          throw std::runtime_error("component_name is empty");
        }

        status_code_message.message = json_val["status_msg"].asString();
        if (status_code_message.message.empty()) {
          throw std::runtime_error("status_msg is empty");
        }
      } else {
        throw std::runtime_error("wrong datatype throwing an exception");
      }
    } else {
      logger::warn(__FILE__, " deserializeEventPayload :: failed to deserialize");
    }
  } catch (std::exception &e) {
    logger::warn(__FILE__, " failed to deserialize the message: {}", e.what());
    StatusCodeMessage status_code{};
    return status_code;
  }
  return status_code_message;
}

const std::string StatusCodeEvent::getStatusOriginString(const StatusCodeMessage &status_code) { // todo make this right
  if (kEventStatusOriginToNodeName.find(status_code.origin) != kEventStatusOriginToNodeName.end()) {
    return kEventStatusOriginToNodeName.at(status_code.origin);
  }
  return "Unknown";
}

} // namespace events
} // namespace sdk
} // namespace next
