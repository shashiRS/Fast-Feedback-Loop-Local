/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     web_server.hpp
 * @brief    Handling websocket connections
 */

#ifndef NEXT_BRIDGESDK_INTERFACE_WEB_SERVER_HPP
#define NEXT_BRIDGESDK_INTERFACE_WEB_SERVER_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace next {
namespace bridgesdk {
namespace webserver {

class Session;

enum class ConnectionType { normal, foxglove };
using ForwardMsgCallback = std::function<void(const std::vector<uint8_t> &, const ConnectionType)>;

class IWebServer {
public:
  IWebServer() = default;
  virtual ~IWebServer() = default;
  virtual void SetIpAddress(const std::string &ip_adress) = 0;
  virtual void SetPortNumber(uint16_t port_number) = 0;
  virtual size_t GetNumOfActiveConnections() = 0;
  virtual void SetNumOfConnections(uint8_t num_connections) = 0;
  virtual void BroadcastMessageWithNotification(const std::vector<uint8_t> &data, size_t message_id,
                                                std::vector<size_t> session_ids,
                                                std::function<void(size_t)> notification_callback,
                                                bool binary = false) = 0;
  virtual void AddForwardMessageCallback(ForwardMsgCallback callback) = 0;
  virtual void ActivateListingToSocket() = 0;
  virtual std::unordered_map<size_t, std::shared_ptr<Session>> &GetActiveSessions() = 0;
  virtual std::vector<size_t> GetSessionIds(const ConnectionType) const = 0;
};

std::shared_ptr<next::bridgesdk::webserver::IWebServer> CreateWebServer();

} // namespace webserver
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_INTERFACE_WEB_SERVER_HPP
