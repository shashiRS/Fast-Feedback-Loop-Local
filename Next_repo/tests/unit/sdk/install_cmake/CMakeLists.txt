# This test is work in progress 
# - here the generic install and cmake functions provided by coreSDK should be tested

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)
find_package(Boost REQUIRED)

next_create_test(
  NAME install_cmake
  SRCS
    install_test.cpp
  PRIVATELINKLIBS
    eCAL::core
)

install_eCAL(
  PATH tests/cases/install_cmake
  COMPONENT __CIP_TEST_COMMON
)
