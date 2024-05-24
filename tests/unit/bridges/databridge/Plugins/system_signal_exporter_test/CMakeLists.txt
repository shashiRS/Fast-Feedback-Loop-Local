set(TEST_PACKAGE system_signal_exporter_test)

find_package(VFR_Utilities REQUIRED)

# TODO make this nice
if (MSVC)
  # on windows we have to link against generic_writer and zlib
  # zlib should be automatically linked when linking against generic_writer
  # also we should not have to give the lib name itself
  set(VFR_UTILITIES_LIB_SEARCH_EXTEND "/*.lib")
else ()
  set(VFR_UTILITIES_LIB_SEARCH_EXTEND "/*.a")
endif ()

file(GLOB VFR_UTILITIES_LIBS ${VFR_Utilities_LIBRARIES}${VFR_UTILITIES_LIB_SEARCH_EXTEND})

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
  system_signal_exporter_test.cpp
  ${PLUGINS_SRC_DIR}/core/system_SignalExporter/signal_exporter.cpp
  ${PLUGINS_SRC_DIR}/core/system_SignalExporter/signal_exporter.h
  RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test1.sdl
  PRIVATEINCLUDEDIRS
  ${PLUGINS_INTERFACE_DIR}
  ${PLUGINS_SRC_DIR}/core/system_SignalExporter
  PRIVATELINKLIBS
  Boost::filesystem
  ${VFR_UTILITIES_LIBS}
  jsoncpp_static
  next_plugin
  next_sdk::next_appsupport
  INSTALLTARGETS
  mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
)
target_include_directories(${TEST_PACKAGE} SYSTEM PRIVATE ${VFR_Utilities_INCLUDE_DIR})
target_compile_definitions(${TEST_PACKAGE} PUBLIC UNICODE)
