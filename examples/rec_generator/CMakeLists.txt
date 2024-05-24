include(${CMAKE_SOURCE_DIR}/src/cmake/link_helper.cmake)
include(${CMAKE_SOURCE_DIR}/src/cmake/install_helper.cmake)

set(TARGET_NAME rec_generator)

add_executable(${TARGET_NAME}
  main.cpp
)

# needed for pack_memory_to_corepackage.h
target_include_directories(${TARGET_NAME} 
PRIVATE 
  ${PLUGINS_SRC_DIR}/core/system_RecExporter
  ${CMAKE_SOURCE_DIR}/examples/common_headers
)

# needed for <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp> inside the pack_memory_to_corepackage.h
target_link_libraries(${TARGET_NAME} 
PRIVATE 
  databridge_lib
  mts_swc::mts_swc # Magno SWC Toolbox
)

install(
  TARGETS ${TARGET_NAME}
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${TARGET_NAME}
)

install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/generated_rec.sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/packages.txt
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${TARGET_NAME}
)

# this does not work from inside the next repo itself, only for external use for now
#next_install(
#  COMPONENT ${TARGET_NAME}
#  BINDIR ${TARGET_NAME}
#)
