/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     web_server_impl.hpp
 * @brief    Handling websocket connections
 */

#ifndef NEXT_BRIDGESDK_WEBSERVER_WEB_SERVER_HPP
#define NEXT_BRIDGESDK_WEBSERVER_WEB_SERVER_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include <next/bridgesdk/web_server.hpp>

#include "tcp_session.hpp"

namespace next {
namespace bridgesdk {
namespace webserver {

//!  WebServer server to handle incoming connections to forward and receive message
/*!
 */
class WebServer : public IWebServer, public std::enable_shared_from_this<WebServer> {
public:
  WebServer(const std::string &ip_address = "127.0.0.1", uint16_t port_number = 8080, std::uint8_t num_connections = 1);
  virtual ~WebServer();

  virtual void SetIpAddress(const std::string &ip_address) override;
  virtual void SetPortNumber(uint16_t port_number) override;
  virtual size_t GetNumOfActiveConnections() override;
  virtual void SetNumOfConnections(std::uint8_t num_connections) override;
  virtual void BroadcastMessageWithNotification(const std::vector<uint8_t> &data, size_t message_id,
                                                std::vector<size_t> session_ids,
                                                std::function<void(size_t)> notification_callback,
                                                bool binary = false) override;
  virtual void StartListeningToPort(boost::asio::ip::tcp::endpoint endpoint);
  virtual void ActivateListingToSocket() override;
  virtual std::unordered_map<size_t, std::shared_ptr<Session>> &GetActiveSessions() override;

  void DoAcceptConnection();
  void OnAcceptConnection(boost::system::error_code ec);
  void OnSessionClose(size_t session_id);
  void AddForwardMessageCallback(ForwardMsgCallback callback);
  virtual std::vector<size_t> GetSessionIds(const ConnectionType type) const;

private:
  void ForwardMessageCallback(const std::vector<uint8_t> &data, const ConnectionType);

  // Function which is called by the session, to inform the web server about the connection type
  /*
   * After the client upgraded the session to WebSocket this function is called by the session handler. The web server
   * can then handle the session accordingly.
   */
  virtual void SessionTypeChanged(const size_t session_id, const ConnectionType connection_type);

  boost::asio::ip::address ip_address_;
  uint16_t port_number_;
  std::uint8_t num_connections_;
  std::unique_ptr<boost::asio::io_context> ioc_;
  std::vector<std::thread> ioc_threads_;
  mutable std::mutex sessions_list_mutex_;
  // Listening
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
  std::unordered_map<size_t, std::shared_ptr<Session>> active_sessions_all_;
  std::unordered_map<ConnectionType, std::unordered_map<size_t, std::shared_ptr<Session>>> active_sessions_type_;
  mutable std::mutex sessions_mutex_;
  std::vector<ForwardMsgCallback> forward_message_callbacks_;
  std::mutex callback_list_mutex_;
  size_t session_id_{0};
  boost::optional<boost::asio::io_context::work> work_; // Helper for closing safely the connection.
  std::chrono::time_point<std::chrono::steady_clock> after_send_time_;
};
} // namespace webserver
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_WEBSERVER_WEB_SERVER_HPP
