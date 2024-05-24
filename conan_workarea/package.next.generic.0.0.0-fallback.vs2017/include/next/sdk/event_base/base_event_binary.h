/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_event_binary.h
 * @brief    Base class for declaration of event payloads in binary format
 *
 **/
#ifndef NEXTSDK_BASE_EVENT_BINARY_H_
#define NEXTSDK_BASE_EVENT_BINARY_H_

#include <atomic>
#include <memory>
#include <string>

#include "base_event.h"
#include "base_event_binary_impl.h"

namespace next {
namespace sdk {
namespace events {

template <typename UserType>
class BaseEventBinary : public BaseEventTemplate<payload_type, UserType> {
private:
  std::shared_ptr<BaseEventBinaryImpl> _impl;

public:
  BaseEventBinary(std::string event_name)
      : BaseEventTemplate<payload_type, UserType>(event_name, demangle_name(typeid(UserType).name())) {
    _impl = std::make_shared<BaseEventBinaryImpl>();
    setUpEventRequestListener();
  }
  virtual ~BaseEventBinary() {
    BaseEventTemplate<payload_type, UserType>::shutdown_ = true;
    std::lock_guard<std::mutex> guard{publish_event_mutex_};
    std::lock_guard<std::mutex> guard_event{pull_event_mutex_};

    RegisteredEvent reg_event; // To fix the jenkins build error
    reg_event.unique_id = BaseEventTemplate<payload_type, UserType>::_event_id;
    _impl->unregister(this->getClassName(), this->getEventName(), reg_event);

    // to unregister pull request event
    reg_event.unique_id = request_id_;
    _impl->unregister(this->getClassName(), this->getEventName(), reg_event);
  }
  bool initialize() {
    auto binded_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onEvent, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    auto subs_event_callback =
        std::bind(&BaseEventTemplate<payload_type, UserType>::onEventListenerAdded, this, std::placeholders::_1);
    if (BaseEventTemplate<payload_type, UserType>::_event_id == 0) {
      BaseEventTemplate<payload_type, UserType>::_event_id =
          BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds(); // use current Nanoseconds as unique ID
    }
    RegisteredEvent event{
        BaseEventTemplate<payload_type, UserType>::_event_id,
        binded_callback,
        subs_event_callback,
        {},
    };
    return _impl->initializeBinaryEvent(this->getClassName(), this->getEventName(), event);
  }
  virtual void subscribe() override {
    auto binded_callback = std::bind(&BaseEventTemplate<payload_type, UserType>::onEvent, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    auto subs_event_callback =
        std::bind(&BaseEventTemplate<payload_type, UserType>::onEventListenerAdded, this, std::placeholders::_1);

    if (BaseEventTemplate<payload_type, UserType>::_event_id == 0) {
      BaseEventTemplate<payload_type, UserType>::_event_id =
          BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds(); // use current Nanoseconds as unique ID
    }
    RegisteredEvent event{
        BaseEventTemplate<payload_type, UserType>::_event_id,
        binded_callback,
        subs_event_callback,
        {},
    };
    _impl->subscribe(this->getClassName(), this->getEventName(), event);
  }

  virtual void publish(const UserType &message) override {
    std::list<std::string> list_of_responses;
    publish(message, list_of_responses);
  }
  virtual void publish(const UserType &message, std::list<std::string> &list_of_responses) override {

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
        {},
    };

    std::lock_guard<std::mutex> guard{publish_event_mutex_};
    auto class_name = this->getClassName();
    auto event_name = this->getEventName();
    auto serialised_message = serializeEventPayload(message);
    _impl->publish(class_name, event_name, serialised_message, event, list_of_responses);

    last_sent_event_data_ = message;
    last_event_sent = true;
  }

  virtual int getNumberOfListeners(void) {
    return _impl->getNumberOfSubscribers(this->getClassName(), this->getEventName());
  }

  virtual void requestEventUpdate() {
    payload_type payload = StringToPayload("ping");
    std::list<std::string> list_of_responses;
    next::sdk::events::RegisteredEvent empty_event;
    empty_event.unique_id = request_id_;
    empty_event.is_pull_requested = true;
    _impl->requestEventUpdate(this->getClassName(), this->getEventName(), payload, empty_event, list_of_responses);
  }
  virtual void addOnEventRequestedHook(std::function<void(const payload_type &)> hook) {
    pullrequest_hooks_.push_back(hook);
  }

protected:
  virtual payload_type serializeEventPayload(
      const UserType &) const = 0; //< fills internal payload struct from protobuf -> updates header automatically
  virtual UserType deserializeEventPayload(const payload_type &) const = 0; //< returns the struct payload

private:
  void setUpEventRequestListener(void) {

    auto pull_request_event_callback = [&](const payload_type &payload, long long, long long) {
      std::lock_guard<std::mutex> guard{pull_event_mutex_};

      if (this->last_event_sent) {
        publish(this->last_sent_event_data_);
      }
      // Not using this hook now, can adapte later based on requirment
      for (auto &hook : pullrequest_hooks_) {
        hook(payload);
      }
    };

    request_id_ = BaseEventTemplate<payload_type, UserType>::getCurrentNanoSeconds();
    bool is_pull_requested = true;
    RegisteredEvent event{request_id_, pull_request_event_callback, {}, {}, is_pull_requested};
    _impl->subscribe(this->getClassName(), this->getEventName(), event);
  }
  UserType last_sent_event_data_{};
  size_t request_id_{0};
  std::vector<std::function<void(const payload_type &)>> pullrequest_hooks_;
  std::atomic<bool> last_event_sent{false};
  mutable std::mutex publish_event_mutex_{};
  mutable std::mutex pull_event_mutex_{};
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_BASE_EVENT_BINARY_H_
