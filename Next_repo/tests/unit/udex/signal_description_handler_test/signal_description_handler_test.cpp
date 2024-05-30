#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <string>
#include <vector>

#include <json/json.h>

#include "signal_description_handler.h"

namespace next {
namespace udex {
namespace ecal_util {

TEST(SignalDescriptionHandler, MakeStringFromVec_test) {

  auto signal_description_hanlder = new SignalDescriptionHandler();
  std::vector<char> test_input = {'t', 'e', 's', 't'};
  EXPECT_EQ(signal_description_hanlder->MakeStringFromVec(test_input), "test");

  std::vector<char> test_input1 = {};
  EXPECT_EQ(signal_description_hanlder->MakeStringFromVec(test_input1), "");

  std::vector<char> test_input2 = {'t', 'e', 's', 't', '\n', '!', 'x', '@', '%', '!', '&', '\0'};
  EXPECT_NE(signal_description_hanlder->MakeStringFromVec(test_input2), "test\n!x@%!&\0");

  std::vector<char> test_input3 = {'A', 'x', 'f', 't', '\n', '!', 'x', '@', '%', '!', '&', '\0', '\t'};
  EXPECT_NE(signal_description_hanlder->MakeStringFromVec(test_input3), "Axft\n!x@%!&\0\t");
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, SetMetaInformation_test) {

  auto signal_description_hanlder = new SignalDescriptionHandler();
  BasicInformation basic_info_test;
  basic_info_test.parent_url = "device.group.port";
  basic_info_test.mode = PackageMode::PACKAGE_MODE_SDL;
  basic_info_test.dummy = 100;
  EXPECT_EQ(signal_description_hanlder->SetBasicInformation(basic_info_test), true);

  BasicInformation basic_info_test_read;
  basic_info_test_read = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(basic_info_test.parent_url, basic_info_test_read.parent_url) << "read meta info wrong";
  EXPECT_EQ(basic_info_test.dummy, basic_info_test_read.dummy) << "read meta info wrong";
  EXPECT_EQ(basic_info_test.mode, basic_info_test_read.mode) << "read meta info wrong";
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, SetMetaInformation_test_1) {

  auto signal_description_hanlder = new SignalDescriptionHandler();
  BasicInformation basic_info_test = {};
  EXPECT_EQ(signal_description_hanlder->SetBasicInformation(basic_info_test), true);

  BasicInformation basic_info_test_read = {};
  basic_info_test_read = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(basic_info_test.parent_url, basic_info_test_read.parent_url) << "read meta info wrong";
  EXPECT_EQ(basic_info_test.dummy, basic_info_test_read.dummy) << "read meta info wrong";
  EXPECT_EQ(basic_info_test.mode, basic_info_test_read.mode) << "read meta info wrong";
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, SetBinarySignalDescription_test) {
  std::vector<char> test_input = {'t', 'e', 's', 't', '\n'};
  auto signal_description_hanlder = new SignalDescriptionHandler();
  EXPECT_EQ(signal_description_hanlder->SetBinarySignalDescription(test_input), true)
      << "setting binary description wrong";
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, ToString_test) {
  auto signal_description_hanlder = new SignalDescriptionHandler();

  /*TEST 1 : with valid data*/
  BasicInformation basic_info_test;
  std::string test_url = "device.group.port";
  int dummy_value = 100;
  uint16_t test_instance = 1;
  uint16_t test_vaddr = 123;
  uint16_t test_source_id = 321;
  uint16_t test_cycle_id = 1010;
  basic_info_test.parent_url = test_url;
  basic_info_test.data_source_name = "device";
  basic_info_test.view_name = "view";
  basic_info_test.group_name = "group";
  basic_info_test.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_MTA;
  basic_info_test.mode = PackageMode::PACKAGE_MODE_SDL;
  basic_info_test.dummy = dummy_value;
  basic_info_test.instance_id = test_instance;
  basic_info_test.vaddr = test_vaddr;
  basic_info_test.source_id = test_source_id;
  basic_info_test.cycle_id = test_cycle_id;
  Json::Value data_creator;
  EXPECT_EQ(signal_description_hanlder->SetBasicInformation(basic_info_test), true) << "meta info setting failed";

  std::vector<char> test_input = {'t', '\n', 's', 't'};
  EXPECT_EQ(signal_description_hanlder->SetBinarySignalDescription(test_input), true)
      << "setting binary description wrong";

  data_creator["basic_info"]["parent_url"] = basic_info_test.parent_url;
  data_creator["basic_info"]["data_source_name"] = basic_info_test.data_source_name;
  data_creator["basic_info"]["view_name"] = basic_info_test.view_name;
  data_creator["basic_info"]["group_name"] = basic_info_test.group_name;
  data_creator["basic_info"]["format_type"] = next::sdk::types::getPackageFormatTypeString(basic_info_test.format_type);
  data_creator["basic_info"]["dummy"] = basic_info_test.dummy;
  data_creator["basic_info"]["mode"] = basic_info_test.mode;
  data_creator["basic_info"]["vaddr"] = test_vaddr;
  data_creator["basic_info"]["source_id"] = test_source_id;
  data_creator["basic_info"]["cycle_id"] = test_cycle_id;
  data_creator["basic_info"]["instance_id"] = test_instance;
  data_creator["binary_data_description"] = signal_description_hanlder->MakeStringFromVec(test_input);
  auto test = signal_description_hanlder->ToString();
  Json::FastWriter writer;
  std::string raw_json_string = writer.write(data_creator);
  // auto test1 = data_creator.toStyledString();
  EXPECT_EQ(test, raw_json_string) << "To string failed";
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, Base64Test) {
  //  auto signal_description_hanlder = new SignalDescriptionHandler();
  //
  //  /*TEST 1 : with valid data*/
  //  BasicInformation basic_info_test;
  //  std::string test_url = "device.group.port";
  //  int dummy_value = 100;
  //  basic_info_test.parent_url = test_url;
  //  basic_info_test.mode = PackageMode::SDL;
  //  basic_info_test.dummy = dummy_value;
  //  Json::Value data_creator;
  //  EXPECT_EQ(signal_description_hanlder->SetBasicInformation(basic_info_test), true)<<"meta info setting failed";
  //
  //  std::vector< char> test_input = {'t','\n','\0','t'};
  //  EXPECT_EQ(signal_description_hanlder->SetBinarySignalDescription(test_input), true) << "setting binary description
  //  wrong";
  //
  //  data_creator["basic_info"]["parent_url"] = basic_info_test.parent_url;
  //  data_creator["basic_info"]["dummy"] = basic_info_test.mode;
  //  data_creator["basic_info"]["mode"] = basic_info_test.dummy;
  //  data_creator["binary_data_description"] = signal_description_hanlder->MakeStringFromVec(test_input);
  //
  //  EXPECT_EQ(signal_description_hanlder->ToString(), data_creator.toStyledString()) << "To string failed";
}

TEST(SignalDescriptionHandler, ParseSignalDescriptionFromString_test_1) {
  auto signal_description_hanlder = new SignalDescriptionHandler();
  std::string input;
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), false);
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, ParseSignalDescriptionFromString_test_2) {
  auto signal_description_hanlder = new SignalDescriptionHandler();
  // TEST 2 : input description has differenet fields missing
  // reuse the test string from the ToString_test test and just delete the words indicated in the following comments

  // full string
  std::string input =
      "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
      "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":0,\"parent_url\":\"device.group.port\",\"source_"
      "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);

  // input doesn't have "basic_info";
  input = "{\n\t\"\" : \n\t{\n\t\t\"data_source_name\" : \"device\",\n\t\t\"dummy\" : 100,\n\t\t\"view_name\" : "
          "\"view\",\n\t\t\"group_name\" : \"group\",\n\t\t\"format_type\" : 10,\n\t\t\"mode\" : "
          "0,\n\t\t\"parent_url\" : \"device.group.port\"\n\t},\n\t\"binary_data_description\" : \"t\\nst\"\n}\n";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), false);

  // input have basic_info  but no "parent_url" and "dummy";
  input = "{\n\t\"basic_info\" : \n\t{\n\t\t\"data_source_name\" : \"device\",\n\t\t\"\" : 100,\n\t\t\"view_name\" : "
          "\"view\",\n\t\t\"group_name\" : \"group\",\n\t\t\"format_type\" : 10,\n\t\t\"mode\" : 0,\n\t\t\"\" : "
          "\"device.group.port\"\n\t},\n\t\"binary_data_description\" : \"t\\nst\"\n}\n";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), false);

  // input have basic_info  and  dummy and not "parent_url";
  input = "{\n\t\"basic_info\" : \n\t{\n\t\t\"data_source_name\" : \"device\",\n\t\t\"dummy\" : "
          "100,\n\t\t\"view_name\" : \"view\",\n\t\t\"group_name\" : \"group\",\n\t\t\"format_type\" : "
          "10,\n\t\t\"mode\" : 0,\n\t\t\"\" : \"device.group.port\"\n\t},\n\t\"binary_data_description\" : "
          "\"t\\nst\"\n}\n";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), false);

  // input have basic_info  and  no "binary_data_description";
  input =
      "{\n\t\"basic_info\" : \n\t{\n\t\t\"data_source_name\" : \"device\",\n\t\t\"dummy\" : 100,\n\t\t\"view_name\" : "
      "\"view\",\n\t\t\"group_name\" : \"group\",\n\t\t\"format_type\" : 10,\n\t\t\"mode\" : 0,\n\t\t\"parent_url\" : "
      "\"device.group.port\"\n\t},\n\t\"\" : \"t\\nst\"\n}\n";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), false);
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, ParseSignalDescriptionFromString_test_3) {
  // TEST 3 : test for SignalDescription->binary_data_description
  auto signal_description_hanlder = new SignalDescriptionHandler();

  // reuse the same string from ToString_test and replace "t\\nst\" with "TEST_BINARY_DESCRIPTION\"

  std::string input =
      "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
      "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":0,\"parent_url\":\"device.group.port\",\"source_"
      "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"TEST_BINARY_DESCRIPTION\"}";

  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  std::vector<char> teststring = {'T', 'E', 'S', 'T', '_', 'B', 'I', 'N', 'A', 'R', 'Y', '_',
                                  'D', 'E', 'S', 'C', 'R', 'I', 'P', 'T', 'I', 'O', 'N'};
  EXPECT_EQ(signal_description_hanlder->GetSignalDescription(), teststring);
  EXPECT_EQ(signal_description_hanlder->GetParentUrl(), "device.group.port");
  BasicInformation ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.dummy, 100);
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_SDL);
  EXPECT_EQ(ouput.parent_url, "device.group.port");
  EXPECT_EQ(ouput.data_source_name, "device");
  EXPECT_EQ(ouput.view_name, "view");
  EXPECT_EQ(ouput.group_name, "group");
  EXPECT_EQ(ouput.format_type, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_MTA);
  // is there any way to check contents of binary_data_description??
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, ParseSignalDescriptionFromString_test_modes) {
  // TEST 3 : test for SignalDescription->binary_data_description
  auto signal_description_hanlder = new SignalDescriptionHandler();
  // reuse the same string from ToString_test and just adapt the mode value
  std::string input =
      "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
      "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":0,\"parent_url\":\"device.group.port\",\"source_"
      "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  BasicInformation ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_SDL);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":1,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_CAN);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":2,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_FIBEX);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":3,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_BINARY);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":4,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_DEBUG);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":-1,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.mode, PACKAGE_MODE_NOTSET);
  delete signal_description_hanlder;
}

