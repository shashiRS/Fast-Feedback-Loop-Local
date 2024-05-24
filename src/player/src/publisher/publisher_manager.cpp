/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#include "publisher_manager.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/udex/explorer/signal_explorer.hpp>

enum class PlayerIniFields { DefaultSection, Stepping, RealtimeFactor };

bool PublisherManager::InitOrchestrator() {

  is_orchestrator_enabled_ = orchestrator_master_.loadConfiguration();
  trigger_status_handler_.Reset();

  return is_orchestrator_enabled_;
}

PublisherManager::PublisherManager() : speed_factor_ptr_(new SpeedFactor()) {
  next::udex::explorer::SignalExplorer sign_expl;
  sign_expl.searchSignalTree("some random keywork_that does not exist");
}

bool PublisherManager::Setup() {
  this->trigger_mapper_.Reset();
  orchestrator_master_.resetOrchestrator();

  SettingUpFilters();
  SettingUpSteppingParams();

  bool orchestrator_setup = InitOrchestrator();
  if (!orchestrator_setup) {
    this->trigger_mapper_.initialize();
  }

  if (is_orchestrator_enabled_) {
    orchestrator_master_.setupDataBasedTriggering(udex_service_->GetTopicsAndHashes());
  }
  UpdateTriggerMap();
  return true;
}

void PublisherManager::UpdateTriggerMap() {
  // this needs adaption from the udex -> should give back a list with hash | topic
  auto it = udex_service_->GetTopicsAndHashes().begin();

  // go through all previously succesfully registered ports and create a port for each
  while (it != udex_service_->GetTopicsAndHashes().end()) {

    // find the trigger in the pair vector
    auto it_triggers = GetTriggerFromTopic(it->first);

    // ranges only work in c++20 and above
    // auto it_triggers = std::ranges::find((*trigger_mapper_.get_trigger_vector()),
    //  it->first, &std::pair<std::string, std::string>::second;

    if (it_triggers != (*trigger_mapper_.get_trigger_vector()).end()) {
      std::string method = std::get<1>(*it_triggers);

      // if this URL is configured for triggering, add this as well to the trigger map
      std::pair<std::string, std::unique_ptr<eCAL::CServiceClient>> method_server_combi =
          std::make_pair(method, (std::make_unique<eCAL::CServiceClient>(std::get<0>(*it_triggers))));

      // TODO: what should be used as host name? Player component?
      (*trigger_mapper_.get_trigger_map())[it->second] =
          std::make_pair(method, (std::make_unique<eCAL::CServiceClient>(std::get<0>(*it_triggers))));
      std::get<1>((*trigger_mapper_.get_trigger_map())[it->second])->Create(std::get<0>(*it_triggers));
      std::get<1>((*trigger_mapper_.get_trigger_map())[it->second])->SetHostName("");

      debug(__FILE__, "Created trigger:{0} with URL: {1}", std::get<0>(*it_triggers), it->first);
    }

    it++;
  }
}

std::vector<std::tuple<std::string, std::string, std::string>>::iterator
PublisherManager::GetTriggerFromTopic(std::string topicName) {
  return std::find_if((*trigger_mapper_.get_trigger_vector()).begin(), (*trigger_mapper_.get_trigger_vector()).end(),
                      [topicName](const std::tuple<std::string, std::string, std::string> &current_element) {
                        return topicName == std::get<2>(current_element);
                      });
}

bool PublisherManager::SettingUpSteppingParams() {
  // flags to check if keys defined
  bool cycle_id_found = false;
  bool time_step_found = false;
  bool realtime_factor_found = false;

  // init values
  stepping_mode_ = 0;
  cycle_id_from_config_file_ = 0;
  timestamp_from_config_file_ = 100;
  realtime_factor_from_config_file_ = 10;

  // TODO when invalid arguments are read, the member variables might in in inconsistent sate
  // see https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-6799
  // also handle that within the configuration negetive numbers might be given, they are not making any sense
  if (auto cycle_id = next::appsupport::ConfigClient::getConfig()->get_int(
          next::appsupport::configkey::player::getCycleIdKey(), -1);
      cycle_id != -1) {
    debug(__FILE__, "cycle_id :{0}", cycle_id);
    cycle_id_from_config_file_ = static_cast<uint32_t>(cycle_id);
    cycle_id_found = true;
    stepping_mode_ = STEPPING_BY_CYCLE_ID;
  } else if (auto time_step = next::appsupport::ConfigClient::getConfig()->get_int(
                 next::appsupport::configkey::player::getSteppingKey(), -1);
             time_step > 0) {
    timestamp_from_config_file_ = static_cast<uint64_t>(time_step);
    time_step_found = true;
    cycle_id_from_config_file_ = 0;
    stepping_mode_ = STEPPING_BY_TIMESTAMP;
  } else {
    warn(__FILE__, "Config File read failed, proceeding with default cycle_id: {0}", kDefaultTimestamp);
    timestamp_from_config_file_ = kDefaultTimestamp;
    cycle_id_from_config_file_ = kDefaultCycleId;
    stepping_mode_ = 0;
  }
  auto realtime_factor = next::appsupport::ConfigClient::getConfig()->get_float(
      next::appsupport::configkey::player::getRealTimeFactorKey(), -1.f);
  if (realtime_factor > 0.f) {
    debug(__FILE__, "realtime_factor: {0}", realtime_factor);
    realtime_factor_from_config_file_ = realtime_factor;
    realtime_factor_found = true;
  } else {
    warn(__FILE__, "Config File read failed, proceeding with default realtime_factor: {0}", kDefaultRealtimeFactor);
    realtime_factor_from_config_file_ = kDefaultRealtimeFactor;
    realtime_factor_found = true;
    // set default value in config client for usage in bridge (send to GUI)
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::player::getRealTimeFactorKey(),
                                                     realtime_factor_from_config_file_);
  }

  if (!cycle_id_found && !time_step_found) {
    error(__FILE__, "cycle_id or time_step not defined");
    return false;
  }

  if (!realtime_factor_found) {
    error(__FILE__, "realtime_factor not defined");
    return false;
  }

  return true;
}

