next_create_test(
  NAME next_ifc_cxx_std_test
  SRCS next_ifc_cxx_std_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next # C++14
    #next_udex::next_udex # C++17
    #mtscpp::mtscpp # Only the Magno SDK as link dependency # C++17
    #eCAL::core # (none)
    #next_sdk::next_appsupport # (none)
    #next_sdk::next_sdk # (none)
    #next_control::next_control # (none)
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
)

set_property(TARGET next_ifc_cxx_std_test PROPERTY CXX_STANDARD 14)

next_create_test(
  NAME control_cxx_std_test
  SRCS control_cxx_std_test.cpp
  PRIVATELINKLIBS
    next_control
    jsoncpp_static
  INSTALLTARGETS eCAL::core
)

set_property(TARGET control_cxx_std_test PROPERTY CXX_STANDARD 14)
