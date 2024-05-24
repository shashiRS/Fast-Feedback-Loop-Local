/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     common.hpp
 * @brief    Common settings of publisher and subscriber
 *
 **/

#ifndef PERFORMANCETEST_COMMON_H_
#define PERFORMANCETEST_COMMON_H_

#include <array>
#include <string>

const char *topic_prefix = "Topic_";
constexpr const size_t message_array_size = 1024;
const std::string topic_description = "TopicDescriptionDummyStuff";
const std::string topic_type = "TopicDummyType";

struct test_payload {
  size_t cycle{0};
  size_t msg_number{0};
  std::array<unsigned char, message_array_size> data;
  test_payload() { std::memset(data.data(), 0, data.size() * sizeof(unsigned char)); }
};

#endif // PERFORMANCETEST_COMMON_H_
