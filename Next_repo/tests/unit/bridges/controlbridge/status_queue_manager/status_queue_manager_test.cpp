
#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal.h>
#include <gtest/gtest.h>

#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>

#include "status_queue_manager.hpp"

namespace next {
namespace controlbridge {

TEST(StatusQueueManagerTest, register_condition_and_get_conditions) {
  StatusQueueManager status_queue_manager;

  Condition cond1({"test_component1"}, {1});
  status_queue_manager.RegisterCondition(cond1);

  Condition cond2({"test_component2"}, {2});
  status_queue_manager.RegisterCondition(cond2);

  Condition cond3({"test_component3"}, {3});
  status_queue_manager.RegisterCondition(cond3);

  auto registered_conditions = status_queue_manager.getConditions();

  EXPECT_EQ(registered_conditions.size(), 3);
}

TEST(StatusQueueManagerTest, get_queue_by_id) {
  StatusQueueManager status_queue_manager;

  auto queue = status_queue_manager.getQueueById(1234);
  EXPECT_EQ(queue.status_codes_.size(), 0);
}

TEST(StatusQueueManagerTest, parse_status_code_msg) {
  StatusQueueManager status_queue_manager;

  Condition condition({"test_component"}, {100});
  status_queue_manager.RegisterCondition(condition);

  // parse 3 messages that have same component name
  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{100, "test_component", "test_message"}));

  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{200, "test_component", "test_message"}));

  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{300, "test_component", "test_message"}));

  auto status_queue = status_queue_manager.getQueueById(condition.GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 3);

  // parse 3 more messages that have same id but component names are diferent
  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{100, "test_component1", "test_message"}));

  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{100, "test_component2", "test_message"}));

  EXPECT_TRUE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{100, "test_component3", "test_message"}));

  status_queue = status_queue_manager.getQueueById(condition.GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 6);

  // parse message that soesn't fit the condition
  EXPECT_FALSE(status_queue_manager.ParseStatusCodeMsg(
      next::sdk::events::StatusCodeMessage{500, "test_component5", "test_message"}));

  status_queue = status_queue_manager.getQueueById(condition.GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 6);
}

