/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file    Ecal_monitor.cpp
 * @brief   Monitors the Ecal Subscriptions and Publishers, and creates a directional graph
 **/

#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include <next/sdk/sdk_macros.h>

#include <ecal/ecal.h>

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
// disable warning of protobuf
// the code `if (sizeof(int32) == sizeof(long))` produces
//    warning C4127: conditional expression is constant
//    note: consider using 'if constexpr' statement instead
// TODO: enable warning C4127 once protobuf was updated
NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4127)
#include <ecal/core/pb/monitoring.pb.h>
NEXT_RESTORE_WARNINGS_WINDOWS
NEXT_RESTORE_WARNINGS_WINDOWS

#include <json/json.h>

#include <next/plugins_utils/helper_functions.h>

#include "ecal_monitor.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100 4127 4146 4800) // disable proto warnings
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace next {
namespace plugins {
using namespace next::plugins_utils;

struct Publisher {
  int dfreq;
  int tsize;
};

struct Subscriber {
  std::string dummy;
};

struct PortStatuses {
  std::string name;
  int32_t dataSize;
  bool isActive;
  std::string type;
};

struct NodeDescription {
  std::map<std::string, Publisher> publisher;
  std::map<std::string, Subscriber> subscriber;
  int connectedLinks;
  std::string description;
};

struct AlwaysVisibleData {
  std::string name;
  std::string value;
  std::map<std::string, std::string> details;
};

struct HiddenData {
  int dummy;
};

struct Nodes {
  std::string name;
  bool isOnline;
  std::string type;
  std::vector<PortStatuses> inputPorts;
  std::vector<PortStatuses> outputPorts;
  NodeDescription extra_info;
  AlwaysVisibleData alwaysVisibleData;
  HiddenData hiddenData;
};

struct Edges {
  std::string group_id;
  std::string pub_uname;
  std::string sub_uname;
  std::string from_port;
  std::string to_port;
  int data_size;
};

struct TopicInfo {
  std::string topic_name;
  std::string pub_name;
  int data_size;
};

void EcalMonitor::enable_thread() {
  if (!background_running_) {
    background_running_ = true;
    background_thread_ = std::thread(&EcalMonitor::dataUpdate, this);
  }
}

void EcalMonitor::disable_thread() {
  if (background_running_ == true) {
    background_running_ = false;
    background_thread_.join();
  }
}

EcalMonitor::EcalMonitor(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  plugin_path_ = path;
  debug(__FILE__, "Hello system_EcalMonitor plugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("system_EcalMonitor"));
}

EcalMonitor::~EcalMonitor() {
  eCAL::Finalize();
  disable_thread();
}

std::unordered_map<std::string, Nodes> CreateNodesFromEcalMonitor(eCAL::pb::Monitoring &monitoring) {

  std::unordered_map<std::string, Nodes> all_nodes;

  // collect topic infos
  // for all topics
  for (auto topic : monitoring.topics()) {
    // Use of pointer instead of variable to gain speed and avoid lot of map copy
    Nodes temp_nodes;
    Nodes *found_node;
    // define node name
    temp_nodes.name = topic.uname();
    // define pubs and subs
    std::string topic_name = topic.tname();

    Publisher pub;
    pub.dfreq = topic.dfreq();
    pub.tsize = topic.tsize();

    Subscriber sub;
    sub.dummy = "";

    PortStatuses portStatus;

    portStatus.name = topic_name;
    portStatus.dataSize = topic.tsize();
    portStatus.isActive = topic.connections_loc() > 0 ? true : false;
    portStatus.type = topic.ttype();

    auto found_node_it = all_nodes.find(temp_nodes.name);
    if (found_node_it != all_nodes.end()) {
      found_node = &found_node_it->second;
      if (topic.direction() == "publisher") {
        found_node->extra_info.publisher[topic_name] = pub;
        found_node->outputPorts.push_back(portStatus);
        found_node->extra_info.connectedLinks++;
      } else if (topic.direction() == "subscriber") {
        found_node->extra_info.subscriber[topic_name] = sub;
        found_node->inputPorts.push_back(portStatus);
        found_node->extra_info.connectedLinks++;
      }
      found_node_it->second = *found_node;
    } else {
      if (topic.direction() == "publisher") {
        temp_nodes.extra_info.publisher[topic_name] = pub;
        temp_nodes.extra_info.connectedLinks = 1;
      } else if (topic.direction() == "subscriber") {
        temp_nodes.extra_info.subscriber[topic_name] = sub;
        temp_nodes.extra_info.connectedLinks = 1;
      }
      temp_nodes.extra_info.description = "Here is place for a description!";
      temp_nodes.name = topic.uname();
      // define status of node
      if (pub.tsize != 0) {
        temp_nodes.isOnline = true;
      }
      // define node Type
      if (pub.tsize == 0) {
        temp_nodes.type = "Input";
      } else if (pub.tsize != 0) {
        temp_nodes.type = "Output";
      }
      if (topic.direction() == "publisher") {
        temp_nodes.outputPorts.push_back(portStatus);
      } else if (topic.direction() == "subscriber") {
        temp_nodes.inputPorts.push_back(portStatus);
      }
      temp_nodes.alwaysVisibleData.name = "GIA";
      temp_nodes.alwaysVisibleData.value = "active";
      temp_nodes.alwaysVisibleData.details = {{"used version", "x.x.x"}};
      all_nodes.insert(std::pair<std::string, Nodes>(temp_nodes.name, temp_nodes));
    }
  }
  return all_nodes;
}

std::vector<Edges> CreateEdgesFromEcalMonitor(eCAL::pb::Monitoring &monitoring) {
  // Map with topic name as key and pair of <uname and topic size> as element
  std::unordered_multimap<std::string, TopicInfo> pub_topics;
  std::map<std::string, std::vector<std::string>> sub_topics;
  TopicInfo topic_info;

  for (auto topic : monitoring.topics()) {
    if (topic.direction() == "publisher") {
      topic_info.topic_name = topic.tname();
      topic_info.pub_name = topic.uname();
      topic_info.data_size = topic.tsize();
      pub_topics.insert(std::pair<std::string, TopicInfo>(topic.tname(), topic_info));
    } else if (topic.direction() == "subscriber") {
      sub_topics[topic.tname()].push_back(topic.uname());
    }
  }

  std::vector<Edges> all_edges;
  Edges edge;

  for (auto pub : pub_topics) {
    std::string tname = pub.second.topic_name;
    std::string pub_uname = pub.second.pub_name;
    int data_size = pub.second.data_size;
    for (auto sub_uname : sub_topics[tname]) {

      if (sub_uname == pub_uname)
        continue;
      size_t last_dot = tname.find_last_of('.');

      edge.data_size = data_size;
      edge.pub_uname = pub_uname;
      edge.sub_uname = sub_uname;
      edge.from_port = tname;
      edge.to_port = tname;
      edge.group_id = pub_uname + '/' + tname + '/' + sub_uname + '/' + tname;
      all_edges.push_back(edge);
      if (last_dot != std::string::npos) {
        std::string short_tname = tname.substr(0, last_dot);

        edge.from_port = short_tname;
        edge.to_port = short_tname;
        all_edges.push_back(edge);
        edge.from_port = short_tname;
        edge.to_port = tname;
        all_edges.push_back(edge);
        edge.from_port = tname;
        edge.to_port = short_tname;
        all_edges.push_back(edge);
      }
    }
  }
  return all_edges;
}

void EcalMonitor::CreateFlatBufferEdges(flatbuffers::FlatBufferBuilder &builder, std::vector<Edges> all_edges,
                                        std::vector<flatbuffers::Offset<profiler::Edge>> &fbs_edges) {
  for (auto edge : all_edges) {
    auto from_comp_name = builder.CreateString(edge.pub_uname);
    auto from_comp_port = builder.CreateString(edge.from_port);
    auto to_comp_name = builder.CreateString(edge.sub_uname);
    auto to_comp_port = builder.CreateString(edge.to_port);
    auto group_id = builder.CreateString(edge.group_id);

    auto from_connection = profiler::CreateConnection(builder, from_comp_name, from_comp_port);
    auto to_connection = profiler::CreateConnection(builder, to_comp_name, to_comp_port);
    auto built_edge =
        profiler::CreateEdge(builder, group_id, from_connection, to_connection, builder.CreateString("ok"),
                             edge.data_size >= 0 ? static_cast<uint64_t>(edge.data_size) : 0u);

    fbs_edges.push_back(built_edge);
  }
}

void EcalMonitor::CreateFlatBufferNodes(flatbuffers::FlatBufferBuilder &builder,
                                        std::unordered_map<std::string, Nodes> &all_nodes,
                                        std::vector<flatbuffers::Offset<profiler::Node>> &fbs_nodes) {
  for (auto node_pod : all_nodes) {
    std::vector<flatbuffers::Offset<profiler::PortStatuses>> outputPorts_v;
    std::vector<flatbuffers::Offset<profiler::PortStatuses>> inputPorts_v;

    for (auto port : node_pod.second.inputPorts) {
      auto input = profiler::CreatePortStatuses(builder, builder.CreateString(port.name),
                                                port.dataSize >= 0 ? static_cast<uint64_t>(port.dataSize) : 0u,
                                                port.isActive, builder.CreateString(port.type));
      inputPorts_v.push_back(input);
    }
    for (auto port : node_pod.second.outputPorts) {
      auto output = profiler::CreatePortStatuses(builder, builder.CreateString(port.name),
                                                 port.dataSize >= 0 ? static_cast<uint64_t>(port.dataSize) : 0u,
                                                 port.isActive, builder.CreateString(port.type));
      outputPorts_v.push_back(output);
    }
    auto inputPorts_fbs_v = builder.CreateVector(inputPorts_v);
    auto outputPorts_fbs_v = builder.CreateVector(outputPorts_v);

    auto basic_pr = profiler::CreateBasic(builder, builder.CreateString(node_pod.second.name));

    auto dion = profiler::CreateDataTransferInputOutputNodes(builder, inputPorts_fbs_v, outputPorts_fbs_v);

    auto node = profiler::CreateNode(builder, basic_pr, dion);
    fbs_nodes.push_back(node);
  }
}

void EcalMonitor::dataUpdate(void) {
  while (background_running_) {
    NEXT_DISABLE_DEPRECATED_WARNING
    // monitoring instance to store complete snapshot
    eCAL::pb::Monitoring monitoring;
    std::string monitoring_s;

    eCAL::Monitoring::GetMonitoring(monitoring_s);
    monitoring.ParseFromString(monitoring_s);
    NEXT_RESTORE_WARNINGS

    flatbuffers::FlatBufferBuilder builder(2000);

    builder.ForceDefaults(true);

    std::unordered_map<std::string, Nodes> all_nodes;
    std::vector<Edges> all_edges;

    // Creates nodes and then the fbs for nodes
    all_nodes = CreateNodesFromEcalMonitor(monitoring);
    std::vector<flatbuffers::Offset<profiler::Node>> fbs_nodes;
    CreateFlatBufferNodes(builder, all_nodes, fbs_nodes);

    // Creates edges and then the fbs for edges
    all_edges = CreateEdgesFromEcalMonitor(monitoring);
    std::vector<flatbuffers::Offset<profiler::Edge>> fbs_edges;
    CreateFlatBufferEdges(builder, all_edges, fbs_edges);

    auto nodes_vector = builder.CreateVector(fbs_nodes);

    auto edges_vector = builder.CreateVector(fbs_edges);

    auto datatransfer = profiler::CreatedataTransfer(builder, nodes_vector, edges_vector);

    builder.Finish(datatransfer);

    next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
    sensor_info.data_view = "profiler";
    sensor_info.sensor_id = 1;
    sensor_info.cache_name = "dummy";

    std::pair<std::string, std::string> structure_version_pair{"NetworkGraph", "0.0.1"};

    bridgesdk::plugin::FlatbufferPackage flatbuffer;
    flatbuffer.fbs_binary = builder.GetBufferPointer();
    flatbuffer.fbs_size = builder.GetSize();

    plugin_publisher_->sendFlatbufferData(plugin_path_ + "\\network_graph.fbs", flatbuffer, sensor_info,
                                          structure_version_pair);
    std::this_thread::sleep_for(std::chrono::milliseconds(update_rate_));
  }
}

bool EcalMonitor::init() {

  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);

  return true;
}

bool EcalMonitor::enterReset() {
  // so far we are doing nothing here
  debug(__FILE__, "entering reset");
  return true;
}

bool EcalMonitor::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool EcalMonitor::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool EcalMonitor::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {
  std::string up_rate, is_on_off;

  config_ = config;

  if (!GetValueFromPluginConfig(config, "Update Rate", up_rate)) {
    warn(__FILE__, "Performance Viewer missing config value: Update Rate");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "NetworkGraph", is_on_off)) {
    warn(__FILE__, "Performance Viewer missing config value: Update Rate");
    return false;
  }

  if (is_on_off == "on") {
    if (!up_rate.empty() || up_rate != "") {
      auto tmp = std::stoi(up_rate);
      if (tmp != update_rate_) {
        update_rate_ = tmp;
      }
    }
    enable_thread();
  } else {
    disable_thread();
  }

  return true;
}

bool EcalMonitor::removeConfig(const next::bridgesdk::plugin_config_t & /*config*/) {
  disable_thread();
  update_rate_ = 2000;
  return true;
}

bool EcalMonitor::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"Update Rate", {next::bridgesdk::ConfigType::EDITABLE, "2000"}});
  config_fields.insert({"NetworkGraph", {next::bridgesdk::ConfigType::SELECTABLE, "on"}});
  config_fields.insert({"NetworkGraph", {next::bridgesdk::ConfigType::SELECTABLE, "off"}});
  return true;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::EcalMonitor(path); }
