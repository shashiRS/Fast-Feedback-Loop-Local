#include "player_control_manager.h"

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

namespace next {
namespace controlbridge {
namespace unit_test {

TEST(COMMAND_HANDLER, DISABLED_HandleOpen) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserRequestedInputOpen",
  "source": "NextGUI",
  "payload": {
    "fileName": "C:/Users/myUser/myInputFile.rrec"
  }
})";
  handler.HandleJsonPayload(jsonString);
  std::string responseString =
      R"({"channel":"player","event":"RecordingIsBeingLoaded","payload":{"fileName":"C:/Users/myUser/myInputFile.rrec","filePath":"myInputFile.rrec","loadingProgress":"1.000000"},"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseData(responseString.begin(), responseString.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  next::controlbridge::command_handler::Response res;
  // int cnt = 0;
  // while (cnt <= 10) {
  handler.PopResponseQueue(res);
  handler.HandleResponse(res);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //  cnt++;
  //}
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::OPEN);
  EXPECT_EQ(responseData, handler.GetLastResponse());
  handler.ShutDown();
}

TEST(COMMAND_HANDLER, DISABLED_HandlePause) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserRequestedInputOpen",
  "source": "NextGUI",
  "payload": {
    "fileName": "C:/Users/myUser/myInputFile.rrec"
  }
})";
  handler.HandleJsonPayload(jsonString);
  std::string responseString =
      R"({"channel":"player","event":"RecordingIsBeingLoaded","payload":{"fileName":"C:/Users/myUser/myInputFile.rrec","filePath":"myInputFile.rrec","loadingProgress":"1.000000"},"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseData(responseString.begin(), responseString.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::string jsonStringPause = R"({
  "channel": "player",
  "event": "UserPressedPauseButton",
  "source": "NextGUI",
  "payload": {}
})";
  handler.HandleJsonPayload(jsonStringPause);
  //   std::string responseStringPause =
  //       R"({"channel":"player","event":"PlaybackIsPaused","payload":{"currentSimulationStep":"50","timestamp":"1","totalSimulationSteps":"300"},"source":"ControlBridge"}
  // )";
  //   std::vector<uint8_t> responseDataPause(responseStringPause.begin(), responseStringPause.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  next::controlbridge::command_handler::Response res;
  handler.PopResponseQueue(res);
  handler.HandleResponse(res);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::PAUSE);
  // EXPECT_EQ(responseDataPause, handler.GetLastResponse());
  handler.ShutDown();
}

TEST(COMMAND_HANDLER, DISABLED_HandlePlay) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserRequestedInputOpen",
  "source": "NextGUI",
  "payload": {
    "fileName": "C:/Users/myUser/myInputFile.rrec"
  }
})";
  handler.HandleJsonPayload(jsonString);
  std::string responseString =
      R"({"channel":"player","event":"RecordingIsBeingLoaded","payload":{"fileName":"C:/Users/myUser/myInputFile.rrec","filePath":"myInputFile.rrec","loadingProgress":"1.000000"},"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseData(responseString.begin(), responseString.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  next::controlbridge::command_handler::Response res;
  // int cnt = 0;
  // while (cnt <= 10) {
  handler.PopResponseQueue(res);
  handler.HandleResponse(res);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //  cnt++;
  //}
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::OPEN);
  EXPECT_EQ(responseData, handler.GetLastResponse());
  std::string jsonStringPlay = R"({
  "channel": "player",
  "event": "UserPressedPlayButton",
  "source": "NextGUI",
  "payload": {
    "forward": true
  }
})";
  handler.HandleJsonPayload(jsonStringPlay);
  std::string responseStringPlay =
      R"({"channel":"player","event":"PlaybackIsPlaying","payload":{"currentSimulationStep":"300","direction":"forward","fileName":"test.rrec","speedMultiple":"1","timestamp":"1","totalSimulationSteps":"300"},"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseDataPlay(responseStringPlay.begin(), responseStringPlay.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // cnt = 0;
  // while (cnt <= 30) {
  //   handler.PopResponseQueue(res);
  //   handler.HandleResponse(res);
  //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //   cnt++;
  // }
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::PLAY);
  // EXPECT_EQ(responseDataPlay, handler.GetLastResponse());
  handler.ShutDown();
}

TEST(COMMAND_HANDLER, DISABLED_HandleClose) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserRequestedInputOpen",
  "source": "NextGUI",
  "payload": {
    "fileName": "C:/Users/myUser/myInputFile.rrec"
  }
})";
  handler.HandleJsonPayload(jsonString);
  std::string responseString =
      R"({"channel":"player","event":"RecordingIsBeingLoaded","payload":{"fileName":"C:/Users/myUser/myInputFile.rrec","filePath":"myInputFile.rrec","loadingProgress":"1.000000"},"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseData(responseString.begin(), responseString.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  next::controlbridge::command_handler::Response res;
  // int cnt = 0;
  // while (cnt <= 10) {
  handler.PopResponseQueue(res);
  handler.HandleResponse(res);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //  cnt++;
  //}
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::OPEN);
  EXPECT_EQ(responseData, handler.GetLastResponse());
  std::string jsonStringClose = R"({
  "channel": "player",
  "event": "UserPressedInputClearButton",
  "source": "NextGUI",
  "payload": {}
})";
  handler.HandleJsonPayload(jsonStringClose);
  std::string responseStringClose =
      R"({"channel":"player","event":"RecordingIsClosing","payload":null,"source":"ControlBridge"}
)";
  std::vector<uint8_t> responseDataClose(responseStringClose.begin(), responseStringClose.end());
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  handler.PopResponseQueue(res);
  handler.HandleResponse(res);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::CLOSE);
  EXPECT_EQ(responseDataClose, handler.GetLastResponse());
  handler.ShutDown();
}

TEST(COMMAND_HANDLER, DISABLED_HandleRewind) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserPressedJumpToBeginButton",
  "source": "NextGUI",
  "payload": {}
})";
  handler.HandleJsonPayload(jsonString);
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::REWIND);
}

TEST(COMMAND_HANDLER, DISABLED_HandleJump) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({
  "channel": "player",
  "event": "UserDraggedSlider",
  "source": "NextGUI",
  "payload": {
    "cycleNumber": 5
  }
})";
  handler.HandleJsonPayload(jsonString);
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::JUMP);
}

TEST(COMMAND_HANDLER, DISABLED_HandleIllformedCommand) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"({"event": "fly"})";
  handler.HandleJsonPayload(jsonString);
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::NONE);
}

TEST(COMMAND_HANDLER, DISABLED_HandleOpenProjectDirectory) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"(
  {
    "channel": "player",
    "event": "UserRequestedProjectDirectoryOpen",
    "source": "NextGUI",
    "payload": {
      "directory": "C:/Users/myProjectDirectory"
    }
  }
  )";
  handler.HandleJsonPayload(jsonString);
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::OPENDIRECTORY);
}

TEST(COMMAND_HANDLER, DISABLED_HandleOpenBinary) {
  next::controlbridge::command_handler::CommandHandler handler;
  std::string jsonString = R"(
  {
    "channel": "player",
    "event": "UserRequestedBinaryOpen",
    "source": "NextGUI",
    "payload": {
      "fileName": "C:/Users/myUser/carmaker.exe"
    }
  }
  )";
  handler.HandleJsonPayload(jsonString);
  EXPECT_EQ(handler.GetLastCmd(), next::control::commands::Command::OPENBINARY);
}

} // namespace unit_test
} // namespace controlbridge
} // namespace next
