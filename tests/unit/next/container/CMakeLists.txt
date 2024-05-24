add_library(dummy_next_interface SHARED
  dummy_component_main.cpp
)

next_create_test(
  NAME next_container_args_test
  SRCS next_container_args_test.cpp
  PRIVATEINCLUDEDIRS ${NEXT_CONTAINER_SRC_DIR}
  PRIVATELINKLIBS
  next
  INSTALLTARGETS
  USE_CUSTOM_ECAL_DOMAIN
)

next_create_test(
  NAME next_container_dll_loader_test
  SRCS next_container_dll_loader_test.cpp
  RESOURCES ${TEST_RESOURCES}
  PRIVATEINCLUDEDIRS ${NEXT_CONTAINER_SRC_DIR}
  PRIVATELINKLIBS
  next
  Boost::system
  Boost::filesystem
  Boost::date_time # required by filesystem
  dummy_next_interface
  INSTALLTARGETS
  USE_CUSTOM_ECAL_DOMAIN
)