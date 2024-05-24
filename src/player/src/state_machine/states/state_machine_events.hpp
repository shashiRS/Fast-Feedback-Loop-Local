/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_achine_events.hpp
 * @brief    Events used in the control of the player state machine
 *
 **/
#ifndef STATE_MACHINE_EVENTS_H
#define STATE_MACHINE_EVENTS_H
#include <string>

#include "./../error_codes.hpp"

enum class ESTATES { E_NEW, E_OPENING, E_READY, E_UNLOAD_RECORDING, E_SKIP_TO, E_PLAY, E_ERROR, E_SHUTDOWN };

// Events
struct baseCommand {
  baseCommand(){};
  baseCommand(std::string name) : name(name) {}
  std::string name = "default";
};

struct command_OPEN : public baseCommand {
  command_OPEN(const std::vector<std::string> &names) : recName(names), baseCommand::baseCommand("command_OPEN") {}

  std::vector<std::string> recName;
};
struct command_CLOSE : public baseCommand {
  command_CLOSE() : baseCommand::baseCommand("command_CLOSE") {}
};
struct command_STEP_FORWARD : public baseCommand {
  command_STEP_FORWARD(int32_t steps) : steps(steps), baseCommand::baseCommand("command_STEP_FORWARD") {}
  command_STEP_FORWARD(int32_t steps, uint64_t timestamp_until)
      : steps(steps), timestamp_until(timestamp_until),
        until_play(true), baseCommand::baseCommand("command_STEP_FORWARD") {}
  int32_t steps = 0;
  uint64_t timestamp_until;
  bool until_play = false;
};

struct test_command_READY : public baseCommand {
  test_command_READY() : baseCommand::baseCommand("test_command_READY") {}
};
struct command_PAUSE : public baseCommand {
  command_PAUSE() : baseCommand::baseCommand("command_PAUSE") {}
};
struct command_JUMP : public baseCommand {
  command_JUMP(uint64_t skipTimestamp, std::string timestamp_type)
      : skipTimestamp(skipTimestamp), timestampType(timestamp_type), baseCommand::baseCommand("command_JUMP") {}
  uint64_t skipTimestamp;
  std::string timestampType;
};
struct event_recording_loaded_OK : public baseCommand {
  event_recording_loaded_OK() : baseCommand::baseCommand("event_recording_loaded_OK") {}
};
struct event_ERROR : public baseCommand {
  event_ERROR(PlayerError err) : error(err), baseCommand::baseCommand("event_ERROR") {}
  PlayerError error;
};
struct event_handled_error : public baseCommand {
  event_handled_error() : baseCommand::baseCommand("event_handled_error") {}
};
struct event_unhandled_error : public baseCommand {
  event_unhandled_error() : baseCommand::baseCommand("event_unhandled_error") {}
};
struct event_recording_unloaded_OK : public baseCommand {
  event_recording_unloaded_OK() : baseCommand::baseCommand("event_recording_unloaded_OK") {}
};
struct event_jump_complete : public baseCommand {
  event_jump_complete() : baseCommand::baseCommand("event_jump_complete") {}
};
struct event_step_complete : public baseCommand {
  event_step_complete() : baseCommand::baseCommand("event_step_complete") {}
};
struct event_shutdown : public baseCommand {
  event_shutdown() : baseCommand::baseCommand("event_shutdown") {}
};
struct reset_on_exception : public baseCommand {
  reset_on_exception() : baseCommand::baseCommand("event_exception_cought") {}
};
#endif
