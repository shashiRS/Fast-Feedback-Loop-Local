#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk_macros.h>
#include "signal_exporter.h"

#include <next/sdk/sdk.hpp>
#include <next/udex/publisher/data_publisher.hpp>

namespace next {
namespace plugins {
class SignalExporterTester : public SignalExporter {
public:
  SignalExporterTester(const char *path) : SignalExporter(path) {}
  void purgeTreeExtractorFromNotNeededSignalsTester() { purgeTreeExtractorFromNotNeededSignals(); }
  void addValidSubscriptionsToTreeAndSubscribersTester(std::vector<TopicPair> &valid_signal_urls) {
    addValidSubscriptionsToTreeAndSubscribers(valid_signal_urls);
  }

  std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor> getTrees(std::string topic) {
    auto tree_search = topic_tree_extractor_map_.find(topic);
    if (tree_search == topic_tree_extractor_map_.end()) {
      return {};
    }
    return topic_tree_extractor_map_.at(topic).signalTree;
  }

  std::list<std::string> getSignals(std::string topic) {
    auto tree_search = topic_tree_extractor_map_.find(topic);
    if (tree_search == topic_tree_extractor_map_.end()) {
      return {};
    }
    return topic_tree_extractor_map_.at(topic).required_signals;
  }

  const std::unordered_map<std::string, ctkGexSignalData> &GetExporterSignalList() { return map_gex_signal_to_url_; }
};

class SignalExporterFixture : public next::sdk::testing::TestUsingEcal {
public:
  SignalExporterFixture() { this->instance_name_ = "SignalExporterFixture"; }
  void SetUp() override {
    TestUsingEcal::SetUp();
    publisher_ = std::make_shared<next::udex::publisher::DataPublisher>("SignalExporterFixture");

    publisher_->RegisterDataDescription("test1.sdl", true);
  }
  std::shared_ptr<next::udex::publisher::DataPublisher> publisher_;
  std::string topic_name_VehDyn = "SIM VFB.AlgoVehCycle.VehDyn";
  std::string topic_name_VehDyn1 = "SIM VFB.AlgoVehCycle.VehDyn1";
};

TEST_F(SignalExporterFixture, AddSingleSignalFromOneTopic_success) {
  auto myTester = SignalExporterTester("asdf");
  myTester.init();

  std::vector<SignalExporter::TopicPair> signals;
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber", topic_name_VehDyn});
  myTester.addValidSubscriptionsToTreeAndSubscribersTester(signals);
  auto tree = myTester.getTrees(topic_name_VehDyn);
  ASSERT_TRUE(tree.get() != nullptr);
  constexpr size_t num_signals = 42;
  EXPECT_EQ(tree->signals_.size(), num_signals);
  auto signals_requested = myTester.getSignals(topic_name_VehDyn);
  ASSERT_EQ(signals_requested.size(), 1);
  EXPECT_EQ(signals_requested.front(), "SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber");
}

TEST_F(SignalExporterFixture, AddSingleArraySignalFromOneTopic_success) {
  auto myTester = SignalExporterTester("asdf");
  myTester.init();

  std::vector<SignalExporter::TopicPair> signals;
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.State", topic_name_VehDyn});
  myTester.addValidSubscriptionsToTreeAndSubscribersTester(signals);

  auto resultlist = myTester.GetExporterSignalList();
  ASSERT_EQ(resultlist.size(), 1);
  EXPECT_NE(resultlist.find("SIM VFB.AlgoVehCycle.VehDyn.State"), resultlist.end());
}

TEST_F(SignalExporterFixture, AddSingleArrayWithIndexFromOneTopic_success) {
  auto myTester = SignalExporterTester("asdf");
  myTester.init();

  std::vector<SignalExporter::TopicPair> signals;
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.State[0]", topic_name_VehDyn});
  myTester.addValidSubscriptionsToTreeAndSubscribersTester(signals);

  auto resultlist = myTester.GetExporterSignalList();
  ASSERT_EQ(resultlist.size(), 1);
  EXPECT_NE(resultlist.find("SIM VFB.AlgoVehCycle.VehDyn.State"), resultlist.end());
}

TEST_F(SignalExporterFixture, PurgeSignalTree_singleSignalRequested) {
  auto myTester = SignalExporterTester("asdf");
  myTester.init();

  std::vector<SignalExporter::TopicPair> signals;
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber", topic_name_VehDyn});
  myTester.addValidSubscriptionsToTreeAndSubscribersTester(signals);
  auto tree = myTester.getTrees(topic_name_VehDyn);
  ASSERT_TRUE(tree.get() != nullptr);
  constexpr size_t num_signals = 42;
  EXPECT_EQ(tree->signals_.size(), num_signals);

  myTester.purgeTreeExtractorFromNotNeededSignalsTester();
  tree = myTester.getTrees(topic_name_VehDyn);
  ASSERT_TRUE(tree.get() != nullptr);
  EXPECT_EQ(tree->signals_.size(), 1);
}

TEST_F(SignalExporterFixture, PurgeSignalTree_multiSignalMultiTree) {
  auto myTester = SignalExporterTester("asdf");
  myTester.init();

  std::vector<SignalExporter::TopicPair> signals;
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.uiVersionNumber", topic_name_VehDyn});
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp", topic_name_VehDyn});
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn1.Lateral.Curve.Curve", topic_name_VehDyn1});
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn1.Lateral.SlipAngle.Variance", topic_name_VehDyn1});
  signals.push_back({"SIM VFB.AlgoVehCycle.VehDyn1.State[0]", topic_name_VehDyn1});
  myTester.addValidSubscriptionsToTreeAndSubscribersTester(signals);
  auto tree = myTester.getTrees(topic_name_VehDyn);
  ASSERT_TRUE(tree.get() != nullptr);
  constexpr size_t num_signals = 42;
  EXPECT_EQ(tree->signals_.size(), num_signals);

  tree = myTester.getTrees(topic_name_VehDyn1);
  ASSERT_TRUE(tree.get() != nullptr);
  EXPECT_EQ(tree->signals_.size(), num_signals);

  myTester.purgeTreeExtractorFromNotNeededSignalsTester();
  tree = myTester.getTrees(topic_name_VehDyn);
  ASSERT_TRUE(tree.get() != nullptr);
  EXPECT_EQ(tree->signals_.size(), 2);
  tree = myTester.getTrees(topic_name_VehDyn1);
  ASSERT_TRUE(tree.get() != nullptr);
  EXPECT_EQ(tree->signals_.size(), 3);
}

} // namespace plugins
} // namespace next