next_create_test(
  NAME version_manager_test
  SRCS
    version_manager_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/version_manager/version_manager.hpp
  PRIVATELINKLIBS
    next_sdk
)
