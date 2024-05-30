/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     tcp_session.cpp
 * @brief    Handling tcp sessions
 */

#include <tcp_session.hpp>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/error.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace bridgesdk {
namespace webserver {

const std::string kSupportedFoxgloveProtocol{"foxglove.websocket.v1"};

Session::Session(boost::asio::ip::tcp::socket &&socket, size_t session_id, ForwardMsgCallback frw_msg_callback,
                 SessionClosedNotificationCallback session_close_notification,
                 SessionTypeChangeCallback session_type_change_notification)
    : ws_(std::move(socket)), session_id_{session_id}, forward_message_callback_{frw_msg_callback},
      session_close_notification_{session_close_notification}, session_type_change_notification_{
                                                                   session_type_change_notification} {
  debug(__FILE__, "Session created with ID: {0}", session_id_);
  send_thread_ = std::thread([this] { fill_queue_context_.run(); });
}

Session::~Session() {
  debug(__FILE__, "Session is closed with ID: {0}", session_id_);
  boost::system::error_code ec;
  // TODO how to close? is ws_.close() fine?
  // ws_.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
  ws_.close(boost::beast::websocket::close_code::normal, ec);
  session_close_notification_(session_id_);
  work_guard_.reset();
  if (send_thread_.joinable()) {
    send_thread_.join();
  }
}

void Session::Run() {
  boost::beast::http::async_read(ws_.next_layer(), buffer_, upgrade_request_,
                                 boost::beast::bind_front_handler(&Session::OnUpgrade, shared_from_this()));
}

void Session::OnUpgrade(boost::beast::error_code ec, size_t) {
  if (ec) {
    error(__FILE__, "OnUpgrade failed: ", ec.message());
    return;
  }
  boost::beast::http::token_list client_tokens(upgrade_request_[boost::beast::http::field::sec_websocket_protocol]);
  auto tokenIt = std::find(client_tokens.begin(), client_tokens.end(), kSupportedFoxgloveProtocol);
  connection_type_ = ConnectionType::normal;
  if (client_tokens.end() != tokenIt) {
    debug(__FILE__, "Foxglove request recognized, switching to Foxglove mode.");
    ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
      // Set the client field on the request
      res.set(boost::beast::http::field::sec_websocket_protocol, kSupportedFoxgloveProtocol);
    }));
    connection_type_ = ConnectionType::foxglove;
  } else {
    debug(__FILE__, "No special WebSocket upgrade request, keeping normal mode (e.g. not switching to "
                    "foxglove).");
  }
  session_type_change_notification_(session_id_, connection_type_);
  ws_.async_accept(upgrade_request_, boost::beast::bind_front_handler(&Session::OnAccept, shared_from_this()));
}

void Session::OnAccept(boost::system::error_code ec) {
  if (ec) {
    error(__FILE__, "Failing accept connection: {0}", ec.message());
    return;
  }
  if (ConnectionType::foxglove == connection_type_) {
    boost::json::object server_info_json;
    server_info_json["op"] = "serverInfo";
    server_info_json["name"] = "NEXT simulation backend";
    boost::json::array arr;
    arr = {"time"};
    server_info_json["capabilities"] = arr;
    server_info_json["sessionId"] = std::to_string(session_id_); // TODO check id

    std::string server_info_string = boost::json::serialize(server_info_json);
    ws_.write(boost::asio::buffer(server_info_string));
  }
  // Read a message
  DoRead();
}

void Session::DoRead() {
  try {
    // Read a message into our buffer
    ws_.async_read(buffer_, boost::beast::bind_front_handler(&Session::OnRead, shared_from_this()));
  } catch (...) {
    warn(__FILE__, "Failing to read new message from socket");
  }
}

void Session::OnRead(boost::system::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    // This indicates that the session was closed
    if (ec == boost::beast::websocket::error::closed || ec == boost::asio::error::operation_aborted ||
        ec == boost::asio::error::connection_aborted) {
      debug(__FILE__, "Session close-request received with ID: {0} and message: {1} and value: {2}", session_id_,
            ec.message(), ec.value());
      if (session_close_notification_) {
        session_close_notification_(session_id_);
      }
    } else {
      warn(__FILE__, "Failing reading message: {0} and value: {1}", ec.message(), ec.value());
    }
    return;
  }

  std::vector<uint8_t> data(buffer_.size());
  buffer_.prepare(buffer_.size());

  long offset = 0;
  for (auto it : buffer_.data()) {
    std::copy(static_cast<const uint8_t *>(it.data()), static_cast<const uint8_t *>(it.data()) + it.size(),
              data.begin() + offset);
    offset += static_cast<long>(it.size());
  }
  buffer_.commit(buffer_.size());
  buffer_.consume(buffer_.size());
  if (forward_message_callback_) {
    forward_message_callback_(data, connection_type_);
  }
  DoRead();
}

