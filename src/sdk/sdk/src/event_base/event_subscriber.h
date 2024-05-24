/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_subscriber.h
 * @brief    Implementation for event subscriber
 *
 **/
#ifndef EVENT_SUBSCRIBER_H_
#define EVENT_SUBSCRIBER_H_

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <next/sdk/event_base/event_types.h>

#include "event_types_hook_helper.h"

// forward declaration of eCAL lib classes
namespace eCAL {
class CSubscriber;
struct SReceiveCallbackData;
struct SSubEventCallbackData;
} // namespace eCAL

namespace next {
namespace sdk {
namespace events {

class EventSubscriber : public EventTypesHookHelper<eCAL::CSubscriber> {
public:
  /*!
   * @brief Construct a new Event Subscriber object
   *
   */
  EventSubscriber();

  /*!
   * @brief Destroy the Event Subscriber object
   *
   */
  virtual ~EventSubscriber();

  bool subscribe(const std::string &event_class, const std::string &topic_name, const RegisteredEvent &event);
  bool initialize(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);

protected:
  void DestroyEcalType(std::shared_ptr<eCAL::CSubscriber> ecalInstance) override;

private:
  void onDataReceive(const char *, const struct eCAL::SReceiveCallbackData *data);
  void onListenerAddedEvent(const char *topic_name, const eCAL::SSubEventCallbackData *data);

  bool AddSubscriber(const std::string &event_class, const std::string &topic_name, bool is_pull_requested = false);
  std::mutex protect_shutdown_;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // EVENT_SUBSCRIBER_H_
