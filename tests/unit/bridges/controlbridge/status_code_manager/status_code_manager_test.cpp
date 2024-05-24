#include "status_code_manager.hpp"
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <json/json.h>
#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>
#include <thread>
#include "web_server_mock.hpp"

namespace next {
namespace controlbridge {

std::string CreateJsonString(const next::sdk::events::StatusCodeMessage &status_event, std::vector<UiAction> &actions) {
  Json::Value root;
  root["channel"] = "console";
  root["source"] = "NextBridge";
  root["event"] = "backendNotification";

  Json::Value status_event_json;
  status_event_json["status_code"] = status_event.toInt();
  status_event_json["severity"] = status_event.getSeverityAsString();
  status_event_json["status_origin"] = next::sdk::events::StatusCodeEvent::getStatusOriginString(status_event);
  status_event_json["status_component_name"] = status_event.component_name;
  status_event_json["status_message"] = status_event.message;

  for (auto &action : actions) {
    Json::Value action_json;
    action_json["ui_action"] = action.getUiActionTypeString();
    action_json["is_requested"] = action.is_requested;

    status_event_json["ui_action_request"].append(action_json);
  }

  root["payload"]["StatusEventData"].append(status_event_json);

  Json::FastWriter writer;
  return writer.write(root);
}

TEST(StatusCodeManagerTest, Send_StatusCode_Single) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  StatusCodeManager status_code_manger("StatusCodeManagerTest");
  status_code_manger.SetWebserver(webserver_mock);

  sdk::events::StatusCodeMessage status_code_data;
  status_code_data.component_name = "ComponentName";
  status_code_data.origin = sdk::events::StatusOrigin::DEFAULT;
  status_code_data = 1111;
  status_code_data.message = "StatusMessage";

  std::vector<UiAction> actions;

  std::vector<uint8_t> json_payload;
  status_code_manger.CreateJsonMessagePayload(status_code_data, actions, json_payload);
  status_code_manger.SendMessageWithWebserver(json_payload);

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";

  std::string send_data = "";
  for (auto &elem : webserver_mock->data_) {
    send_data.append(elem.begin(), elem.end());
  }
  std::string expected_data = CreateJsonString(status_code_data, actions);

  EXPECT_EQ(expected_data, send_data);
}

TEST(StatusCodeManagerTest, Send_StatusCode_Multi) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  StatusCodeManager status_code_manger("StatusCodeManagerTest");
  status_code_manger.SetWebserver(webserver_mock);

  sdk::events::StatusCodeMessage status_code_data;
  status_code_data.component_name = "ComponentName";
  status_code_data.origin = sdk::events::StatusOrigin::DEFAULT;
  status_code_data = 1111;
  status_code_data.message = "StatusMessage";

  std::vector<UiAction> actions;

  std::vector<uint8_t> json_payload;
  status_code_manger.CreateJsonMessagePayload(status_code_data, actions, json_payload);
  status_code_manger.SendMessageWithWebserver(json_payload);

  ASSERT_EQ(webserver_mock->data_.size(), 1) << "No data received";

  std::string send_data = "";
  for (auto &elem : webserver_mock->data_) {
    send_data.append(elem.begin(), elem.end());
  }
  std::string expected_data;
  expected_data = CreateJsonString(status_code_data, actions);

  EXPECT_EQ(expected_data, send_data);

  webserver_mock->data_.clear();
  send_data.clear();

  status_code_data.component_name = "ComponentName2";
  status_code_data.origin = sdk::events::StatusOrigin::NEXT_CONTROLBRIDGE;
  status_code_data = 2222;
  status_code_data.message = "StatusMessage2";
  json_payload.clear();
  status_code_manger.CreateJsonMessagePayload(status_code_data, actions, json_payload);
  status_code_manger.SendMessageWithWebserver(json_payload);

  ASSERT_EQ(webserver_mock->data_.size(), 1) << "No data received";

  for (auto &elem : webserver_mock->data_) {
    send_data.append(elem.begin(), elem.end());
  }
  expected_data = CreateJsonString(status_code_data, actions);

  EXPECT_EQ(expected_data, send_data);
}

