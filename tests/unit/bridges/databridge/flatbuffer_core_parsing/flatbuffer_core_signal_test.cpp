#include <cip_test_support/gtest_reporting.h>
#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>
#include <next/bridgesdk/schema/core/signal_generated.h>
#include <next/bridgesdk/schema/core/signal_update_generated.h>
#include <next/bridgesdk/schema/core/types_generated.h>
#include <boost/filesystem.hpp>
#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(CORE_SIGNAL_FLATBUFFER_CHECK, core_signal_create_signal_list) {
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  // create a signal list with 2 signals
  std::string signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.HeaderObjList.iNumOfUsedObjects";
  size_t hash = std::hash<std::string>{}(signal_name);
  auto type_double = Core::CreateTypeDouble(builder, 55.32);
  auto signal_num_of_objects = Core::CreateSignal(builder, hash, Core::AnyValue::TypeDouble, type_double.Union());
  auto signal_info_num_of_objects = Core::CreateSignalInfoDirect(builder, signal_name.c_str(), hash);

  signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.sigHeader.uiMeasurementCounter";
  hash = std::hash<std::string>{}(signal_name);
  auto type_uint32 = Core::CreateTypeUInt32(builder, 4242);
  auto signal_measurement_counter = Core::CreateSignal(builder, hash, Core::AnyValue::TypeUInt32, type_uint32.Union());
  auto signal_info_measurement_counter = Core::CreateSignalInfoDirect(builder, signal_name.c_str(), hash);

  std::vector<flatbuffers::Offset<Core::SignalInfo>> signal_info;
  signal_info.push_back(signal_info_num_of_objects);
  signal_info.push_back(signal_info_measurement_counter);

  std::vector<flatbuffers::Offset<Core::Signal>> signals;
  signals.push_back(signal_num_of_objects);
  signals.push_back(signal_measurement_counter);

  auto sig_info_vector = builder.CreateVector(signal_info);
  auto sig_vector = builder.CreateVector(signals);
  auto signal_list = Core::CreateSignalList(builder, sig_info_vector, sig_vector);

  builder.Finish(signal_list);

  // parse signal list to json
  outputmanager::PluginDataClassParser data_class_parser;

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "signalList";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "";
  sensor_info.timestamp_microsecond = 11160351874;
  std::pair<std::string, std::string> structure_version_pair{"SignalList", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "signal.fbs";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  const std::string expected_json_signal_list_output = R"(
  {
  "dataStream": {
    "signalList": {
      "ADC4xx": {
        "FDP_CSB": {
          "m_tpfOutput": {
            "HeaderObjList": {
              "iNumOfUsedObjects": 55.32
            },
            "sigHeader": {
              "uiMeasurementCounter": 4242
            }
          }
        }
      }
    },
    "timestamp": 11160351874
  }
 })";

  Json::Value output_json, expected_json;
  Json::Reader reader;

  bool parseSuccess = reader.parse(result, output_json, false);
  ASSERT_EQ(parseSuccess, true) << "parsing output json object failed";
  parseSuccess &= reader.parse(expected_json_signal_list_output, expected_json, false);
  ASSERT_EQ(parseSuccess, true) << "parsing expected json object failed";

  ASSERT_EQ(output_json, expected_json) << "Output JSON different than expected";
}

