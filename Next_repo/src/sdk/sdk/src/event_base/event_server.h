/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_server.h
 * @brief    Implementation for event ecal Service Server
 *
 **/
#ifndef EVENT_SERVER_H_
#define EVENT_SERVER_H_

#include <map>
#include <memory>
#include <string>

#include <next/sdk/event_base/event_types.h>

#include "event_types_hook_helper.h"

// forward declaration of eCAL lib classes
namespace eCAL {
class CServiceServer;
struct SServerEventCallbackData;
} // namespace eCAL

namespace next {
namespace sdk {
namespace events {

class EventServer : public EventTypesHookHelper<eCAL::CServiceServer> {
public:
  /*!
   * @brief Construct a new Event Server object
   *
   */
  EventServer();

  /*!
   * @brief Destroy the Event Server object
   *
   */
  virtual ~EventServer();

  bool listen(const std::string &event_class, const std::string &topic_name, const RegisteredEvent &event);
  bool initialize(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);

protected:
  void DestroyEcalType(std::shared_ptr<eCAL::CServiceServer> ecalInstance) override;

private:
  bool AddServer(const std::string &event_class, const std::string &topic_name);
  int onMethodCallback(const std::string &method_, const std::string &req_type_, const std::string &resp_type_,
                       const std::string &request_, std::string &response_);
  void onListenerAddedEvent(const char *name, const eCAL::SServerEventCallbackData *data);
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // EVENT_SERVER_H_