TEST(StatusCodeManagerTest, Send_StatusCode_Action) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  StatusCodeManager status_code_manger("StatusCodeManagerTest");
  status_code_manger.SetWebserver(webserver_mock);

  sdk::events::StatusCodeMessage status_code_data;
  status_code_data.component_name = "ComponentName";
  status_code_data.origin = sdk::events::StatusOrigin::DEFAULT;
  status_code_data = 1111;
  status_code_data.message = "StatusMessage";

  std::vector<UiAction> actions;
  actions.push_back({UiActionType::PlayWarning, true});

  std::vector<uint8_t> json_payload;
  status_code_manger.CreateJsonMessagePayload(status_code_data, actions, json_payload);
  status_code_manger.SendMessageWithWebserver(json_payload);

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";

  std::string send_data = "";
  for (auto &elem : webserver_mock->data_) {
    send_data.append(elem.begin(), elem.end());
  }
  std::string expected_data = CreateJsonString(status_code_data, actions);

  EXPECT_EQ(expected_data, send_data);
}

TEST(StatusCodeManagerTest, Send_StatusCode_MultiAction) {
  auto webserver_mock = std::make_shared<next::test::WebServerMock>();

  StatusCodeManager status_code_manger("StatusCodeManagerTest");
  status_code_manger.SetWebserver(webserver_mock);

  sdk::events::StatusCodeMessage status_code_data;
  status_code_data.component_name = "ComponentName";
  status_code_data.origin = sdk::events::StatusOrigin::DEFAULT;
  status_code_data = 1111;
  status_code_data.message = "StatusMessage";

  std::vector<UiAction> actions;
  actions.push_back({UiActionType::PlayWarning, true});
  actions.push_back({UiActionType::ExporterWarning, true});
  actions.push_back({UiActionType::VisualNotification, true});

  std::vector<uint8_t> json_payload;
  status_code_manger.CreateJsonMessagePayload(status_code_data, actions, json_payload);
  status_code_manger.SendMessageWithWebserver(json_payload);

  ASSERT_EQ(1, webserver_mock->data_.size()) << "No data received or too many data received";

  std::string send_data = "";
  for (auto &elem : webserver_mock->data_) {
    send_data.append(elem.begin(), elem.end());
  }
  std::string expected_data = CreateJsonString(status_code_data, actions);

  EXPECT_EQ(expected_data, send_data);
}

class StatusCodeManagerTester : public StatusCodeManager {
public:
  StatusCodeManagerTester(const std::string &component_name) : StatusCodeManager(component_name) { InitEvaluators(); };

  std::vector<next::controlbridge::Condition> GetEvaluatorsConditionsTest() {
    return status_queue_manager_.getConditions();
  }

  std::vector<UiAction> ProcessStatusCodeTest(next::sdk::events::StatusCodeMessage status_code) {
    std::vector<UiAction> ui_actions;
    ProcessMessageToActions(status_code, ui_actions);
    return ui_actions;
  }

  std::vector<UiAction> FilterUniqueActionsTest(std::vector<UiAction> &actions) { return FilterUniqueActions(actions); }
};

bool isConditionInList(const next::controlbridge::Condition &targetCondition,
                       const std::vector<next::controlbridge::Condition> &conditions) {
  return std::any_of(conditions.begin(), conditions.end(), [&targetCondition](const next::controlbridge::Condition &c) {
    return c.isEqual(targetCondition);
  });
}
TEST(StatusCodeManagerTest, validate_conditions) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");

  auto conditions = status_code_manger_tester.GetEvaluatorsConditionsTest();
  EXPECT_EQ(conditions.size(), 2);

  CheckAnySimulationNodeHasWarning node_evaluator;
  auto node_evaluator_condition = node_evaluator.ProvideCondition();
  CheckExporterHasWarning exporter_evaluator;
  auto exporter_evaluator_condition = exporter_evaluator.ProvideCondition();

  EXPECT_TRUE(isConditionInList(node_evaluator_condition, conditions));
  EXPECT_TRUE(isConditionInList(exporter_evaluator_condition, conditions));
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_CheckActions_exporter_warning) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{341, "exporter1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_TRUE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{341, "exporter2", "test_message"});
  ASSERT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{241, "exporter1", "test_message"});
  ASSERT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{241, "exporter2", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_CheckActions_play_warning_false) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component2", "test_message"});
  ASSERT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component2", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_CheckActions_resulting_2_actions) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{341, "exporter1", "test_message"});
  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{341, "exporter2", "test_message"});
  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{241, "exporter1", "test_message"});
  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{241, "exporter2", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::ExporterWarning);
  EXPECT_FALSE(actions[0].is_requested);

  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component2", "test_message"});
  status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component2", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_1action_no_actions) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{2211, "next_databridge", "[DataBridge] Init finished"});
  EXPECT_EQ(actions.size(), 0);
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_1action_no_actions_1action) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{2211, "next_databridge", "[DataBridge] Init finished"});
  EXPECT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component2", "test_message"});
  ASSERT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);
}

