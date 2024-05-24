/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_manager_mock.hpp
 * @brief    mock of the Data Manager
 *
 **/

#ifndef UDEX_SIGNAL_EXPLORER_MOCK_H_
#define UDEX_SIGNAL_EXPLORER_MOCK_H_

#include <next/udex/explorer/signal_explorer.hpp>

namespace next {
namespace test {

class SignalExplorerMock : public next::udex::explorer::SignalExplorer {
public:
  SignalExplorerMock() {}
  ~SignalExplorerMock() {}

  virtual std::vector<next::udex::ChildInfo> GetChildByUrl([[maybe_unused]] std::string parent_url) override {
    return child_info_;
  }

  virtual std::vector<std::string> GetPackageUrl([[maybe_unused]] std::string signal_url) override {
    return topic_by_url_;
  }

  virtual std::unordered_map<std::string, std::vector<std::string>> GetFullUrlTree() override { return url_tree_; }
  virtual std::vector<std::string> getDeviceByFormatType(const std::string &) override {
    return dummy_data_source_names_;
  }

  void setTopicByUrl(std::vector<std::string> topic) { topic_by_url_ = topic; }
  void setDeviceByFormatType(std::vector<std::string> devices) { dummy_data_source_names_ = devices; }
  void SetFullUrlTree(std::unordered_map<std::string, std::vector<std::string>> url_tree) { url_tree_ = url_tree; }
  void SetChildInfo(std::vector<next::udex::ChildInfo> child_info) { child_info_ = child_info; }

private:
  std::vector<std::string> topic_by_url_{};
  std::vector<std::string> dummy_data_source_names_{};
  std::unordered_map<std::string, std::vector<std::string>> url_tree_{};
  std::vector<next::udex::ChildInfo> child_info_{};
};

} // namespace test
} // namespace next

#endif // UDEX_SIGNAL_EXPLORER_MOCK_H_
