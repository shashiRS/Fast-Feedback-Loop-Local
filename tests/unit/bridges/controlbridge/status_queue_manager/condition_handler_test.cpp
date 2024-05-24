
#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal.h>
#include <gtest/gtest.h>

#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>

#include "condition_handler.hpp"

namespace next {
namespace controlbridge {

TEST(ConditionHandlerTest, condition_handler_register_condition) {
  Condition condition1({"test_component"}, {1});
  Condition condition2({"test_component"}, {2});

  ConditionHandler condition_handler;
  condition_handler.RegisterCondition(condition1);
  auto condition_list = condition_handler.GetConditions();
  EXPECT_EQ(condition_list.size(), 1);

  condition_handler.RegisterCondition(condition2);
  condition_list = condition_handler.GetConditions();
  EXPECT_EQ(condition_list.size(), 2);

  condition_handler.ClearConditions();
  condition_list = condition_handler.GetConditions();
  EXPECT_EQ(condition_list.size(), 0);
}

TEST(ConditionHandlerTest, condition_handler_get_condition_id_for_message) {
  Condition condition1({"test_component1"}, {101});
  Condition condition2({"test_component2"}, {202});
  Condition condition3({"test_component2"}, {303});
  Condition condition4({"test_component3"}, {303});

  ConditionHandler condition_handler;
  condition_handler.RegisterCondition(condition1);
  condition_handler.RegisterCondition(condition2);

  auto test_message1 = next::sdk::events::StatusCodeMessage{101, "test_component1", "test_message"};
  auto test_message2 = next::sdk::events::StatusCodeMessage{202, "test_component2", "test_message"};
  auto test_message3 = next::sdk::events::StatusCodeMessage{111, "test_component2", "test_message"};
  auto test_message4 = next::sdk::events::StatusCodeMessage{303, "test_component", "test_message"};
  auto test_message5 = next::sdk::events::StatusCodeMessage{303, "test_component2", "test_message"};
  auto test_message_invalid = next::sdk::events::StatusCodeMessage{999, "test_component9", "test_message"};

  // message fits condition 1 but not condition 2
  auto res1 = condition_handler.GetConditionsIdsForMessage(test_message1);
  ASSERT_EQ(res1.size(), 1);
  EXPECT_EQ(res1[0], condition1.GetId());
  EXPECT_NE(res1[0], condition2.GetId());

  // message fits condition 2 but not condition 1
  auto res2 = condition_handler.GetConditionsIdsForMessage(test_message2);
  ASSERT_EQ(res2.size(), 1);
  EXPECT_EQ(res2[0], condition2.GetId());
  EXPECT_NE(res2[0], condition1.GetId());

  // message does not fit any condition
  auto res3 = condition_handler.GetConditionsIdsForMessage(test_message_invalid);
  ASSERT_EQ(res3.size(), 0);

  condition_handler.RegisterCondition(condition3);
  condition_handler.RegisterCondition(condition4);

  // name fits 2 conditions
  auto res_multiple_id1 = condition_handler.GetConditionsIdsForMessage(test_message3);
  ASSERT_EQ(res_multiple_id1.size(), 2);

  // code fits 2 conditions
  auto res_multiple_id2 = condition_handler.GetConditionsIdsForMessage(test_message4);
  ASSERT_EQ(res_multiple_id2.size(), 2);

  // code and name fit 3 conditions
  auto res_multiple_id3 = condition_handler.GetConditionsIdsForMessage(test_message5);
  ASSERT_EQ(res_multiple_id3.size(), 3);
}

} // namespace controlbridge
} // namespace next