TEST(StatusCodeManagerTest, ProcessStatusCodes_no_spam) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");
  std::vector<UiAction> actions;

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_TRUE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5214, "SIM_NODE_component1", "test_message"});
  EXPECT_EQ(actions.size(), 0);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  ASSERT_EQ(actions.size(), 1);
  EXPECT_EQ(actions[0].type, UiActionType::PlayWarning);
  EXPECT_FALSE(actions[0].is_requested);

  actions = status_code_manger_tester.ProcessStatusCodeTest(
      next::sdk::events::StatusCodeMessage{5212, "SIM_NODE_component1", "test_message"});
  EXPECT_EQ(actions.size(), 0);
}

TEST(StatusCodeManagerTest, filter_unique_actions_true) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");

  std::vector<UiAction> actions;
  actions.push_back({UiActionType::PlayWarning, true});
  actions.push_back({UiActionType::PlayWarning, false});
  actions.push_back({UiActionType::ExporterWarning, true});
  actions.push_back({UiActionType::ExporterWarning, false});
  actions.push_back({UiActionType::VisualNotification, true});
  actions.push_back({UiActionType::VisualNotification, false});

  auto unique_actions = status_code_manger_tester.FilterUniqueActionsTest(actions);
  ASSERT_EQ(unique_actions.size(), 3);

  for (auto &action : unique_actions) {
    EXPECT_TRUE(action.is_requested);
  }
}

TEST(StatusCodeManagerTest, filter_unique_actions_false) {
  StatusCodeManagerTester status_code_manger_tester("StatusCodeManagerTest");

  std::vector<UiAction> actions;
  actions.push_back({UiActionType::PlayWarning, false});
  actions.push_back({UiActionType::PlayWarning, false});
  actions.push_back({UiActionType::ExporterWarning, false});
  actions.push_back({UiActionType::ExporterWarning, false});
  actions.push_back({UiActionType::VisualNotification, false});
  actions.push_back({UiActionType::VisualNotification, false});

  auto unique_actions = status_code_manger_tester.FilterUniqueActionsTest(actions);
  ASSERT_EQ(unique_actions.size(), 3);
  for (auto &action : unique_actions) {
    EXPECT_FALSE(action.is_requested);
  }
}

TEST(StatusCodeManagerTest, int_to_status_code) {
  next::sdk::events::StatusCodeMessage status_code(1234);

  EXPECT_EQ(status_code.code_counter, 34) << "Code counter not ok";
  EXPECT_EQ(status_code.origin, next::sdk::events::StatusOrigin::DEFAULT) << "Origin not ok";
  EXPECT_EQ(status_code.severity, next::sdk::events::StatusSeverity::SEV_SUCCESS) << "Severity not ok";
}

TEST(StatusCodeManagerTest, status_code_to_int) {
  next::sdk::events::StatusCodeMessage status_code;
  status_code.severity = next::sdk::events::StatusSeverity::SEV_ERROR;
  status_code.origin = next::sdk::events::StatusOrigin::NEXT_DATABRIDGE;
  status_code.code_counter = 42;

  int result = status_code.toInt();
  int origin = result / 1000;       // first digit
  int severity = result / 100 % 10; // second digit
  int code_counter = result % 100;  // last 2 digits

  EXPECT_EQ(severity, static_cast<int>(next::sdk::events::StatusSeverity::SEV_ERROR)) << "Severity not ok";
  EXPECT_EQ(origin, static_cast<int>(next::sdk::events::StatusOrigin::NEXT_DATABRIDGE)) << "Origin not ok";
  EXPECT_EQ(code_counter, status_code.code_counter) << "Code counter not ok ";
}

