/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     web_server_impl.cpp
 * @brief    Handling tcp sessions
 */

#include "web_server_impl.hpp"

#include <array>
#include <list>

#include <next/sdk/logger/logger.hpp>

using boost::asio::ip::tcp;

namespace next {
namespace bridgesdk {
namespace webserver {

std::shared_ptr<next::bridgesdk::webserver::IWebServer> CreateWebServer() {
  return std::make_shared<next::bridgesdk::webserver::WebServer>();
}

//! Constructor to initialize parameters needed for making websocket connection.
/*!
 *
 * @param ip_address Ip address for listening to incoming request.
 * @param port_number Port number.
 * @param num_connections Number of connections can be handled simultaneously.
 */
WebServer::WebServer(const std::string &ip_address, uint16_t port_number, std::uint8_t num_connections)
    : ip_address_{boost::asio::ip::make_address(ip_address)}, port_number_{port_number},
      num_connections_{num_connections}, ioc_{std::make_unique<boost::asio::io_context>(num_connections_)},
      acceptor_{*ioc_.get()}, socket_{*ioc_.get()}, work_{*ioc_.get()} {}

//! Destructor for safe destructing threads.
/*!
 */
WebServer::~WebServer() {

  work_ = boost::none;

  if ((ioc_.get() != nullptr) && (!ioc_->stopped())) {
    ioc_->dispatch([&] {
      boost::system::error_code ec;
      acceptor_.cancel(ec);
      if (ec) {
        debug(__FILE__, "Could not close acceptor {0}", ec.message());
      }
    });

    ioc_->stop();

    for (auto &thrd : ioc_threads_) {
      thrd.join();
    }

    ioc_.release();
  }
  for (auto &thrd : ioc_threads_) {
    if (thrd.joinable()) {
      thrd.join();
    }
  }
  ioc_threads_.clear();
}

//! Set the Ip address to use for listening incoming request.
/*!
 *
 * @param ip_address Ip address for listening incoming request.
 */
void WebServer::SetIpAddress(const std::string &ip_address) { ip_address_ = boost::asio::ip::make_address(ip_address); }

//! Set the port number.
/*!
 *
 * @param port_number Port number.
 */
void WebServer::SetPortNumber(uint16_t port_number) { port_number_ = port_number; }

//! Set number of connections to be handled by websocket manager.
/*!
 *
 * @param num_connections Number of connections.
 */
void WebServer::SetNumOfConnections(std::uint8_t num_connections) { num_connections_ = num_connections; }

//! Calling the websocket write operation to send data to client.
/*!
 * After sending the data to all recipients a notification call back will be triggered to tell the broadcaster that the
 * message has been sent and the memory can be used for another signal again.
 *
 * Note: If a session ID is given twice in session_ids, the same data will be sent twice to the corresponding client.
 * This is not a bug, it is a feature.
 *
 * @param data The payload data to be published to client
 * @param message_id The ID of current memory slot for using later in memory pool manager.
 * @param notification_callback Notification callback after the message has been published.
 * @param binary Sending the data in binary format. If false, the data will be sent in text format.
 */
void WebServer::BroadcastMessageWithNotification(const std::vector<uint8_t> &data, size_t message_id,
                                                 std::vector<size_t> session_ids,
                                                 std::function<void(size_t)> notification_callback, bool binary) {
  std::lock_guard<std::mutex> mtx{sessions_mutex_};
  if (data.empty()) {
    warn(__FILE__, "empty data received!");
    notification_callback(message_id);
  }

  std::list<std::shared_ptr<Session>> valid_sessions;
  if (session_ids.empty()) {
    for (const auto &session : active_sessions_all_) {
      if (session.second) {
        valid_sessions.push_back(session.second);
      }
    }
  } else {
    for (const auto id : session_ids) {
      if (const auto it = active_sessions_all_.find(id); it != active_sessions_all_.end() && it->second) {
        valid_sessions.push_back(it->second);
      } else {
        warn(__FILE__, "The requested session {} is invalid, data not sent.", id);
      }
    }
  }

  if (!valid_sessions.empty()) {
    std::function<void(size_t)> callback{notification_callback};
    if (valid_sessions.size() > 1) {
      callback = [sessions = valid_sessions.size(), notification_callback](size_t id) {
        static unsigned int calls{0};
        if (++calls == sessions) {
          notification_callback(id);
        }
      };
    }
    for (auto s : valid_sessions) {
      s->Send(&data, message_id, callback, binary);
    }
  } else {
    notification_callback(message_id);
  }
}

//! Configuration to start accepting incoming connection.
/*!
 *
 * @param endpoint Contains IP address and port number for listening to.
 */
void WebServer::StartListeningToPort(tcp::endpoint endpoint) {
  boost::system::error_code ec;

  debug(__FILE__, "Start listening to Port: {0}", endpoint.port());

  // Open the acceptor.
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    warn(__FILE__, "Failing opening socket: {0}", ec.message());
    return;
  }

  // Allow address reuse.
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    warn(__FILE__, "Failing socket option setting: {0}", ec.message());
    return;
  }

  // Bind to the server address.
  acceptor_.bind(endpoint, ec);
  if (ec) {
    warn(__FILE__, "Failing connection binding: {0}", ec.message());
    return;
  }

