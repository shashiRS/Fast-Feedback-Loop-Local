find_package(Boost REQUIRED COMPONENTS system program_options)

next_create_test(
  NAME lfc_trigger_app
  SRCS
    lfc_trigger_app.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
    next_appsupport
    next_control
    Boost::program_options
  PRIVATEINCLUDEDIRS ${PROJECT_BINARY_DIR}
  INSTALLTARGETS
    mts_sentinel::client
)

create_version_info(
  RC_FILE testapp.rc 
  VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  TARGET_NAME lfc_trigger_app
)
