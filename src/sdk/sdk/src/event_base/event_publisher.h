/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_publisher.h
 * @brief    Implementation for event publisher
 *
 **/
#ifndef EVENT_PUBLISHER_H_
#define EVENT_PUBLISHER_H_

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <next/sdk/event_base/event_types.h>

#include "event_types_hook_helper.h"

// forward declaration of eCAL lib class
namespace eCAL {
class CPublisher;
struct SPubEventCallbackData;
} // namespace eCAL

namespace next {
namespace sdk {
namespace events {

class EventPublisher : public EventTypesHookHelper<eCAL::CPublisher> {
public:
  /*!
   * @brief Construct a new Event Publisher object
   *
   */
  EventPublisher();

  /*!
   * @brief Destroy the Event Publisher object
   *
   */
  virtual ~EventPublisher();

  //! see EventManager::publish
  bool publish(const std::string &event_class, const std::string &event_name, const payload_type &payload,
               const RegisteredEvent &event, std::list<std::string> &list_of_responses);
  bool initialize(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);
  //! see EventManager::getNumberOfSubscribers
  int getNumberOfSubscribers(const std::string &event_class, const std::string &event_name);

protected:
  void DestroyEcalType(std::shared_ptr<eCAL::CPublisher> ecalInstance) override;

private:
  void onListenerAdded(const char *topic_name, const eCAL::SPubEventCallbackData *data);

  std::shared_ptr<eCAL::CPublisher> AddPublisher(const std::string &event_class, const std::string &event_name,
                                                 bool is_pull_requested = false);
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // EVENT_PUBLISHER_H_
