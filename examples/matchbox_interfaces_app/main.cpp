/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */
#include <chrono>
#include <iostream>
#include <thread>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>
#include "next/appsupport/startup.hpp"
#include "next/control/event_types/player_command.h"

class TreeExtractorTester {
public:
  TreeExtractorTester() {
    std::cout << "This app provides interface calls" << std::endl;

    next::udex::struct_extractor::PackageTreeExtractor tester_tree("ADC4xx.EMF.EMFGlobalTimestamp");
    std::cout << tester_tree.SetupSuccessful() << std::endl;
    NEXT_DISABLE_DEPRECATED_WARNING
    std::cout << tester_tree.init_successful_ << std::endl;
    NEXT_RESTORE_WARNINGS
    std::cout << tester_tree.SetupSuccessful() << std::endl;
    for (const auto &signal : tester_tree.signals_) {
      std::cout << signal.name << std::endl;
      std::cout << (int)signal.value->GetTypeOfAnyValue() << std::endl;
      std::cout << signal.value->ArraySize() << std::endl;
      std::cout << signal.value->Get().asDouble << std::endl;
      std::cout << signal.value->Get(0).asDouble << std::endl;
    }
  }
  ~TreeExtractorTester() {}
};

class ExtractorTester {
public:
  ExtractorTester() {
    auto _subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("_Subscriber");
    auto _extractor = std::make_shared<next::udex::extractor::DataExtractor>();
    auto _explorer = std::make_shared<next::udex::explorer::SignalExplorer>();

    _subscriber->Subscribe("test", _extractor);
    _extractor->connectCallback([](std::string) {}, "test");
    _extractor->SetBlockingExtractionForUrl("test", true);
    auto signalInfo = _extractor->GetInfo("test");
    std::cout << "result data array size" << signalInfo.array_size;
    next::udex::TimestampedDataPacket result_data = _extractor->GetExtractedData("test");
    std::cout << "result data time " << result_data.timestamp;
    next::udex::dataType data;
    _extractor->GetValue("test", next::udex::SignalType::SIGNAL_TYPE_UINT64, data);
    auto result = _explorer->GetPackageUrl("test");
    auto search_result = _explorer->searchSignalTree("test");
    auto sdl_result = _explorer->generateSdl("test");
  }
  ~ExtractorTester() {}
};

class PlayerCommandTester {
public:
  PlayerCommandTester() {
    auto _player_command_event = std::make_shared<next::control::events::PlayerCommandEvent>("command handler event");
    _player_command_event->publish({});
    _player_command_event->addEventHook([]() {});
    _player_command_event->subscribe();
    _player_command_event->getEventData();
  }
  ~PlayerCommandTester() {}
};

class PublisherTester {
public:
  PublisherTester() {
    auto publisher = std::make_shared<next::udex::publisher::DataPublisher>("");
    publisher->RegisterDataDescription("filepath", true, DESCRIPTION_TYPE_SDL);
    std::vector<char> test;
    test.push_back(1);
    publisher->RegisterDataDescription("filepath", test.data(), test.size(), true);
    publisher->publishData("test", test.data(), test.size());
    publisher->Reset();
  }
  ~PublisherTester() {}
};

int main(int, char **) {
  int exit_code = 0;
  next::appsupport::NextStartUp::InitializeNext("tester", {}, true);

  auto test = TreeExtractorTester();
  auto testextractor = ExtractorTester();
  auto testPlayerCommand = PlayerCommandTester();
  auto testPublisherTester = PublisherTester();

  return (exit_code);
}
