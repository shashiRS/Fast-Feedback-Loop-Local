/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_client.h
 * @brief    interface class orchestrator for event handling for Next Player Control
 *
 **/

#ifndef __I_ORCHESTRATOR_TRIGGER_HANDLER_H__
#define __I_ORCHESTRATOR_TRIGGER_HANDLER_H__

#include <string>
#include "../control_config.h"
#include "flow_control_config.h"

namespace next {
namespace control {
namespace orchestrator {

class OrchestratorTriggerHandlerImpl;
class CONTROL_LIBRARY_EXPORT OrchestratorTriggerHandler {

public:
  //! creates a OrchestratorTriggerHandler with the given node name.
  /*! initialize  OrchestratorTriggerHandlerImpl
   */
  explicit OrchestratorTriggerHandler(const std::string &node_node);

  //! destroys the OrchestratorTriggerHandler
  /*! deletes  the private implementation
   */
  virtual ~OrchestratorTriggerHandler();

  //! registers the call back cb to the port name
  /*!
   * @param method_name name of the port
   * @param cb callback of the port
   */
  void registerHook(const std::string &method_name, trigger_hook cb);

  //!  invokeTrigger triggers the registered hook
  /*!
   * @param node_name name of the node
   * @param method_name name of the method
   * @param timestamp timestamp part of the payload
   * @param flow_id  flowid part of the payload
   * @return list of string responses from the callbacks
   */
  std::list<std::string> invokeTrigger(const std::string &node_name, const std::string &method_name,
                                       const std::string &timestamp, const std::string &flow_id);

  //!  invokeTrigger triggers the registered hook
  /*!
   * @param method_name name of the method
   * @param timestamp timestamp part of the payload
   * @param flow_id  flowid part of the payload
   * @return list of string responses from the callbacks
   */
  std::list<std::string> invokeTrigger(const std::string &method_name, const std::string &timestamp = "",
                                       const std::string &flow_id = "");

  //! extractTriggerInfo function extracts the payload
  /*!
   * @param method_name name of the port
   * @return OrchestratorTrigger
   */
  next::control::events::OrchestratorTrigger extractTriggerInfo(const std::string &method_name);

private:
  OrchestratorTriggerHandlerImpl *p_impl_ = nullptr;
};
} // namespace orchestrator
} // namespace control
} // namespace next

#endif // __I_ORCHESTRATOR_TRIGGER_HANDLER_H__
