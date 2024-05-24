/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_manager_mock.hpp
 * @brief    mock of the Data Manager
 *
 **/

#ifndef DATA_MANAGER_MOCK_H_
#define DATA_MANAGER_MOCK_H_

#include <chrono>
#include <list>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/data_manager.hpp>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>

namespace next {
namespace test {

class DataManagerMock : public next::databridge::data_manager::IDataManager {
public:
  void PublishDataToClient(const uint8_t *payload, const size_t size,
                           [[maybe_unused]] const bool force_no_cache = false,
                           [[maybe_unused]] const std::vector<size_t> & = {},
                           [[maybe_unused]] const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                           [[maybe_unused]] const bool binary = false) override {
#ifdef _DEBUG
    std::cout << "Mock DataManager PublishDataToClient" << std::endl;
#endif
    if (nullptr == payload) {
      std::cout << "payload is nullptr" << std::endl;
      return;
    }
#ifdef _DEBUG
    std::cout << "bytes provided: " << size << std::endl;
#endif

    std::vector<uint8_t> data(payload, payload + size);
    data_.push_back(data);
  }

  void PublishFlatbufferDataToClient(const std::string &, const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                     const bridgesdk::plugin::SensorInfoForFbs &,
                                     const std::pair<std::string, std::string> &, const bool = false) override {

    if (flatbuffer.fbs_size != 0) {
      flatbuffer_data_.clear();
      flatbuffer_data_.resize(flatbuffer.fbs_size);
      memcpy(&flatbuffer_data_[0], flatbuffer.fbs_binary, flatbuffer.fbs_size);
    }
  }
  virtual void PublishPluginDescriptionToClient(const std::string &, const std::string &,
                                                const next::bridgesdk::plugin_config_t &) override{};
  void StartWebServer(const std::string &, uint16_t, uint8_t) {}

  void SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) { web_server_ptr_ = ptr; }

  std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebsocketServer() { return web_server_ptr_; }

  void SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback) {}

  std::shared_ptr<bridgesdk::plugin_addons::RegisteredSensors> GetRegisteredSensors() {
    static std::shared_ptr<bridgesdk::plugin_addons::RegisteredSensors> sensor_list =
        std::make_shared<bridgesdk::plugin_addons::RegisteredSensors>();
    return sensor_list;
  }

  void PublishDataToClientDirectly(const uint8_t *, const size_t, const std::vector<size_t> & = {},
                                   const std::chrono::milliseconds = std::chrono::milliseconds(1000)) {}

  void SetFoxgloveMode(bool) {}

  std::list<std::vector<uint8_t>> data_;
  std::vector<uint8_t> flatbuffer_data_{};
  std::shared_ptr<bridgesdk::webserver::IWebServer> web_server_ptr_;
};

} // namespace test
} // namespace next

#endif // DATA_MANAGER_MOCK_H_
