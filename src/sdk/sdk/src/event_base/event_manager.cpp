/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_manager.cpp
 * @brief    implementation of class for Eventmanager which subscribe and send events over ecal
 *
 **/

#include "event_manager.h"

#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/logger/logger.hpp>

#include "event_client.h"
#include "event_publisher.h"
#include "event_server.h"
#include "event_subscriber.h"
#include "event_types_hook_helper.h"

namespace next {
namespace sdk {
namespace events {

// put this outside of function scope as it is needed for two functions
static std::map<std::string, std::shared_ptr<EventPublisher>> event_publishers;
static std::map<std::string, std::shared_ptr<EventSubscriber>> event_subscribers;
static std::map<std::string, std::shared_ptr<EventClient>> event_clients;
static std::map<std::string, std::shared_ptr<EventServer>> event_servers;

template <typename eCALType>
std::shared_ptr<eCALType> SafeGetEcalType(EventManager::ConditionalWait &condition,
                                          std::function<std::shared_ptr<eCALType>()> getCall) {
  std::shared_ptr<eCALType> sub;
  {
    std::lock_guard<std::mutex> sub_guard{condition.mutex_};
    condition.counter_++;
    sub = getCall();
  }
  return sub;
}

void SafeGiveEcalTypeBack(EventManager::ConditionalWait &condition) {
  {
    std::lock_guard<std::mutex> sub_guard{condition.mutex_};
    condition.counter_--;
  }
  condition.cv_.notify_all();
}

std::shared_ptr<EventPublisher> getPublisherSingleton(const std::string &class_name) {
  if (event_publishers.find(class_name) == event_publishers.end()) {
    event_publishers.emplace(std::make_pair(class_name, std::make_shared<EventPublisher>()));
  }
  return event_publishers[class_name];
}

std::shared_ptr<EventSubscriber> getSubscriberSingleton(const std::string &class_name) {
  if (event_subscribers.find(class_name) == event_subscribers.end()) {
    event_subscribers.emplace(std::make_pair(class_name, std::make_shared<EventSubscriber>()));
  }
  return event_subscribers[class_name];
}

std::shared_ptr<EventClient> getClientSingleton(const std::string &class_name) {
  if (event_clients.find(class_name) == event_clients.end()) {
    event_clients.emplace(std::make_pair(class_name, std::make_shared<EventClient>()));
  }
  return event_clients[class_name];
}

std::shared_ptr<EventServer> getServerSingleton(const std::string &class_name) {
  if (event_servers.find(class_name) == event_servers.end()) {
    event_servers.emplace(std::make_pair(class_name, std::make_shared<EventServer>()));
  }
  return event_servers[class_name];
}

bool EventManager::subscribe(const std::string &event_class, const std::string &event_name,
                             const RegisteredEvent &event) {
  if (shutdown_) {
    return true;
  }
  std::function<std::shared_ptr<EventSubscriber>()> getsubscriber_call = [&]() {
    return getSubscriberSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  std::shared_ptr<EventSubscriber> sub = SafeGetEcalType(subscriber_shutdown_protect_, getsubscriber_call);
  auto sub_return = sub->subscribe(event_class, event_name, event);
  SafeGiveEcalTypeBack(subscriber_shutdown_protect_);
  return sub_return;
}

bool EventManager::initializeEventBinary(const std::string &event_class, const std::string &event_name,
                                         const RegisteredEvent &event) {
  if (shutdown_) {
    return true;
  }
  std::function<std::shared_ptr<EventPublisher>()> getpublisher_call = [&]() {
    return getPublisherSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  std::shared_ptr<EventPublisher> pub = SafeGetEcalType(publisher_shutdown_protect_, getpublisher_call);
  bool retVal = pub->initialize(event_class, event_name, event);
  SafeGiveEcalTypeBack(publisher_shutdown_protect_);

  std::function<std::shared_ptr<EventSubscriber>()> getsubscriber_call = [&]() {
    return getSubscriberSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  std::shared_ptr<EventSubscriber> sub = SafeGetEcalType(subscriber_shutdown_protect_, getsubscriber_call);
  retVal &= sub->initialize(event_class, event_name, event);
  SafeGiveEcalTypeBack(subscriber_shutdown_protect_);

  return retVal;
}

bool EventManager::initializeEventBinaryBlocking(const std::string &event_class, const std::string &event_name,
                                                 const RegisteredEvent &event) {
  if (shutdown_) {
    return true;
  }
  bool retVal = true;
  {
    std::lock_guard<std::mutex> server_guard{server_shutdown_protect_.mutex_};
    retVal = retVal & getServerSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested))
                          ->initialize(event_class, event_name, event);
  }
  {
    std::lock_guard<std::mutex> client_guard{client_shutdown_protect_.mutex_};
    retVal = retVal & getClientSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested))
                          ->initialize(event_class, event_name, event);
  }
  return retVal;
}

int EventManager::getNumberOfSubscribers(const std::string &event_class, const std::string &event_name) {
  if (shutdown_) {
    return true;
  }
  std::lock_guard<std::mutex> subscriber_guard{publisher_shutdown_protect_.mutex_};
  auto retVal = getPublisherSingleton(EventHelper::createName(event_class, event_name))
                    ->getNumberOfSubscribers(event_class, event_name);
  publisher_shutdown_protect_.cv_.notify_all();
  return retVal;
}

bool EventManager::publish(const std::string &event_class, const std::string &event_name, const payload_type &payload,
                           const RegisteredEvent &event, std::list<std::string> &list_of_responses) {
  if (shutdown_) {
    return true;
  }

  std::function<std::shared_ptr<EventPublisher>()> getpublisher_call = [&]() {
    return getPublisherSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  std::shared_ptr<EventPublisher> pub = SafeGetEcalType(publisher_shutdown_protect_, getpublisher_call);
  auto pub_return = pub->publish(event_class, event_name, payload, event, list_of_responses);
  SafeGiveEcalTypeBack(publisher_shutdown_protect_);
  return pub_return;
}

bool EventManager::listen(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event) {
  if (shutdown_) {
    return true;
  }
  std::lock_guard<std::mutex> server_guard{server_shutdown_protect_.mutex_};
  return getServerSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested))
      ->listen(event_class, event_name, event);
}

bool EventManager::call(const std::string &event_class, const std::string &event_name, const payload_type &payload,
                        const RegisteredEvent &event, std::list<std::string> &list_of_responses) {
  if (shutdown_) {
    return true;
  }
  std::lock_guard<std::mutex> client_guard{client_shutdown_protect_.mutex_};
  return getClientSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested))
      ->call(event_class, event_name, payload, event, list_of_responses);
}

