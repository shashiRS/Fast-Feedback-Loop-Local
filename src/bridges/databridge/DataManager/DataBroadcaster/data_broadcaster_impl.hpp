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

#ifndef NEXT_DATABROADCASTER_INCLUDE_DATA_BROADCASTER_HPP
#define NEXT_DATABROADCASTER_INCLUDE_DATA_BROADCASTER_HPP

#include <functional>
#include <list>
#include <thread>
#include <unordered_map>

#include <boost/lockfree/spsc_queue.hpp>

#include <next/bridgesdk/web_server.hpp>
#include "data_broadcaster.hpp"

namespace next {

namespace databridge {

using MessageID = size_t;

constexpr const static std::size_t kPayloadSizeInByte{1000};
constexpr const static std::size_t kNumSignals{1000}; //!< Number of signal can be processed in total in same time
constexpr const std::chrono::milliseconds kNoFreeMemorySlotWaitTime =
    std::chrono::milliseconds(10); /**< The amount of time to wait when attempting to find a new memory slot. */
template <typename T>
using LockFreeQueue = boost::lockfree::spsc_queue<T>;
constexpr const long long kMaxDurationValidMemInMs = 2000; //!< Max time can a memory slot be occupied.
constexpr const static int kThreadSleep2Ms = 1;

namespace data_broadcaster {

//!  DataBroadcaster Class to handle requests for a memory slot and forwarding to responsible socket manager.
/*!
  The class provide functionality to manage memory pool to be filled with signal data and forwad to socket hander.
*/
class DataBroadcasterImpl : public IDataBroadcaster {
public:
  DataBroadcasterImpl();
  virtual ~DataBroadcasterImpl();

  DataBroadcasterImpl(DataBroadcasterImpl &&) = delete;
  DataBroadcasterImpl(DataBroadcasterImpl &) = delete;

  //! Starting Websocket server to accpet incoming connections.
  /*!
   \param ip_address Ip-Address.
   \param port_number The port number.
   \param num_connections Number of connections.
  */
  virtual void StartWebSocketSever(const std::string &ip_address, uint16_t port_number,
                                   std::uint8_t num_connections) override;

  //! Set the used web server.
  /*!
    \param ptr Shared pointer of the web server.
  */
  virtual void SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) override;

  //! Get the used web server.
  /*!
    \return Shared pointer of the web server.
  */
  virtual std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebsocketServer() override;

  //! The function manage requests for new memory slot. The request will be added to a request-queue to be handled in
  //! the request-response thread
  virtual std::future<BroadcastDataTypeIter> RequestNewMemory() override;

  //! The function will call responsible signal handler on the socket manager side to publish data.
  /*!
    \param data New available data to be send via socket handler.
  */
  virtual void SendDataToSocketManager(size_t id, const BroadcastType &payload,
                                       const std::vector<size_t> &session_ids = {}, bool binary = false) override;

  //! The function will add the ID of processed (send) signal to the queue of published signal.
  /*!
    \param id To be added to the published signal queue.
  */
  virtual void DataPublishingFinished(size_t id) override;

  //! The function will handle the demands of new memory slot by providing a new free one to requester.
  /*!
    \param id_value Given ID of new available memory slot.
  */
  void HandleRequestResponse();

  //! Looking into the list of memory to find reference to the next free one.
  /*!
    \return Reference to the next free memory slot.
  */
  BroadcastDataTypeIter FindNextFreeMemorySlot();

  //! The function will run on a separate thread to manage new request and published signals.
  void HandleRequesterReq(size_t &id_value);

  //! The function will handle publisher request for putting memory slot to free after it has been published.
  void HandlePublisherReq();

  //! Shut down the webserver connected to this broadcaster
  void ShutDownWebServer();

  //! Return reference to the available memory slot.
  /*!
    \return Reference to the next free memory slot.
  */
  virtual BroadcastDataTypeIter GetRefToNextfreeMemSlot() override;

  //! Return reference to map container tracker for processing memory slot.
  /*!
    \return Reference to map container from ID to memory slot.
  */
  virtual MapIdToIterDataType GetProcessedSignalTracker() override;

  //! Return reference to the list of memory slots.
  /*!
    \return Reference to the list of memory slots.
  */
  virtual std::list<BroadcastDataType> GetSignalPool() override;

  //! Set a callback to be calleck after receiving message from client.
  /*!
    \param callback callback with message input.
  */
  virtual void SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) override;

private:
  void MessageCallback(const std::vector<uint8_t> &, const bridgesdk::webserver::ConnectionType);
  bool IsMemorySlotValid(BroadcastDataType &data);

  //! Its a pool of Message ID's has been published.
  LockFreeQueue<MessageID> publisher_pool_{kNumSignals};

  //! Its a pool of requested memory slot to be handled in Req-Res thread.
  std::list<std::promise<BroadcastDataTypeIter>> requester_pool_;

  //! Memory pool of BroadcastDataType type.
  std::list<BroadcastDataType> signal_pool_;

  //! A map from Signal-ID to signals to be used for tracking of used signals in publisher.
  MapIdToIterDataType processing_signals_;

  //! Worker thread to handle Request-Response part
  std::unique_ptr<std::thread> worker_{nullptr};

  //! Reference to the next free memory slot.
  BroadcastDataTypeIter next_free_mem_slot_;

  //! Taking care of safe finishing the process.
  std::atomic<bool> active_worker_thread_{true};

  std::shared_ptr<next::bridgesdk::webserver::IWebServer> web_server_;

  std::mutex requester_pool_sync{};

  bridgesdk::webserver::ForwardMsgCallback forward_message_callback_;

  //! condition to start memory request thread up
  std::condition_variable memory_request_condition_;
  std::mutex memory_request_mutex_;

  std::atomic<bool> condition_wait_check_{false};
};

} // namespace data_broadcaster
} // namespace databridge
} // namespace next

#endif // NEXT_DATABROADCASTER_INCLUDE_DATA_BROADCASTER_HPP
