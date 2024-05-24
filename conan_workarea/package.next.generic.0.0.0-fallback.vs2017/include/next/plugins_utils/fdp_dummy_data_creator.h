#ifndef NEXT_PLUGINS_UTILS_FDP_DUMMY_DATA_CREATOR_H_
#define NEXT_PLUGINS_UTILS_FDP_DUMMY_DATA_CREATOR_H_

#include <fstream>
#include <iterator>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>
#include <next/udex/data_types.hpp>

#include "Platform_Types.h"

namespace next {
namespace plugins_utils {

void createGeneralInfoFile(bridgesdk::plugin_addons::Signals &general_signals_info, std::string file_path) {
  std::ofstream ofs(file_path);
  if (ofs) {

    for (auto &[key, value] : general_signals_info.raw_signal_info) {
      std::string str;
      str += key + "=";
      str += "signal_offset:" + std::to_string(value.signal_offset) + ",";
      str += "payload_size_:" + std::to_string(value.payload_size_) + ",";
      str += "sig_type_:" + std::to_string((int)value.sig_type_) + ",";
      str += "array_length_:" + std::to_string(value.array_length_) + ",";
      str += "can_data:";

      str += "";

      ofs << str;
      ofs << "\n";
    }
  }
}

std::vector<std::string> tokenize(std::string str, std::string delimiter) {
  std::vector<std::string> result;

  while (!str.empty()) {
    auto pos = str.find(delimiter);
    if (pos == std::string::npos)
      break;

    std::string sub_str = str.substr(0, pos);
    str = str.substr(pos + 1, str.length());
    result.push_back(sub_str);
  }
  if (!str.empty())
    result.push_back(str);

  return result;
}

void createGeneralInfoOutofDumpedFile(std::string file_path,
                                      next::bridgesdk::plugin_addons::Signals &general_signals_info) {
  std::ifstream ifs(file_path);
  while (ifs) {
    std::string str;
    ifs >> str;

    if (str.empty())
      return;
    std::vector<std::string> key_value = tokenize(str, "=");
    std::pair<std::string, next::bridgesdk::plugin_addons::RawSignalInfo> raw_signal;

    next::bridgesdk::plugin_addons::RawSignalInfo raw_signal_info{
        0, 0, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL, 0};

    std::vector<std::string> properties_commas = tokenize(key_value[1], ",");

    for (auto property : properties_commas) {
      std::vector<std::string> property_name_value = tokenize(property, ":");
      if (property_name_value[0] == "signal_offset") {
        raw_signal_info.signal_offset = std::stoull(property_name_value[1]);
      } else if (property_name_value[0] == "payload_size_") {
        raw_signal_info.payload_size_ = std::stoull(property_name_value[1]);
      } else if (property_name_value[0] == "sig_type_") {
        raw_signal_info.sig_type_ = (next::bridgesdk::plugin_addons::SignalType)std::stoi(property_name_value[1]);
      } else if (property_name_value[0] == "array_length_") {
        raw_signal_info.array_length_ = std::stoull(property_name_value[1]);
      }
    }

    raw_signal.first = key_value[0];
    raw_signal.second = raw_signal_info;

    general_signals_info.raw_signal_info.insert(raw_signal);
  }
}

void createGeneralInfoOutofDumpedFile(std::string file_path, next::udex::Signal &general_signals_info) {
  std::ifstream ifs(file_path);
  while (ifs) {
    std::string str;
    ifs >> str;

    if (str.empty())
      return;
    std::vector<std::string> key_value = tokenize(str, "=");
    std::pair<std::string, next::udex::PayloadType> raw_signal;

    next::udex::PayloadType raw_signal_info{0, next::udex::SignalType::SIGNAL_TYPE_BOOL, 0, 0};

    std::vector<std::string> properties_commas = tokenize(key_value[1], ",");

    for (auto property : properties_commas) {
      std::vector<std::string> property_name_value = tokenize(property, ":");
      if (property_name_value[0] == "signal_offset") {
        raw_signal_info.start_offset_ = std::stoull(property_name_value[1]);
      } else if (property_name_value[0] == "payload_size_") {
        raw_signal_info.payload_size_ = std::stoull(property_name_value[1]);
      } else if (property_name_value[0] == "sig_type_") {
        raw_signal_info.sig_type_ = (next::udex::SignalType)std::stoi(property_name_value[1]);
      } else if (property_name_value[0] == "array_length_") {
        raw_signal_info.array_length_ = std::stoull(property_name_value[1]);
      }
    }

    raw_signal.first = key_value[0];
    raw_signal.second = raw_signal_info;
    general_signals_info.data_.insert(raw_signal);
  }
}

void dumpPayLoadAsFile(const next::bridgesdk::ChannelMessagePointers *datablock, std::string file_path) {
  if (datablock->size <= 0) {
    warn(__FILE__, " datablock is empty, no file would be created");
    return;
  }

  std::ofstream ofs(file_path);
  if (ofs) {
    const size_t count = datablock->size / sizeof(int);
    const int *in = reinterpret_cast<const int *>(datablock->data);
    std::vector<int> ints(in, in + count);

    std::ostream_iterator<int> output_iterator(ofs, "\n");
    std::copy(ints.begin(), ints.end(), output_iterator);
  }
}

void createVectorOutOfDumpedPayloadFile(std::string file_path, std::vector<int> &result) {
  std::ifstream ifs(file_path);
  while (ifs) {
    int d = 0;
    ifs >> d;
    result.push_back(d);
  }
}

} // namespace plugins_utils
} // namespace next

#endif // NEXT_PLUGINS_UTILS_FDP_DUMMY_DATA_CREATOR_H_
