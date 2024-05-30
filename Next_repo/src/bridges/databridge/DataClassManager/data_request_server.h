/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_request_server.h
 *  @brief    stub
 */

#ifndef NEXT_DATABRIDGE_DATA_REQUEST_SERVER_H_
#define NEXT_DATABRIDGE_DATA_REQUEST_SERVER_H_

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

#include <json/json.h>

#include <next/bridgesdk/web_server.hpp>
#include "event_handler_helper.h"

namespace next {
namespace databridge {
class DataClassManager;
} // namespace databridge
} // namespace next

namespace next {
namespace databridge {
namespace data_request_server {

struct WebSocketRequest {
  int priority;
  std::string request;
  bridgesdk::webserver::ConnectionType connection_type;
  inline bool operator<(const WebSocketRequest &o) const { return priority < o.priority; }
};

/*!
 * The data request server enables a data request
 * from multiple connected clients. Its API allows
 * to activate or deactivate data classes provided
 * by the data bridge.
 */
class DataRequestServer {
public:
  //! Constructor
  DataRequestServer() = default;

  //!  Creates a TCP server
  /*!
   * @param port_set int value corresponding to the server ip.
   */
  void StartupWebServer(const std::string &ip_adress_set, uint16_t port_set);

  //! Set the used web server
  /*!
   * @param ptr The shared pointer of the web server.
   */
  void SetWebServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr);

  //! Get the used web server
  /*!
   * @return The shared pointer of the web server.
   */
  std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebServer();

  //!  Shut down a TCP server
  /*!
   * @param port_set int value corresponding to the server ip.
   */
  void ShutDownServer();

  //! Adds requests to m_requestQueue.
  /*!
   * @param req request (priority + text of the request).
   */
  void AddRequestToQueue(std::string req_str, bridgesdk::webserver::ConnectionType type);

  //! Handles requests and forward them to Data Classes Controller server.
  /*!
   * @param req request (priority + text of the request).
   */
  void HandleRequest(const WebSocketRequest &req);

  //! Checks and handles requests
  /*!
   * Checks if m_requestQueue is not empty,
   * And if it's so handles its requests.
   * And remove them from the container in the end.
   */
  void CheckQueueAndReply();

  //! Getter of the top rated request
  /*!
   * Mainly used for testing.
   *
   * @return const link to the top prioritized request
   */
  const WebSocketRequest &GetTopRatedRequest() const;

  //! Launches infinite loop of checkQueueAndReply invoking in a separate thread
  void CheckQueueAndReplyInBackgroundMode();

  //! Getter of request_queue size
  /*!
   * @return request_queue size
   */
  size_t GetRequestNbr() const;

  //! Setter of sp_data_class_manager
  /*!
   * @param spDataClassManager shared_ptr pointing to DataClassManager.
   */
  void SetupDataClassManager(std::shared_ptr<DataClassManager> spDataClassManager);

  //! A destructor.
  virtual ~DataRequestServer();

private:
  std::priority_queue<WebSocketRequest> request_queue;
  mutable std::mutex request_queue_mutex;
  // boost::lockfree::queue<req_type> request_queue;
  std::shared_ptr<next::bridgesdk::webserver::IWebServer> sp_web_server;
  std::weak_ptr<DataClassManager> wp_data_class_manager;

  std::atomic<bool> b_check_queue{true};
  std::future<void> check_queue_and_reply_fut;
  ui_request::EventHandlerHelper handler_;
};
} // namespace data_request_server
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_DATA_REQUEST_SERVER_H_
