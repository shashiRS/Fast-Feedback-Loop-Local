#include <unordered_map>

#include <cip_test_support/gtest_reporting.h>

#include <next/next.hpp>
#include <sync_manager/sync_manager.hpp>

#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/sdk.hpp>
#include <next_impl.hpp>

// #include <memory>

namespace next {

class SyncManagerTest : public SyncManager {
public:
  SyncManagerTest(const std::string &component_name) : SyncManager::SyncManager(component_name) {
    component_name_ = component_name;
  }

  void WriteTestConfig(const std::string &port_name = "Test", const std::string &sync_algo = "GET_LAST_ELEMENT",
                       const std::string &sync_mode = "Timestamp") {
    next::appsupport::ConfigClient::do_init(component_name_);
    auto config = next::appsupport::ConfigClient::getConfig();
    config->put("test", "test_value");
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getCompositionPortUrlKey(component_name_, port_name),
        "ADC4xx." + component_name_ + "." + port_name);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncAlgoKey(component_name_, port_name), sync_algo);
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncModeKey(component_name_, port_name), sync_mode);
  }
  std::unordered_map<std::string, SyncPortConfig> GetSyncPortConfigMap() { return this->m_sync_port_config_; }

  SyncMode GetSyncModeEnum(const std::string &sync_mode) {
    if (sync_mode == "timestamp") {
      return SYNC_TIMESTAMP;
    } else if (sync_mode == "sync_id") {
      return SYNC_SYNCID;
    } else if (sync_mode == "sync_ref_timestamp") {
      return SYNC_SYNC_REF_TIMESTAMP;
    } else if (sync_mode == "sync_ref_sync_id") {
      return SYNC_SYNC_REF_SYNCID;
    } else {
      return SYNC_UNSUPPORTED;
    }
  }

  void WriteSyncValueMap(const std::string &port_name, next::udex::SyncValue sync_value) {
    this->m_sync_value_.insert(std::make_pair(port_name, sync_value));
  };
  std::string component_name_;
  next::sdk::InitChain::ConfigMap initmap_;
};

class SyncManagerFixture : public next::sdk::testing::TestUsingEcal {
public:
  SyncManagerFixture() : TestUsingEcal() {
    this->instance_name_ = "sync_manager_test";
    next::sdk::logger::register_to_init_chain();
  }
  void SetUp() override { next::appsupport::ConfigClient::do_reset(); }
  void TearDown() override { next::appsupport::ConfigClient::do_reset(); }
};

TEST_F(SyncManagerFixture, define) {
  std::string component_name = "SyncTest";
  auto sync = std::make_shared<SyncManagerTest>(component_name);
  ASSERT_TRUE(sync);
}

TEST_F(SyncManagerFixture, setSyncConfig) {
  std::string component_name = "SyncTest";
  std::string sync_algo = "GET_LAST_ELEMENT", test_port = "test", sync_mode = "Timestamp",
              subscribe_url = "uiTimeStamp_FrSp";

  ComponentInputPortConfig input_port_config = {};
  input_port_config.port_name = test_port;
  input_port_config.port_size = sizeof(uint32_t);

  auto sync = std::make_shared<SyncManagerTest>(component_name);
  sync->WriteTestConfig(test_port, sync_algo, sync_mode);

  std::shared_ptr<next::udex::extractor::DataExtractor> extractor =
      std::make_shared<next::udex::extractor::DataExtractor>();
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber;
  data_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("component");
  auto sync_algo_cfg = sync->SetSyncConfig(input_port_config, subscribe_url, subscribe_url, extractor, data_subscriber);

  ASSERT_EQ(sync_algo_cfg.algorythm_type, next::udex::getSyncAlgoTypeEnum(sync_algo));
  ASSERT_EQ(sync_algo_cfg.composition_sync_signal_url, ("ADC4xx." + component_name + "." + test_port));
}

TEST_F(SyncManagerFixture, getSyncValue_CorrectInput) {
  std::string component_name = "SyncTest";
  std::string sync_algo = "GET_LAST_ELEMENT", test_port = "test", sync_mode = "Timestamp",
              subscribe_url = "uiTimeStamp_FrSp";

  auto sync = std::make_shared<SyncManagerTest>(component_name);
  // sync->WriteTestConfig(test_port, sync_algo, sync_mode);
  next::udex::SyncValue sync_value_input;
  uint32_t signal_value = 12345;
  sync_value_input.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT32;
  sync_value_input.signal_value = signal_value;
  sync->WriteSyncValueMap(test_port, sync_value_input);

  auto sync_value_response = sync->GetSyncValue(test_port);
  ASSERT_EQ(sync_value_response.signal_type, sync_value_input.signal_type);
  ASSERT_EQ(sync_value_response.signal_value, sync_value_input.signal_value);
}

} // namespace next
