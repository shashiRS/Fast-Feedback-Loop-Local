/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_data_publisher_mock.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_MOCK_H
#define NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_MOCK_H

#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>

namespace next {
namespace test {

class PluginDataPublisherMock : public next::bridgesdk::plugin_addons::PluginDataPublisher {

public:
  PluginDataPublisherMock(next::bridgesdk::plugin::Plugin *plugin) : PluginDataPublisher(plugin) {}
  virtual ~PluginDataPublisherMock() {}

  std::vector<uint8_t> capturedData;

  virtual void sendResultData(const uint8_t *data, const size_t size, const std::vector<size_t> & = {},
                              const std::chrono::milliseconds = std::chrono::milliseconds(1000),
                              const bool = false) override {
    capturedData.assign(data, data + size);
  }

  virtual void sendFlatbufferData(const std::string &, const next::bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                  const next::bridgesdk::plugin::SensorInfoForFbs &,
                                  const std::pair<std::string, std::string> &) override {
    flatbuffer_data_.resize(flatbuffer.fbs_size);
    memcpy(&flatbuffer_data_[0], flatbuffer.fbs_binary, flatbuffer.fbs_size);
  }

  std::vector<std::uint8_t> flatbuffer_data_{};
};

} // namespace test
} // namespace next
#endif // NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_MOCK_H