TEST(CORE_SIGNAL_FLATBUFFER_CHECK, test_signal_hash_caching) {
  // this test checks the signal hash caching by parsing twice the same signals, but sending signal info only first time

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  // create a signal list with 2 signals
  std::string signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.HeaderObjList.iNumOfUsedObjects";
  size_t hash = std::hash<std::string>{}(signal_name);
  auto type_double = Core::CreateTypeDouble(builder, 55.32);
  auto signal_num_of_objects = Core::CreateSignal(builder, hash, Core::AnyValue::TypeDouble, type_double.Union());
  auto signal_info_num_of_objects = Core::CreateSignalInfoDirect(builder, signal_name.c_str(), hash);

  signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.sigHeader.uiMeasurementCounter";
  hash = std::hash<std::string>{}(signal_name);
  auto type_uint32 = Core::CreateTypeUInt32(builder, 4242);
  auto signal_measurement_counter = Core::CreateSignal(builder, hash, Core::AnyValue::TypeUInt32, type_uint32.Union());
  auto signal_info_measurement_counter = Core::CreateSignalInfoDirect(builder, signal_name.c_str(), hash);

  std::vector<flatbuffers::Offset<Core::SignalInfo>> signal_info;
  signal_info.push_back(signal_info_num_of_objects);
  signal_info.push_back(signal_info_measurement_counter);

  std::vector<flatbuffers::Offset<Core::Signal>> signals;
  signals.push_back(signal_num_of_objects);
  signals.push_back(signal_measurement_counter);

  auto sig_info_vector = builder.CreateVector(signal_info);
  auto sig_vector = builder.CreateVector(signals);
  auto signal_list_with_sig_info = Core::CreateSignalList(builder, sig_info_vector, sig_vector);
  builder.Finish(signal_list_with_sig_info);

  bridgesdk::plugin::FlatbufferPackage flatbuffer_sig_info;
  flatbuffer_sig_info.fbs_binary = builder.GetBufferPointer();
  flatbuffer_sig_info.fbs_size = builder.GetSize();

  // create second signal list without signal info data
  flatbuffers::FlatBufferBuilder builder2;

  signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.HeaderObjList.iNumOfUsedObjects";
  hash = std::hash<std::string>{}(signal_name);
  type_double = Core::CreateTypeDouble(builder2, 42.42);
  signal_num_of_objects = Core::CreateSignal(builder2, hash, Core::AnyValue::TypeDouble, type_double.Union());
  signal_info_num_of_objects = Core::CreateSignalInfoDirect(builder2, signal_name.c_str(), hash);

  signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.sigHeader.uiMeasurementCounter";
  hash = std::hash<std::string>{}(signal_name);
  type_uint32 = Core::CreateTypeUInt32(builder2, 1212);
  signal_measurement_counter = Core::CreateSignal(builder2, hash, Core::AnyValue::TypeUInt32, type_uint32.Union());
  signal_info_measurement_counter = Core::CreateSignalInfoDirect(builder2, signal_name.c_str(), hash);

  std::vector<flatbuffers::Offset<Core::Signal>> signals_;
  signals_.push_back(signal_num_of_objects);
  signals_.push_back(signal_measurement_counter);

  sig_vector = builder2.CreateVector(signals_);
  std::vector<flatbuffers::Offset<Core::SignalInfo>> info_empty;
  auto info_empty_vector = builder2.CreateVector(info_empty);
  auto signal_list_without_sig_info = Core::CreateSignalList(builder2, info_empty_vector, sig_vector);
  builder2.Finish(signal_list_without_sig_info);

  bridgesdk::plugin::FlatbufferPackage flatbuffer_no_sig_info;
  flatbuffer_no_sig_info.fbs_binary = builder2.GetBufferPointer();
  flatbuffer_no_sig_info.fbs_size = builder2.GetSize();

  // parse signal list to json
  outputmanager::PluginDataClassParser data_class_parser;
  next::bridgesdk::plugin::SensorInfoForFbs sensor_info_1{"signalList", "", 1, 1234567890};
  next::bridgesdk::plugin::SensorInfoForFbs sensor_info_2{"signalList", "", 1, 9876543210};
  std::pair<std::string, std::string> structure_version_pair{"SignalList", "0.0.1"};

  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "signal.fbs";

  // first call with signal informations
  std::string result_1;
  ASSERT_TRUE(data_class_parser.parseDataClass(result_1, path_to_fbs.string(), flatbuffer_sig_info, sensor_info_1,
                                               structure_version_pair));

  // second_call without signal informations
  std::string result_2;
  ASSERT_TRUE(data_class_parser.parseDataClass(result_2, path_to_fbs.string(), flatbuffer_no_sig_info, sensor_info_2,
                                               structure_version_pair));

  const std::string expected_result_with_signal_info = R"(
    {"dataStream":{"signalList":{
	    "ADC4xx" : 
	    {
		    "FDP_CSB" : 
		    {
			    "m_tpfOutput" : 
			    {
				    "HeaderObjList" : 
				    {
					    "iNumOfUsedObjects" : 55.32
				    },
				    "sigHeader" : 
				    {
					    "uiMeasurementCounter" : 4242
				    }
			    }
		    }
	    }
    }
    ,"timestamp":1234567890}}
   )";

  const std::string expected_result_without_signal_info = R"(
    {"dataStream":{"signalList":{
	    "ADC4xx" : 
	    {
		    "FDP_CSB" : 
		    {
			    "m_tpfOutput" : 
			    {
				    "HeaderObjList" : 
				    {
					    "iNumOfUsedObjects" : 42.42
				    },
				    "sigHeader" : 
				    {
					    "uiMeasurementCounter" : 1212
				    }
			    }
		    }
	    }
    }
    ,"timestamp":9876543210}}
   )";

  Json::Value expected_res_with_info, expected_res_without_info, out_info, out_no_info;
  Json::Reader reader;

  reader.parse(expected_result_with_signal_info, expected_res_with_info, false);
  reader.parse(expected_result_without_signal_info, expected_res_without_info, false);
  reader.parse(result_1, out_info, false);
  reader.parse(result_2, out_no_info, false);

  EXPECT_EQ(expected_res_with_info, out_info);
  EXPECT_EQ(expected_res_without_info, out_no_info);
}