template <typename eCALType>
bool SafeUnregisterEcalType(const RegisteredEvent &event, EventManager::ConditionalWait &condition,
                            std::function<std::shared_ptr<eCALType>()> getCall) {
  std::shared_ptr<eCALType> sub;
  {
    std::lock_guard<std::mutex> sub_guard{condition.mutex_};
    condition.counter_++;
    sub = getCall();
  }
  // its very important to free up here our mutex lock as the call to the hook protect ends up in a dedlok
  bool result = sub->unregisterHooks(event);
  {
    std::lock_guard<std::mutex> sub_guard{condition.mutex_};
    condition.counter_--;
  }
  condition.cv_.notify_all();
  return result;
}

bool EventManager::unregister(const RegisteredEvent &event, const std::string &event_class,
                              const std::string &event_name) {
  if (shutdown_) {
    return true;
  }

  bool result = true;

  std::function<std::shared_ptr<EventPublisher>()> getpub_call = [&]() {
    return getPublisherSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  result &= SafeUnregisterEcalType<EventPublisher>(event, publisher_shutdown_protect_, getpub_call);

  std::function<std::shared_ptr<EventSubscriber>()> getsub_call = [&]() {
    return getSubscriberSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  result &= SafeUnregisterEcalType<EventSubscriber>(event, subscriber_shutdown_protect_, getsub_call);

  std::function<std::shared_ptr<EventServer>()> getserver_call = [&]() {
    return getServerSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  result &= SafeUnregisterEcalType<EventServer>(event, server_shutdown_protect_, getserver_call);

  std::function<std::shared_ptr<EventClient>()> getclient_call = [&]() {
    return getClientSingleton(EventHelper::createName(event_class, event_name, event.is_pull_requested));
  };
  result &= SafeUnregisterEcalType<EventClient>(event, client_shutdown_protect_, getclient_call);

  return result;
}

std::unique_ptr<EventManager> &EventManager::GetInstance() {
  static std::unique_ptr<EventManager> eventmanager =
      std::unique_ptr<EventManager>(new EventManager()); // std::make_unique<EventManager>();
  // check if reset() was called
  if (!eventmanager) {
    eventmanager = std::unique_ptr<EventManager>(new EventManager());
  }
  return eventmanager;
}

EventManager::EventManager() {
  if (!eCAL::IsInitialized()) {
    if (next::sdk::logger::isInitialized()) {
      next::sdk::logger::warn(__FILE__, "eCAL not initialized");
    }
  }
  // reset EventManager by InitChain
  static InitChain::El init_el(
      InitPriority::EVENT_MANAGER, [](const InitChain::ConfigMap &, int) { return true; },
      [this](const InitChain::ConfigMap &, int) { clean(); });
}

void EventManager::resetAll() {

  shutdown_ = true;
  std::lock_guard<std::mutex> guard_shutdown_{shutdown_mutex_};
  {
    std::unique_lock<std::mutex> publisher_lock{publisher_shutdown_protect_.mutex_};
    publisher_shutdown_protect_.cv_.wait(publisher_lock, [&] { return publisher_shutdown_protect_.counter_ == 0; });
    event_publishers.clear();
  }
  {
    std::unique_lock<std::mutex> subscriber_lock{subscriber_shutdown_protect_.mutex_};
    subscriber_shutdown_protect_.cv_.wait(subscriber_lock, [&] { return subscriber_shutdown_protect_.counter_ == 0; });
    event_subscribers.clear();
  }

  {
    std::lock_guard<std::mutex> client_lock{client_shutdown_protect_.mutex_};
    event_clients.clear();
  }

  {
    std::lock_guard<std::mutex> server_lock{server_shutdown_protect_.mutex_};
    event_servers.clear();
  }
}

void EventManager::clean() {
  // reset EventManager singelton
  GetInstance().reset();
}

EventManager::~EventManager() { resetAll(); }

} // namespace events
} // namespace sdk
} // namespace next
