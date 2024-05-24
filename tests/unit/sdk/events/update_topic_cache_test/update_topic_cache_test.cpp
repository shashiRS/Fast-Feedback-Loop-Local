/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     update_topic_cache_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/update_topic_cache.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

class UpdateTopicCacheFixture : public testing::TestUsingEcal {
public:
  UpdateTopicCacheFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "UpdateTopicCacheFixture";
  }
};

class OverwriteUpdateTopicCacheFixture : public UpdateTopicCache {
public:
  OverwriteUpdateTopicCacheFixture(std::string name) : UpdateTopicCache(name) {}

  next::sdk::events::payload_type serialize(const UpdateTopicCacheMessage &message) const {
    return serializeEventPayload(message);
  }

  UpdateTopicCacheMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(UpdateTopicCacheFixture, de_serializeEventPayloadTest) {
  OverwriteUpdateTopicCacheFixture bin_event("UpdateTopicCache_de_serializeEventPayloadTest");
  UpdateTopicCacheMessage initial_message, final_message;
  initial_message.topics.push_back("topic1");
  initial_message.topics.push_back("topic2");
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.topics, initial_message.topics);
}

TEST_F(UpdateTopicCacheFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  OverwriteUpdateTopicCacheFixture bin_event("UpdateTopicCache_de_serializeEventPayloadTestEmptyStrBuffer");
  UpdateTopicCacheMessage final_message;
  next::sdk::events::payload_type mess_buf;
  std::vector<std::string> empty_vec;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.topics, empty_vec);
}

TEST_F(UpdateTopicCacheFixture, serialize_deserialize_empty) {
  OverwriteUpdateTopicCacheFixture bin_event("UpdateTopicCache_de_serializeEventPayloadTestEmptyStrBuffer");
  UpdateTopicCacheMessage final_message;

  auto payload = bin_event.serialize(final_message);
  auto message = bin_event.deserialize(payload);
  EXPECT_EQ(final_message.topics.size(), 0);
}

TEST_F(UpdateTopicCacheFixture, serialize_deserialize_single_topic) {
  OverwriteUpdateTopicCacheFixture bin_event("UpdateTopicCache_de_serializeEventPayloadTestEmptyStrBuffer");
  UpdateTopicCacheMessage final_message;
  final_message.topics.push_back("asdf");

  auto payload = bin_event.serialize(final_message);
  auto message = bin_event.deserialize(payload);
  EXPECT_EQ(final_message.topics.size(), 1);
  EXPECT_EQ(final_message.topics.front(), "asdf");
}

} // namespace events
} // namespace sdk
} // namespace next
