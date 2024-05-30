#include "sync_manager/sync_config_reader.hpp"
#include <gtest/gtest.h>

namespace next {

TEST(SyncConfigReader, config_reader_with_v0_config_test) {
  next::appsupport::ConfigClient::do_init("next_player");
  next::appsupport::ConfigClient::do_init("nextsync_0");
  next::appsupport::ConfigClient::getConfig()->putCfg("sync_config_reader.json");
  next::appsupport::ConfigClient::finish_init();

  std::string component = "nextsync_0";
  std::string port = "NextSync_0.requestPort_VehDyn_sync_syncref";

  ComponentInputPortConfig input_port_config = {};
  input_port_config.port_name = port;

  SyncConfig sync_config;
  SyncConfigReader sync_config_reader;
  sync_config_reader.ReadSyncConfig(component, input_port_config, sync_config);

  EXPECT_TRUE(sync_config.sync_id.sync_ref_sync_value_member_name == "sSigHeader.uiMeasurementCounter");
  EXPECT_TRUE(sync_config.sync_id.port_member_data_url == "VehDyn.uiMeasurementCounter");

  EXPECT_TRUE(sync_config.timestamp.sync_ref_sync_value_member_name == "sSigHeader.uiTimeStamp");
  EXPECT_TRUE(sync_config.timestamp.port_member_data_url == "VehDyn.uiTimeStamp");

  // Check if sync_mode is "SYNC_SYNC_REF_TIMESTAMP" (1)
  EXPECT_TRUE(sync_config.mode == 1);

  EXPECT_TRUE(sync_config.sync_value.syncref_base_control_url == "undefined");
  EXPECT_TRUE(sync_config.sync_value.syncref_sync_header_url ==
              "ARS5xx.RawDataCycle.ALN_SyncRef.sSigHeader.uiTimeStamp");

  // Check if algorythm_type is "FIND_EXACT_VALUE" (1)
  EXPECT_TRUE(sync_config.algo_port_level.algorythm_type == 1);
  EXPECT_TRUE(sync_config.algo_port_level.sync_signal_url == "ARS5xx.RawDataCycle.ALN_SyncRef.VehDyn.uiTimeStamp");
  EXPECT_TRUE(sync_config.algo_port_level.composition_sync_signal_url ==
              "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");

  next::appsupport::ConfigClient::do_reset();
}

TEST(SyncConfigReader, config_reader_with_v1_config_test) {
  next::appsupport::ConfigClient::do_init("nextsync_0");
  next::appsupport::ConfigClient::getConfig()->putCfg("sync_config_reader_v1.json");
  next::appsupport::ConfigClient::finish_init();

  std::string component = "nextsync_0";
  std::string port = "NextSync_0.requestPort_VehDyn_sync_syncref";

  ComponentInputPortConfig input_port_config = {};
  input_port_config.port_name = port;

  SyncConfig sync_config;
  SyncConfigReader sync_config_reader;
  sync_config_reader.ReadSyncConfig(component, input_port_config, sync_config);

  EXPECT_TRUE(sync_config.sync_id.sync_ref_sync_value_member_name == "pOddYP0Image.uiMeasurementCounter");
  EXPECT_TRUE(sync_config.sync_id.port_member_data_url == "sigHeader.uiMeasurementCounter");

  EXPECT_TRUE(sync_config.timestamp.sync_ref_sync_value_member_name == "pOddYPoImage.uiTimestamp");
  EXPECT_TRUE(sync_config.timestamp.port_member_data_url == "sigHeader.uiTimeStamp");

  // Check if sync_mode is "SYNC_SYNC_REF_TIMESTAMP" (1)
  EXPECT_TRUE(sync_config.mode == 1);

  EXPECT_TRUE(sync_config.sync_value.timestamp_url == "Orchestrator.flow1_sicore.sSigHeader.Timestamp");
  EXPECT_TRUE(sync_config.sync_value.sync_id_url == "Orchestrator.flow1_sicore.sSigHeader.uiMeasurementCounter");
  EXPECT_TRUE(sync_config.sync_value.syncref_base_control_url == "SIM VFB.GS_sicore.sSigHeader.TimeStamp");
  EXPECT_TRUE(sync_config.sync_value.syncref_sync_header_url == "ADC5xx.Syncref.SigHeader.Timestamp");

  // Check if algorythm_type is "GET_LAST_ELEMENT" (0)
  EXPECT_TRUE(sync_config.algo_port_level.algorythm_type == 0);
  EXPECT_TRUE(sync_config.algo_port_level.sync_signal_url == "ADC5xx.Syncref.SigHeader.sigHeader.uiTimeStamp");
  EXPECT_TRUE(sync_config.algo_port_level.composition_sync_signal_url ==
              "ARS5xx.AlgoVehCycle.VehDyn.pOddYPoImage.uiTimestamp");

  next::appsupport::ConfigClient::do_reset();
}

} // namespace next
