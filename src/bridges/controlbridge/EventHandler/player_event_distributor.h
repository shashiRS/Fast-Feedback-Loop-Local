/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     meta_event_distributor.h
 * @brief    implementation of small module to handle meta event messages
 **/

#ifndef NEXT_META_EVENT_DISTRIBUTOR_H_
#define NEXT_META_EVENT_DISTRIBUTOR_H_

#include <next/control/event_types/player_state_event.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_server.hpp>
#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace controlbridge {
namespace event_handler {

class PlayerEventDistributor {
public:
  /*!
   * @brief Construct a new Meta Event Distributor object
   *
   * @param webserver_in
   */
  PlayerEventDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
                         const std::string &name = "",
                         std::shared_ptr<next::appsupport::ConfigServer> configServer = nullptr);

  /*!
   * @brief Destroy the Meta Event Distributor object
   *
   */
  virtual ~PlayerEventDistributor();

  /*!
   * @brief This function will be called when an event is received and will call the other functions to handle and
   * distribute the event data
   *
   */
  void runMetaInformationDistribution();

  /*!
   * @brief Get the Message object from the meta Event Data
   *
   * @return next::control::events::PlayerMetaInformationMessage
   */
  next::control::events::PlayerMetaInformationMessage getMessage();

  /*!
   * @brief Create a Json object from the received message
   *
   * @param received_msg PlayerMetaInformationMessage received from the Meta Event Publisher
   * @return std::string Json formatted string with the information from the message
   */
  std::string createJson(next::control::events::PlayerMetaInformationMessage received_msg);

  /*!
   * @brief Create a Json object with the current Sil factor fron the configuration client
   *
   * @return std::string Json formatted string with the information
   */
  std::string createSilFactorJson();

  /*!
   * @brief Sends the created JSON data to the GUI
   *
   * @param msg Json formatted string that will be sent to GUI
   * @return true Successfully sent the message
   * @return false Error while sending the message
   */
  bool sendJsonToGUI(std::string msg);

  next::control::events::PlayerStateEvent player_event_subscriber_;
  std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_;
  std::string name_;
  size_t webserver_connection_count_{0u};

private:
  next::control::events::PlayerState current_player_state_{next::control::events::PlayerState::UNSPECIFIED};
  bool state_changed_{false};
  bool skip_GUI_update_{false};
  bool ConnectionStatusChanged();
  void SendSilFactor();
  // this is saved in case a new connection is available, to update the GUI;
  std::string last_opened_recording_{""};
  std::shared_ptr<next::appsupport::ConfigServer> config_server_{nullptr};
  bool connectionChanged = false;
};

} // namespace event_handler
} // namespace controlbridge
} // namespace next

#endif // NEXT_META_EVENT_DISTRIBUTOR_H_