  // Start listening for connections.
  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    warn(__FILE__, "Failing connection listening: {0}", ec.message());
    return;
  }

  if (!acceptor_.is_open()) {
    warn(__FILE__, "Acceptor is not open: {0}", ec.message());
    return;
  }
  DoAcceptConnection();
}

//! Start an asynchronous call for accepting new connection request.
/*!
 */
void WebServer::DoAcceptConnection() {
  acceptor_.async_accept(socket_, std::bind(&WebServer::OnAcceptConnection, shared_from_this(), std::placeholders::_1));
}

//! called from session when new data arrives
//! executes all registered callbacks with the same data
/*!
 * @param data incoming message
 */
void WebServer::ForwardMessageCallback(const std::vector<uint8_t> &data, const ConnectionType type) {
  std::lock_guard<std::mutex>{callback_list_mutex_};
  for (auto callback : forward_message_callbacks_) {
    if (callback) {
      callback(data, type);
    }
  }
}

//! Accepting the new incoming connection and initializing new session. The session will be put in a container for
//! later
//! tracking of active sessions.
/*!
 * @param ec The error code after accepting new connection.
 */
void WebServer::OnAcceptConnection(boost::system::error_code ec) {
  if (ec) {
    warn(__FILE__, "Failing accepting connection: {0}", ec.message());
  } else {
    debug(__FILE__, "Socket connection created with session ID number: {0}", session_id_);
    // Create the session and run it
    std::lock_guard<std::mutex> mtx{sessions_mutex_};
    auto new_session = std::make_shared<Session>(
        std::move(socket_), session_id_,
        std::bind(&WebServer::ForwardMessageCallback, shared_from_this(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&WebServer::OnSessionClose, shared_from_this(), std::placeholders::_1),
        std::bind(&WebServer::SessionTypeChanged, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    new_session->Run();
    active_sessions_all_[session_id_++] = new_session;
  }

  // Accept another connection
  DoAcceptConnection();
}

//! The function will be called after the session has been canceled from client side.
//! tracking of active sessions.
/*!
 * @param session_id The session id of closed session.
 */
void WebServer::OnSessionClose(size_t session_id) {
  std::lock_guard<std::mutex> mtx{sessions_mutex_};
  for (auto &pair : active_sessions_type_) {
    pair.second.erase(session_id);
  }
  active_sessions_all_.erase(session_id);
}
//! Return number of active sessions .
/*!
 * @return number of active sessions.
 */
size_t WebServer::GetNumOfActiveConnections() {
  std::lock_guard<std::mutex> mtx{sessions_mutex_};
  return active_sessions_all_.size();
}
//! Return the list of active sessions.
/*!
 * @return List of active sessions.
 */
std::unordered_map<size_t, std::shared_ptr<Session>> &WebServer::GetActiveSessions() {
  std::lock_guard<std::mutex> mtx{sessions_mutex_};
  return active_sessions_all_;
}

//! Running the io_contexts in separate threads and starting to accept incoming connections.
/*!
 */
void WebServer::ActivateListingToSocket() {

  debug(__FILE__, "Setup accepting connection within diff. thread ");
  StartListeningToPort(tcp::endpoint{ip_address_, port_number_});

  ioc_threads_.reserve(num_connections_);
  for (auto i = num_connections_; i > 0; --i) {
    try {
      ioc_threads_.emplace_back([this] { ioc_->run(); });
    } catch (const std::exception &e) {
      warn(__FILE__, "Error starting listening on the socket: {}", e.what());
    }
  }
}

//! Setup callback function to handle receiving message from client.
/*!
 *
 * @param callback Callback function called with received message from client.
 */
void WebServer::AddForwardMessageCallback(ForwardMsgCallback callback) {
  if (callback) {
    forward_message_callbacks_.push_back(std::move(callback));
  } else {
    warn(__FILE__, "AddForwardMessageCallback: the given callback is empty");
  }
}

std::vector<size_t> WebServer::GetSessionIds(const ConnectionType type) const {
  std::vector<size_t> res;
  std::lock_guard<std::mutex> mtx{sessions_mutex_};
  auto it = active_sessions_type_.find(type);
  if (active_sessions_type_.end() != it) {
    res.reserve(it->second.size());
    for (const auto &s : it->second) {
      res.push_back(s.first);
    }
  }
  return res;
}

void WebServer::SessionTypeChanged(const size_t session_id, const ConnectionType connection_type) {
  const std::array<ConnectionType, 2> connection_types{ConnectionType::normal, ConnectionType::foxglove};
  auto ptr_it = active_sessions_all_.find(session_id);
  debug(__FILE__, "Session type for session {} changed.", session_id);
  if (ptr_it == active_sessions_all_.end()) {
    warn(__FILE__, "For the session {} a change of the connection type is reported, but this session is unknown.",
         session_id);
    return;
  }
  // check whether the current session is already referenced in another map, if so delete it there
  for (ConnectionType type : connection_types) {
    if (connection_type == type) {
      continue;
    }
    if (auto it = active_sessions_type_.find(type); it != active_sessions_type_.end()) {
      it->second.erase(session_id);
    }
  }
  // and add it to the correct map
  active_sessions_type_[connection_type][session_id] = ptr_it->second;
}

} // namespace webserver
} // namespace bridgesdk
} // namespace next
