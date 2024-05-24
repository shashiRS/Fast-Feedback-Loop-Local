/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_request_server_tests.cpp
 *  @brief    DataRequestServer unit tests
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <../DataClassManager/data_request_server.h>
#include "data_class_manager.h"

using namespace std::chrono_literals;
using namespace next::databridge;
using namespace next::databridge::data_request_server;
/**
 * This unit-test case tests work of DataRequestServer in single thread mode.
 */
TEST(DATA_REQUEST_SERVER, SINGLE_THREAD_TEST) {
  // creating DataRequestServer sp_server and DataClassManager
  auto sp_server = std::make_shared<DataRequestServer>();
  auto sp_manager = std::make_shared<DataClassManager>();
  // setting up WebServer port
  sp_server->StartupWebServer("127.0.0.1", 17001);
  // connecting Data Class Manager with Data Request Server
  sp_manager->SetupDataRequestServer(sp_server);
  // adding requests with different priority to the queue and
  // checking the request on the top (the higher priority the higher position of the request in the queue).

  const std::string request_prio_0a{R"({
   "priority": 0,
   "channel": "dataStream",
   "event": "3DView",
   "source": "NextGUI",
   "payload": {
     "dataClasses":[{"name": "classname1", "version": "0.0.1", "arguments": ["test1", "test2"]}, 
                    {"name": "classname2", "version": "0.0.1", "arguments": ["test1", "test2"]}]
     }
   })"};
  const std::string request_prio_1a{R"({
   "priority": 1,
   "channel": "dataStream",
   "event": "GeneralWidgetEvent",
   "source": "NextGUI",
   "payload": {
     "dataClass": { "name": "dummy", "version": "0.1.1", "arguments":["asdf","asdfasdf"]}
   }
   })"};
  const std::string request_prio_1b{R"({
   "priority": 1,
   "channel": "dataStream",
   "event": "UsedDataStreamUrisChanged",
   "source": "NextGUI",
   "payload": {
     "usedUris": ["some.uri.here.2"]
   }
   })"};
  const std::string request_prio_2{R"({
   "priority": 2,
   "channel": "dataStream",
   "event": "UsedDataStreamUrisChanged",
   "source": "NextGUI",
   "payload": {
     "usedUris": ["some.uri.here.1", "some.another.uri.here.2"]
   }
   })"};
  const std::string request_prio_0b{R"({
   "priority": 0,
   "channel": "dataStream",
   "event": "UsedDataStreamUrisChanged",
   "source": "NextGUI",
   "payload": {
     "usedUris": ["some.uri.here.1", "some.another.uri.here.2", "some.another.uri.here.3"]
   }
   })"};

  sp_server->AddRequestToQueue(request_prio_0a, next::bridgesdk::webserver::ConnectionType::normal);
  EXPECT_EQ(request_prio_0a, sp_server->GetTopRatedRequest().request);

  sp_server->AddRequestToQueue(request_prio_1a, next::bridgesdk::webserver::ConnectionType::normal);
  EXPECT_EQ(request_prio_1a, sp_server->GetTopRatedRequest().request);

  sp_server->AddRequestToQueue(request_prio_1b, next::bridgesdk::webserver::ConnectionType::normal);
  EXPECT_EQ(request_prio_1a, sp_server->GetTopRatedRequest().request);

  sp_server->AddRequestToQueue(request_prio_2, next::bridgesdk::webserver::ConnectionType::normal);
  EXPECT_EQ(request_prio_2, sp_server->GetTopRatedRequest().request);

  sp_server->AddRequestToQueue(request_prio_0b, next::bridgesdk::webserver::ConnectionType::normal);
  EXPECT_EQ(request_prio_2, sp_server->GetTopRatedRequest().request);

  // handling all the requests and remove them from the queue
  sp_server->CheckQueueAndReply();
  EXPECT_EQ(0, sp_server->GetRequestNbr());
}

/**
 * This unit-test case tests work of DataRequestServer in multi thread mode.
 */