TEST(CORE_SIGNAL_FLATBUFFER_CHECK, test_signal_with_no_info) {
  // test creates a flatbuffer package with no signal info and expects empty signalList from parser
  // plugins should make sure that no new signals are send without sending at least once the sig info
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  // create a signal list with 2 signals
  std::string signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.HeaderObjList.iNumOfUsedObjects";
  size_t hash = std::hash<std::string>{}(signal_name);
  auto type_double = Core::CreateTypeDouble(builder, 55.32);
  auto signal_num_of_objects = Core::CreateSignal(builder, hash, Core::AnyValue::TypeDouble, type_double.Union());

  signal_name = "ADC4xx.FDP_CSB.m_tpfOutput.sigHeader.uiMeasurementCounter";
  hash = std::hash<std::string>{}(signal_name);
  auto type_uint32 = Core::CreateTypeUInt32(builder, 4242);
  auto signal_measurement_counter = Core::CreateSignal(builder, hash, Core::AnyValue::TypeUInt32, type_uint32.Union());

  std::vector<flatbuffers::Offset<Core::SignalInfo>> signal_info;
  // this will be empty

  std::vector<flatbuffers::Offset<Core::Signal>> signals;
  signals.push_back(signal_num_of_objects);
  signals.push_back(signal_measurement_counter);

  auto sig_info_vector = builder.CreateVector(signal_info);
  auto sig_vector = builder.CreateVector(signals);
  auto signal_list_with_sig_info = Core::CreateSignalList(builder, sig_info_vector, sig_vector);
  builder.Finish(signal_list_with_sig_info);

  bridgesdk::plugin::FlatbufferPackage flatbuffer_sig_info;
  flatbuffer_sig_info.fbs_binary = builder.GetBufferPointer();
  flatbuffer_sig_info.fbs_size = builder.GetSize();

  // parse signal list to json
  outputmanager::PluginDataClassParser data_class_parser;
  next::bridgesdk::plugin::SensorInfoForFbs sensor_info_1{"signalList", "", 1, 1234567890};
  std::pair<std::string, std::string> structure_version_pair{"SignalList", "0.0.1"};

  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "signal.fbs";

  std::string result;
  ASSERT_TRUE(data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer_sig_info, sensor_info_1,
                                               structure_version_pair));

  const std::string expected_output_str = R"(
    {
        "dataStream":{
            "signalList":"",
            "timestamp":1234567890
        }
    }
  )";
  Json::Value output, expected;
  Json::Reader reader;

  reader.parse(expected_output_str, expected, false);
  reader.parse(result, output, false);

  EXPECT_EQ(output, expected);
}