uint32_t PublisherManager::GetCycleIdFromConfigFile() { return cycle_id_from_config_file_; }

uint64_t PublisherManager::GetTimeStampFromConfigFile() { return timestamp_from_config_file_; }

uint32_t PublisherManager::GetSteppingMode() { return stepping_mode_; }
// bool publisher_manager::GetEoF() { return inputEOF_; }
float PublisherManager::GetRealTimeFactorFromConfigFile() { return realtime_factor_from_config_file_; }

void PublisherManager::ResetSpeedFactor() { speed_factor_ptr_->Reset(); }

float PublisherManager::GetCurrentSpeedFactor() { return current_speed_factor_; }

float PublisherManager::GetLastSpeedFactor() { return last_valid_speed_factor_; }

void PublisherManager::TriggerOrchestratorDataBased(std::unique_ptr<next::sdk::types::IPackage> &package) {

  package->ResetPayloads();
  int watchdog_package_not_stuck = 0;
  while (package->PayloadAvailable()) {
    if (watchdog_package_not_stuck > 100) {
      error(__FILE__, "Limit of 100 subpackages exceeded. Please create smaller packages");
      return;
    }
    watchdog_package_not_stuck++;

    switch (package->GetMetaType()) {
    case next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME: {
      uint64_t timestamp = package->GetPackageHeader().timestamp;
      TriggerOrchestratorDataBasedByPackageName(package->GetPackageName(), timestamp);
      break;
    }
    case next::sdk::types::IPackage::PACKAGE_META_TYPE_INFO: {
      size_t hash = next::sdk::types::PackageHash::hashMetaInfo(package->GetMetaInfo());
      uint64_t timestamp = package->GetPackageHeader().timestamp;
      TriggerOrchestratorDataBasedByMetaInfo(hash, timestamp);
      break;
    }
    default: {
      error(__FILE__,
            "Handling of package meta type {0} is not supported yet in "
            "TriggerOrchestratorDataBased(). No triggering will happen with this package.",
            static_cast<int>(package->GetMetaType()));
      break;
    }
    }

    // loop through package
    package->GetPayload();
  }
}

void PublisherManager::TriggerOrchestratorDataBasedByPackageName(const std::string &package_name, uint64_t time_stamp) {
  if (!is_orchestrator_enabled_) {
    return;
  }
  if (!package_name.empty() || package_name != "") {
    orchestrator_master_.triggerDataBased(time_stamp, package_name, trigger_status_handler_.current_status_map_);
  }
}

void PublisherManager::TriggerOrchestratorDataBasedByMetaInfo(size_t hash, uint64_t timestamp) {
  if (is_orchestrator_enabled_) {
    orchestrator_master_.triggerDataBased(timestamp, hash, trigger_status_handler_.current_status_map_);
  } else {
    auto it_ports_trigger = trigger_mapper_.get_trigger_map()->find(hash);
    if (it_ports_trigger != trigger_mapper_.get_trigger_map()->end()) {
      // trigger with host name and method name URL
      std::get<1>(it_ports_trigger->second)
          ->Call(std::get<0>(it_ports_trigger->second), std::get<0>(it_ports_trigger->second));
    }
  }
}

void PublisherManager::TriggerOrchestratorCycleBased(uint32_t cycleId, bool isCycleStart, uint64_t timestamp) {
  if (is_orchestrator_enabled_) {
    orchestrator_master_.triggerCycleBased(cycleId, trigger_status_handler_.current_status_map_, isCycleStart,
                                           timestamp);
  }
}

void PublisherManager::TriggerOrchestratorTimeStampBased(uint64_t time_stamp) {
  if (!is_orchestrator_enabled_) {
    return;
  }
  orchestrator_master_.triggerTimestampBased(time_stamp, trigger_status_handler_.current_status_map_);
}

