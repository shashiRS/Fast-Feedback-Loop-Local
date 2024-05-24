
#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal.h>
#include <gtest/gtest.h>

#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>

#include "condition.hpp"

namespace next {
namespace controlbridge {

class ConditionCheckTest : public ::testing::Test {
public:
  ConditionCheckTest() {
    condition_ = Condition({"test_component"}, {101}, {next::sdk::events::StatusSeverity::SEV_SUCCESS},
                           {next::sdk::events::StatusOrigin::SIM_NODE});
  };

  Condition condition_;
};

TEST(ConditionTest, condition_generate_id) {
  Condition condition1({"test_component"}, {1});
  EXPECT_NE(condition1.GetId(), 0);

  Condition condition2({"test_component"}, {2});
  EXPECT_NE(condition2.GetId(), 0);

  EXPECT_NE(condition1.GetId(), condition2.GetId());

  Condition condition3({"test_component"}, {2});
  EXPECT_NE(condition3.GetId(), 0);

  EXPECT_EQ(condition2.GetId(), condition3.GetId());
}

TEST(ConditionTest, condition_constructor) {
  Condition condition1({"test_component"}, {}, {}, {});
  auto a = condition1.GetId();
  EXPECT_NE(a, 0);

  Condition condition2({}, {1}, {}, {});
  auto b = condition2.GetId();
  EXPECT_NE(b, 0);

  Condition condition3({}, {}, {next::sdk::events::StatusSeverity::SEV_DEFAULT}, {});
  auto c = condition3.GetId();
  EXPECT_NE(c, 0);

  Condition condition4({}, {}, {}, {next::sdk::events::StatusOrigin::DEFAULT});
  auto d = condition4.GetId();
  EXPECT_NE(d, 0);

  EXPECT_FALSE(a == b || a == c || a == d || b == c || b == d || c == d);
}

TEST_F(ConditionCheckTest, condition_is_equal) {
  Condition condition1({"same_test_component"}, {}, {}, {});
  Condition condition2({"same_test_component"}, {}, {}, {});

  EXPECT_TRUE(condition1.isEqual(condition2));

  Condition condition3({"different_test_component"}, {}, {}, {});
  EXPECT_FALSE(condition1.isEqual(condition3));
}

TEST_F(ConditionCheckTest, condition_check_name) {
  // component name fits
  EXPECT_TRUE(condition_.Check(next::sdk::events::StatusCodeMessage{100, "test_component", "test_message"}));
}

TEST_F(ConditionCheckTest, condition_check_code) {
  // status code fits
  EXPECT_TRUE(condition_.Check(next::sdk::events::StatusCodeMessage{101, "", "test_message"}));
}

TEST_F(ConditionCheckTest, condition_check_severity) {
  // severity fits (the 2 is the success value)
  EXPECT_TRUE(condition_.Check(next::sdk::events::StatusCodeMessage{1211, "", "test_message"}));
}

TEST_F(ConditionCheckTest, condition_check_origin) {
  // component origin fits
  EXPECT_TRUE(condition_.Check(next::sdk::events::StatusCodeMessage{5000, "", "test_message"}));
}

TEST_F(ConditionCheckTest, condition_check_nothing) {
  // nothing fits
  EXPECT_FALSE(condition_.Check(next::sdk::events::StatusCodeMessage{3333, "", "test_message"}));
}

TEST_F(ConditionCheckTest, condition_empty_condition) {
  Condition condition;

  EXPECT_FALSE(condition.Check(next::sdk::events::StatusCodeMessage{4242, "test_component", "test_message"}));
}

} // namespace controlbridge
} // namespace next