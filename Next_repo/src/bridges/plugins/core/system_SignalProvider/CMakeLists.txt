set(Plugin_NAME system_SignalProvider)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    signal_provider.h
    signal_provider.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_udex
    next_plugin
    jsoncpp_static
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/signalProvider.fbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)