TEST(CORE_SIGNAL_FLATBUFFER_CHECK, signal_list_fbs_to_fbs) {
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  // create a list of 10 signals in an array, TypeDouble
  std::string signal_name;
  std::hash<std::string> hasher;
  size_t hashed_name;

  std::vector<flatbuffers::Offset<Core::Signal>> signals_vector;
  std::vector<flatbuffers::Offset<Core::SignalInfo>> info_vector;

  for (size_t i = 0; i < 10; i++) {
    signal_name = "ARS5xx.AlgoVehCycle.VehSig.VehSigMain.State[" + std::to_string(i) + "]";
    hashed_name = hasher(signal_name);
    auto signal = Core::CreateSignal(builder, hashed_name, Core::AnyValue::TypeDouble,
                                     Core::CreateTypeDouble(builder, 10.0 + static_cast<double>(i)).Union());

    auto info = Core::CreateSignalInfoDirect(builder, signal_name.c_str(), hashed_name);

    signals_vector.push_back(signal);
    info_vector.push_back(info);
  }

  auto serialized_signals_vector = builder.CreateVector(signals_vector);
  auto serialized_signal_info_vector = builder.CreateVector(info_vector);

  auto signal_list = Core::CreateSignalList(builder, serialized_signal_info_vector, serialized_signals_vector);
  builder.Finish(signal_list);

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info{"signalList", "", 1};
  std::pair<std::string, std::string> structure_version_pair{"SignalList", "0.0.1"};

  outputmanager::PluginDataClassParser data_class_parser;

  std::string result_fbs;
  data_class_parser.parseDataClassToFlatbuffer(result_fbs, flatbuffer, 1, sensor_info, structure_version_pair);
  size_t offset = 13;
  auto fbs_binary = (void *)&(result_fbs.data()[offset]);
  auto data = Core::GetSignalUpdate(fbs_binary);

  EXPECT_EQ(data->signal_list()->new_signal_info()->size(), 10u);
  EXPECT_EQ(data->signal_list()->signals()->size(), 10u);

  EXPECT_EQ(data->meta()->cache_name()->str(), "");
  EXPECT_EQ(data->meta()->widget_stream()->str(), "signalList");

  // check first and last signal names from info
  EXPECT_EQ(data->signal_list()->new_signal_info()->Get(0)->signal_name()->str(),
            "ARS5xx.AlgoVehCycle.VehSig.VehSigMain.State[0]");
  EXPECT_EQ(data->signal_list()->new_signal_info()->Get(9)->signal_name()->str(),
            "ARS5xx.AlgoVehCycle.VehSig.VehSigMain.State[9]");

  // check first and last hashes from info
  EXPECT_EQ(data->signal_list()->new_signal_info()->Get(0)->hash(),
            hasher("ARS5xx.AlgoVehCycle.VehSig.VehSigMain.State[0]"));
  EXPECT_EQ(data->signal_list()->new_signal_info()->Get(9)->hash(),
            hasher("ARS5xx.AlgoVehCycle.VehSig.VehSigMain.State[9]"));

  // check first and last signal values from signal list
  EXPECT_EQ(data->signal_list()->signals()->Get(0)->value_as_TypeDouble()->value(), 10.0);
  EXPECT_EQ(data->signal_list()->signals()->Get(9)->value_as_TypeDouble()->value(), 19.0);
}

} // namespace databridge
} // namespace next
