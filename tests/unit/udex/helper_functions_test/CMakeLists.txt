next_create_test(
  NAME helper_functions_test
  SRCS helper_functions_test.cpp
  PRIVATELINKLIBS
    next_udex
  USESDB
)

# disable sign compare warnings
if(MSVC)
  target_compile_options(helper_functions_test PRIVATE "/wd4389")
endif()
