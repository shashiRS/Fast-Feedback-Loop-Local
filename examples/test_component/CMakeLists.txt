include(${CMAKE_SOURCE_DIR}/src/cmake/link_helper.cmake)
include(${CMAKE_SOURCE_DIR}/src/cmake/install_helper.cmake)

set(EXE_NAME test_component_node)
set(DLL_NAME lib_${EXE_NAME})

# =========== EXE build + install ===========
add_executable(${EXE_NAME}
  main.cpp
)

link_against_next(TARGET ${EXE_NAME})

install(
  TARGETS ${EXE_NAME}
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${EXE_NAME}
)

# =========== DLL build + install ===========
add_library(${DLL_NAME} SHARED 
  main.cpp
)

link_against_next(TARGET ${DLL_NAME})

target_compile_definitions(${DLL_NAME} PUBLIC ECO_EXPORTS)

install(
  TARGETS ${DLL_NAME}
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${EXE_NAME}
)

# =========== EXTRA FILES install ===========
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/test_component.sdl
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${EXE_NAME}
)

# this does not work from inside the next repo itself, only for external use for now
#next_install(
#  COMPONENT ${EXE_NAME}
#  BINDIR ${EXE_NAME}
#)
