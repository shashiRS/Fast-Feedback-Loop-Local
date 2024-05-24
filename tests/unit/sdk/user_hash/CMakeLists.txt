next_create_test(
  NAME user_hash_test
  SRCS
    user_hash_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/user_hash/user_hash.hpp
  PRIVATELINKLIBS
    next_sdk
)
