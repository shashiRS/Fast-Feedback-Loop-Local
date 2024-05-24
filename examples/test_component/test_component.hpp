/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_TEST_COMPONENT_HPP
#define NEXT_TEST_COMPONENT_HPP

#include <next/next.hpp>
#include <next/sdk/sdk.hpp>

class TestComponent : public next::NextComponentInterface {
public:
  TestComponent(const std::string &name) : next::NextComponentInterface(name) {}

  virtual bool enterConfig() override {
    std::vector<next::ComponentInputPortConfig> input_ports;

    // A,B *sync types = 6
    next::ComponentInputPortConfig nearest_A;
    nearest_A.port_name = "test_type_A_sync_nearest";
    nearest_A.sync_mapping = {};
    nearest_A.port_version_number = 1;
    nearest_A.data_type_name = "TestSigHeader";
    nearest_A.port_size = 28;
    input_ports.push_back(nearest_A);

    next::ComponentInputPortConfig nearest_B;
    nearest_B.port_name = "test_type_B_sync_nearest";
    nearest_B.sync_mapping = {};
    nearest_B.port_version_number = 1;
    nearest_B.data_type_name = "TestSigHeader";
    nearest_B.port_size = 28;
    input_ports.push_back(nearest_B);

    next::ComponentInputPortConfig exact_A;
    exact_A.port_name = "test_type_A_sync_exact";
    exact_A.sync_mapping = {};
    exact_A.port_version_number = 1;
    exact_A.data_type_name = "TestSigHeader";
    exact_A.port_size = 28;
    input_ports.push_back(exact_A);

    next::ComponentInputPortConfig exact_B;
    exact_B.port_name = "test_type_B_sync_exact";
    exact_B.sync_mapping = {};
    exact_B.port_version_number = 1;
    exact_B.data_type_name = "TestSigHeader";
    exact_B.port_size = 28;
    input_ports.push_back(exact_B);

    next::ComponentInputPortConfig latest_A;
    latest_A.port_name = "test_type_A_sync_latest";
    latest_A.sync_mapping = {};
    latest_A.port_version_number = 1;
    latest_A.data_type_name = "TestSigHeader";
    latest_A.port_size = 28;
    input_ports.push_back(latest_A);

    next::ComponentInputPortConfig latest_B;
    latest_B.port_name = "test_type_B_sync_latest";
    latest_B.sync_mapping = {};
    latest_B.port_version_number = 1;
    latest_B.data_type_name = "TestSigHeader";
    latest_B.port_size = 28;
    input_ports.push_back(latest_B);

    SetupInput("", input_ports);

    SetupOutput({"test_component.sdl"}, {});
    auto triggerCallback = [&](std::string &error) { return OnTrigger(error); };
    SetupTriggers({std::make_pair("exec1", triggerCallback)});
    return true;
  }

  int OnTrigger(std::string &) {

    next::sdk::logger::info(__FILE__, "Trigger callback {0}", trigger_counter_++);

    // use truct SignalHeader_64, struct generic_signal_64, struct Package from other PR insted of offsets

    next::DataPackage sync_nearest_A = getTriggerAsyncPush("test_type_A_sync_nearest");
    next::DataPackage sync_nearest_B = getTriggerAsyncPush("test_type_B_sync_nearest");
    next::DataPackage sync_exact_A = getTriggerAsyncPush("test_type_A_sync_exact");
    next::DataPackage sync_exact_B = getTriggerAsyncPush("test_type_B_sync_exact");
    next::DataPackage sync_latest_A = getTriggerAsyncPush("test_type_A_sync_latest");
    next::DataPackage sync_latest_B = getTriggerAsyncPush("test_type_B_sync_latest");

    triggerAsyncPush("out.test_type_A_sync_nearest", sync_nearest_A);
    triggerAsyncPush("out.test_type_B_sync_nearest", sync_nearest_B);
    triggerAsyncPush("out.test_type_A_sync_exact", sync_exact_A);
    triggerAsyncPush("out.test_type_B_sync_exact", sync_exact_B);
    triggerAsyncPush("out.test_type_A_sync_latest", sync_latest_A);
    triggerAsyncPush("out.test_type_B_sync_latest", sync_latest_B);

    return 0;
  }

  virtual bool enterReset() override {
    trigger_counter_ = 0;
    return true;
  }

  virtual bool enterInit() override { return true; }
  virtual bool enterShutdown() override { return true; }
  virtual bool onError(std::string error, std::exception *e = nullptr) override {

    next::sdk::logger::error(__FILE__, "error {0}, {1}", error, e->what());
    return true;
  }

private:
  int trigger_counter_ = 0;
};

#endif // NEXT_TEST_COMPONENT_HPP
