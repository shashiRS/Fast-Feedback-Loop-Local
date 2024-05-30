set(Plugin_NAME system_SignalExporter)

find_package(VFR_Utilities REQUIRED)

# TODO make this nice
if(MSVC)
  # on windows we have to link against generic_writer and zlib
  # zlib should be automatically linked when linking against generic_writer
  # also we should not have to give the lib name itself
  set(VFR_UTILITIES_LIB_SEARCH_EXTEND "/*.lib")
else()
  set(VFR_UTILITIES_LIB_SEARCH_EXTEND "/*.a") 
endif()

file(GLOB VFR_UTILITIES_LIBS ${VFR_Utilities_LIBRARIES}${VFR_UTILITIES_LIB_SEARCH_EXTEND})

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    signal_exporter.h
    signal_exporter.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    Boost::filesystem
    ${VFR_UTILITIES_LIBS}
    jsoncpp_static
    next_plugin
    next_sdk::next_appsupport
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/signal_exporter.fbs
    UNICODE
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

target_include_directories(${Plugin_NAME} SYSTEM PRIVATE ${VFR_Utilities_INCLUDE_DIR})

if(UNIX)
  set_property(
    SOURCE signal_exporter.cpp
    APPEND
    PROPERTY COMPILE_FLAGS
      "-Wno-cpp"
  )
endif()

# deactivate for time warnings
target_compile_definitions(${Plugin_NAME}
  PRIVATE
  # silence warning about std::ctime
  _CRT_SECURE_NO_WARNINGS
)
