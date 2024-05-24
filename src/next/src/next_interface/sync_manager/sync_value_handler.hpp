#ifndef SYNC_VALUE_HANDLER_H_
#define SYNC_VALUE_HANDLER_H_

#include <next/data_types.hpp>

#include <next/udex/data_types.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

namespace next {

class SyncValueHandler {
public:
  SyncValueHandler(std::string selection_url, std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber);

private:
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_;
  std::shared_ptr<next::udex::extractor::DataExtractor> sync_value_selector_extractor_;
}; // SyncValueHandler

} // namespace next

#endif // SYNC_VALUE_HANDLER_H_