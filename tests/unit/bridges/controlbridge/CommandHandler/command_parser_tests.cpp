#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include "gui_command_parser.h"

namespace next {
namespace controlbridge {
namespace unit_test {

TEST(COMMAND_PARSER, Parsejson) {
  next::controlbridge::gui_command_parser::CommandParser parser;
  parser.parseJson(R"({
  "channel": "player",
  "event": "UserRequestedInputOpen",
  "source": "NextGUI",
  "payload": {
    "fileName": "C:/Users/myUser/myInputFile.rrec"
  }
})");
  EXPECT_EQ(parser.GetCommand(), next::controlbridge::gui_command_parser::GuiCommand::OPEN);
}
TEST(COMMAND_PARSER, ParseJsonSilFactor) {
  next::controlbridge::gui_command_parser::CommandParser parser;
  parser.parseJson(R"({
  "id":"b45a0262-d155-4b36-860d-fe35a217a691",
  "channel":"player",
  "event":"UserUpdatedSilFactor",
  "source":"NextGUI",
  "payload":{
     "sil":"8"
   }
})");
  EXPECT_EQ(parser.GetCommand(), next::controlbridge::gui_command_parser::GuiCommand::UPDATESILFACTOR);
}
} // namespace unit_test
} // namespace controlbridge
} // namespace next
