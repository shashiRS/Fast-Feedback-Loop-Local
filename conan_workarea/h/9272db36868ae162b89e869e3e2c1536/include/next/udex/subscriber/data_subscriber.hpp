/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     Data_subscriber.h
 * @brief    Interface to handle the registration of a new data subscriber
 *           to enable receiving data via the simulation network
 */

#ifndef NEXT_UDEX_DATA_SUBSCRIBER_H
#define NEXT_UDEX_DATA_SUBSCRIBER_H

#include <memory>
#include <string>

#include <next/sdk/sdk_macros.h>

#include "../extractor/data_extractor.hpp"
#include "../udex_config.h"

namespace next {
namespace udex {
namespace subscriber {

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class DataSubscriberImpl;

struct SubscriptionInfo {
  size_t ecal_subscriber_id;
  std::shared_ptr<next::udex::extractor::DataExtractor> data_extractor;
  std::string url;
  std::string topic;
};

class UDEX_LIBRARY_EXPORT DataSubscriber {
public:
  DataSubscriber() = delete;

  //! creates a DataSubscriber with the given node name.
  /*! initialize ecal node
   *
   * @param data_subscriber_name  Name of the node
   */
  DataSubscriber(const std::string &data_subscriber_name);

  //! Destructor for Data Subscriber
  //! Shuts down the subscriptions eCAL node
  virtual ~DataSubscriber();

  //! Resets the subscribtions
  /*!
   * Removes all subscriptions and clears internal maps, resets database connection
   *
   * @return true if successful, false if error occured
   */
  virtual bool Reset(void);

  //! returns the internally used name
  virtual std::string getName();

  //! subscribe to signal url and provide an instance of Data Extractor to be used for this subscription
  /*!
   * @param url  Signal Url name that will be subscribed to
   * @param dataExtractor  Instance that will be used for extracting data
   * @return id if the subscription was successfully
   */
  virtual size_t Subscribe(const std::string &url, std::shared_ptr<extractor::DataExtractor> dataExtractor,
                           const bool force_subscription = true);

  //! Unsubscribe for a a DataSubscriber with the given node id.
  /*!
   * @param id  Name of the id of the node
   */
  virtual void Unsubscribe(const size_t id);

  //! get curent subscriptions information (ecal_id, url, topic, extractor)
  /*!
   * @return map with id as key and information for all the subscriptions
   */
  virtual std::map<size_t, SubscriptionInfo> getSubscriptionsInfo();

  /**
   * @brief Checks if the subscriber is connected to a specified URL.
   *
   * @param url The URL to check for connection status.
   * @return true if the subscriber is connected to the specified URL, false otherwise.
   */
  virtual bool IsConnected(const std::string &url);

private:
  std::unique_ptr<DataSubscriberImpl> impl_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace subscriber
} // namespace udex
} // namespace next
#endif // NEXT_UDEX_DATA_SUBSCRIBER_H
