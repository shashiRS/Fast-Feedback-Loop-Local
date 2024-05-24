set(TEST_PACKAGE system_signalprovider_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
  signal_provider_test.cpp
  ${PLUGINS_SRC_DIR}/core/system_SignalProvider/signal_provider.cpp
  PRIVATEINCLUDEDIRS
  ${PLUGINS_SRC_DIR}/core/system_SignalProvider
  ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
  eCAL::core
  eCAL::pb
  flatbuffers::flatbuffers
  jsoncpp_static
  next_plugin
  USESDB
  INSTALLTARGETS
  mts_sentinel::client
  mts_core_lib::modules
)
