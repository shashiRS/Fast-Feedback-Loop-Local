############################ helper for creating PLUGIN  shared library
# you have to give at least the name and some sources
# example: next_create_plugin(NAME myPlugin SRCS my_plugin.cpp)
# optionally you can give:
#  PUBLICINCLUDEDIRS  - public include dirs
#  PRIVATEINCLUDEDIRS - private include dirs
#  PUBLICLINKLIBS     - linking publicly against given libs
#  PRIVATELINKLIBS    - linking privatly against given libs
#  FLATBUFFERS        - flatbuffer files which are used (path relative to ${PUBLIC_SCHEMA_DIR})
#  INSTALLTARGETS     - targets which have to be installed (not used for linking)
#  RESOURCES          - resource files
#  UNICODE            - set the compile options for UNICODE
#  TYPE               - type of the plugin ('core' or 'project'), keeps the plugins categorised
#                       prints a fatal error and stops CMake when no value is given
#  DATABRIDGEBUILDDIR - path to the  data-bridge output folder which IDE i.e vstudio would use as a default path
#                       (e.g needed for debugging from the IDE)
# the function will create a target named with your given name (in the example "myPlugin"),
# use this target name to set other things when needed
function(next_create_plugin)
  set(oneValueArgs NAME TYPE PROJECTNAME DATABRIDGEBUILDDIR INSTALL_DIR INSTALL_TARGET)
  set(multiValueArgs SRCS PUBLICINCLUDEDIRS PRIVATEINCLUDEDIRS PUBLICLINKLIBS PRIVATELINKLIBS FLATBUFFERS INSTALLTARGETS RESOURCES)
  set(options UNICODE)
  cmake_parse_arguments(PLUGIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT DEFINED PLUGIN_NAME)
    message(FATAL_ERROR "there is no name given for the Plugin")
  endif ()
  if (NOT DEFINED PLUGIN_SRCS)
    message(FATAL_ERROR "no source files given for plugin ${PLUGIN_NAME}")
  endif ()
  message("adding plugin shared library ${PLUGIN_NAME}")

  if (DEFINED PLUGIN_PROJECTNAME)
    set(EXTEND_DIRECTORY_BY_PROJECT "/${PLUGIN_PROJECTNAME}")
  else ()
    set(EXTEND_DIRECTORY_BY_PROJECT "")
  endif ()

  set(PLUGIN_PATH_EXTENSION "${PLUGIN_TYPE}${EXTEND_DIRECTORY_BY_PROJECT}/${PLUGIN_NAME}")


  if (NOT DEFINED PLUGIN_DATABRIDGEBUILDDIR)
    set(PLUGINS_BUILD_DIR "${DATABRIDGE_PLUGINS_OUT_DIR}/${PLUGIN_PATH_EXTENSION}")
  else ()
    set(PLUGINS_BUILD_DIR "${PLUGIN_DATABRIDGEBUILDDIR}/${PLUGIN_PATH_EXTENSION}")
  endif ()


  if (NOT DEFINED PLUGIN_TYPE)
    message(FATAL_ERROR "There is no plugin type provided for ${PLUGIN_NAME}")
  endif ()

  add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})

  if (PLUGIN_UNICODE)
    target_compile_definitions(${PLUGIN_NAME} PUBLIC UNICODE)
  endif ()

  target_compile_definitions(${PLUGIN_NAME} PUBLIC BRIDGESDKSHARED)

  target_link_libraries(${PLUGIN_NAME}
    PUBLIC
    ${PLUGIN_PUBLICLINKLIBS}
    PRIVATE
    ${PLUGIN_PRIVATELINKLIBS}
  )

  target_include_directories(${PLUGIN_NAME}
    PUBLIC
    ${PLUGIN_PUBLICINCLUDEDIRS}
    PRIVATE
    ${PLUGIN_PRIVATEINCLUDEDIRS}
  )

  set_target_properties(${PLUGIN_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PLUGINS_BUILD_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PLUGINS_BUILD_DIR}"
  )

  if (DEFINED PLUGIN_TYPE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES
      FOLDER "Bridges/Databridge/Plugins"
    )
  endif ()

  if (DEFINED PLUGIN_FLATBUFFERS)
    set(fbs_files "")
    foreach (fbs ${PLUGIN_FLATBUFFERS})
      list(APPEND fbs_files "${PUBLIC_SCHEMA_DIR}/${fbs}")
    endforeach ()
    flatbuffers_generate_headers(
      TARGET ${PLUGIN_NAME}_fbs
      SCHEMAS ${fbs_files}
    )
    target_link_libraries(${PLUGIN_NAME}
      PRIVATE
      ${PLUGIN_NAME}_fbs
      flatbuffers::flatbuffers
    )
  endif ()

  set(INSTALL_DIR_PLUGIN bin/plugins/${PLUGIN_PATH_EXTENSION})
  if (DEFINED PLUGIN_INSTALL_DIR)
    set(INSTALL_DIR_PLUGIN ${PLUGIN_INSTALL_DIR})
  endif ()

  if (DEFINED PLUGIN_INSTALL_TARGET)
    set(INSTALL_TARGET_NAME ${PLUGIN_INSTALL_TARGET})
  else ()
    set(INSTALL_TARGET_NAME TARGET_COMPONENT)
  endif ()

  if (MSVC)
    install(
      TARGETS ${PLUGIN_NAME}
      COMPONENT ${INSTALL_TARGET_NAME}
      RUNTIME DESTINATION ${INSTALL_DIR_PLUGIN}
    )
    install(
      FILES $<TARGET_PDB_FILE:${PLUGIN_NAME}>
      COMPONENT ${INSTALL_TARGET_NAME}
      DESTINATION ${INSTALL_DIR_PLUGIN}
      OPTIONAL
    )
  else ()
    install(
      TARGETS ${PLUGIN_NAME}
      COMPONENT ${INSTALL_TARGET_NAME}
      LIBRARY DESTINATION ${INSTALL_DIR_PLUGIN}
    )
  endif ()

  install(
    FILES ${PLUGIN_RESOURCES}
    COMPONENT ${INSTALL_TARGET_NAME}
    DESTINATION ${INSTALL_DIR_PLUGIN}
  )
  install(
    TARGETS ${PLUGIN_INSTALLTARGETS}
    COMPONENT ${INSTALL_TARGET_NAME}
    DESTINATION ${INSTALL_DIR_PLUGIN}
  )


  foreach (tgt ${PLUGIN_RESOURCES})
    add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tgt} $<TARGET_FILE_DIR:${PLUGIN_NAME}>
    )
  endforeach ()

endfunction()
###########################
