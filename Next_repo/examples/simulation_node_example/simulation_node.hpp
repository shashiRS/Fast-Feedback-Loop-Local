/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_SIMULATION_NODE_HPP
#define NEXT_SIMULATION_NODE_HPP

#include <next/next.hpp>
#include <next/sdk/sdk.hpp>

class SimulationNode : public next::NextComponentInterface {
public:
  SimulationNode(const std::string &name) : next::NextComponentInterface(name) {}
  virtual bool enterConfig() override {
    next::ComponentInputPortConfig inport;
    inport.port_name = "TestPort1";
    inport.sync_mapping = {};
    inport.port_version_number = 1;
    inport.data_type_name = "TestSigHeader";
    inport.port_size = 100;
    SetupInput("", {inport});
    next::ComponentOutputPortConfig outport;
    outport.port_size = 112;
    outport.port_version_number = 1;
    outport.port_name = "View1.Group1";
    std::vector<next::ComponentOutputPortConfig> outports;
    outports.push_back(outport);
    outport.port_name = "View1.Group11";
    outports.push_back(outport);

    SetupOutput({"test_signal.sdl"}, outports);
    auto triggerCallback = [&](std::string &error) { return OnTrigger(error); };
    SetupTriggers({std::make_pair("exec1", triggerCallback)});
    return true;
  }

  int OnTrigger(std::string &) {

    next::sdk::logger::info(__FILE__, "Trigger callback {0}", trigger_counter_++);
    next::DataPackage values = getTriggerAsyncPush("TestPort1");

    size_t offset = 8;
    uint64_t *Timestamp = reinterpret_cast<uint64_t *>((char *)std::get<0>(values) + offset);

    const size_t size_output = 88;
    char dataOut[size_output];
    memset(dataOut, 0, size_output);

    size_t offsetOutput = 20;
    uint64_t *timestamp_out = reinterpret_cast<uint64_t *>(&(dataOut[offsetOutput]));
    *timestamp_out = *Timestamp;

    next::DataPackage output;
    std::get<0>(output) = dataOut;
    std::get<1>(output) = size_output;

    triggerAsyncPush("View1.Group1", output);
    triggerAsyncPush("View1.Group11", output);

    triggerAsyncPush(0x20350000, output);

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

#endif // NEXT_SIMULATION_NODE_HPP
