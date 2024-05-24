/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     version_info_event_distributor.h
 * @brief    implementation of small module to handle version info event messages
 **/

#ifndef NEXT_VERSION_INFO_EVENT_DISTRIBUTOR_H_
#define NEXT_VERSION_INFO_EVENT_DISTRIBUTOR_H_

#include <ecal/ecal.h>
#include <next/sdk/events/version_info.h>
#include <next/sdk/utilities/threadsafe_list.h>
#include <next/bridgesdk/web_server.hpp>
#include <next/sdk/version_manager/version_manager.hpp>
#include <thread>

namespace next {
namespace controlbridge {
namespace event_handler {

/*
 * @startuml
 * (*) --> "VersionEventDistributor"
 * "VersionEventDistributor" --> "Start up Version Event"
 * "Start up Version Event" --> "add the hook + subscriber"
 * "add the hook + subscriber" --> "hook"
 * "Player Verison Event" --> "hook"
 * "Databridge Verison Event" --> "hook"
 * "hook" #red -> "create JSON from event"
 * "create JSON from event" -> "Send to GUI"
 * @enduml
 */
class VersionEventDistributor {
public:
  /*!
   * @brief Construct a new VersionInfo Event Distributor object
   * @param webserver_in
   */
  VersionEventDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in);

  /*!
   * @brief Destroy the VersionInfo Event Distributor object
   */
  virtual ~VersionEventDistributor();

  /*!
   * @brief This function will be called for starting the thread
   */
  void startVersionThread();

  /*!
   * @brief This function will be called when an event is received and will call the other functions to handle and
   * distribute the VersionInfo event data
   */
  void getVersionInfoEventHook();

  /*!
   * @brief Sends the created JSON data to the GUI
   * @param msg Json formatted string that will be sent to GUI
   */
  void sendJsonToGUI(std::string msg);

  /*!
   * @brief Create a Json object from the received message
   * @param received_msg VersionInfoPackage received from the Meta Event Publisher
   * @return std::string Json formatted string with the information from the message
   */
  std::string createJson(const next::sdk::events::VersionInfoPackage &version_info);

  /*!
   * @brief Request for versions from all other componets
   * next,next_udex,next_sdk,next_control are not sending the version to control bridge.
   * filling the hardcoded version info of these componets and send to GUI along with the controlbridge version
   */
  void requestVersions();

private:
  /*!
   * @brief This function is used to bind the the event hook and subscribe
   */
  void startVersion();

  std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_;
  next::sdk::events::VersionInfo version_event_subscriber_;
  std::thread version_thread_;
  std::mutex version_mutex_;
  next::sdk::threadsafe_list<char *> message_list;
};

} // namespace event_handler
} // namespace controlbridge
} // namespace next

#endif // NEXT_VERSION_INFO_EVENT_DISTRIBUTOR_H_
