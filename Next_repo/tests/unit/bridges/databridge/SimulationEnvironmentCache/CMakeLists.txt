next_create_test(
  NAME simulation_environment_cache
  SRCS 
    ${DATABRIDGE_SRC_DIR}/DataManager/OutputManager/simulation_environment_cache.cpp
    ${DATABRIDGE_SRC_DIR}/DataManager/OutputManager/simulation_environment_cache.h
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation_environment_cache_test.cpp
  PUBLICLINKLIBS
    databridge_lib
    Boost::date_time
  PRIVATEINCLUDEDIRS
    ${DATABRIDGE_SRC_DIR}/DataManager/OutputManager
)
