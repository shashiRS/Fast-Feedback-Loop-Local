#include "sync_value_handler.hpp"

#include <next/sdk/logger/logger.hpp>

namespace next {

SyncValueHandler::SyncValueHandler(
    std::string selection_url,
    [[maybe_unused]] std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber) {
  sync_value_selector_extractor_ = std::make_shared<next::udex::extractor::DataExtractor>();
  data_subscriber_->Subscribe(selection_url, sync_value_selector_extractor_);
}

} // namespace next