void Session::DoWrite(const std::vector<std::uint8_t> *data, size_t message_id,
                      std::function<void(size_t)> notification_callback, bool binary) {
  std::lock_guard<std::mutex> lk(mtx_queue_);
  msg_queue_.push({data, message_id, binary, notification_callback});

  if (!msg_queue_.empty() && write_ongoing_.load()) {
    return; // writing is ongoing
  } else {
    bool f{false};
    if (write_ongoing_.compare_exchange_strong(f, true)) {
      if (binary) {
        ws_.binary(true);
      } else {
        ws_.text(true);
      }
      ws_.async_write(
          boost::asio::buffer(*data, data->size()),
          boost::beast::bind_front_handler(&Session::OnWrite, shared_from_this(), message_id, notification_callback));
    }
  }
}

//! Start writing message into socket.
/*!
 * @param ec Error occurs during write.
 * @param bytes_transferred Number of byte could transfer.
 * @param message_id ID of published message.
 * @param notification_callback Notification callback after message has been send.
 */
void Session::OnWrite(size_t message_id, std::function<void(size_t)> notification_callback, boost::beast::error_code ec,
                      std::size_t bytes_transferred) {

  std::lock_guard<std::mutex> lk(mtx_queue_);
  boost::ignore_unused(bytes_transferred);

  if (ec == boost::beast::websocket::error::closed || ec == boost::asio::error::operation_aborted) {
    warn(__FILE__, "Failing publishing message: {0} and value: {1}", ec.message(), ec.value());
    notification_callback(message_id);
    session_close_notification_(session_id_);
    write_ongoing_ = false;
    return;
  }

  if (ec) {
    warn(__FILE__, "Failing publishing message: {0} and value: {1}", ec.message(), ec.value());
    notification_callback(message_id);
    msg_queue_.reset();
    write_ongoing_ = false;
    return;
  }

  notification_callback(message_id);
  msg_queue_.pop();

  if (msg_queue_.read_available() > 800) {
    msg_queue_.consume_all([&](const MsgQueueData &elm) { elm.notification_callback_(elm.id_); });
    warn(__FILE__, "reset the message queue. packets lost!!!");
  }

  if (msg_queue_.read_available() > 0) {
    const auto &front = msg_queue_.front();
    if (front.binary_) {
      ws_.binary(true);
    } else {
      ws_.text(true);
    }
    if (front.data_->size() != 0) {
      ws_.async_write(boost::asio::buffer(*(front.data_), front.data_->size()),
                      boost::beast::bind_front_handler(&Session::OnWrite, shared_from_this(), front.id_,
                                                       front.notification_callback_));
    } else {
      warn(__FILE__, "Received an empty payload with message id: {0}. Stop ongoing writing...", front.id_);
      // front.notification_callback_(front.id_);
      msg_queue_.pop();
      write_ongoing_ = false;
    }
  } else {
    write_ongoing_ = false;
  }
}

void Session::DoClose() {
  ws_.async_close(boost::beast::websocket::close_code::normal, [this](boost::system::error_code ec) {
    if (ec.value()) {
      warn(__FILE__, "Failing closing the session with message: {0} and value: {1}", ec.message(), ec.value());
    }
    session_close_notification_(session_id_);
  });
}

void Session::SetForwardMessageCallback(ForwardMsgCallback callback) { forward_message_callback_ = callback; }

void Session::Send(const std::vector<std::uint8_t> *data, size_t message_id,
                   std::function<void(size_t)> notification_callback, bool binary) {
  boost::asio::post(fill_queue_context_, boost::beast::bind_front_handler(&Session::DoWrite, shared_from_this(), data,
                                                                          message_id, notification_callback, binary));
}

} // namespace webserver
} // namespace bridgesdk
} // namespace next
