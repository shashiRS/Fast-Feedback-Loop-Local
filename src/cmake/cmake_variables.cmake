########################## Variables for directories
set(BASE_SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(SDK_SRC_DIR "${BASE_SRC_DIR}/sdk/sdk")
set(APPSUPPORT_SRC_DIR "${BASE_SRC_DIR}/sdk/appsupport")

set(UDEX_SRC_DIR "${BASE_SRC_DIR}/udex")

set(CONTROL_DIR "${BASE_SRC_DIR}/control")
set(EVENTCONTROL_SRC_DIR "${CONTROL_DIR}/src/event_control")
set(LOG_COLLECTOR_SRC_DIR "${CONTROL_DIR}/src/log_collector")
set(ORCHESTRATOR_SRC_DIR "${CONTROL_DIR}/src/orchestrator")

set(BRIDGESDK_SRC_DIR "${BASE_SRC_DIR}/bridges/bridgesdk")
set(CONTROLBRIDGE_SRC_DIR "${BASE_SRC_DIR}/bridges/controlbridge")
set(DATABRIDGE_SRC_DIR "${BASE_SRC_DIR}/bridges/databridge")
set(PLUGIN_MANAGER_SRC_DIR "${DATABRIDGE_SRC_DIR}/PluginManager")
set(PLUGINS_SRC_DIR "${BASE_SRC_DIR}/bridges/plugins")

set(PLAYER_SRC_DIR "${BASE_SRC_DIR}/player/src")

set(NEXT_SRC_DIR "${BASE_SRC_DIR}/next/src/next_interface")
set(NEXT_CONTAINER_SRC_DIR "${BASE_SRC_DIR}/next/src/container")
set(NEXT_INTERFACE_DIR "${BASE_SRC_DIR}/next/interface")

set(UNIT_TEST_COMMON_DIR "${CMAKE_SOURCE_DIR}/tests/unit/common")

########################## Variables for files
set(ECAL_INI_FILE "${CMAKE_SOURCE_DIR}/src/conf/ecal.ini")
set(ECAL_INI_TEMPLATE "${CMAKE_SOURCE_DIR}/src/conf/ecal.ini.in")

############## interfaces include directories
set(SDK_INTERFACE_DIR "${SDK_SRC_DIR}/interface")
set(APPSUPPORT_INTERFACE_DIR "${APPSUPPORT_SRC_DIR}/interface")

set(UDEX_INTERFACE_DIR "${UDEX_SRC_DIR}/interface")

set(CONTROL_INTERFACE_DIR "${CONTROL_DIR}/interface")

set(BRIDGESDK_INTERFACE_DIR "${BRIDGESDK_SRC_DIR}/interface")
set(DATABRIDGE_INTERFACE_DIR "${DATABRIDGE_SRC_DIR}/interface")
set(PLUGINS_INTERFACE_DIR "${PLUGINS_SRC_DIR}/interface")
set(PERFORMANCEVIEWER_INTERFACE_DIR "${PLUGINS_SRC_DIR}/core/system_PerformanceViewer/interface")
set(BRIDGESDK_SCHEMA_DIR "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema")

set(PLAYER_INTERFACE_DIR "${BASE_SRC_DIR}/player/interface")

############## output directories for databridge and its plugins
set(DATABRIDGE_OUT_DIR "${CMAKE_BINARY_DIR}/databridge")
set(DATABRIDGE_PLUGINS_OUT_DIR "${DATABRIDGE_OUT_DIR}/plugins")
set(DATABRIDGE_SCHEMAS_OUT_DIR "${DATABRIDGE_OUT_DIR}/schema")
