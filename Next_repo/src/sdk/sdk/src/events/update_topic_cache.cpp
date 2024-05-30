/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     update_topic_cache.cpp
 * @brief    event emmitted by nodes when publishers created/deleted
 *
 **/

#include <json/json.h>

#include <next/sdk/events/update_topic_cache.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type UpdateTopicCache::serializeEventPayload([[maybe_unused]] const UpdateTopicCacheMessage &message) const {
  std::string jsonised_string;
  try {
    Json::Value json_val;
    json_val["topics"] = Json::arrayValue;
    for (auto topic : message.topics) {
      json_val["topics"].append(topic);
    }
    Json::FastWriter fastWriter;
    jsonised_string = fastWriter.write(json_val);
  } catch (std::exception &e) {
    logger::warn(__FILE__, "StatusCode::deserializeEventPayload :: failed to serializ: {}", e.what());
  }
  return StringToPayload(jsonised_string);
}

UpdateTopicCacheMessage UpdateTopicCache::deserializeEventPayload([[maybe_unused]] const payload_type &strbuf) const {
  UpdateTopicCacheMessage message;

  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (!jsonVal.isMember("topics")) {
      logger::warn(__FILE__, "UpdateTopicCache::deserializeEventPayload :: does not contain topic member");
      return message;
    }
    if (!jsonVal["topics"].isArray()) {
      logger::warn(__FILE__, "UpdateTopicCache::deserializeEventPayload :: topics not an array");
      return message;
    }
    for (auto &topic : jsonVal["topics"]) {
      if (topic.isString()) {
        message.topics.push_back(topic.asString());
      } else {
        logger::warn(__FILE__, "UpdateTopicCache::deserializeEventPayload :: Array does not contain string");
      }
    }
  } else {
    logger::warn(__FILE__, "UpdateTopicCache::deserializeEventPayload :: json deserialze failed");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
