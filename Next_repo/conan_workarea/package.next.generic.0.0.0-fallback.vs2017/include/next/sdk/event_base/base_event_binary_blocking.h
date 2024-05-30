/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_event_binary_blocking.h
 * @brief    Base class for declaration of event payloads in binary format
 *
 **/
#ifndef NEXTSDK_BASE_EVENT_BINARY_BLOCKING_H_
#define NEXTSDK_BASE_EVENT_BINARY_BLOCKING_H_

#include <memory>
#include <string>

#include "base_event.h"
#include "base_event_binary_impl.h"

namespace next {
namespace sdk {
namespace events {

template <typename UserType>
class BaseEventBinaryBlocking : public BaseEventTemplate<payload_type, UserType> {
private:
  std::shared_ptr<BaseEventBinaryImpl> _impl;

public:
  BaseEventBinaryBlocking(std::string event_name)
      : BaseEventTemplate<payload_type, UserType>(event_name, demangle_name(typeid(UserType).name())) {
    _impl = std::make_shared<BaseEventBinaryImpl>();
  }
  virtual ~BaseEventBinaryBlocking() {
    _impl->unregister(this->getClassName(), this->getEventName(),
                      {BaseEventTemplate<payload_type, UserType>::_event_id});
  }
  virtual void subscribe() override {
    auto binded_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onEvent, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    auto subs_event_callback =
        std::bind(&BaseEventTemplate<payload_type, UserType>::onEventListenerAdded, this, std::placeholders::_1);
    auto sync_event_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onSyncEvent, this,
                                         std::placeholders::_1, std::placeholders::_2);

    if (BaseEventTemplate<payload_type, UserType>::_event_id == 0) {
      BaseEventTemplate<payload_type, UserType>::_event_id =
          BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds(); // use current Nanoseconds as unique ID
    }
    RegisteredEvent event{BaseEventTemplate<payload_type, UserType>::_event_id, binded_callback, subs_event_callback,
                          sync_event_callback};
    _impl->listen(this->getClassName(), this->getEventName(), event);
  }

  virtual void publish(const UserType &message) override {
    std::list<std::string> list_of_responses;
    publish(message, list_of_responses);
  }
  bool initialize() {
    auto binded_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onEvent, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    auto subs_event_callback =
        std::bind(&BaseEventTemplate<payload_type, UserType>::onEventListenerAdded, this, std::placeholders::_1);
    auto sync_event_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onSyncEvent, this,
                                         std::placeholders::_1, std::placeholders::_2);

    if (BaseEventTemplate<payload_type, UserType>::_event_id == 0) {
      BaseEventTemplate<payload_type, UserType>::_event_id =
          BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds(); // use current Nanoseconds as unique ID
    }
    RegisteredEvent event{BaseEventTemplate<payload_type, UserType>::_event_id, binded_callback, subs_event_callback,
                          sync_event_callback};
    return _impl->initializeBinaryEventBlocking(this->getClassName(), this->getEventName(), event);
  }

  virtual void publish(const UserType &message, std::list<std::string> &responses) override {
    auto subs_event_callback =
        std::bind(&BaseEventTemplate<payload_type, UserType>::onEventListenerAdded, this, std::placeholders::_1);
    if (BaseEventTemplate<payload_type, UserType>::_event_id == 0) {
      BaseEventTemplate<payload_type, UserType>::_event_id =
          BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds(); // use current Nanoseconds as unique ID
    }
    RegisteredEvent event{
        BaseEventTemplate<payload_type, UserType>::_event_id,
        {},
        subs_event_callback,
    };
    _impl->call(this->getClassName(), this->getEventName(), serializeEventPayload(message), event, responses);
  }

protected:
  virtual payload_type serializeEventPayload(
      const UserType &) const = 0; // fills internal payload struct from protobuf -> updates header automatically
  virtual UserType deserializeEventPayload(const payload_type &) const = 0; // returns the struct payload
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_BASE_EVENT_BINARY_BLOCKING_H_
