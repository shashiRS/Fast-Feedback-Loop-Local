#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <boost/filesystem.hpp>

#include <next/bridgesdk/schema/profiler/network_graph_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(NETWORK_GRAPH, network_graph_fps_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);

  auto basic1 = builder.CreateString("emf-node");
  auto basic2 = builder.CreateString("controlbridge");
  auto basic3 = builder.CreateString("node3");
  auto basic4 = builder.CreateString("node4");

  std::vector<flatbuffers::Offset<profiler::PortStatuses>> outputPorts_v;

  std::vector<flatbuffers::Offset<profiler::PortStatuses>> inputPorts_v;
  auto input = profiler::CreatePortStatuses(builder, builder.CreateString("PortName"), 19, true);
  inputPorts_v.push_back(input);
  auto inputPorts_fbs_v = builder.CreateVector(inputPorts_v);

  auto dion = profiler::CreateDataTransferInputOutputNodes(builder, inputPorts_fbs_v, inputPorts_fbs_v);

  auto basic_pr = profiler::CreateBasic(builder, builder.CreateString("processName"));
  auto node1 = profiler::CreateNode(builder, basic_pr, dion);

  auto basic_pr1 = profiler::CreateBasic(builder, basic1);
  auto node2 = profiler::CreateNode(builder, basic_pr1, dion);

  auto basic_pr2 = profiler::CreateBasic(builder, basic2);
  auto node3 = profiler::CreateNode(builder, basic_pr2, dion);

  auto basic_pr3 = profiler::CreateBasic(builder, basic3);
  auto node4 = profiler::CreateNode(builder, basic_pr3, dion);

  auto basic_pr4 = profiler::CreateBasic(builder, basic4);
  auto node5 = profiler::CreateNode(builder, basic_pr4, dion);

  /// create edges now

  auto from_comp_name = builder.CreateString("emf-node");
  auto from_comp_port = builder.CreateString("o1");
  auto from_connection = profiler::CreateConnection(builder, from_comp_name, from_comp_port);

  auto to_comp_name = builder.CreateString("controlbridge");
  auto to_comp_port = builder.CreateString("i1");
  auto to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);

  auto group_id = builder.CreateString("emf-node/o1/controlbridge/i1");

  auto edge1 =
      profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("error"), 30066);

  to_comp_name = builder.CreateString("node3");
  to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);
  group_id = builder.CreateString("node3/o1/controlbridge/i1");

  auto edge2 =
      profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("error"), 24300);

  to_comp_name = builder.CreateString("databridge");
  to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);
  group_id = builder.CreateString("node3/o1/databridge/i1");
  auto edge3 =
      profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("error"), 21600);

  to_comp_name = builder.CreateString("databridge");
  to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);
  group_id = builder.CreateString("node3/o1/databridge/i1");
  auto edge4 =
      profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("ok"), 30066);

  to_comp_name = builder.CreateString("node4");
  to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);
  group_id = builder.CreateString("node3/o1/node4/i1");
  auto edge5 =
      profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("error"), 300);

  std::vector<flatbuffers::Offset<profiler::Node>> v_n = {node1, node2, node3, node4, node5};
  auto nodes_vector = builder.CreateVector(v_n);

  std::vector<flatbuffers::Offset<profiler::Edge>> e_n = {edge1, edge2, edge3, edge4, edge5};
  auto edges_vector = builder.CreateVector(e_n);

  auto datatransfer = profiler::CreatedataTransfer(builder, nodes_vector, edges_vector);

  builder.Finish(datatransfer);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "profiler";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "dummy";

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
}

} // namespace databridge
} // namespace next
