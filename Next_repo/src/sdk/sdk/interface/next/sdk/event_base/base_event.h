/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_event.h
 * @brief    Base class for declaration of event payloads
 *
 **/
#ifndef NEXTSDK_BASE_EVENT_H_
#define NEXTSDK_BASE_EVENT_H_

#include <atomic>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "event_types.h"

#ifdef __linux__
#include <cxxabi.h>
#endif

namespace next {
namespace sdk {
namespace events {

#ifdef __linux__
inline std::string demangle_name(const char *mangled_name) {

  std::size_t len = 0;
  int status = 0;
  std::unique_ptr<char, decltype(&std::free)> ptr(__cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status),
                                                  &std::free);
  return ptr.get();
}
#else
inline std::string demangle_name(const char *name) {
  std::string demangled_name(name);
  const std::vector<std::string> type_str{"struct ", "class "};
  for (auto type : type_str) {
    if (demangled_name.rfind(type, 0) == 0) {
      return demangled_name.substr(type.size());
    }
  }
  return demangled_name;
}
#endif

struct Next_Event_Header {
  long long timestamp = 0; // given by received message
  long long id = 0;        // given by received message
}; // payloads must be derived from the base class Next_Event which holds the header //TODO: is health state also
// required? Header can be expanded in futures
/*!
 * @brief Base Event Template class, pure virtual.
 * used to define an Event which can be send e.g. over eCal
 *
 * @tparam PayloadType the type it seriliaze to
 * @tparam UserType the data which should be send
 */
template <typename PayloadType, typename UserType>
class BaseEventTemplate {

public:
  /*!
   * @brief Construct a new Base Event Template object
   *
   * @param name the event name, should be unique. used for registration/publishing
   */
  BaseEventTemplate(std::string event_name, std::string event_class)
      : _event_name(event_name), _event_class(event_class) {}

  virtual ~BaseEventTemplate() { std::lock_guard<std::recursive_mutex> guard_deconstructor{event_callback_}; }

  //! returns the control header of the event
  virtual Next_Event_Header getHeader() const { return header; }

  //! Get the Name of the event
  virtual std::string getEventName() const { return _event_name; }

  //! Get the underlying implementation class
  virtual std::string getClassName() const { return _event_class; }

  //! Get the latest event data
  virtual UserType getEventData() {
    std::lock_guard<std::recursive_mutex> guard{protect_event_data};
    return current_event_data_;
  };

  /*!
   * @brief Add a hook to be called  when the event is triggered. These hooks are called first
   *         and afterwards the synced hooks are called
   *
   * @param hook hook that will be called
   */
  virtual void addEventHook(const notify_hook &hook) { event_hooks.push_back(std::move(hook)); }

  /*!
   * @brief Add a synced hook to be called when the event is triggered.
   *        These hooks are called after the unsynced hooks
   *
   * @param hook hook that will be called
   */
  virtual void addSyncEventHook(const notify_sync_hook &hook) { sync_event_hooks.push_back(hook); }

  /*!
   * @brief Add a hook to be notified when a new subscriber is added for the event
   *
   * @param event_hook hook that will be called
   */
  virtual void addListenerAddedHook(const subs_event_hook &event_hook) {
    std::cout << __FILE__ << "EVENT:" << _event_name
              << "This option is not working right now. Fixes are expected with new eCAL version" << std::endl;
    std::lock_guard<std::mutex> guard{protect_sub_hooks_};
    subs_event_hooks.push_back(std::move(event_hook));
  }

  //! subscribe this Event at the network
  virtual void subscribe() = 0;

  //! publish/send event
  virtual void publish(const UserType &) = 0;
  virtual void publish(const UserType &, std::list<std::string> &responses) = 0;

  //! callback when event was received. see event_types.h -> event_callback_type for more inf

  virtual void onEvent(const PayloadType &message_raw, long long time, long long id) {
    std::lock_guard<std::recursive_mutex> guard{protect_event_data};
    std::lock_guard<std::recursive_mutex> guard_deconstructor{event_callback_};

    if (shutdown_) {
      return;
    }
    current_event_data_ = deserializeEventPayload(message_raw);
    header.id = id;
    header.timestamp = time;

    for (auto hook : event_hooks) {
      hook();
    }
  }

  virtual void onEventListenerAdded(const std::string &topic) {
    std::lock_guard<std::mutex> guard{protect_sub_hooks_};
    for (auto hook : subs_event_hooks) {
      hook(topic);
    }
  }

  virtual void onSyncEvent(const PayloadType &message_raw, std::string &response) {
    std::lock_guard<std::mutex> guard{protect_sync_event_hooks_};
    for (auto hook : sync_event_hooks) {
      hook(message_raw, response);
    }
  }

protected:
  inline uint64_t getCurrentNanoSeconds() {
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
  }
  //! serialize event data to a e.g. a binary format
  virtual PayloadType serializeEventPayload(const UserType &) const = 0;
  //! reverse serialization
  virtual UserType deserializeEventPayload(const PayloadType &) const = 0;

  //! contains all hooks registered by addEventHook
  std::vector<notify_hook> event_hooks;
  std::vector<subs_event_hook> subs_event_hooks;
  std::vector<notify_sync_hook> sync_event_hooks;

  std::mutex protect_sub_hooks_{};
  std::mutex protect_sync_event_hooks_{};
  std::recursive_mutex protect_event_data{};
  UserType current_event_data_{};
  size_t _event_id = 0;
  std::atomic<bool> shutdown_{false}; //!< required for a derived class shutdown mechanism. only set on derived classes!

private:
  std::recursive_mutex event_callback_{};

  //! contains the event header
  Next_Event_Header header;
  //! unique event name
  std::string _event_name; // can be set in derived events
  std::string _event_class;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_BASE_EVENT_H_
