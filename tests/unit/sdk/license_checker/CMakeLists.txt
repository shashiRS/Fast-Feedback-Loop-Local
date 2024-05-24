next_create_test(
  NAME license_checker
  SRCS license_checker_test.cpp
   ${APPSUPPORT_SRC_DIR}/src/license_checker/license_checker.cpp
  RESOURCES 
  ${CMAKE_CURRENT_LIST_DIR}/license_tester_file.json
  PRIVATEINCLUDEDIRS
  ${APPSUPPORT_SRC_DIR}/src/license_checker
  PRIVATELINKLIBS 
    next_sdk::next_appsupport
	jsoncpp_static
	Boost::filesystem
)
