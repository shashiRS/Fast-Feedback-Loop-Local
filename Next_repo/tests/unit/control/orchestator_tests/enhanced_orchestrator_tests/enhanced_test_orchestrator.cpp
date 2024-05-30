#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>

#include <next/control/orchestrator/orchestrator_client.h>
#include <next/control/orchestrator/orchestrator_master.h>

class OrchestratorTests : public next::sdk::testing::TestUsingEcal {

public:
  void callback(next::control::orchestrator::OrchestratorErrors &error_val, [[maybe_unused]] std::string &err_msg) {
    if (error_val == next::control::orchestrator::OrchestratorErrors::SUCCESS) {
      done_ = true;
      payload_ = orchestrator_client_->extractTriggerInfo(method_name_);
    } else {
      done_ = false;
    }
  }

  void error_callback(next::control::orchestrator::OrchestratorErrors &error_val,
                      [[maybe_unused]] std::string &err_msg) {
    error_val = next::control::orchestrator::OrchestratorErrors::EXEC_FAILED;
    err_msg = "{\"callback_message\":\"something wrong happened\", \"error_code\": -42}";
  }

protected:
  OrchestratorTests() {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_));

    this->instance_name_ = "OrchestratorTests";
    node_name_ = "test_node";
    method_name_ = "test_method";
    eCAL::Util::EnableLoopback(true);

    orchestrator_master_ = std::make_unique<next::control::orchestrator::OrchestratorMaster>();
    orchestrator_client_ = std::make_unique<next::control::orchestrator::OrchestratorTriggerHandler>(node_name_);
    binded_callback_ = std::bind(&OrchestratorTests::callback, this, std::placeholders::_1, std::placeholders::_2);
    binded_error_callback_ =
        std::bind(&OrchestratorTests::error_callback, this, std::placeholders::_1, std::placeholders::_2);
  }

  ~OrchestratorTests() {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_));
    orchestrator_master_.reset();
    orchestrator_client_.reset();
  }

  std::unique_ptr<next::control::orchestrator::OrchestratorMaster> orchestrator_master_;         // client real
  std::unique_ptr<next::control::orchestrator::OrchestratorTriggerHandler> orchestrator_client_; // // server listens

  std::string node_name_, method_name_;
  static constexpr size_t duration_ = 2000;
  std::function<void(next::control::orchestrator::OrchestratorErrors &, std::string &)> binded_callback_;
  std::function<void(next::control::orchestrator::OrchestratorErrors &, std::string &)> binded_error_callback_;
  std::atomic_bool done_ = false;
  next::control::events::OrchestratorTrigger payload_;
};

TEST_F(OrchestratorTests, DISABLED_orchestrator_client_registers_client_invokes) {
  orchestrator_client_->registerHook(method_name_, binded_callback_);
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_));

  auto fire_thread = std::thread([&]() {
    while (!done_) {
      orchestrator_client_->invokeTrigger(node_name_, method_name_, "1000", "1");
      std::this_thread::sleep_for(std::chrono::milliseconds(2 * duration_));
    }
  });

  fire_thread.join();
  EXPECT_EQ(done_, true);
}

TEST_F(OrchestratorTests, orchestrator_client_registers_client_invokes) {
  orchestrator_client_->registerHook(method_name_, binded_callback_);
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_));
  std::string timestamp = "1000", flow_id = "1";
  orchestrator_client_->invokeTrigger(method_name_, timestamp, flow_id);
  EXPECT_EQ(payload_.flow_id, "1");
  EXPECT_EQ(payload_.trigger_timestamp, "1000");
  EXPECT_EQ(done_, true);
}

TEST_F(OrchestratorTests, orchestrator_master_get_trigger_status) {
  using TriggerStatus = next::control::orchestrator::TriggerStatus;

  orchestrator_client_->registerHook(method_name_, binded_callback_);
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_));

  TriggerStatus status_expect_success = orchestrator_master_->fireTrigger(node_name_, method_name_);
  TriggerStatus status_expect_no_resp = orchestrator_master_->fireTrigger("dummy_node", "dummy_method");

  EXPECT_EQ(status_expect_success, TriggerStatus::SUCCESS);
  EXPECT_EQ(status_expect_no_resp, TriggerStatus::NO_RESPONSE);
}

TEST_F(OrchestratorTests, orchestrator_master_get_trigger_status_exec_error) {
  using TriggerStatus = next::control::orchestrator::TriggerStatus;

  orchestrator_client_->registerHook(method_name_, binded_error_callback_);
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_));

  TriggerStatus status_expect_error = orchestrator_master_->fireTrigger(node_name_, method_name_);

  EXPECT_EQ(status_expect_error, TriggerStatus::EXEC_WARN);
}