TEST(SignalDescriptionHandler, ParseSignalDescriptionFromString_test_format_type) {
  auto signal_description_hanlder = new SignalDescriptionHandler();
  // reuse the same string from ToString_test and just adapt the format_type value
  std::string input =
      "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
      "timebase\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":1,\"parent_url\":\"device.group.port\",\"source_"
      "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  BasicInformation ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.format_type, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_TIMEBASE);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"mts."
          "mta\",\"group_name\":\"group\",\"instance_id\":1,\"mode\":1,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.format_type, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_MTA);

  input = "{\"basic_info\":{\"cycle_id\":1010,\"data_source_name\":\"device\",\"dummy\":100,\"format_type\":\"\","
          "\"group_name\":\"group\",\"instance_id\":1,\"mode\":1,\"parent_url\":\"device.group.port\",\"source_"
          "id\":321,\"vaddr\":123,\"view_name\":\"view\"},\"binary_data_description\":\"t\\nst\"}";
  EXPECT_EQ(signal_description_hanlder->ParseSignalDescriptionFromString(input), true);
  ouput = signal_description_hanlder->GetBasicInformation();
  EXPECT_EQ(ouput.format_type, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN);
  delete signal_description_hanlder;
}

} // namespace ecal_util
} // namespace udex
} // namespace next
