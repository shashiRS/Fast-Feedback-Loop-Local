set(TEST_PACKAGE system_SignalRawData_test)
find_package(eCAL REQUIRED)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    signal_rawdata_provider_test.cpp
    ${PLUGINS_SRC_DIR}/core/system_SignalRawData/signal_rawdata_provider.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_SRC_DIR}/core/system_SignalRawData
    ${PLUGINS_INTERFACE_DIR}
	  ${DATABRIDGE_INTERFACE_DIR}
	  ${UNIT_TEST_COMMON_DIR}

  PRIVATELINKLIBS
  next_plugin
  jsoncpp_static
  USESDB
  INSTALLTARGETS
  next_plugin
  
  #   mts_sentinel::client
  #   mts_core_lib::modules
)
