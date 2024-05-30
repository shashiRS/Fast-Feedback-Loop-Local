# set the default CXX standard to 11
set(CMAKE_CXX_STANDARD 14)

next_create_test(
  NAME sdk_cxx_interface_test
  SRCS cxx_interface_test.cpp
  PRIVATELINKLIBS
    next_sdk
    next_appsupport
)

# reset the default CXX standard to 17
set(CMAKE_CXX_STANDARD 17)
