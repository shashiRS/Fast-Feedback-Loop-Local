#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <next/bridgesdk/schema/profiler/network_graph_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

struct PortData {
  std::string name;
  bool isActive;
  int dataSize;
  std::string type;

  PortData() : name(""), isActive(false), dataSize(0), type(""){};
  PortData(std::string _name, bool _isActive, int _dataSize, std::string _type)
      : name(_name), isActive(_isActive), dataSize(_dataSize), type(_type){};
};

TEST(ECAL_MONITOR_FBS_CHECK, eCALMonitor_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  flatbuffers::FlatBufferBuilder builder(2000);

  // Create nodes
  std::vector<std::string> node_name_reference{"controlbridge", "databridge"};
  auto basic_controlbridge = profiler::CreateBasic(builder, builder.CreateString(node_name_reference[0]));
  auto basic_databridge = profiler::CreateBasic(builder, builder.CreateString(node_name_reference[1]));

  PortData dataTransfer_inputPort1_reference("i1", true, 30066, "");
  PortData dataTransfer_inputPort2_reference("i2", true, 10, "EventType");
  PortData dataTransfer_outputPort1_reference("o1", true, 30066, "otherType");
  PortData dataTransfer_outputPort2_reference("o2", true, 10, "None");

  std::vector<PortData> inputPort_reference_v{dataTransfer_inputPort1_reference, dataTransfer_inputPort2_reference};
  std::vector<PortData> outputPort_reference_v{dataTransfer_outputPort1_reference, dataTransfer_outputPort2_reference};

  std::vector<flatbuffers::Offset<profiler::PortStatuses>> outputPorts_v;
  std::vector<flatbuffers::Offset<profiler::PortStatuses>> inputPorts_v;
  inputPorts_v.push_back(
      profiler::CreatePortStatuses(builder, builder.CreateString("i1"), 30066, true, builder.CreateString("")));
  inputPorts_v.push_back(
      profiler::CreatePortStatuses(builder, builder.CreateString("i2"), 10, true, builder.CreateString("EventType")));
  outputPorts_v.push_back(profiler::CreatePortStatuses(builder, builder.CreateString("o1"), 30066, true,
                                                       builder.CreateString("otherType")));
  outputPorts_v.push_back(
      profiler::CreatePortStatuses(builder, builder.CreateString("o2"), 10, true, builder.CreateString("None")));

  // builder.createVector
  auto input_fbs_v = builder.CreateVector(inputPorts_v);
  auto ouput_fbs_v = builder.CreateVector(outputPorts_v);
  auto datatransfer_inputoutput_nodes = profiler::CreateDataTransferInputOutputNodes(builder, input_fbs_v, ouput_fbs_v);
  auto node_controlbridge = profiler::CreateNode(builder, basic_controlbridge, datatransfer_inputoutput_nodes);
  auto node_databridge = profiler::CreateNode(builder, basic_databridge, datatransfer_inputoutput_nodes);

  std::vector<flatbuffers::Offset<profiler::Node>> fbs_nodes;
  fbs_nodes.push_back(node_controlbridge);
  fbs_nodes.push_back(node_databridge);
  auto nodes_vector = builder.CreateVector(fbs_nodes);

  // Create edge
  auto from_comp_name = builder.CreateString(node_name_reference[1]);
  auto from_comp_port = builder.CreateString(dataTransfer_outputPort1_reference.name);
  auto to_comp_name = builder.CreateString(node_name_reference[0]);
  auto to_comp_port = builder.CreateString(dataTransfer_inputPort1_reference.name);

  auto from_connection = profiler::CreateConnection(builder, from_comp_name, from_comp_port);
  auto to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);

  std::string source_id = node_name_reference[1] + "/" + dataTransfer_outputPort1_reference.name;
  std::string target_id = node_name_reference[0] + "/" + dataTransfer_inputPort1_reference.name;
  auto group_id = builder.CreateString(source_id + "/" + target_id);
  std::string linkstatus_reference = "error";
  size_t dataSize_reference = 30066;

  auto edge = profiler::CreateEdge(builder, group_id, from_connection, to_connection,
                                   builder.CreateString(linkstatus_reference), dataSize_reference);
  std::vector<flatbuffers::Offset<profiler::Edge>> fbs_edges;
  fbs_edges.push_back(edge);
  auto edges_vector = builder.CreateVector(fbs_edges);

  // Create datatransfer based on nodes and edge
  auto datatransfer = profiler::CreatedataTransfer(builder, nodes_vector, edges_vector);

  builder.Finish(datatransfer);

  next::bridgesdk::time_t timeStamp_reference = 3000u;
  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "profiler";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "dummy";
  sensor_info.timestamp_microsecond = timeStamp_reference;

  std::pair<std::string, std::string> structure_version_pair{"NetworkGraph", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "network_graph.fbs";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(result, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // testing the nodes
  size_t node_idx = 0;
  for (auto node : plugin_output["dataStream"]["profiler"]["dataTransfer"]["nodes"]) {
    ASSERT_EQ(node["basic"]["name"], node_name_reference[node_idx]);
    Json::Value::ArrayIndex json_idx = 0;
    for (size_t i = 0; i < node["DataTransfer"]["inputPorts"].size(); i++) {
      EXPECT_EQ(node["DataTransfer"]["inputPorts"][json_idx]["name"], inputPort_reference_v[i].name)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["inputPorts"][json_idx]["isActive"], inputPort_reference_v[i].isActive)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["inputPorts"][json_idx]["dataSize"], inputPort_reference_v[i].dataSize)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["inputPorts"][json_idx]["type"], inputPort_reference_v[i].type)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      json_idx++;
    }
    json_idx = 0;
    for (size_t i = 0; i < node["DataTransfer"]["outputPorts"].size(); i++) {
      EXPECT_EQ(node["DataTransfer"]["outputPorts"][json_idx]["name"], outputPort_reference_v[i].name)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["outputPorts"][json_idx]["isActive"], outputPort_reference_v[i].isActive)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["outputPorts"][json_idx]["dataSize"], outputPort_reference_v[i].dataSize)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      EXPECT_EQ(node["DataTransfer"]["outputPorts"][json_idx]["type"], outputPort_reference_v[i].type)
          << "Error: values are not matching in node[" << node_idx << "][inputPorts][" << i << "]";
      json_idx++;
    }
    node_idx++;
  }

  // testing the edge
  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["from"]["componentName"],
            node_name_reference[1]);
  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["from"]["componentPort"],
            dataTransfer_outputPort1_reference.name);

  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["to"]["componentName"],
            node_name_reference[0]);
  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["to"]["componentPort"],
            dataTransfer_inputPort1_reference.name);

  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["linkStatus"], linkstatus_reference);
  // dataSize_reference must be casted to int, otherwise the operator==() of Json reports false
  ASSERT_EQ(plugin_output["dataStream"]["profiler"]["dataTransfer"]["edges"][0]["dataSize"],
            static_cast<int32_t>(dataSize_reference));

  // testing the timeStamp
  // timeStamp_reference must be casted to int, otherwise the operator==() of Json reports false
  ASSERT_EQ(plugin_output["dataStream"]["timestamp"], static_cast<int32_t>(timeStamp_reference));
}

} // namespace databridge
} // namespace next