TEST(StatusQueueManagerTest, parse_name_and_code) {
  StatusQueueManager status_queue_manager1;
  StatusQueueManager status_queue_manager2;

  std::vector<Condition> conditions_vec1 = {{{"test_component1"}, {100}},
                                            {{"test_component1"}, {200}},
                                            {{"test_component1"}, {300}},
                                            {{"test_component1"}, {400}}};
  std::vector<Condition> conditions_vec2 = {{{"test_component1"}, {100, 200, 300, 400}}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{100, "test_component1", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{200, "test_component1", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{300, "test_component1", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{400, "test_component1", "test_message"};

  for (const auto &condition : conditions_vec1) {
    status_queue_manager1.RegisterCondition(condition);
  }

  status_queue_manager1.ParseStatusCodeMsg(test_message1);
  status_queue_manager1.ParseStatusCodeMsg(test_message2);
  status_queue_manager1.ParseStatusCodeMsg(test_message3);
  status_queue_manager1.ParseStatusCodeMsg(test_message4);

  // name fits for all 4 conditions, all 4 status to all 4 queues
  for (auto &condition : conditions_vec1) {
    auto status_queue = status_queue_manager1.getQueueById(condition.GetId());
    EXPECT_EQ(status_queue.status_codes_.size(), 4);
  }

  for (const auto &condition : conditions_vec2) {
    status_queue_manager2.RegisterCondition(condition);
  }

  status_queue_manager2.ParseStatusCodeMsg(test_message1);
  status_queue_manager2.ParseStatusCodeMsg(test_message2);
  status_queue_manager2.ParseStatusCodeMsg(test_message3);
  status_queue_manager2.ParseStatusCodeMsg(test_message4);

  // 4 names fit to the one condition, all 4 status in one queue
  auto status_queue = status_queue_manager2.getQueueById(conditions_vec2[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 4);
}

TEST(StatusQueueManagerTest, parse_name) {
  StatusQueueManager status_queue_manager1;
  StatusQueueManager status_queue_manager2;

  std::vector<Condition> conditions_vec1 = {{{"test_component1"}, {}}, {{"test_component2"}, {}}};
  std::vector<Condition> conditions_vec2 = {{{"test_component1", "test_component2"}, {}}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{100, "test_component1", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{100, "test_component1", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{100, "test_component2", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{100, "test_component2", "test_message"};

  for (const auto &condition : conditions_vec1) {
    status_queue_manager1.RegisterCondition(condition);
  }

  status_queue_manager1.ParseStatusCodeMsg(test_message1);
  status_queue_manager1.ParseStatusCodeMsg(test_message2);
  status_queue_manager1.ParseStatusCodeMsg(test_message3);
  status_queue_manager1.ParseStatusCodeMsg(test_message4);

  // 2 names fits for each condition, 2 status to each of the 2 queues
  for (auto &condition : conditions_vec1) {
    auto status_queue = status_queue_manager1.getQueueById(condition.GetId());
    EXPECT_EQ(status_queue.status_codes_.size(), 2);
  }

  for (const auto &condition : conditions_vec2) {
    status_queue_manager2.RegisterCondition(condition);
  }

  status_queue_manager2.ParseStatusCodeMsg(test_message1);
  status_queue_manager2.ParseStatusCodeMsg(test_message2);
  status_queue_manager2.ParseStatusCodeMsg(test_message3);
  status_queue_manager2.ParseStatusCodeMsg(test_message4);

  // 4 names fit to condition, 4 status to queue
  auto status_queue = status_queue_manager2.getQueueById(conditions_vec2[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 4);
}

TEST(StatusQueueManagerTest, parse_code) {
  StatusQueueManager status_queue_manager1;
  StatusQueueManager status_queue_manager2;

  std::vector<Condition> conditions_vec1 = {{{}, {100}}, {{}, {200}}, {{}, {300}}, {{}, {400}}};
  std::vector<Condition> conditions_vec2 = {{{}, {100, 200, 300, 400}}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{100, "", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{200, "", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{300, "", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{400, "", "test_message"};

  for (const auto &condition : conditions_vec1) {
    status_queue_manager1.RegisterCondition(condition);
  }

  status_queue_manager1.ParseStatusCodeMsg(test_message1);
  status_queue_manager1.ParseStatusCodeMsg(test_message2);
  status_queue_manager1.ParseStatusCodeMsg(test_message3);
  status_queue_manager1.ParseStatusCodeMsg(test_message4);

  // 1 code fits for each condition, 1 status to each of the 4 queues
  for (auto &condition : conditions_vec1) {
    auto status_queue = status_queue_manager1.getQueueById(condition.GetId());
    EXPECT_EQ(status_queue.status_codes_.size(), 1);
  }

  for (const auto &condition : conditions_vec2) {
    status_queue_manager2.RegisterCondition(condition);
  }

  status_queue_manager2.ParseStatusCodeMsg(test_message1);
  status_queue_manager2.ParseStatusCodeMsg(test_message2);
  status_queue_manager2.ParseStatusCodeMsg(test_message3);
  status_queue_manager2.ParseStatusCodeMsg(test_message4);

  // 4 codes fit to condition, 4 status to queue
  auto status_queue = status_queue_manager2.getQueueById(conditions_vec2[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 4);
}

TEST(StatusQueueManagerTest, parse_severity) {
  StatusQueueManager status_queue_manager1;
  StatusQueueManager status_queue_manager2;

  using sever = next::sdk::events::StatusSeverity;

  std::vector<Condition> conditions_vec1 = {{{}, {}, {sever::SEV_INFO}},
                                            {{}, {}, {sever::SEV_SUCCESS}},
                                            {{}, {}, {sever::SEV_WARNING}},
                                            {{}, {}, {sever::SEV_ERROR}}};
  std::vector<Condition> conditions_vec2 = {
      {{}, {}, {sever::SEV_INFO, sever::SEV_SUCCESS, sever::SEV_WARNING, sever::SEV_ERROR}}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{100, "", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{200, "", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{300, "", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{400, "", "test_message"};

  for (const auto &condition : conditions_vec1) {
    status_queue_manager1.RegisterCondition(condition);
  }

  status_queue_manager1.ParseStatusCodeMsg(test_message1);
  status_queue_manager1.ParseStatusCodeMsg(test_message2);
  status_queue_manager1.ParseStatusCodeMsg(test_message3);
  status_queue_manager1.ParseStatusCodeMsg(test_message4);

  // 1 severity fits for each condition, 1 status to each of the 4 queues
  for (auto &condition : conditions_vec1) {
    auto status_queue = status_queue_manager1.getQueueById(condition.GetId());
    EXPECT_EQ(status_queue.status_codes_.size(), 1);
  }

  for (const auto &condition : conditions_vec2) {
    status_queue_manager2.RegisterCondition(condition);
  }

  status_queue_manager2.ParseStatusCodeMsg(test_message1);
  status_queue_manager2.ParseStatusCodeMsg(test_message2);
  status_queue_manager2.ParseStatusCodeMsg(test_message3);
  status_queue_manager2.ParseStatusCodeMsg(test_message4);

  // 4 severity fit to condition, 4 status to queue
  auto status_queue = status_queue_manager2.getQueueById(conditions_vec2[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 4);
}

TEST(StatusQueueManagerTest, parse_origin) {
  StatusQueueManager status_queue_manager1;
  StatusQueueManager status_queue_manager2;

  using orig = next::sdk::events::StatusOrigin;

  std::vector<Condition> conditions_vec1 = {{{}, {}, {}, {orig::NEXT_DATABRIDGE}},
                                            {{}, {}, {}, {orig::NEXT_CONTROLBRIDGE}},
                                            {{}, {}, {}, {orig::NEXT_PLAYER}},
                                            {{}, {}, {}, {orig::SIM_NODE}}};
  std::vector<Condition> conditions_vec2 = {
      {{}, {}, {}, {orig::NEXT_DATABRIDGE, orig::NEXT_CONTROLBRIDGE, orig::NEXT_PLAYER, orig::SIM_NODE}}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{2000, "", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{3000, "", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{4000, "", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{5000, "", "test_message"};

  for (const auto &condition : conditions_vec1) {
    status_queue_manager1.RegisterCondition(condition);
  }

  status_queue_manager1.ParseStatusCodeMsg(test_message1);
  status_queue_manager1.ParseStatusCodeMsg(test_message2);
  status_queue_manager1.ParseStatusCodeMsg(test_message3);
  status_queue_manager1.ParseStatusCodeMsg(test_message4);

  // 1 origin fits for each condition, 1 status to each of the 4 queues
  for (auto &condition : conditions_vec1) {
    auto status_queue = status_queue_manager1.getQueueById(condition.GetId());
    EXPECT_EQ(status_queue.status_codes_.size(), 1);
  }

  for (const auto &condition : conditions_vec2) {
    status_queue_manager2.RegisterCondition(condition);
  }

  status_queue_manager2.ParseStatusCodeMsg(test_message1);
  status_queue_manager2.ParseStatusCodeMsg(test_message2);
  status_queue_manager2.ParseStatusCodeMsg(test_message3);
  status_queue_manager2.ParseStatusCodeMsg(test_message4);

  // 4 origin fit to condition, 4 status to queue
  auto status_queue = status_queue_manager2.getQueueById(conditions_vec2[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 4);
}

TEST(StatusQueueManagerTest, parse_nothing) {
  StatusQueueManager status_queue_manager;

  std::vector<Condition> conditions_vec = {{}};

  auto test_message1 = next::sdk::events::StatusCodeMessage{100, "", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{100, "test_component", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{111, "", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{111, "test_component", "test_message"};

  for (const auto &condition : conditions_vec) {
    status_queue_manager.RegisterCondition(condition);
  }

  status_queue_manager.ParseStatusCodeMsg(test_message1);
  status_queue_manager.ParseStatusCodeMsg(test_message2);
  status_queue_manager.ParseStatusCodeMsg(test_message3);
  status_queue_manager.ParseStatusCodeMsg(test_message4);

  // nothing should be parsed
  auto status_queue = status_queue_manager.getQueueById(conditions_vec[0].GetId());
  EXPECT_EQ(status_queue.status_codes_.size(), 0);
}

} // namespace controlbridge
} // namespace next
