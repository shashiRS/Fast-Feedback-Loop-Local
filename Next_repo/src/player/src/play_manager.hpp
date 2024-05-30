/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     play_manager.hpp
 * @brief    Player Manager Class
 * Controls the interractions with the player
 **/

#ifndef PLAY_MANAGER_H_
#define PLAY_MANAGER_H

#include <condition_variable>
#include <memory>
#include <mutex>

#include <ecal/ecal.h>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/log/logger.hpp>
#include <mts/runtime/memory_allocator.hpp>
#include <mts/runtime/offline/replay_proxy.hpp>
#include <mts/runtime/service_container.hpp>
NEXT_RESTORE_WARNINGS

#include <next/control/event_types/player_command.h>
#include <next/sdk/sdk.hpp>

#include "publisher/publisher_manager.hpp"
#include "reader/file_reader.hpp"
#include "resource_manager/resource_manager.hpp"
#include "sdk_deprecated/threadsafe_queue.hpp"
#include "state_machine/player_state_machine.hpp"

namespace mts {
class player;
}

/*!
 *  This class initializes and controls the player components
 */
class PlayManager {
public:
  PlayManager();
  virtual ~PlayManager();
  bool Execute();
  void HandleRequestMessage();
  bool Initialize();
  void RunCommandLine();
  void Shutdown();
  void UpdateSilFactorFromConfig(const float silFactor);

private:
  bool is_initialized_{false};

  // Program arguments taken from main
  // ArgumentHandler program_args_;

  std::shared_ptr<mts::runtime::memory_allocator> memory_allocator_;
  std::shared_ptr<std::mutex> main_mutex_;
  std::shared_ptr<std::condition_variable> condition_var_main_;
  // Request request_;
  next::control::message::request request_;
  // new control server impl
  std::shared_ptr<next::control::events::PlayerCommandEvent> command_handler_server_;
  next::player::threadsafe_queue<next::control::message::request> message_queue_{};

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_;
  std::shared_ptr<FileReader> file_reader_ptr_;
  std::shared_ptr<IResourceManager> resource_manager_ptr_;
};

#endif // PLAY_MANAGER_H_