TEST(DATA_REQUEST_SERVER, MULTI_THREAD_TEST) {
  // creating DataRequestServer sp_server and DataClassManager
  auto sp_server = std::make_shared<DataRequestServer>();
  auto sp_manager = std::make_shared<DataClassManager>();
  // setting up WebServer port
  sp_server->StartupWebServer("127.0.0.1", 17002);
  // connecting Data Class Manager with Data Request Server
  // sp_server->SetupDataClassManager(sp_manager);
  sp_manager->SetupDataRequestServer(sp_server);

  // handling all the requests and remove them from the queue in background mode
  sp_server->CheckQueueAndReplyInBackgroundMode();

  // adding requests with different priority to the queue

  const std::string request_prio_0a{R"({
  "priority": 0,
  "channel": "dataStream",
  "event": "UsedDataStreamUrisChanged",
  "source": "NextGUI",
  "payload": {
    "usedUris": ["some.uri.here.1"]
  }
  })"};
  const std::string request_prio_0b{R"({
  "priority": 0,
  "channel": "dataStream",
  "event": "UsedDataStreamUrisChanged",
  "source": "NextGUI",
  "payload": {
    "usedUris": ["some.uri.here.1", "some.another.uri.here.2", "some.another.uri.here.3"]
  }
  })"};
  const std::string request_prio_1{R"({
  "priority": 1,
  "channel": "dataStream",
  "event": "UsedDataStreamUrisChanged",
  "source": "NextGUI",
  "payload": {
    "usedUris": ["some.uri.here.2"]
  }
  })"};
  const std::string request_prio_2{R"({
  "priority": 2,
  "channel": "dataStream",
  "event": "UsedDataStreamUrisChanged",
  "source": "NextGUI",
  "payload": {
    "usedUris": ["some.uri.here.1", "some.another.uri.here.2"]
  }
  })"};

  sp_server->AddRequestToQueue(request_prio_0a, next::bridgesdk::webserver::ConnectionType::normal);
  sp_server->AddRequestToQueue(request_prio_1, next::bridgesdk::webserver::ConnectionType::normal);
  sp_server->AddRequestToQueue(request_prio_2, next::bridgesdk::webserver::ConnectionType::normal);
  sp_server->AddRequestToQueue(request_prio_0b, next::bridgesdk::webserver::ConnectionType::normal);

  // waiting for 150 milli second and check if the queue is empty
  // all the requests should be handled and removed from the queue in background mode
  std::this_thread::sleep_for(150ms);
  EXPECT_EQ(0, sp_server->GetRequestNbr());
}

/**
 * This unit-test case tests event handling in DataClassManager.
 */
