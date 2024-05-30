/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     lifecycle_test.exe
 *  @brief    Testing the life cycle interface on a dummy node
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/lifecycle/lifecycle_interface.hpp>

class dummyNode : public next::sdk::lifecycle::ComponentLifeCycle {

public:
  std::string content = "Initial";

  dummyNode(std::string node_name) { this->node_name = node_name; }

  std::string get_node_name() { return node_name; }

private:
  std::string node_name;

  void onConfigure() { content = "Configured"; };

  void onCleanup() { content = "Cleanup"; };

  void onActivate() { content = "Activated"; };

  void onDeactivate() { content = "Deactivated"; };

  void onShutdown() { content = "Shutdown"; };

  void onError() { content = "Error"; };
};

TEST(lifecycle_test, check_valid_transitions) {
  dummyNode node = dummyNode("Dummy Node");

  std::cout << "Node name: " << node.get_node_name().c_str() << std::endl;
  std::cout << "---" << std::endl;

  // check initial state of the node
  EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Unconfigured");

  // check onConfigure
  if (node.configure()) {
    EXPECT_EQ(node.content, "Configured");
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Inactive");
  } else {
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Unconfigured");
  }

  // check onActivate
  if (node.activate()) {
    EXPECT_EQ(node.content, "Activated");
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Active");
  } else {
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Inactive");
  }

  // check onDeactivate
  if (node.deactivate()) {
    EXPECT_EQ(node.content, "Deactivated");
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Inactive");
  } else {
    EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Active");
  }

  // check onShutdown
  node.activate();

  node.shutdown();
  EXPECT_EQ(node.content, "Shutdown");
  EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Finalized");
}

TEST(lifecycle_test, check_invalid_transitions) {
  dummyNode node = dummyNode("Dummy Node 2");

  std::cout << "Node name: " << node.get_node_name().c_str() << std::endl;
  std::cout << "---" << std::endl;

  // check invalid transition: Unconfigured -> Active
  node.activate();
  EXPECT_EQ(node.getStateAsString(node.getCurrentState()), "Unconfigured");
}
