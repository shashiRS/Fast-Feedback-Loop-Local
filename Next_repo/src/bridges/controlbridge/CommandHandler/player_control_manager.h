/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     player_control_manager.h
 * @brief    Calls appropriate function
 *
 * Calls the appropriate method depending on the parsed Json command
 * Also passes the required params.
 */

#ifndef NEXT_CONTROLBRIDGE_PLAYER_CONTROL_MANAGER_
#define NEXT_CONTROLBRIDGE_PLAYER_CONTROL_MANAGER_

#include <future>
#include <map>
#include <string>
#include <thread>

#include <next/sdk/utilities/threadsafe_queue.h>
#include <next/appsupport/config/config_server.hpp>
#include <next/bridgesdk/web_server.hpp>

#include "../EventHandler/player_event_distributor.h"
#include "gui_command_parser.h"

namespace next {
namespace controlbridge {
namespace command_handler {

constexpr const static int kThreadSleep1Ms = 1;

enum class SimulationType { OpenLoop, CloseLoop, InvalidMode };

struct Response {
  gui_command_parser::GuiCommand cmd;
  std::map<std::string, std::string> payload;
  bool success;
};
//! Class to handle all commands
/*!
 *
 */
class PlayerControlManager {
public:
  //! Startup Webserver and parser
  /*!
   *
   * @param ip_address Ip address for listening to incoming request.
   * @param port_number Port number.
   * @param num_connections Number of connections that can be handled simultaneously.
   */

  void StartUp(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
               std::shared_ptr<next::appsupport::ConfigServer> configServer, std::string trigger_node_name,
               std::string trigger_name);

  //! Shutdown webserver and parser
  /*!
   * @exception std::invalid_argument or unknown from operating system
   * from shuttingdown webserver
   */
  void ShutDown();

  //! Handle Json response and send to GUI
  /*!
   * @param res Response to be sent to the GUI
   */
  void HandleResponse(Response res);

  //! Parse Json payload and call appropriate command handler
  void HandleJsonPayload(std::string jsonMessage);

  PlayerControlManager();

  virtual ~PlayerControlManager();

private:
  std::string GetStringParam(const std::map<std::string, std::vector<std::string>> &params, const std::string &key);
  std::vector<std::string> GetStringListParam(const std::map<std::string, std::vector<std::string>> &params,
                                              const std::string &key);
  //! Handle Open file command
  void HandleOpenFile();
  //! Handle Play command
  /*!
   * @param forward
   */
  void HandlePlay(bool forward);

  //! Handle Play_Until command
  /*!
   * @param forward
   */
  void HandlePlayUntil(bool forward, const std::string &play_until_timestamp);

  //! Handle Pause command
  void HandlePause();
  //! Handle Rewind command
  void HandleRewind();
  //! Handle Close command
  void HandleClose();
  //! Handle Fastforward command
  void HandleFastForward();
  //! Handle Jump Command
  /*!
   * @param step
   */
  void HandleJump(uint64_t timestamp, std::string timestamp_type);

  //! Handle Step Command
  /*!
   * @param step
   */
  void HandleStepForward(int32_t step);

  //! Handle Update Sil Factor Command
  /*!
   * @param updatedStepFactor
   */
  void HandleUpdateSilFactor(float updatedSilFactor);

  //! Handle JumpToEnd Command
  /*!
   * @param step
   */
  void HandleJumpToEnd();

  void HandleOpenProjectDirectory();
  void HandleOpenBinary();

  //! Perform actions/Call functions depending on the command
  /*!
   *  @param cmd Command received from GUI
   *  @params Extra information required for performing the actions
   */
  bool HandleCommand(const next::controlbridge::gui_command_parser::GuiCommand cmd,
                     std::map<std::string, std::vector<std::string>> params);

  //! callback to be registered to the webserver
  /*!
   * @param data Data to be written by webserver
   */
  void PayloadCallback(std::vector<uint8_t> data);

  //! callback to be registered to the player handlers
  /*!
   * @param res Response to be sent to the GUI
   */
  void ResponseCallback(Response res);

  //! Callback function after json response has been broadcasted
  /*!
   * @param id Message id
   */
  void CommandCompleted(size_t id);

  //! Consume json commands in an infinite loop
  void StartJsonConsumerLoop();

  //! Produce json response in an infinite loop
  void StartJsonProducerLoop();

  //! Send response back to the GUI
  /*!
   * @param data The payload data to be published to client
   * @param message_id The ID of current memrory slot for using later in memory pool manager.
   * @param session_ids List of session ids the message will be broadcasted to
   */
  void SendResponse(size_t id, const std::vector<std::uint8_t> &payload, std::vector<size_t> session_ids);

  std::vector<uint8_t> response_payload; //!< response payload
  std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver{nullptr};
  //!< Webserver instance
  gui_command_parser::CommandParser jsonCommandParser;                           //!< parser instance
  gui_command_parser::GuiCommand last_cmd{gui_command_parser::GuiCommand::NONE}; //!< Last command executed
  next::sdk::threadsafe_queue<std::string> payload_queue; //!< Threadsafe queue for storing json payload
  next::sdk::threadsafe_queue<Response> response_queue;   //!< Threadsafe queue for storing response
  std::thread json_consumer;                              //!< Thread which consumes the json string from the GUI
  std::thread json_producer;                              //!< Thread which creates the response string for the GUI
  std::vector<std::thread> threads;                       //!< Vector of threads
  std::atomic<bool> close_threads{false};                 //!< Close threads
  std::vector<std::string> current_filepath;              //!< Full path of the file
  std::vector<std::string> current_filename;              //!< Name of the file
  std::string requested_timestamp_string;                 //!< Requested jump timestamp
  std::string requested_timestamp_type;                   //!< Requested timestamp type for jump
  std::string requested_steps_string;                     //!< Reqested steps
  std::string updated_sil_factor_string;                  //!< Updated sil factor
  std::shared_ptr<next::control::events::PlayerCommandEvent> player_command_handler_{nullptr};
  std::shared_ptr<next::controlbridge::event_handler::PlayerEventDistributor> meta_event_distributor_{nullptr};
  std::shared_ptr<next::appsupport::ConfigServer> config_server_{nullptr};
  bool player_running{false};                             //! player state
  bool player_paused{false};                              //! player paused
  SimulationType simul_type{SimulationType::InvalidMode}; //! Simulation Type
  std::string exe_path;                                   //! Exe file path
  std::string exe_name;                                   //! Exe file name
  std::string conf_path;                                  //! ini file path
  std::string conf_name;                                  //! ini file name
  std::string carmaker_path;                              //! Carmaker path
};

} // namespace command_handler
} // namespace controlbridge
} // namespace next

#endif // NEXT_CONTROLBRIDGE_PLAYER_CONTROL_MANAGER_
