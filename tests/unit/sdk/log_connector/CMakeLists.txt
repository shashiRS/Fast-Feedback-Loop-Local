next_create_test(
  NAME log_connector
  SRCS
    log_connector_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk::next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS
    ${PROJECT_BINARY_DIR}
)
