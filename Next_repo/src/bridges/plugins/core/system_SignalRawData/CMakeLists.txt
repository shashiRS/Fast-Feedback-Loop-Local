set(Plugin_NAME system_SignalRawData)

next_create_plugin(
  NAME
  ${Plugin_NAME}
  SRCS
  signal_rawdata_provider.hpp
  signal_rawdata_provider.cpp
  PRIVATEINCLUDEDIRS
  ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
  next_plugin
  jsoncpp_static
  RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/signal_raw_data_provider.fbs
  TYPE
  core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)
