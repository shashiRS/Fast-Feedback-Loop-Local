/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     publisher_manager.hpp
 * @brief    Player Manager Class
 * Controls the interractions with the player
 **/

#ifndef PUBLISHER_MANAGER_H_
#define PUBLISHER_MANAGER_H_

#include <memory>
#include <string>
#include <vector>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/udex/extractor_proxy.hpp>
NEXT_RESTORE_WARNINGS

#include <next/control/orchestrator/orchestrator_master.h>

#include "../resource_manager/resource_manager.hpp"
#include "speed_factor.hpp"
#include "trigger_handler.hpp"
#include "trigger_mapper.hpp"

#define STEPPING_BY_CYCLE_ID 1
#define STEPPING_BY_TIMESTAMP 2

constexpr uint32_t kDefaultCycleId = 0u;
constexpr uint64_t kDefaultTimestamp = 100u;
constexpr float kDefaultRealtimeFactor = 10.f;

class PublisherManager {
private:
  TriggerStatusHandler trigger_status_handler_;
  trigger_mapper trigger_mapper_;
  std::shared_ptr<UdexService> udex_service_;
  uint32_t cycle_id_from_config_file_;
  uint64_t timestamp_from_config_file_;
  uint32_t stepping_mode_;
  float realtime_factor_from_config_file_ = -1;
  std::string filter_expressions_;
  // orchestrator and related member functions and variables
  next::control::orchestrator::OrchestratorMaster orchestrator_master_;
  bool InitOrchestrator();

  /*! @brief This methods checks the package meta type and calls
   * TriggerOrchestratorDataBasedByPackageName() or
   * TriggerOrchestratorDataBasedByHash()
   *
   * @param[in] instance of IPackage - the package to be checked for any signal based triggering
   *
   * @return void
   */
  void TriggerOrchestratorDataBased(std::unique_ptr<next::sdk::types::IPackage> &package);

  /*! @brief This methods calls the Orchestrator with URL and timestamp so that
   * the orchestrator can check if trigger event should be sent.
   * The URL is determined by using the package name.
   *
   * @param[in] instance of IPackage - the package to be checked for any signal based triggering
   *
   * @return void
   */
  void TriggerOrchestratorDataBasedByPackageName(const std::string &package_name, uint64_t time_stamp);

  /*! @brief This methods calls the Orchestrator with URL and timestamp so that
   * the orchestrator can check if trigger event should be sent.
   * The URL is determined by using the hash.
   *
   * @param[in] instance of IPackage - the package to be checked for any signal based triggering
   *
   * @return void
   */
  void TriggerOrchestratorDataBasedByMetaInfo(size_t hash, uint64_t timestamp);

  /*! @brief This method calls the Orchestrator with a cycle id to check if a flow trigger needs to be fired.
   *
   * @param[in] cycleId - the cycle id that needs to be checked by the orchestrator
   * @param[in] isCycleStart - flag to identify if the cycle id to check is a cycle start or end package
   * @param[in] timestamp - the timestamp used for starting the flow
   *
   * @return void
   */
  void TriggerOrchestratorCycleBased(uint32_t cycleId, bool isCycleStart, uint64_t timestamp);

  /*! @brief This method calls the Orchestrator with a timestamp to check if a flow trigger needs to be fired.
   *
   * @param[in] timestamp - the timestamp used to check for starting the flow
   *
   * @return void
   */
  void TriggerOrchestratorTimeStampBased(uint64_t timestamp);

  void UpdateTriggerMap();
  std::vector<std::tuple<std::string, std::string, std::string>>::iterator GetTriggerFromTopic(std::string topicName);

  bool is_orchestrator_enabled_ = true;
  std::shared_ptr<next::player::BufferQueue> package_buffer_ptr_;
  std::shared_ptr<IResourceManager> resource_manager_ptr_;
  std::shared_ptr<SpeedFactor> speed_factor_ptr_;
  float current_speed_factor_;
  float last_valid_speed_factor_;
  std::vector<std::string> filter_urls_;

public:
  PublisherManager();
  bool Setup();
  bool PublishDataPackage(uint64_t &timestamp, uint32_t &cycleId, uint8_t &cycleState, bool &eof);
  bool Initialize(std::shared_ptr<IResourceManager> resourceManager);
  bool SettingUpSteppingParams();
  void SettingUpFilters();
  uint32_t GetCycleIdFromConfigFile();
  uint64_t GetTimeStampFromConfigFile();
  uint32_t GetSteppingMode();
  float GetRealTimeFactorFromConfigFile();
  void ResetSpeedFactor();
  float GetCurrentSpeedFactor();
  float GetLastSpeedFactor();
  std::string GetFilterExpression();
  void UpdateFilterExpression();
  void UpdateSpeedFactor(float speedFactor);
  void SetSteppingMode(uint32_t stepMode);
};

#endif // PUBLISHER_MANAGER_H_
