set(TEST_PACKAGE rec_export_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS rec_export_test.cpp
  RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cdl
    ${CMAKE_CURRENT_SOURCE_DIR}/test.sdl
  PUBLICLINKLIBS
    Boost::filesystem
    databridge_lib
    mts_swc::mts_swc # Magno SWC Toolbox
  PRIVATEINCLUDEDIRS
    ${PLUGINS_SRC_DIR}/core/system_RecExporter
  INSTALLTARGETS
    mts_core_lib::modules
)
