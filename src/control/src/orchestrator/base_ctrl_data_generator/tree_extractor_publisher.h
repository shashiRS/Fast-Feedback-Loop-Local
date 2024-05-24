/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     tree_extractor_publisher.h
 * @brief    creator of tree extractor publisher
 *
 **/

#ifndef NEXT_TREE_EXTRACTOR_PUBLISHER_HPP
#define NEXT_TREE_EXTRACTOR_PUBLISHER_HPP

#include <fstream>
#include <string>

#include <next/sdk/sdk.hpp>

#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>

namespace next {
namespace control {
namespace orchestrator {

class TreeExtractorPublisher {
public:
  TreeExtractorPublisher(const std::string &flow_name, const std::string &client_name)
      : client_name_(client_name), flow_name_(flow_name), publisher_(flow_name + "_" + client_name),
        base_control_data_port_name_("Orchestrator." + flow_name_ + "_" + client_name_ + ".SigHeader") {}
  ~TreeExtractorPublisher() {
    if (tree_package_ptr_ != nullptr) {
      free(tree_package_ptr_);
    }
  }

  bool Setup(const std::string &sdl_name) {
    if (!ParseSdlToFlow(sdl_name)) {
      warn(__FILE__, "Parse sdl failed");
      is_ready_ = false;
      return false;
    }
    if (!CreatePublisher()) {
      warn(__FILE__, "Publisher creation failed");
      is_ready_ = false;
      return false;
    }
    if (!CreateTreeExtractor()) {
      warn(__FILE__, "TreeExtractor creation failed - Setup()");
      is_ready_ = false;
      return false;
    }
    is_ready_ = true;
    return true;
  }

  bool IsReady() const { return is_ready_; }

  std::string GetFullSignalName(const std::string &base_control_url) {
    return "Orchestrator." + flow_name_ + "_" + client_name_ + "." + base_control_url;
  }

  std::shared_ptr<next::udex::AnyValue> findMatchingValueInTreeExtractor(const std::string base_control_url) {
    std::string expanded_signal_name = GetFullSignalName(base_control_url);

    if (tree_extractor.get() == nullptr) {
      error(__FILE__, "TreeExtractor creation failed - findMatchingValueInTreeExtractor()");
      return {};
    }

    auto find_signal = [&](const next::udex::struct_extractor::PackageTreeExtractor::Signal &signal) {
      return signal.name == expanded_signal_name;
    };
    auto found_signal = std::find_if(tree_extractor->signals_.begin(), tree_extractor->signals_.end(), find_signal);
    if (found_signal == tree_extractor->signals_.end()) {
      debug(__FILE__, "Signal not found {0}", base_control_url);
      return {};
    }
    return found_signal->value;
  }

  template <class valueType>
  bool SetValue(const valueType &value, const std::string &base_control_url) {
    auto found_signal = findMatchingValueInTreeExtractor(base_control_url);
    if (found_signal.get() == nullptr) {
      return false;
    }
    return SetAnyValueFromTemplate(found_signal, value);
  }

  bool SetValueFromValueUnion(const udex::ValueUnion &value, udex::SignalType type,
                              const std::string base_control_url) {
    auto found_signal = findMatchingValueInTreeExtractor(base_control_url);
    if (found_signal.get() == nullptr) {
      return false;
    }
    SetAnyValueFromValueUnion(found_signal, value, type);
    return true;
  }

