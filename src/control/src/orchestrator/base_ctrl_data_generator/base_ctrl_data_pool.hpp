/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_ctrl_data_pool.h
 * @brief    safes and subscribes and provides the data for base control data generation
 *
 **/
#ifndef NEXT_BASE_CTRL_DATA_POOL_HPP
#define NEXT_BASE_CTRL_DATA_POOL_HPP

#include <algorithm>

#include <next/sdk/sdk.hpp>

#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include "base_ctrl_config_reader.hpp"
#include "next/udex/explorer/signal_explorer.hpp"

namespace next {
namespace control {
namespace orchestrator {

struct ConnectedSignal {
  next::udex::ValueUnion value;
  next::udex::SignalType signal_type;
};

struct DataPool {
  std::map<std::string, float> fixed_signal;
  std::map<std::string, ConnectedSignal> connected_signal;
};

class BaseControlDataPool {
  struct FixedValues {
    size_t current_value_counter;
    std::vector<float> values;
    std::string control_data_url_name;
  };
  struct ConnectedValues {
    std::shared_ptr<next::udex::AnyValue> value;
    std::string control_data_url_name;
  };

  struct DataPoolBackend {
    std::vector<FixedValues> fixed_values;
    std::vector<ConnectedValues> connected_values;
    std::unordered_map<std::string, std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor>>
        tree_extractors;
  };

public:
  BaseControlDataPool(const std::string &flow_name) : flow_name_(flow_name) {
    subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>(flow_name_);
    explorer_ = std::make_unique<next::udex::explorer::SignalExplorer>();
    extractor_ = std::make_shared<next::udex::extractor::DataExtractor>();
    extractor_->SetBlockingExtraction(true);
  }

  virtual ~BaseControlDataPool(){};

  bool SetupInput(const std::vector<TriggerDataInfo> &DataInfo) {
    for (auto &info_exec : DataInfo) {
      bool all_connected = true;
      auto hash_name = createHashName(info_exec.component_name, info_exec.execution_name);
      auto &myBackend = all_data_[hash_name];

      auto &all_signals = info_exec.info;
      for (auto signal : all_signals) {

        if (signal.type == SignalType::connected) {
          std::shared_ptr<next::udex::AnyValue> myValue =
              SearchOrAddTreeExtractorForSignal(signal.connection_name, myBackend);

          if (myValue.get() == nullptr) {
            all_connected = false;
          } else {
            myBackend.connected_values.push_back({myValue, signal.control_data_url_name});
          }
        }

        if (signal.type == SignalType::fixed_values) {
          myBackend.fixed_values.push_back({0, signal.fixed_value_array, signal.control_data_url_name});
        }
      }

      if (!all_connected) {
        warn(__FILE__, "Not all control data URLs could be connected for flow: {0}", hash_name);
      }
    }

    return true;
  }

  DataPool GetNewData(const std::string &component_name, const std::string &execution_name, bool &success) {
    DataPool result;

    auto hash_name = createHashName(component_name, execution_name);

    updateResultFixValues(result, hash_name);
    success = updateConnectedValues(result, hash_name);
    if (!success) {
      debug(__FILE__, "not all signals available for {0}", hash_name);
    }
    return result;
  }

  bool IsConnected() {
    bool all_connected = true;

    for (auto sub : subscriber_->getSubscriptionsInfo()) {
      all_connected &= subscriber_->IsConnected(sub.second.topic);
    }

    return all_connected;
  }

private:
  std::string createHashName(const std::string &component_name, const std::string &exectuion_name) {
    return component_name + "_" + exectuion_name;
  }

  void updateResultFixValues(DataPool &result, const std::string &hash_name) {
    auto &current_fixed_values = all_data_[hash_name].fixed_values;

    for (auto &fixed_values : current_fixed_values) {
      if (fixed_values.current_value_counter < fixed_values.values.size()) {
        result.fixed_signal[fixed_values.control_data_url_name] =
            fixed_values.values[fixed_values.current_value_counter];
        fixed_values.current_value_counter++;
      } else {
        result.fixed_signal[fixed_values.control_data_url_name] = fixed_values.values[fixed_values.values.size() - 1];
      }
    }
  }