bool PublisherManager::PublishDataPackage(uint64_t &timestamp, uint32_t &cycleId, uint8_t &cycleState, bool &eof) {
  bool retVal = true;
  auto package = package_buffer_ptr_->popBuffer(eof);
  try {
    if (!eof) {
      if (!package) {
        return true;
      }
      timestamp = package->GetPackageHeader().timestamp;
      cycleId = package->GetPackageHeader().cycle_id;
      cycleState = package->GetPackageHeader().cycle_state;
      resource_manager_ptr_->GetCycleManager()->PublishCycle(timestamp, cycleId, cycleState);

      // The package->cycle_state field contains cycle state information:
      // Only packages marked with "cycle body"or with "no cycle" will contain actual package data (non-zero payload
      // length)
      if ((cycleState & mts::extensibility::cycle_body) != 0 || (cycleState == mts::extensibility::no_cycle)) {
        // only publish packages that have a body with data inside
        if (udex_service_->PublishPackage(package)) {
          resource_manager_ptr_->GetReplayStatistics()->published_package_count++;
          resource_manager_ptr_->GetReplayStatistics()->published_bytes += package->GetSize();
        } else {
          resource_manager_ptr_->GetReplayStatistics()->publish_fail_count++;
          debug(__FILE__,
                "Failed to publish message: ts = {0} size = {1} publish fail count = {2} last publish time = {3}ms",
                timestamp, package->GetSize(), resource_manager_ptr_->GetReplayStatistics()->publish_fail_count,
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    resource_manager_ptr_->GetReplayStatistics()->last_publish_time.time_since_epoch())
                    .count());
          retVal = false;
        }
        TriggerOrchestratorDataBased(package);
      }

      TriggerOrchestratorTimeStampBased(timestamp);

      if ((cycleState & mts::extensibility::cycle_start) != 0) {
        TriggerOrchestratorCycleBased(cycleId, true, timestamp);
      }
      if ((cycleState & mts::extensibility::cycle_end) != 0) {
        TriggerOrchestratorCycleBased(cycleId, false, timestamp);
      }

      trigger_status_handler_.PublishStatusCurrentPackage();

      current_speed_factor_ = speed_factor_ptr_->UpdateSimulationSpeed(timestamp);
      if (current_speed_factor_ != -1) {
        last_valid_speed_factor_ = current_speed_factor_;
      }

    } else {
      retVal = false;
    }
  } catch (std::exception &) {
    resource_manager_ptr_->GetReplayStatistics()->publish_fail_count++;
    debug(__FILE__, "Failed to publish message: ts = {0} size = {1} publish fail count = {2} last publish time = {3}ms",
          timestamp, package->GetSize(), resource_manager_ptr_->GetReplayStatistics()->publish_fail_count,
          std::chrono::duration_cast<std::chrono::milliseconds>(
              resource_manager_ptr_->GetReplayStatistics()->last_publish_time.time_since_epoch())
              .count());
    retVal = false;
  }

  return retVal;
}

bool PublisherManager::Initialize(std::shared_ptr<IResourceManager> resourceManager) {
  speed_factor_ptr_->Reset();
  resource_manager_ptr_ = resourceManager;
  udex_service_ = resource_manager_ptr_->GetUdexService();
  package_buffer_ptr_ = resource_manager_ptr_->GetDataBuffer();

  speed_factor_ptr_->SetSpeedFactor(GetRealTimeFactorFromConfigFile());
  current_speed_factor_ = 0;
  return true;
}

void PublisherManager::SettingUpFilters() {
  using namespace next::appsupport;
  filter_expressions_.clear();
  std::vector<std::string> expression_filters;

  expression_filters =
      ConfigClient::getConfig()->getStringList(next::appsupport::configkey::player::getFiltersExpresionKey(), {});
  filter_urls_ = ConfigClient::getConfig()->getStringList(next::appsupport::configkey::player::getFiltersUrlKey(), {});

  for (const auto &expression_filter : expression_filters) {
    if (!expression_filter.empty()) {
      debug(__FILE__, "Adding expression filter: {0}", expression_filter);
      if (filter_expressions_.empty()) {
        filter_expressions_ += expression_filter;
      } else {
        filter_expressions_ += "OR " + expression_filter;
      }
    }
  }
}

std::string PublisherManager::GetFilterExpression() { return filter_expressions_; }

void PublisherManager::UpdateFilterExpression() {
  if (!filter_urls_.empty()) {
    std::string filter_expression = resource_manager_ptr_->GetUdexService()->GetFilterExpressionFromUrls(filter_urls_);
    if (filter_expression.size()) {
      if (filter_expressions_.empty()) {
        filter_expressions_ += filter_expression;
      } else {
        filter_expressions_ += "OR " + filter_expression;
      }
    }
  }
}

void PublisherManager::UpdateSpeedFactor(float speedFactor) { speed_factor_ptr_->SetSpeedFactor(speedFactor); }

void PublisherManager::SetSteppingMode(uint32_t stepMode) { stepping_mode_ = stepMode; }