  bool Publish(size_t timestamp) {
    if (tree_package_ptr_ == nullptr || tree_package_size_ == 0) {

      if (tree_extractor.get() == nullptr) {
        warn(__FILE__, "No publisher found. Could not publish");
        return false;
      }
    }
    bool result =
        publisher_.publishData(base_control_data_port_name_, tree_package_ptr_, tree_package_size_, false, timestamp);
    ClearTreePackage();
    return result;
  }

private:
  void ClearTreePackage() {
    if (tree_package_ptr_ == nullptr) {
      return;
    }
    memset(tree_package_ptr_, 0, tree_package_size_);
  }
  bool CreateTreeExtractor() {
    next::udex::extractor::DataExtractor extractor = next::udex::extractor::DataExtractor();
    auto info = extractor.GetInfo(base_control_data_port_name_);
    for (int i_request = 0; i_request < 100; i_request++) {
      if (info.signal_size != 0) {
        break;
      }
      info = extractor.GetInfo(base_control_data_port_name_);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (info.signal_size == 0) {
      return false;
    }
    tree_extractor = std::make_shared<udex::struct_extractor::PackageTreeExtractor>(base_control_data_port_name_);
    tree_package_size_ = info.signal_size;
    tree_package_ptr_ = (char *)malloc(tree_package_size_);
    if (tree_package_ptr_ == nullptr) {
      return false;
    }
    ClearTreePackage();
    tree_extractor->SetMemory(tree_package_ptr_, tree_package_size_);
    return true;
  }
  bool CreatePublisher() {
    publisher_.SetDataSourceInfo("Orchestrator", 1, 1);
    publisher_.RegisterDataDescription((const char *)"SigHeader", (void *)sdl_string_.c_str(), sdl_string_.size(),
                                       true);
    return true;
  }

  bool ParseSdlToFlow(const std::string &sdl_file_name) {
    std::ifstream ifs(sdl_file_name);
    if (!ifs.is_open()) {
      error("Could noropen sdl file {0}", sdl_file_name);
      return false;
    }
    std::string sdl_file_str(std::istreambuf_iterator<char>{ifs}, {});
    replace(sdl_file_str, "?ViewName?", flow_name_ + "_" + client_name_);
    sdl_string_ = sdl_file_str;
    return true;
  }
  bool replace(std::string &str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
      return false;
    str.replace(start_pos, from.length(), to);
    return true;
  }

  template <class valueType>
  bool SetAnyValueFromTemplate(std::shared_ptr<next::udex::AnyValue> to, valueType value) {
    next::udex::ValueUnion myUnion;
    switch (to->GetTypeOfAnyValue()) {
    case udex::SignalType::SIGNAL_TYPE_CHAR:
      myUnion.asChar = static_cast<char>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT8:
      myUnion.asUint8 = static_cast<uint8_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT8:
      myUnion.asInt8 = static_cast<int8_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT16:
      myUnion.asUint16 = static_cast<uint16_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT16:
      myUnion.asInt16 = static_cast<int16_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT32:
      myUnion.asUint32 = static_cast<uint32_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT32:
      myUnion.asInt32 = static_cast<int32_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT64:
      myUnion.asUint64 = static_cast<uint64_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT64:
      myUnion.asInt64 = static_cast<int64_t>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_FLOAT:
      myUnion.asFloat = static_cast<float>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_DOUBLE:
      myUnion.asDouble = static_cast<double>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_BOOL:
      myUnion.asBool = static_cast<bool>(value);
      break;
    case udex::SignalType::SIGNAL_TYPE_STRUCT:
    default:
      warn(__FILE__, " Signal type of URL {} is STRUCT, which is unsupported.", to->GetFullUrl());
      return false;
    }
    return to->Set(myUnion);
  }

  bool SetAnyValueFromValueUnion(std::shared_ptr<next::udex::AnyValue> to, next::udex::ValueUnion from,
                                 udex::SignalType from_type) {
    switch (from_type) {
    case udex::SignalType::SIGNAL_TYPE_CHAR:
      SetAnyValueFromTemplate(to, from.asChar);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT8:
      SetAnyValueFromTemplate(to, from.asUint8);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT8:
      SetAnyValueFromTemplate(to, from.asInt8);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT16:
      SetAnyValueFromTemplate(to, from.asUint16);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT16:
      SetAnyValueFromTemplate(to, from.asInt16);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT32:
      SetAnyValueFromTemplate(to, from.asUint32);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT32:
      SetAnyValueFromTemplate(to, from.asInt32);
      break;
    case udex::SignalType::SIGNAL_TYPE_UINT64:
      SetAnyValueFromTemplate(to, from.asUint64);
      break;
    case udex::SignalType::SIGNAL_TYPE_INT64:
      SetAnyValueFromTemplate(to, from.asInt64);
      break;
    case udex::SignalType::SIGNAL_TYPE_FLOAT:
      SetAnyValueFromTemplate(to, from.asFloat);
      break;
    case udex::SignalType::SIGNAL_TYPE_DOUBLE:
      SetAnyValueFromTemplate(to, from.asDouble);
      break;
    case udex::SignalType::SIGNAL_TYPE_BOOL:
      SetAnyValueFromTemplate(to, from.asBool);
      break;
    case udex::SignalType::SIGNAL_TYPE_STRUCT:
      warn(__FILE__, " Signal type of URL {} is STRUCT, which is unsupported.", to->GetFullUrl());
      return false;
    }
    return true;
  }

private:
  std::string client_name_;
  std::string flow_name_;

  std::string sdl_string_;
  next::udex::publisher::DataPublisher publisher_;
  std::string base_control_data_port_name_;

  std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor> tree_extractor;

  char *tree_package_ptr_ = nullptr;
  size_t tree_package_size_ = 0;
  bool is_ready_ = false;

public:
  friend class TreeExtractorPublisherTester;
};

} // namespace orchestrator
} // namespace control
} // namespace next

#endif // NEXT_TREE_EXTRACTOR_PUBLISHER_HPP
