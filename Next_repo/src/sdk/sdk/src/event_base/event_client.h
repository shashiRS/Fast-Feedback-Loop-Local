/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_client.h
 * @brief    Implementation for event publisher
 *
 **/
#ifndef EVENT_CLIENT_H_
#define EVENT_CLIENT_H_

#include <list>
#include <map>
#include <memory>
#include <string>

#include <next/sdk/event_base/event_types.h>

#include "event_types_hook_helper.h"

// forward declaration of eCAL lib class
namespace eCAL {
class CServiceClient;
struct SClientEventCallbackData;
} // namespace eCAL

namespace next {
namespace sdk {
namespace events {

class EventClient : public EventTypesHookHelper<eCAL::CServiceClient> {
public:
  /*!
   * @brief Construct a new Event eCal Client
   *
   */
  EventClient();

  /*!
   * @brief Destroy the Event Client
   *
   */
  virtual ~EventClient();

  //! see EventManager::call
  bool call(const std::string &event_class, const std::string &event_name, const payload_type &payload,
            const RegisteredEvent &event, std::list<std::string> &list_of_responses);
  bool initialize(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);

protected:
  void DestroyEcalType(std::shared_ptr<eCAL::CServiceClient> ecalInstance) override;

private:
  void onClientConnectionCallback(const char *name, const eCAL::SClientEventCallbackData *data);
  std::shared_ptr<eCAL::CServiceClient> AddClient(const std::string &event_class, const std::string &event_name);
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // EVENT_CLIENT_H_