TEST(StatusCodeManagerTest, status_code_init) {
  next::sdk::events::StatusCodeMessage status_code_1;
  next::sdk::events::StatusCodeMessage status_code_2(1234);
  next::sdk::events::StatusCodeMessage status_code_3 = 1234;
  next::sdk::events::StatusCodeMessage status_code_4 = 12345;
  next::sdk::events::StatusCodeMessage status_code_5(123, "test_comp", "test_msg");
  next::sdk::events::StatusCodeMessage status_code_6(42, "comp_with_invalid_code", "test_msg");
  next::sdk::events::StatusCodeMessage status_code_7(9999, "test_comp", "test_msg");

  EXPECT_TRUE(status_code_2 == status_code_3);
  EXPECT_TRUE(status_code_1 == status_code_4);

  EXPECT_EQ(status_code_5.origin, next::sdk::events::StatusOrigin::DEFAULT);
  EXPECT_EQ(status_code_5.severity, next::sdk::events::StatusSeverity::SEV_INFO);
  EXPECT_EQ(status_code_5.code_counter, 23);
  EXPECT_EQ(status_code_5.component_name, "test_comp");
  EXPECT_EQ(status_code_5.message, "test_msg");

  EXPECT_EQ(status_code_6.origin, next::sdk::events::StatusOrigin::DEFAULT);
  EXPECT_EQ(status_code_6.severity, next::sdk::events::StatusSeverity::SEV_DEFAULT);
  EXPECT_EQ(status_code_6.code_counter, 0);
  EXPECT_EQ(status_code_6.component_name, "comp_with_invalid_code");
  EXPECT_EQ(status_code_6.message, "test_msg");

  // this is a status code created with invalid code, expect defaults for origin and sev
  EXPECT_EQ(status_code_7.origin, next::sdk::events::StatusOrigin::DEFAULT);
  EXPECT_EQ(status_code_7.severity, next::sdk::events::StatusSeverity::SEV_DEFAULT);
  EXPECT_EQ(status_code_7.code_counter, 99);
  EXPECT_EQ(status_code_7.component_name, "test_comp");
  EXPECT_EQ(status_code_7.message, "test_msg");
}

TEST(StatusCodeManagerTest, status_code_severity_string) {
  next::sdk::events::StatusCodeMessage status_code_sev_default;
  next::sdk::events::StatusCodeMessage status_code_sev_info(1134);
  next::sdk::events::StatusCodeMessage status_code_sev_success = 1234;
  next::sdk::events::StatusCodeMessage status_code_sev_warn = 1345;
  next::sdk::events::StatusCodeMessage status_code_sev_err = 1445;

  EXPECT_EQ(status_code_sev_default.getSeverityAsString(), "Default");
  EXPECT_EQ(status_code_sev_info.getSeverityAsString(), "Info");
  EXPECT_EQ(status_code_sev_success.getSeverityAsString(), "Success");
  EXPECT_EQ(status_code_sev_warn.getSeverityAsString(), "Warning");
  EXPECT_EQ(status_code_sev_err.getSeverityAsString(), "Error");
}

TEST(StatusCodeManagerTest, get_status_code) {
  next::sdk::events::StatusCodeMessage status_code_1;
  next::sdk::events::StatusCodeMessage status_code_2(1134);
  next::sdk::events::StatusCodeMessage status_code_3 = 1234;
  next::sdk::events::StatusCodeMessage status_code_4 = 1345;
  next::sdk::events::StatusCodeMessage status_code_5 = 1445;
  next::sdk::events::StatusCodeMessage status_code_6 = 9999;

  EXPECT_EQ(status_code_1.getStatusCode(), 0);
  EXPECT_EQ(status_code_2.getStatusCode(), 134);
  EXPECT_EQ(status_code_3.getStatusCode(), 234);
  EXPECT_EQ(status_code_4.getStatusCode(), 345);
  EXPECT_EQ(status_code_5.getStatusCode(), 445);
  EXPECT_EQ(status_code_6.getStatusCode(), 99);
}

} // namespace controlbridge
} // namespace next
