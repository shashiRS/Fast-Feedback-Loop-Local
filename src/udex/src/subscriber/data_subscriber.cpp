/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_subscriber.cpp
 * @brief    class to handle data subscribing
 */

#include "next/udex/subscriber/data_subscriber.hpp"
#include "data_subscriber_impl.h"

namespace next {
namespace udex {
namespace subscriber {

DataSubscriber::DataSubscriber(const std::string &data_subscriber_name) {
  impl_ = std::make_unique<DataSubscriberImpl>(data_subscriber_name);
}

DataSubscriber::~DataSubscriber() { impl_.reset(); }

bool DataSubscriber::Reset(void) {
  if (!impl_) {
    return false;
  }

  return impl_->Reset();
}

std::string DataSubscriber::getName() {
  if (!impl_) {
    return "Impl not initialized";
  }
  return impl_->getName();
}

size_t DataSubscriber::Subscribe(const std::string &url, std::shared_ptr<extractor::DataExtractor> dataExtractor,
                                 const bool force_subscription) {
  if (!impl_) {
    return 0;
  }

  return impl_->Subscribe(url, dataExtractor, force_subscription);
}

void DataSubscriber::Unsubscribe(const size_t id) {
  if (!impl_) {
    return;
  }

  impl_->Unsubscribe(id);
}

std::map<size_t, SubscriptionInfo> DataSubscriber::getSubscriptionsInfo() {
  if (!impl_) {
    return {};
  }

  return impl_->getSubscriptionsInfo();
}

bool DataSubscriber::IsConnected(const std::string &url) {
  if (!impl_) {
    return false;
  }

  return impl_->IsConnected(url);
}

} // namespace subscriber
} // namespace udex
} // namespace next
