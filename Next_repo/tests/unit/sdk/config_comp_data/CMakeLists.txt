find_package(Boost COMPONENTS json REQUIRED)

next_create_test(
  NAME config_comp_data_test
  SRCS 
    config_comp_data_test.cpp
    config_utilities_test.cpp
    config_comp_data_storage_test.cpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_comp_data.hpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_comp_data_storage.hpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_utilities.hpp
  PRIVATELINKLIBS
    Boost::json
    next_sdk
    next_appsupport
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}/src/config/
  RESOURCES
  USE_CUSTOM_ECAL_DOMAIN
)
