/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_broadcaster_impl.cpp
 *  @brief    see data_broadcaster_impl.hpp
 */

#include <next/sdk/logger/logger.hpp>

#include "data_broadcaster_impl.hpp"

namespace next {
namespace databridge {
namespace data_broadcaster {

std::shared_ptr<IDataBroadcaster> CreateDataBroadcaster() { return std::make_shared<DataBroadcasterImpl>(); }

//! Initialize variable member specially the signal list with fixes size for payload.
DataBroadcasterImpl::DataBroadcasterImpl()
    : signal_pool_{kNumSignals, {std::vector<uint8_t>(kPayloadSizeInByte, 0), BroadcastingStatus::FREE}},
      next_free_mem_slot_{signal_pool_.begin()} {
  // default callback function, if nothing else is set
  forward_message_callback_ = [](const std::vector<uint8_t> &data, const bridgesdk::webserver::ConnectionType) {
    debug(__FILE__, "DataBroadcaster incoming message: {0}", std::string((char *)data.data(), data.size()));
  };

  HandleRequestResponse();
}

void DataBroadcasterImpl::StartWebSocketSever(const std::string &ip_address, uint16_t port_number,
                                              std::uint8_t num_connections) {

  debug(__FILE__, "Start accepting incoming connection");
  web_server_ = bridgesdk::webserver::CreateWebServer();
  web_server_->SetIpAddress(ip_address);
  web_server_->SetPortNumber(port_number);
  web_server_->SetNumOfConnections(num_connections);
  web_server_->AddForwardMessageCallback(
      std::bind(&DataBroadcasterImpl::MessageCallback, this, std::placeholders::_1, std::placeholders::_2));
  try {
    web_server_->ActivateListingToSocket();
  } catch (std::exception &e) {
    warn(__FILE__, "Could not established a connection");
    throw e;
  }
}

void DataBroadcasterImpl::SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) {
  web_server_ = ptr;
  web_server_->AddForwardMessageCallback(
      std::bind(&DataBroadcasterImpl::MessageCallback, this, std::placeholders::_1, std::placeholders::_2));
}

std::shared_ptr<bridgesdk::webserver::IWebServer> DataBroadcasterImpl::GetWebsocketServer() { return web_server_; }

void DataBroadcasterImpl::ShutDownWebServer() { web_server_.reset(); }

DataBroadcasterImpl::~DataBroadcasterImpl() {
  web_server_.reset();
  active_worker_thread_.store(false);
  if (worker_->joinable()) {
    condition_wait_check_.store(true);
    memory_request_condition_.notify_one();
    worker_->join();
  }
}

std::future<BroadcastDataTypeIter> DataBroadcasterImpl::RequestNewMemory() {
  try {
    std::future<BroadcastDataTypeIter> mem_future{};
    {
      std::lock_guard<std::mutex> lck{requester_pool_sync};

      PromiseDataExchange mem_promise{};
      mem_future = mem_promise.get_future();
      requester_pool_.push_back(std::move(mem_promise));
    }
    condition_wait_check_.store(true);
    memory_request_condition_.notify_one();
    return mem_future;
  } catch (const std::future_error &e) {
    error(__FILE__, "future error in RequestNewMemory {} ({})", e.code().message(), e.what());
  } catch (const std::exception &e) {
    error(__FILE__, "exception in RequestNewMemory {0}", e.what());
  }
  throw std::runtime_error("Data manager failed to allocate memory");
}

void DataBroadcasterImpl::SendDataToSocketManager(size_t id, const BroadcastType &payload,
                                                  const std::vector<size_t> &session_ids, bool binary) {

  web_server_->BroadcastMessageWithNotification(
      payload, id, session_ids, std::bind(&DataBroadcasterImpl::DataPublishingFinished, this, std::placeholders::_1),
      binary);
}

void DataBroadcasterImpl::DataPublishingFinished(size_t id) {
  std::lock_guard<std::mutex> lck{requester_pool_sync};
  publisher_pool_.push(id);
  condition_wait_check_.store(true);
  memory_request_condition_.notify_one();
}

void DataBroadcasterImpl::HandleRequesterReq(size_t &id_value) {

  // PromiseDataExchange mem_promise{};
  std::list<std::list<std::promise<BroadcastDataTypeIter>>::iterator> used_promisses{};

  for (auto it = requester_pool_.begin(); it != requester_pool_.end(); ++it) {
    auto new_memory_block = FindNextFreeMemorySlot();
    if ((new_memory_block != std::end(signal_pool_)) && (new_memory_block->status_ == BroadcastingStatus::FREE)) {
      try {
        new_memory_block->status_ = BroadcastingStatus::FILLING;
        new_memory_block->id_ = id_value++;
        new_memory_block->acquire_time_ = std::chrono::system_clock::now();
        // Puting the new memory in map for having the tracking of current occupied memory slot
        processing_signals_[new_memory_block->id_] = new_memory_block;
        it->set_value(new_memory_block);
        used_promisses.push_back(it);
      } catch (...) {
        warn(__FILE__, "exception in HandleRequesterReq");
      }
    }
  }

  for (auto const it : used_promisses) {
    requester_pool_.erase(it);
  }
}

BroadcastDataTypeIter DataBroadcasterImpl::FindNextFreeMemorySlot() {

  if (std::next(next_free_mem_slot_, 1) == std::end(signal_pool_)) {
    next_free_mem_slot_ = signal_pool_.begin();
  }

  auto next_free_slot = std::find_if(next_free_mem_slot_, std::end(signal_pool_),
                                     [&](BroadcastDataType &data) { return IsMemorySlotValid(data); });

  if (next_free_slot != std::end(signal_pool_)) {
    next_free_mem_slot_ = next_free_slot;
  } else {
    info(__FILE__, "No free memory slot at the moment, waiting {} ms to find the next available slot.",
         static_cast<unsigned int>(kNoFreeMemorySlotWaitTime.count()));

    std::this_thread::sleep_for(kNoFreeMemorySlotWaitTime);
    HandlePublisherReq();

    return FindNextFreeMemorySlot();
  }

  return next_free_mem_slot_;
}

void DataBroadcasterImpl::HandleRequestResponse() {
  debug(__FILE__, "Start thread for request and response procedure");
  worker_ = std::make_unique<std::thread>(std::thread([this]() {
    size_t id_value{0};
    while (active_worker_thread_) {
      std::unique_lock<std::mutex> lck(requester_pool_sync);
      memory_request_condition_.wait(lck, [&]() { return condition_wait_check_.load(); });
      condition_wait_check_.store(false);
      HandleRequesterReq(id_value);
      HandlePublisherReq();
    }
  }));
}

void DataBroadcasterImpl::HandlePublisherReq() {
  while (!publisher_pool_.empty()) {
    std::size_t signal_id;
    publisher_pool_.pop(signal_id);
    auto signal = processing_signals_.find(signal_id);
    if (signal != processing_signals_.end()) {
      signal->second->status_ = BroadcastingStatus::FREE;
      processing_signals_.erase(signal);
    }
  }
}

BroadcastDataTypeIter DataBroadcasterImpl::GetRefToNextfreeMemSlot() { return next_free_mem_slot_; }

MapIdToIterDataType DataBroadcasterImpl::GetProcessedSignalTracker() {
  std::lock_guard<std::mutex> lck{requester_pool_sync};
  return processing_signals_;
}

std::list<BroadcastDataType> DataBroadcasterImpl::GetSignalPool() {
  std::lock_guard<std::mutex> lck{requester_pool_sync};
  return signal_pool_;
}

void DataBroadcasterImpl::SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) {

  forward_message_callback_ = callback;
}

void DataBroadcasterImpl::MessageCallback(const std::vector<uint8_t> &data,
                                          const bridgesdk::webserver::ConnectionType connection_type) {
  forward_message_callback_(data, connection_type);
}

bool DataBroadcasterImpl::IsMemorySlotValid(BroadcastDataType &data) {
  if (data.status_ != BroadcastingStatus::FREE) {
    // clear memory slot as fallback (risky)
    auto time_diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - data.acquire_time_);
    if (time_diff.count() > kMaxDurationValidMemInMs) {
      error(__FILE__, "Data loss due to memory slot override.");
      data.status_ = BroadcastingStatus::FREE;
      return true;
    }
  } else if (data.status_ == BroadcastingStatus::FREE) {
    return true;
  }
  return false;
}

} // namespace data_broadcaster
} // namespace databridge
} // namespace next
