/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_broadcaster.hpp
 * @brief    Forwading message to socket manager
 */

#ifndef NEXT_DATABRIDGE_DATA_BROADCASTER_HPP
#define NEXT_DATABRIDGE_DATA_BROADCASTER_HPP

#include <functional>
#include <future>
#include <list>
#include <thread>
#include <unordered_map>

#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace databridge {

enum class BroadcastingStatus : std::int8_t {
  FREE,
  FILLING
}; //!< State of given memory slot during processing by data-broadcaster and socket manager.

using BroadcastType = std::vector<std::uint8_t>; //!< Vector of bytes to be filled with signal data.

struct BroadcastDataType {
  BroadcastDataType() = default;
  ~BroadcastDataType() = default;

  BroadcastType data_;
  BroadcastingStatus status_;
  size_t id_{0};
  std::chrono::time_point<std::chrono::system_clock> acquire_time_{std::chrono::system_clock::duration(0)};
};

using BroadcastDataTypeIter = std::list<BroadcastDataType>::iterator;

using RequesterCallback = std::function<void(BroadcastDataType &)>;
using PromiseDataExchange = std::promise<BroadcastDataTypeIter>;
using ProcessingSignalTracker = std::unordered_map<size_t, BroadcastDataType>;
using MapIdToIterDataType = std::unordered_map<size_t, BroadcastDataTypeIter>;

namespace data_broadcaster {

class IDataBroadcaster {
public:
  IDataBroadcaster() = default;
  virtual ~IDataBroadcaster() = default;

  IDataBroadcaster(IDataBroadcaster &&) = delete;
  IDataBroadcaster(IDataBroadcaster &) = delete;

  virtual void StartWebSocketSever(const std::string &ip_address, uint16_t port_number,
                                   std::uint8_t num_connections) = 0;

  virtual void SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) = 0;

  virtual std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebsocketServer() = 0;

  virtual std::future<BroadcastDataTypeIter> RequestNewMemory() = 0;

  virtual void SendDataToSocketManager(size_t id, const BroadcastType &payload,
                                       const std::vector<size_t> &session_ids = {}, bool binary = false) = 0;

  virtual void DataPublishingFinished(size_t id) = 0;

  virtual BroadcastDataTypeIter GetRefToNextfreeMemSlot() = 0;

  virtual MapIdToIterDataType GetProcessedSignalTracker() = 0;

  virtual std::list<BroadcastDataType> GetSignalPool() = 0;

  virtual void SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) = 0;
};

std::shared_ptr<IDataBroadcaster> CreateDataBroadcaster();

} // namespace data_broadcaster
} // namespace databridge
} // namespace next
#endif // NEXT_DATABRIDGE_DATA_BROADCASTER_HPP
