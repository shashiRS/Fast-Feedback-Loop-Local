next_create_test(
  NAME sdk_logger_cfg_test
  SRCS
    logger_config.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
	${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator.hpp
  PRIVATELINKLIBS
    next_appsupport
    next_sdk
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/logger_cfg.json
)

next_create_test(
  NAME sdk_logger_def_test
  SRCS
    logger_definitions.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
  PRIVATELINKLIBS
    next_sdk
)

next_create_test(
  NAME sdk_logger_init_test
  SRCS
    logger_init_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
  PRIVATELINKLIBS
    next_sdk
)

next_create_test(
  NAME sdk_logger_init_wrong_console_loglevel_test
  SRCS
    logger_init_wrong_console_loglevel_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
  PRIVATELINKLIBS
    next_sdk
)

next_create_test(
  NAME sdk_logger_init_wrong_filenametype_test
  SRCS
    logger_init_wrong_filenametype_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
  PRIVATELINKLIBS
    next_sdk
)
