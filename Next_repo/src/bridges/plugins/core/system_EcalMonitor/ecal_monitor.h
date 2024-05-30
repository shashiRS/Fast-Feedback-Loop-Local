/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ecal_monitor.h
 * @brief    Signal List Plugin
 *
 * This plugin gets a tree structure of the signals available from the backend
 * and puts it into a json
 *
 **/

#ifndef NEXT_PLUGINS_ECAL_MONITOR_PLUGIN_H_
#define NEXT_PLUGINS_ECAL_MONITOR_PLUGIN_H_

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/profiler/network_graph_generated.h>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>

namespace next {
namespace plugins {

struct Nodes;
struct Edges;

class EcalMonitor : public next::bridgesdk::plugin::Plugin {
public:
  EcalMonitor(const char *);
  virtual ~EcalMonitor();

  bool init() override;

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  bool enterReset() override;
  bool exitReset() override;

protected:
  std::thread background_thread_;
  std::atomic_bool background_running_ = false;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;

  void CreateFlatBufferEdges(flatbuffers::FlatBufferBuilder &builder, std::vector<Edges> all_edges,
                             std::vector<flatbuffers::Offset<profiler::Edge>> &fbs_edges);

  void CreateFlatBufferNodes(flatbuffers::FlatBufferBuilder &builder, std::unordered_map<std::string, Nodes> &all_nodes,
                             std::vector<flatbuffers::Offset<profiler::Node>> &fbs_edges);
  void dataUpdate(void);

  void enable_thread();
  void disable_thread();

  std::string plugin_path_;
  std::atomic_int update_rate_ = 2000;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};

private:
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
};

} // namespace plugins
} // namespace next
//
extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *);

#endif // NEXT_PLUGINS_ECAL_MONITOR_PLUGIN_H_