  bool updateConnectedValues(DataPool &result, const std::string hash_name) {
    auto &current_control_data_pool = all_data_[hash_name];
    bool all_found = true;
    for (auto &connect_values : current_control_data_pool.connected_values) {

      // get Tree Extactor for value
      auto tree = current_control_data_pool.tree_extractors[connect_values.value->GetFullUrl()];
      auto data = extractor_->GetExtractedData(tree->GetParentUrl());
      if (data.data != nullptr) {
        tree->SetMemory((const char *)data.data, data.size);
        debug(__FILE__, "Value: {0}", connect_values.value->Get().asUint32);
      } else {
        all_found = false;
      }

      result.connected_signal[connect_values.control_data_url_name] = {connect_values.value->Get(),
                                                                       connect_values.value->GetTypeOfAnyValue()};
    }
    return all_found;
  }

  std::shared_ptr<next::udex::AnyValue> SearchOrAddTreeExtractorForSignal(const std::string &signal_url,
                                                                          DataPoolBackend &backend) const {

    auto result = findAnyValueInTreesList(signal_url, backend);
    if (result.get() != nullptr) {
      return result;
    }

    auto package_url_list = explorer_->GetPackageUrl(signal_url);
    if (package_url_list.size() != 1) {
      return {};
    }
    std::string package_url = package_url_list.at(0);

    result = searchInSignalListOfTrees(signal_url, backend, package_url);
    if (result.get() != nullptr) {
      return result;
    }

    return createTreeExtractorAndConnect(signal_url, backend, package_url);
  }

  std::shared_ptr<udex::AnyValue> searchInSignalListOfTrees(const std::string &signal_url, DataPoolBackend &backend,
                                                            const std::string &package_url) const {
    auto search_name = [&signal_url](const udex::struct_extractor::PackageTreeExtractor::Signal &signal) {
      return signal.name == signal_url;
    };

    for (auto &tree : backend.tree_extractors) {
      if (tree.second->GetParentUrl() == package_url) {
        auto any_value_search = std::find_if(tree.second->signals_.begin(), tree.second->signals_.end(), search_name);
        if (any_value_search != tree.second->signals_.end()) {
          backend.tree_extractors[signal_url] = tree.second;
          return any_value_search->value;
        }
      }
    }
    return {};
  }

  std::shared_ptr<udex::AnyValue> createTreeExtractorAndConnect(const std::string &signal_url, DataPoolBackend &backend,
                                                                const std::string &package_url) const {
    auto search_name = [&signal_url](const udex::struct_extractor::PackageTreeExtractor::Signal &signal) {
      return signal.name == signal_url;
    };

    auto package_tree = std::make_shared<udex::struct_extractor::PackageTreeExtractor>(package_url);
    if (!package_tree->SetupSuccessful()) {
      return {};
    }
    auto any_value_search = std::find_if(package_tree->signals_.begin(), package_tree->signals_.end(), search_name);
    if (any_value_search == package_tree->signals_.end()) {
      return {};
    }
    backend.tree_extractors[signal_url] = package_tree;
    subscriber_->Subscribe(signal_url, extractor_);
    return any_value_search->value;
  }

  std::shared_ptr<udex::AnyValue> findAnyValueInTreesList(const std::string &signal_url,
                                                          DataPoolBackend &backend) const {
    auto found_tree = backend.tree_extractors.find(signal_url);
    if (found_tree != backend.tree_extractors.end()) {
      auto search_name = [&signal_url](const udex::struct_extractor::PackageTreeExtractor::Signal &signal) {
        return signal.name == signal_url;
      };
      auto found_signal =
          std::find_if(found_tree->second->signals_.begin(), found_tree->second->signals_.end(), search_name);
      if (found_signal != found_tree->second->signals_.end()) {
        return found_signal->value;
      } else {
        error(__FILE__, "Signal expected in TreeExtractor but not found");
      }
    }
    return {};
  }

private:
  std::unordered_map<std::string, DataPoolBackend> all_data_;

  std::string flow_name_;

  std::shared_ptr<next::udex::subscriber::DataSubscriber> subscriber_;
  std::unique_ptr<next::udex::explorer::SignalExplorer> explorer_;
  std::shared_ptr<next::udex::extractor::DataExtractor> extractor_;

public:
  friend class BaseControlDataPoolTester;
};

} // namespace orchestrator
} // namespace control
} // namespace next
#endif // NEXT_BASE_CTRL_DATA_POOL_HPP
