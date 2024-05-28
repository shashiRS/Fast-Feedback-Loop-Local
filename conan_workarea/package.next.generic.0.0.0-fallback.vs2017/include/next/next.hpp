/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next.hpp
 * @brief    Provide all necessary functions from the Next Simulation Framework
 *
 * Include this file if you want to use Next in your project.
 *
 * If only specific parts are needed, it is sufficient to only use the corresponding include file.
 *
 */

#ifndef NEXT_H_
#define NEXT_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <next/sdk/sdk_macros.h>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/lifecycle/lifecycle_base.hpp>

#include <next/control/orchestrator/orchestrator_client.h>

#include "data_types.hpp"
#include "next_callbacks.hpp"
#include "next_config.h"

namespace next {
// TODO define what return values are accepted
typedef std::function<int(std::string &)> triggerCallback;

typedef std::tuple<void *, size_t> DataPackage;

struct ComponentInputPortConfig {
  std::string port_name;
  uint32_t port_version_number;
  size_t port_size;
  std::string data_type_name;
  SyncMapping sync_mapping;
};

struct ComponentOutputPortConfig {
  std::string port_name;
  uint32_t port_version_number;
  size_t port_size;
};

struct SimulationStatus {
  std::string rec_name;
};

class NextComponentInterfaceImpl;

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
// disable "non dll-interface class ... used as base for dll-interface class ..." warning
// this is because of usage of LifecycleBase, which in turn uses shared pointer etc.
NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4275)
class DECLSPEC_next NextComponentInterface : public next::NextCallbacks,
                                             public next::appsupport::LifecycleBase,
                                             public next::appsupport::CmdOptions {
  NEXT_RESTORE_WARNINGS_WINDOWS
public:
  NextComponentInterface() = delete;
  NextComponentInterface(const std::string &component_name);
  virtual ~NextComponentInterface();

  /*!
   * @brief Setup function used to define all Input Ports of the component
   *
   * @param input_ports vector of Input ports that belong to the component
   * @return size_t Subscriber ID of the port
   */
  virtual int SetupInput(const std::string &adapter_dll_path, const std::vector<ComponentInputPortConfig> &input_ports);

  virtual uint32_t ConvertVersionNumber(const std::string &version);

  /*!
   * @brief Get the Input Ports object
   *
   * @return std::vector<ComponentInputPortConfig>
   */
  std::vector<ComponentInputPortConfig> getInputPorts();

  /*!
   * @brief Setup function used to define all Output Ports of the component
   *
   *
   * @param filename sdl file with port definitions
   * @param output_ports vector of Output ports that belong to the component
   * @return int 0 if successful, error code if failed
   */
  virtual int SetupOutput(const std::vector<std::string> &sdl_filepath,
                          const std::vector<ComponentOutputPortConfig> &output_ports);

  /*!
   * @brief Get the Output Ports object
   *
   * @return std::vector<ComponentOutputPortConfig>
   */
  std::vector<ComponentOutputPortConfig> getOutputPorts();

  /*!
   * @brief Setup function used to define all triggers used for the component
   *
   * @param triggers vector of std::string  triggername and function triggerCallback
   * @return int 0 if successful, error code if failed
   */
  virtual int SetupTriggers(const std::vector<std::pair<std::string, triggerCallback>> &triggers);

  /*!
   * @brief Setup function to define the sync method for the ports of the component
   *
   * @param sync_information vector of std::string portname and SyncConfig for Sync Method and Sync Value
   * @example SetupSyncref("m_SrrFreespaceInfo0", {{"SRR520.syncRef.sig_m_SrrFreespaceVertexesOctupleTimestamp[39]",
   *            "uiTimeStamp_FrSp"},{"SRR520.syncRef.sig_m_SrrFreespaceVertexesOctupleSyncRef[39]","E2E_SeqCounter_FrSp"})
   * @return int
   */
  virtual int SetupSyncref(const std::vector<std::pair<std::string, SyncMapping>> &syncref_mapping);

  /*!
   * @brief Get the available data for the port
   *
   * @param input_port_name port name to get data for
   * @return std::tuple<void *, size_t> Pointer and size to the available data
   */
  virtual DataPackage getTriggerAsyncPush(const std::string &input_port_name);

  /*!
   * @brief Checks if the Data for the input port is available
   *
   * @param input_port_name port name to check if data is available
   * @return true data is available
   * @return false data is not available
   */
  virtual bool isDataAvailable(const std::string &input_port_name);

  /*!
   * @brief publishes data on the output port of the component
   *
   * @param ouput_port_name port name to publish data on
   * @param data data that should be published
   * @param append_device
   */
  virtual void triggerAsyncPush(std::string output_port_name, DataPackage data, bool append_device = true);

  /*!
   * @brief publishes data on a virtual address
   *
   * @param virtual_address address to publish data on
   * @param data data that should be published
   * @param append_device
   */
  virtual void triggerAsyncPush(const uint64_t virtual_address, DataPackage data, bool append_device = true);

  /*!
   * @brief get the status of the simulation
   *
   * @return append_device
   */
  next::SimulationStatus getStatus();

  /*!
   * @brief returns current time in microseconds
   */
  virtual uint64_t getSystemTimeMicroseconds(void);

  /*!
   * @brief checking whether the port was updated with data from the queue
   *
   * @param port_name the name of the port which is checked
   * @return bool flag indicating if the port has been updated with new data from the queue
   */
  virtual bool isUpdated(const std::string &port_name);

  /*!
   * @brief invokes the remote method of the component
   *
   * @param trigger contains data about the node name and method to be called
   * @param timestamp time stamp sent along with the payload
   * @param flow_id id of the payload to be sent over
   * @return list of trigger responses
   */
  virtual std::list<TriggerReturn> invokeTrigger(Trigger &trigger, const std::string &timestamp,
                                                 const std::string &flow_id);

  /*!
   * @brief invokes the remote method of the component using the timestamp and flow_id of the received trigegr
   *
   * @param trigger contains data about the node name and method to be called
   * @return list of trigger responses
   */
  virtual std::list<TriggerReturn> invokeTrigger(Trigger &trigger);

private:
  bool userEnterReset() final;
  bool userEnterConfigOutput() final;
  bool userEnterConfigInput() final;

  void OnTriggerCallback(next::control::orchestrator::OrchestratorErrors &err, std::string &err_msg,
                         const std::string &method_name, const triggerCallback &client_callback);
  std::unique_ptr<NextComponentInterfaceImpl> impl_;

public:
  friend class NextComponentInterfaceTester;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace next

#endif // NEXT_H_
