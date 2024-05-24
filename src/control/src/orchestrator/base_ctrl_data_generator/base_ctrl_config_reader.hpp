#ifndef NEXT_BASE_CTRL_CONFIG_READER_HPP
#define NEXT_BASE_CTRL_CONFIG_READER_HPP

#include <vector>

#include <boost/lexical_cast.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace control {
namespace orchestrator {

enum class SignalType { connected = 0, fixed_values = 1 };
struct MemberDataInfo {
  std::string control_data_url_name; // SigHeader.uiTimeStamp
  SignalType type;
  std::string connection_name;          // if fixed, empty string,
  std::vector<float> fixed_value_array; // if connected, array empty
};

// should be for each execution of the component
struct TriggerDataInfo {
  std::string component_name;       // some_component
  std::string execution_name;       // runSequencer_response
  std::vector<MemberDataInfo> info; // should be vector
};

struct ControlDataInfo {
  std::string flow_name;
  std::string sdl_file;
  std::vector<TriggerDataInfo> contol_data_urls;
};

class BaseDataReadOutConfig {
public:
  static ControlDataInfo CreateInfoFromFlowId(const std::string &flow_id) {

    ControlDataInfo control_data_info;

    auto sdl_file = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::orchestrator::getControlDataSdl(flow_id), "");

    auto fallback_trigger_info = CreateFallbackTriggerDataInfoFromFlowId(flow_id);

    control_data_info.flow_name = flow_id;
    control_data_info.sdl_file = sdl_file;

    auto components_list = next::appsupport::ConfigClient::getConfig()->getChildren(
        next::appsupport::configkey::orchestrator::getControlDataComponents(flow_id));

    for (auto component : components_list) {
      std::string key = "next_player:orchestrator:" + flow_id + ":clients:" + component;
      auto execution_name_list = next::appsupport::ConfigClient::getConfig()->getChildren(key);

      for (auto execution_name : execution_name_list) {
        auto trigger_data_info = CreateTriggerDataInfo(flow_id, component, execution_name);

        MergeFallbackIntoTriggerDataInfo(fallback_trigger_info, trigger_data_info);
        control_data_info.contol_data_urls.push_back(trigger_data_info);
      }
    }

    return control_data_info;
  }

public:
  friend class BaseDataReadOutConfigTester;

private:
  static TriggerDataInfo CreateFallbackTriggerDataInfoFromFlowId(const std::string &flow_id) {
    TriggerDataInfo trigger_data_info;
    trigger_data_info.component_name = "FallbackComponent";
    trigger_data_info.execution_name = "FallbackExecution";

    auto member_list = next::appsupport::ConfigClient::getConfig()->getChildren(
        next::appsupport::configkey::orchestrator::getControlDataMembers(flow_id));

    for (auto member : member_list) {
      auto key = next::appsupport::configkey::orchestrator::getControlDataFallbackMemberData(flow_id, member);
      auto member_info = CreateMemberInfo(member, key);
      trigger_data_info.info.push_back(member_info);
    }

    return trigger_data_info;
  }

  static TriggerDataInfo CreateTriggerDataInfo(const std::string &flow_id, const std::string &component,
                                               const std::string &execution_name) {
    TriggerDataInfo trigger_data_info;
    trigger_data_info.component_name = component;
    trigger_data_info.execution_name = execution_name;

    auto member_list = next::appsupport::ConfigClient::getConfig()->getChildren(
        next::appsupport::configkey::orchestrator::getControlDataMembersForComponent(flow_id, component,
                                                                                     execution_name));

    for (auto member : member_list) {
      auto key = next::appsupport::configkey::orchestrator::getSpecificControlDataMembersForComponent(
          flow_id, component, execution_name, member);
      auto member_info = CreateMemberInfo(member, key);
      trigger_data_info.info.push_back(member_info);
    }

    return trigger_data_info;
  }

  static MemberDataInfo CreateMemberInfo(const std::string &member, const std::string &key) {
    MemberDataInfo member_data_info;

    member_data_info.control_data_url_name = member;

    auto is_list = next::appsupport::ConfigClient::getConfig()->isList(key);

    if (is_list) {
      auto values_list = next::appsupport::ConfigClient::getConfig()->getStringList(key, {});
      AddFixedValuesToMemberData(member, member_data_info, values_list);
    } else {
      auto value = next::appsupport::ConfigClient::getConfig()->get_string(key, "");
      if (value.empty()) {
        AddFixedValuesToMemberData(member, member_data_info, {});
      } else if (IsNumber(value)) {
        AddFixedValuesToMemberData(member, member_data_info, {value});
      } else {
        AddConnectedSignalToMemberData(member_data_info, value);
      }
    }

    return member_data_info;
  }

  static void AddFixedValuesToMemberData(const std::string &member, MemberDataInfo &member_data_info,
                                         const std::vector<std::string> &values_list) {
    std::string faulty_fixed_values = "";
    bool proper_fixed_values = true;

    member_data_info.type = SignalType::fixed_values;
    member_data_info.connection_name = "";

    for (const auto &value : values_list) {
      try {
        member_data_info.fixed_value_array.push_back(boost::lexical_cast<float>(value));
      } catch (...) {
        faulty_fixed_values += value + " ";
        proper_fixed_values = false;
      }
    }

    if (member_data_info.fixed_value_array.empty()) {
      member_data_info.fixed_value_array.push_back(0.f);
    }

    if (!proper_fixed_values) {
      warn(__FILE__, "Faulty fixed value detected in member: \"{0}\" [ {1}]", member, faulty_fixed_values);
    }
  }

  static void AddConnectedSignalToMemberData(MemberDataInfo &member_data_info, const std::string &connection_name) {
    member_data_info.type = SignalType::connected;
    member_data_info.connection_name = connection_name;
    member_data_info.fixed_value_array = {};
  }

  static void MergeFallbackIntoTriggerDataInfo(const TriggerDataInfo &fallback, TriggerDataInfo &current) {
    std::vector<MemberDataInfo> temp;

    for (auto member : fallback.info) {
      bool member_found = false;

      for (auto current_member : current.info) {
        if (member.control_data_url_name == current_member.control_data_url_name) {
          member_found = true;
          break;
        }
      }

      if (!member_found) {
        temp.push_back(member);
      }
    }

    current.info.insert(current.info.end(), temp.begin(), temp.end());
  }

  static bool IsNumber(const std::string &value) {
    for (const auto &chr : value) {
      if (!(isdigit(chr) || chr == '.')) {
        return false;
      }
    }

    return true;
  }
};

} // namespace orchestrator
} // namespace control
} // namespace next

#endif // NEXT_BASE_CTRL_CONFIG_READER_HPP
