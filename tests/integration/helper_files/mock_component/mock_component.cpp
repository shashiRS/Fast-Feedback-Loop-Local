#include "mock_component.h"

#include <signal.h>

#include <next/appsupport/config/config_base.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/sdk/ecal/ecal_singleton.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profiler.hpp>
#include <next/sdk/shutdown_handler/shutdown_handler.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

namespace next {
namespace integration_test {
// variables and constands for the publisher to set the data source info
const std::string kCmdHead{"[mockComponent]"};
const std::string kNodeName{"dummy_node"};
const std::string kPatternSdl{".sdl"};
std::string dataSourceInfo = "";
constexpr uint16_t kDataSourceId{21u};
constexpr uint16_t kDataSourceInstanceNumber{41u};
// name of the sdl file to register the data description
std::string sdl_name{""};
// urls for the subscriber to kill, start, stop the publishing data
const std::string killTopicUrl{"waitingForKill"};
const std::string kStartPublishSignalUrl{"startPublishData"};
const std::string kStopPublishSignalUrl{"stopPublishData"};
const std::string kPublishSignalControlUrl{"signal_controll"};
const std::string kSignalTopicUrl{"test_signal.View1.Group1"};

std::shared_ptr<next::udex::publisher::DataPublisher> publisher;

void printHelp() {
  info(__FILE__, "{} This component is publishing a dummy Signal to inject it into the UDex", kCmdHead);
  info(__FILE__, "{} The mock_component is called like: dummy_component.exe <nodeID> <sdl_file.sdl>", kCmdHead);
}

void HandleArguments(int argc, char *argv[]) {
  if (argc != 3) {
    printHelp();
    exit(EXIT_FAILURE);
  } else {
    dataSourceInfo = argv[1];
    sdl_name = argv[2];

    try {
      publisher = std::make_shared<next::udex::publisher::DataPublisher>(kNodeName);
    } catch (const std::exception &e) {
      std::cerr << "exception: " << e.what();
      exit(EXIT_FAILURE);
    }

    if (sdl_name.find(kPatternSdl, 0) != std::string::npos) {
      if (!publisher->SetDataSourceInfo(dataSourceInfo, kDataSourceId, kDataSourceInstanceNumber)) {
        error(__FILE__, "{} SetkDataSourceInfo failed!", kCmdHead);
      }
      if (!publisher->RegisterDataDescription(sdl_name, true)) {
        error(__FILE__, "{} RegisterDataDescription failed!", kCmdHead);
      }
    } else {
      error(__FILE__, "{} Argument two is not a .sdl file", kCmdHead);
      exit(EXIT_FAILURE);
    }
  }
}

} // namespace integration_test
} // namespace next

using next::integration_test::kCmdHead;
using next::integration_test::kNodeName;

int main(int argc, char *argv[]) {
  // TODO: use life cycle management instead of initchain

  next::sdk::InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), kNodeName);
  initmap.emplace(next::sdk::logger::getFilenameCfgKey(), kNodeName + ".log");
  initmap.emplace(next::sdk::logger::getSentinelProductNameCfgKey(), kNodeName);
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OVERVIEW);
  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();

  if (!next::sdk::InitChain::Run(initmap)) {
    std::cerr << kCmdHead << " Failed to initialize the mock_component" << std::endl;
    return 1;
  }

  next::sdk::shutdown_handler::register_shutdown_handler();
  next::sdk::EcalSingleton::get_instance();

  next::integration_test::HandleArguments(argc, argv);

  auto subscriber = next::udex::subscriber::DataSubscriber(next::integration_test::kNodeName);
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  // create subscriber, just to check that compiling works
  auto id_1 = subscriber.Subscribe(next::integration_test::killTopicUrl, extractor);
  auto id_2 = subscriber.Subscribe(next::integration_test::kStartPublishSignalUrl, extractor);
  auto id_3 = subscriber.Subscribe(next::integration_test::kStopPublishSignalUrl, extractor);
  // now fake usage of them;
  (void)id_1;
  (void)id_2;
  (void)id_3;
  next::integration_test::Group1 group1;

  debug(__FILE__, "{} start publishing data...", kCmdHead);
  while (!extractor->isDataAvailable(next::integration_test::killTopicUrl) &&
         !next::sdk::shutdown_handler::shutdown_requested()) {
    if ((next::integration_test::sdl_name.find("test_signal.sdl", 0) != std::string::npos) &&
        next::integration_test::dataSourceInfo == "test_signal" &&
        extractor->isDataAvailable(next::integration_test::kStartPublishSignalUrl) &&
        !extractor->isDataAvailable(next::integration_test::kStopPublishSignalUrl)) {
      debug(__FILE__, "{} data is publishing...", next::integration_test::kCmdHead);
      next::integration_test::publisher->publishData(next::integration_test::kSignalTopicUrl, &group1, sizeof(group1));
      group1.counter_variables.counter_uint64_t += 1;
      group1.counter_variables.counter_double += 0.000010;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  debug(__FILE__, "{} close publishing data...", kCmdHead);
  next::integration_test::publisher->Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  next::sdk::InitChain::Reset();
  return 0;
};
