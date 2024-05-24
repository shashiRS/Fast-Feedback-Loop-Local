/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_manager.h
 * @brief    Base class for Eventmanager which subscribe and send events
 *
 **/
#ifndef NEXT_EVENT_MANAGER_H_
#define NEXT_EVENT_MANAGER_H_

#include <condition_variable>
#include <map>
#include <memory>
#include <string>

#include <next/sdk/event_base/event_types.h>
#include <next/sdk/sdk_macros.h>

#include "event_publisher.h"
#include "next/sdk/sdk_config.hpp"

namespace next {
namespace sdk {
namespace events {

/*!
 * @brief EventManager Class
 *
 */
NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk EventManager {

public:
  /*!
   * @brief subscribe to this event and receive callbacks when event was received
   *
   * @param event_class name of the event_class
   * @param event_name optional additional event name
   * @param callback callback used to inform when event was received
   * @return true if successful, otherwise false
   */
  bool subscribe(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);

  /*!
   * @brief initialization function for binary events
   *
   * @param event_class name of the event_class
   * @param event_name event name
   * @param event
   * @return true if successful, otherwise false
   */
  bool initializeEventBinary(const std::string &event_class, const std::string &event_name,
                             const RegisteredEvent &event);

  /*!
   * @brief initialization function for binary events blocking
   *
   * @param event_class name of the event_class
   * @param event_name event name
   * @param event
   * @return true if successful, otherwise false
   */
  bool initializeEventBinaryBlocking(const std::string &event_class, const std::string &event_name,
                                     const RegisteredEvent &event);

  /*!
   * @brief publish event data to network
   *
   * @param event_class name of the event_class
   * @param event_name optional additional event name
   * @param payload binary data of the event to send
   * @param subs_event_callback callback to add when a new subscriber is added. Only valid for first publish per
   * eventtype and class
   * @return true if successful, otherwise false
   */
  bool publish(const std::string &event_class, const std::string &event_name, const payload_type &payload,
               const RegisteredEvent &event, std::list<std::string> &list_of_responses);

  /*!
   * @brief listen to an event method call
   *
   * @param event_class name of the event_class
   * @param event_name method name to call
   * @param callback callback used to inform when event was received
   * 	 * @return true if successful, otherwise false
   */
  bool listen(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);

  /*!
   * @brief call an event method. This call will block!
   *
   * @param event_class name of the event_class
   * @param event_name method name
   * @param payload request data
   * @param callback callback used to inform when event was received
   * @return true if successful, otherwise false
   */
  bool call(const std::string &event_class, const std::string &event_name, const payload_type &payload,
            const RegisteredEvent &event, std::list<std::string> &list_of_responses);

  /*!
   * @brief call an event method. This call will block!
   *
   * @param event_class name of the event_class
   * @param event_name method name
   * @return number of subscribers
   */
  int getNumberOfSubscribers(const std::string &event_class, const std::string &event_name);

  /*!
   * @brief call an event method. This call will block!
   *
   * @param event_class name of the event_class
   * @param event_name method name
   * @return number of subscribers
   */
  bool unregister(const RegisteredEvent &event, const std::string &event_class, const std::string &event_name);

  // create a singleton instance of Eventmanager. this is threadsafe.
  static std::unique_ptr<EventManager> &GetInstance();

  //! destructor may cleanup publisher and subscriber
  virtual ~EventManager();

  struct ConditionalWait {
    int counter_ = 0;
    std::mutex mutex_;
    std::condition_variable cv_;
  };

private:
  void resetAll();
  //! constructor, may init publisher and subscriber
  EventManager();

  //! since we are a singleton don't allow copy and assignment
  EventManager(const EventManager &) = delete;
  EventManager &operator=(const EventManager &) = delete;

  // reset EventManager singelton
  static void clean();

  ConditionalWait publisher_shutdown_protect_;
  ConditionalWait subscriber_shutdown_protect_;
  ConditionalWait client_shutdown_protect_;
  ConditionalWait server_shutdown_protect_;
  std::atomic<bool> shutdown_{false};
  std::mutex shutdown_mutex_;
};
NEXT_RESTORE_WARNINGS_WINDOWS
} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_EVENT_MANAGER_H_