TEST(DATA_CLASS_MANAGER, EVENT_HANDLING) {
  // creating DataRequestServer sp_server and DataClassManager
  auto sp_server = std::make_shared<DataRequestServer>();
  auto sp_manager = std::make_shared<DataClassManager>();
  // setting up WebServer port
  sp_server->StartupWebServer("127.0.0.1", 17003);
  // connecting Data Class Manager with Data Request Server
  sp_manager->SetupDataRequestServer(sp_server);

  // adding request of "3DView" type
  sp_server->AddRequestToQueue(
      R"(
  {
  "priority": 0,
  "channel": "dataStream",
  "event": "3DView",
  "source": "NextGUI",
  "payload": {
     "dataClasses":[{"name": "classname1", "version": "0.0.1", "arguments": ["test1", "test2"]}, 
                    {"name": "classname2", "version": "0.0.1", "arguments": ["test3", "test4"]}]
     }
   })",
      next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();

  DataClassManager::DataRequestItem data_req_item;

  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "classname1");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], "test1");
  EXPECT_EQ(data_req_item.arguments[1], "test2");

  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "classname2");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], "test3");
  EXPECT_EQ(data_req_item.arguments[1], "test4");

  // adding request of "GeneralWidgetEvent" type
  sp_server->AddRequestToQueue(R"({
    "priority": 1,
    "channel": "dataStream",
    "event": "GeneralWidgetEvent",
    "source": "NextGUI",
    "payload": {
     "dataClass": { "name": "dummy", "version": "0.1.1", "arguments":["asdf","asdfasdf"]}
    }})",
                               next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "dummy");
  EXPECT_EQ(data_req_item.version, "0.1.1");
  EXPECT_EQ(data_req_item.arguments[0], "asdf");
  EXPECT_EQ(data_req_item.arguments[1], "asdfasdf");

  // adding request of "UsedDataStreamUrisChanged" type
  sp_server->AddRequestToQueue(
      R"({
   "priority": 1,
   "channel": "dataStream",
   "event": "UsedDataStreamUrisChanged",
   "source": "NextGUI",
   "payload": {
     "usedUris":["some.uri.here.1","some.another.uri.here.2"]
   }
   })",
      next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "signals");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], R"({"usedUris":["some.uri.here.1","some.another.uri.here.2"]})");

  // adding request of "UserPressedSignalUpdateButton" type
  sp_server->AddRequestToQueue(
      R"(
    {
    "channel": "dataStream",
    "event": "UserRequestedDataStreamFormat",
    "source": "NextGUI",
    "payload": {
      "selectedNodeId": "device1.signal.uri.path"
    }
  })",
      next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "SignalList");
  EXPECT_EQ(data_req_item.version, "1.0.0");
  EXPECT_EQ(data_req_item.arguments[0], R"("device1.signal.uri.path")");

  // adding request of "Shutdown" type
  sp_server->AddRequestToQueue(
      R"({
   "priority": 1,
   "channel": "dataStream",
   "event": "Shutdown",
   "source": "NextGUI",
   "payload": {
     "usedUris":["some.uri.here.1","some.another.uri.here.2"]
   }
   })",
      next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "Shutdown");
  EXPECT_EQ(data_req_item.version, "1.0.0");
  EXPECT_EQ(data_req_item.arguments.size(), 0);

  // adding request of "WidgetRequestsDataStreamUpdateEvent" type
  sp_server->AddRequestToQueue(
      R"(
  {
  "priority": 0,
  "channel": "dataStream",
  "event": "WidgetRequestsDataStreamUpdateEvent",
  "source": "NextGUI",
  "payload": {
     "dataClass":{"name": "classname1", "version": "0.0.1"},
     "arguments": ["test1", "test2"]
     }
   })",
      next::bridgesdk::webserver::ConnectionType::normal);

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "classname1");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], "test1");
  EXPECT_EQ(data_req_item.arguments[1], "test2");

  // adding request of "WidgetStartUpCommandChanged" type
  sp_server->AddRequestToQueue(
      R"(
  {
  "priority": 0,
  "channel": "dataStream",
  "event": "WidgetStartUpCommandChanged",
  "source": "NextGUI",
  "payload": {
     "dataClasses":[{"name": "classname1", "version": "0.0.1", "arguments": ["test1", "test2"]}, 
                    {"name": "classname2", "version": "0.0.1", "arguments": ["test3", "test4"]}]
     }
   })",
      next::bridgesdk::webserver::ConnectionType::normal);
  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "classname1");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], "test1");
  EXPECT_EQ(data_req_item.arguments[1], "test2");

  sp_server->CheckQueueAndReply();
  sp_manager->GetDataClassRequestNonBlocking(data_req_item);
  EXPECT_EQ(data_req_item.name, "classname2");
  EXPECT_EQ(data_req_item.version, "0.0.1");
  EXPECT_EQ(data_req_item.arguments[0], "test3");
  EXPECT_EQ(data_req_item.arguments[1], "test4");
}

/**
 * This unit-test case tests using same port multiple times for different
 * webserver and should not crash on shutdown
 */
TEST(DATA_CLASS_MANAGER, USE_SAME_PORT_FOR_DIFF_WEBSERVER) {
  // creating DataRequestServer sp_server and DataClassManager
  auto sp_server = std::make_shared<DataRequestServer>();
  auto sp_server_1 = std::make_shared<DataRequestServer>();
  auto sp_manager = std::make_shared<DataClassManager>();
  auto sp_manager_1 = std::make_shared<DataClassManager>();

  // setting up WebServer port
  sp_server->StartupWebServer("127.0.0.1", 17003);
  // connecting Data Class Manager with Data Request Server
  sp_manager->SetupDataRequestServer(sp_server);
  sp_server->ShutDownServer();

  // setting up WebServer1 port
  sp_server_1->StartupWebServer("127.0.0.1", 17003);
  // connecting Data Class Manager with Data Request Server
  sp_manager_1->SetupDataRequestServer(sp_server_1);
  sp_server_1->ShutDownServer();
}
