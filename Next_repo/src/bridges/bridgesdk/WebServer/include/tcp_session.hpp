/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     tcp_session.hpp
 * @brief    Handling tcp sessions
 */

#ifndef NEXT_BRIDGESDK_WEBSERVER_TCP_SESSION_HPP
#define NEXT_BRIDGESDK_WEBSERVER_TCP_SESSION_HPP

#if defined(_MSC_VER) && !defined(_WIN32_WINNT)
// set to using Windows SDK 10 (not supporting Windows 8.1 or older)
#define _WIN32_WINNT 0x0A00
#endif

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wsign-conversion")
NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wconversion")
#if defined(__GNUC__)
#if __GNUC_PREREQ(9, 0)
// -Wdeprecated-copy was introduced with gcc 9 (Ubuntu 18 uses gcc 7.5)
NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wdeprecated-copy")
#endif
#endif
NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wmaybe-uninitialized")
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
NEXT_RESTORE_WARNINGS_LINUX
#include <boost/asio/buffer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#if defined(__GNUC__)
#if __GNUC_PREREQ(9, 0)
NEXT_RESTORE_WARNINGS_LINUX
#endif
#endif
NEXT_RESTORE_WARNINGS_LINUX
NEXT_RESTORE_WARNINGS_LINUX

#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace bridgesdk {
namespace webserver {

using SessionClosedNotificationCallback = std::function<void(size_t)>;
using SessionTypeChangeCallback = std::function<void(const size_t, const ConnectionType)>;
constexpr const int kBufferSize = 1000;

class Session : public std::enable_shared_from_this<Session> {
public:
  //! Construct the session by overtaking the ownership of socket.
  /*!
   * @param socket The tcp socket.
   * @param session_id Current session ID.
   * @param frw_msg_callback The callback after receiving message from client.
   * @param session_close_notification The callback after client close the session.
   */
  explicit Session(boost::asio::ip::tcp::socket &&socket, size_t session_id, ForwardMsgCallback frw_msg_callback,
                   SessionClosedNotificationCallback session_close_notification,
                   SessionTypeChangeCallback session_type_change_notification);

  //! Destruct the current session.
  virtual ~Session();

  //! Start the asynchronous operation and Accept the websocket handshake.
  /*!
   * This function schedules an asynchronous read of the WebSocket upgrade request. The request will be stored in
   * buffer_ and OnUpgrade is called to handle it.
   */
  void Run();

  //! Start writing message into socket.
  /*!
   * @param data Payload to be published.
   * @param message_id ID of published message.
   * @param notification_callback Notification callback after message has been send.
   */
  void Send(const std::vector<std::uint8_t> *data, size_t message_id, std::function<void(size_t)> notification_callback,
            bool binary);

  //! Set forward message callback to forward read message.
  /*!
   * @param callback Callback to be called when message has been read.
   */
  void SetForwardMessageCallback(ForwardMsgCallback callback);

  //! Close the session.
  void DoClose();

  //! Get the connection type
  inline ConnectionType GetConnectionType() const { return connection_type_; }

private:
  //! Check the upgrade request
  /*!
   * Within this function it is checked whether Foxglove is connecting, inidcated by the supported protocol. If so a
   * decorator for answering the upgrade request towards WebSocket is added, which is adding that the foxglove protocol
   * is supported by the server.
   *
   * @param ec The error which might have occured.
   * @param size Count of bytes received as upgrade request, the request is stored in buffer_.
   */
  void OnUpgrade(boost::beast::error_code ec, size_t size);

  //! Start the reading socket for new message.
  /*!
   * When a foxglove connection is established, a reply is sent to foxglove. Otherwise this is skipped. DoRead is called
   * to schedule asynchronous reading.
   *
   * If via the error code an error establishing the connection is reported, an error message is printed and nothing is
   * done.
   *
   * @param ec The error which might occur during connection setup.
   */
  void OnAccept(boost::beast::error_code ec);

  //! Asynchronous reading of socket.
  void DoRead();

  //! Receive new message and forward via callback.
  /*!
   * @param ec Error occurs during reading of new message.
   * @param bytes_transferred The number of bytes could transfer.
   */
  void OnRead(boost::beast::error_code ec, std::size_t bytes_transferred);

  //! Start writing message into socket.
  /*!
   * @param data Payload to be published.
   * @param message_id ID of published message.
   * @param notification_callback Notification callback after message has been send.
   * @param binary Sending the data in binary mode, otherwise the text mode will be used.
   */
  void DoWrite(const std::vector<std::uint8_t> *data, size_t message_id,
               std::function<void(size_t)> notification_callback, bool binary);

  //! Start writing message into socket.
  /*!
   * @param ec Error occurs during write.
   * @param bytes_transferred Number of byte could transfer.
   * @param message_id ID of published message.
   * @param notification_callback Notification callback after message has been send.
   */
  void OnWrite(size_t message_id, std::function<void(size_t)> notification_callback, boost::beast::error_code ec,
               std::size_t bytes_transferred);

private:
  struct MsgQueueData {
    MsgQueueData(const std::vector<std::uint8_t> *data, const uint64_t id, const bool binary,
                 std::function<void(size_t)> notification_callback)
        : data_(data), id_(id), binary_(binary), notification_callback_(notification_callback){};
    MsgQueueData() = delete;
    const std::vector<std::uint8_t> *data_;
    const uint64_t id_;
    const bool binary_;
    std::function<void(size_t)> notification_callback_;
  };

  boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
  boost::beast::multi_buffer buffer_;
  size_t session_id_;
  ForwardMsgCallback forward_message_callback_;
  SessionClosedNotificationCallback session_close_notification_;
  SessionTypeChangeCallback session_type_change_notification_;
  boost::beast::websocket::close_reason session_close_reason_;
  boost::lockfree::spsc_queue<MsgQueueData> msg_queue_{kBufferSize};
  boost::beast::http::request<boost::beast::http::string_body> upgrade_request_;
  std::atomic<bool> write_ongoing_{false};
  ConnectionType connection_type_;
  std::mutex mtx_queue_;

  boost::asio::io_context fill_queue_context_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_ =
      boost::asio::make_work_guard(fill_queue_context_);
  std::thread send_thread_;
};

} // namespace webserver
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_WEBSERVER_TCP_SESSION_HPP
