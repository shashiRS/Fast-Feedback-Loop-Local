/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_impl.hpp
 * @brief    Provide all functions implementations for the Next Simulation Framework
 *
 * Include this file to use Next implementaion in your project.
 *
 * this file hides implementaion of the next interface functions from outside
 *
 */

#ifndef NEXT_IMPL_H_
#define NEXT_IMPL_H_

#include <functional>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

#include <next/control/orchestrator/orchestrator_client.h>
#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/lifecycle/lifecycle_base.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/appsupport/session_handler/session_client.hpp>
#include <next/sdk/ecal/ecal_singleton.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/udex/data_types.hpp>

#include "next/data_types.hpp"
#include "next/next.hpp"
#include "next/next_callbacks.hpp"
#include "sync_manager/sync_manager.hpp"

#include "next_inport.hpp"
#include "next_outport.hpp"
#include "next_system_service.hpp"

namespace next {

// TODO define what return values are accepted
typedef std::function<int(std::string &)> triggerCallback;

typedef std::tuple<void *, size_t> DataPackage;

class NextComponentInterfaceImpl {

public:
  NextComponentInterfaceImpl() = delete;
  NextComponentInterfaceImpl(const std::string &component_name);
  virtual ~NextComponentInterfaceImpl();
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
  virtual int SetupTriggers(const std::vector<std::pair<std::string, triggerCallback>> &triggers,
                            std::function<void(next::control::orchestrator::OrchestratorErrors &, std::string &,
                                               const std::string &, const triggerCallback &)>);

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
   * @brief Function that get´s called on every trigger. Prepares the synchronized data for the component execution code
   *
   * @param err error message that will be filled. next::control::orchestrator::OrchestratorErrors::SUCCESS or
   * next::control::orchestrator::OrchestratorErrors::FAILED
   * @param method_name method name that will be used to get trigger_info for synchronization
   * @param client_callback callback function that will be executed on the trigger
   */
  void OnTrigger(next::control::orchestrator::OrchestratorErrors &err, std::string &err_msg,
                 const std::string &method_name, const triggerCallback &client_callback);

  /*!
   * @brief Function that gets called on each reset.
   *
   * Resets:
   * Subscriber
   * Publisher
   * Member Variables
   * Maps
   * Vectors
   *
   * @return true on success
   * @return false on failure
   */
  bool userEnterReset();

  /*!
   * @brief Function that gets called at the end of each configuration
   *
   * provides status code that components are ready
   *
   * @return true on success
   * @return false on failure
   */
  bool userEnterConfigOutput();
  bool userEnterConfigInput();

  /*!
   * @brief checking whether the port was updated with data from the queue
   *
   * @param port_name the name of the port which is checked
   * @return bool flag indicating if the port has been updated with new data from the queue
   */
  bool isUpdated(const std::string &port_name);

  /*!
   * @brief invokes the remote method of the component
   *
   * @param node_name the name of the component
   * @param method_name the name of the remote method
   * @param timestamp time stamp sent along with the payload
   * @param flow_id id of the payload to be sent over
   * @return list of trigger responses
   */
  std::list<TriggerReturn> invokeTrigger(const std::string &node_name, const std::string &method_name,
                                         const std::string &timestamp, const std::string &flow_id);

  /*!
   * @brief invokes the remote method of the component using the timestamp and flow_id of the received trigegr
   *
   * @param node_name the name of the component
   * @param method_name the name of the remote method
   * @return list of trigger responses
   */
  std::list<TriggerReturn> invokeTrigger(const std::string &node_name, const std::string &method_name);

protected:
  std::shared_ptr<next::control::orchestrator::OrchestratorTriggerHandler> trigger_handler_;

private:
  /*!
   * @brief Fills the current timestamp based on the url given in the config
   *
   * @param package_url url that should be checked for timestamp
   * @return uint64_t extracted timestamp in microseconds
   */
  uint64_t FillCurrentTimestampMicroseconds(const std::string &package_url);

  /*!
   * @brief Transforms the timestamp based on the unit given in configuration
   *
   * @param timestamp value that should be transformed
   * @return uint64_t transformed timestamp in microseconds
   */
  uint64_t TransformTimestampToMicroseconds(uint64_t timestamp);

  std::list<TriggerReturn> parseTriggerResponse(const std::list<std::string> &responses);

  std::string component_name_;

  std::mutex current_trigger_mtx_;
  // holds the current trigger data for this cycle execution
  next::control::events::OrchestratorTrigger current_trigger_;

  next::appsupport::session::SessionClient session_client_;

  next::inport::NextInport next_inports_;
  next::inport::NextOutport next_outports;
  next::system_service::SimStatusService simulation_status_;
  next::system_service::TimeService time_service_;

public:
  friend class NextComponentInterfaceTester;
};
} // namespace next

#endif // NEXT_IMPL_H_
