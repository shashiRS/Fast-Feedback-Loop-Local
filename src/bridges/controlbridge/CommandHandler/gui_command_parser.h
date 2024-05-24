/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     command_parser.h
 * @brief    small module to parse json commands
 *
 * Parse commands received from GUI and return a map of commands and parameters
 **/

#ifndef CONTROLBRIDGE_COMMAND_PARSER_
#define CONTROLBRIDGE_COMMAND_PARSER_

#include <map>
#include <string>
#include <vector>

#include <next/control/event_types/player_command.h>

namespace next {
namespace controlbridge {
namespace gui_command_parser {

//!< Enum of commands
enum class GuiCommand : uint8_t {
  NONE,
  OPEN,
  PLAY,
  PAUSE,
  CLOSE,
  FASTFORWARD,
  REWIND,
  JUMP,
  STEPBACKWARD,
  STEPFORWARD,
  JUMPTOEND,
  OPENDIRECTORY,
  DIRECTORYLOADED,
  OPENBINARY,
  BINARYLOADED,
  UPDATESILFACTOR,
  SOFTRESET,
  PLAY_UNTIL,
  CHECKWEBSOCKETCONNECTION
};

//!< Mapping of strings to enum
static const std::map<std::string, GuiCommand> stringToCommand = {
    {"UserRequestedInputOpen", GuiCommand::OPEN},
    {"UserPressedPlayButton", GuiCommand::PLAY},
    {"UserPressedPlayUntilTimestampButton", GuiCommand::PLAY_UNTIL},
    {"UserPressedPauseButton", GuiCommand::PAUSE},
    {"UserPressedInputClearButton", GuiCommand::CLOSE},
    {"UserPressedJumpToBeginButton", GuiCommand::REWIND},
    {"UserPressedFastForwardButton", GuiCommand::FASTFORWARD},
    {"UserDraggedSlider", GuiCommand::JUMP},
    {"UserRequestedJumpToTimestamp", GuiCommand::JUMP},
    {"UserPressedStepButton", GuiCommand::STEPFORWARD},
    {"UserPressedStepButton", GuiCommand::STEPBACKWARD},
    {"UserPressedJumpToEndButton", GuiCommand::JUMPTOEND},
    {"UserRequestedProjectDirectoryOpen", GuiCommand::OPENDIRECTORY},
    {"ProjectDirectoryIsLoaded", GuiCommand::DIRECTORYLOADED},
    {"UserRequestedBinaryOpen", GuiCommand::OPENBINARY},
    {"BinaryIsLoaded", GuiCommand::BINARYLOADED},
    {"UserUpdatedSilFactor", GuiCommand::UPDATESILFACTOR},
    {"UserPressedSoftResetButton", GuiCommand::SOFTRESET},
    {"UserPressedCheckWebsocketConnection", GuiCommand::CHECKWEBSOCKETCONNECTION}};

//!< Mapping of enum to strings
static const std::map<GuiCommand, std::string> CommandToString = {
    {GuiCommand::OPEN, "RecordingIsBeingLoaded"},
    {GuiCommand::PLAY, "PlaybackIsPlaying"},
    {GuiCommand::PLAY_UNTIL, "UserPressedPlayUntilTimestampButton"},
    {GuiCommand::PAUSE, "PlaybackIsPaused"},
    {GuiCommand::CLOSE, "RecordingIsClosing"},
    {GuiCommand::FASTFORWARD, "RecordingIsClosing"},
    {GuiCommand::REWIND, "RecordingIsRewinded"},
    {GuiCommand::JUMP, "PlaybackJumped"},
    {GuiCommand::STEPFORWARD, "PlaybackStepedForward"},
    {GuiCommand::STEPBACKWARD, "PlaybackStepedBackward"},
    {GuiCommand::JUMPTOEND, "RecordingJumpedToEnd"},
    {GuiCommand::OPENDIRECTORY, "UserRequestedProjectDirectoryOpen"},
    {GuiCommand::DIRECTORYLOADED, "ProjectDirectoryIsLoaded"},
    {GuiCommand::OPENBINARY, "UserRequestedBinaryOpen"},
    {GuiCommand::BINARYLOADED, "BinaryIsLoaded"},
    {GuiCommand::UPDATESILFACTOR, "SilFactorIsUpdated"},
    {GuiCommand::CHECKWEBSOCKETCONNECTION, "CHECKWEBSOCKETCONNECTION"}};

//! Class to parse the commands
class CommandParser {
public:
  //! Parse Json String
  /*!
   * If json string can be parsed succesfuly it sets command and param variables
   *
   * @param jsonMessage Json string to be parsed
   */
  bool parseJson(const std::string &jsonMessage);

  //! Command Getter
  GuiCommand GetCommand();

  //! Params Getter
  std::map<std::string, std::vector<std::string>> GetParams();

  //! Create response to be sent back to GUI for the current command/status
  /*!
   *
   * @param current_step current step being performed
   * @param total_step total no. of steps to be performed
   */
  std::vector<uint8_t> CreateResponseString(GuiCommand command, std::map<std::string, std::string> payload);

  //! Create error response
  std::vector<uint8_t> CreateErrorResponseString(GuiCommand command);

private:
  GuiCommand command_{GuiCommand::NONE};                   //!< Parsed command
  std::map<std::string, std::vector<std::string>> params_; //!< Map of params e.g. "forward": "true"
};

} // namespace gui_command_parser
} // namespace controlbridge
} // namespace next

#endif // CONTROLBRIDGE_COMMAND_PARSER_
