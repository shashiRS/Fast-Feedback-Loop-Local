next_create_test(
  NAME sdk_strings_test
  SRCS
    strings_test.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/string_helper/strings.hpp
  PRIVATEINCLUDEDIRS
    ${SDK_INTERFACE_DIR}
)
