/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     init_chain_tests.cpp
 *  @brief    Testing init chain
 */

#include <cip_test_support/gtest_reporting.h>

#include <stdexcept>
#include <vector>

#include <next/sdk/init_chain/simple_init_chain.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

struct InitChain : public next::sdk::InitChain {
  static void clear() {
    head_ = nullptr;
    tail_ = nullptr;
    Reset();
  }
};

struct TestHelper {
public:
  TestHelper() {}
  TestHelper(TestHelper const &) = delete;
  TestHelper(TestHelper &&) = delete;

  bool init(InitChain::ConfigMap const &, int i) {
    init_.push_back(i);
    return true;
  }
  bool init2(InitChain::ConfigMap const &config, int, int i) { return init(config, i); }

  void reset(InitChain::ConfigMap const &, int i) { reset_.push_back(i); }
  void reset2(InitChain::ConfigMap const &, int, int i) { reset_.push_back(i); }

  std::vector<int> init_;
  std::vector<int> reset_;
};

class InitChainTest : public ::testing::Test {
public:
  void SetUp() override {}
  void TearDown() override { InitChain::clear(); }
};

bool dummyInitTrue(InitChain::ConfigMap const &, int) { return true; }

bool dummyInitFalse(InitChain::ConfigMap const &, int) { return false; }

bool dummyInitThrow(InitChain::ConfigMap const &, int) { throw std::runtime_error("some exception"); }

void dummyReset(InitChain::ConfigMap const &, int) {}

void dummyResetThrow(InitChain::ConfigMap const &, int) {}

TEST_F(InitChainTest, el_constructor_noInitFunction_exceptionThrown) {
  EXPECT_THROW(InitChain::El(100, nullptr, nullptr), std::invalid_argument);
}

TEST_F(InitChainTest, el_constructor_noResetFunction_exceptionThrown) {
  EXPECT_NO_THROW(InitChain::El(100, dummyInitTrue, nullptr));
}

//! Testing the string split function
TEST_F(InitChainTest, run_fiveElements_correctOrder) {
  TestHelper hlp;

  // inserting into empty list
  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // inserting at the end
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // inserting in the middle
  InitChain::El e3(150, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // replacing the head
  InitChain::El e4(50, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // and an already used level
  InitChain::El e5(100, std::bind(&TestHelper::init2, &hlp, _1, _2, 101),
                   std::bind(&TestHelper::reset2, &hlp, _1, _2, 101));

  ASSERT_TRUE(InitChain::Run());
  ASSERT_EQ(hlp.init_.size(), 5);
  EXPECT_EQ(hlp.init_[0], 50);
  EXPECT_EQ(hlp.init_[1], 101) << "the second module with level 100 should be initialized first";
  EXPECT_EQ(hlp.init_[2], 100);
  EXPECT_EQ(hlp.init_[3], 150);
  EXPECT_EQ(hlp.init_[4], 200);
}

TEST_F(InitChainTest, reset_fiveElements_correctOrder) {
  TestHelper hlp;

  // inserting into empty list
  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // inserting at the end
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // inserting in the middle
  InitChain::El e3(150, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // replacing the head
  InitChain::El e4(50, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  // and an already used level
  InitChain::El e5(100, std::bind(&TestHelper::init2, &hlp, _1, _2, 101),
                   std::bind(&TestHelper::reset2, &hlp, _1, _2, 101));

  ASSERT_TRUE(InitChain::Run());
  ASSERT_EQ(hlp.init_.size(), 5);

  InitChain::Reset();
  ASSERT_EQ(hlp.reset_.size(), 5);
  EXPECT_EQ(hlp.reset_[0], 200);
  EXPECT_EQ(hlp.reset_[1], 150);
  EXPECT_EQ(hlp.reset_[2], 100);
  EXPECT_EQ(hlp.reset_[3], 101) << "the second module with level 100 should be reset second";
  EXPECT_EQ(hlp.reset_[4], 50);
}

// this test tests whether the init function
// * removes the tail
// * setting the previous thing to the new tail
TEST_F(InitChainTest, reset_noResetFunction_correctTailSet) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);

  ASSERT_TRUE(InitChain::Run());
  ASSERT_EQ(hlp.init_.size(), 2);
  InitChain::Reset();
  ASSERT_EQ(hlp.reset_.size(), 1);
  EXPECT_EQ(hlp.reset_[0], 100);
}

// this test tests whether the init function
// * removes the head
// * setting the following element as new head
TEST_F(InitChainTest, reset_noResetFunction_correctHeadSet) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));

  ASSERT_TRUE(InitChain::Run());
  ASSERT_EQ(hlp.init_.size(), 2);
  InitChain::Reset();
  ASSERT_EQ(hlp.reset_.size(), 1);
  EXPECT_EQ(hlp.reset_[0], 200);
}

// this test tests whether the init function
// * removes head and tail element (same element)
// * setting head and tail to nullptr
TEST_F(InitChainTest, reset_noResetFunction_noResetCalled) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);

  ASSERT_TRUE(InitChain::Run());
  ASSERT_EQ(hlp.init_.size(), 2);
  InitChain::Reset();
  EXPECT_EQ(hlp.reset_.size(), 0);
}

TEST_F(InitChainTest, runTwice_noResetInBetween_returnsFalse) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));

  ASSERT_TRUE(InitChain::Run());
  EXPECT_FALSE(InitChain::Run());
}

TEST_F(InitChainTest, runTwice_resetInBetween_returnsTrue) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), std::bind(&TestHelper::reset, &hlp, _1, _2));

  ASSERT_TRUE(InitChain::Run());
  InitChain::Reset();
  EXPECT_TRUE(InitChain::Run());
}

TEST_F(InitChainTest, runTwice_noResetFunctions_returnsTrueNoElementsInitialized) {
  TestHelper hlp;

  InitChain::El e1(100, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);
  InitChain::El e2(200, std::bind(&TestHelper::init, &hlp, _1, _2), nullptr);

  ASSERT_TRUE(InitChain::Run());
  InitChain::Reset();
  ASSERT_EQ(hlp.init_.size(), 2);
  ASSERT_EQ(hlp.reset_.size(), 0);
  hlp.init_.clear();
  hlp.reset_.clear();
  EXPECT_TRUE(InitChain::Run());
  InitChain::Reset();
  EXPECT_EQ(hlp.init_.size(), 0);
  EXPECT_EQ(hlp.reset_.size(), 0);
}

TEST_F(InitChainTest, run_failingElement_returnsFalse) {
  TestHelper hlp;

  InitChain::El e1(100, dummyInitTrue, dummyReset);
  InitChain::El e2(200, dummyInitFalse, dummyReset);
  EXPECT_FALSE(InitChain::Run());
}

TEST_F(InitChainTest, run_exceptionThrowingElement_returnsFalseNoThrow) {
  TestHelper hlp;

  InitChain::El e1(100, dummyInitTrue, dummyReset);
  InitChain::El e2(200, dummyInitThrow, dummyReset);
  bool res = true;
  EXPECT_NO_THROW(res = InitChain::Run());
  EXPECT_FALSE(res);
}

TEST_F(InitChainTest, reset_exceptionThrowingElement_noThrow) {
  TestHelper hlp;

  InitChain::El e1(100, dummyInitTrue, dummyReset);
  InitChain::El e2(200, dummyInitTrue, dummyResetThrow);
  ASSERT_TRUE(InitChain::Run());
  EXPECT_NO_THROW(InitChain::Reset());
}
