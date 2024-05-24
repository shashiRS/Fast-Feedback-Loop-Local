function(next_create_library)

  set(options)
  set(oneValueArgs NAME TYPE FOLDER_VIEW RC_FILE VERSION_FILE_PATH TARGET_COMPILE_PRIVATE_DEF TARGET_COMPILE_PUBLIC_DEF USE_VERSION_INFO)
  set(multiValueArgs PUBLIC_LINK_DIRS PRIVATE_LINK_DIRS PUBLIC_INCLUDE_DIRS PRIVATE_INCLUDE_DIRS PUBLIC_LINK_LIBS PRIVATE_LINK_LIBS SRC_FILES TARGET_COMPILE_PUBLIC_OPT TARGET_COMPILE_PRIVATE_OPT)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED ARG_NAME)
    message(FATAL_ERROR "No target name given")
  endif()

  add_library(${ARG_NAME} ${ARG_TYPE}
    ${ARG_SRC_FILES}
  )

  if(ARG_USE_VERSION_INFO STREQUAL "TRUE")
    create_version_info(
      RC_FILE ${ARG_RC_FILE}
      VERSION_FILE ${ARG_VERSION_FILE_PATH}
      TARGET_NAME ${ARG_NAME}
    )
  endif()

  target_include_directories(${ARG_NAME}
    PRIVATE
      ${ARG_PRIVATE_INCLUDE_DIRS}
    PUBLIC
      ${ARG_PUBLIC_INCLUDE_DIRS}
  )

  target_link_directories(${ARG_NAME}
    PRIVATE
      ${ARG_PRIVATE_LINK_DIRS}
    PUBLIC
      ${ARG_PUBLIC_LINK_DIRS}
  )

  target_link_libraries(${ARG_NAME}
    PRIVATE
      ${ARG_PRIVATE_LINK_LIBS}
    PUBLIC
      ${ARG_PUBLIC_LINK_LIBS}
  )

  target_compile_definitions(${ARG_NAME}
    PRIVATE
      ${ARG_TARGET_COMPILE_PRIVATE_DEF}
    PUBLIC
      ${ARG_TARGET_COMPILE_PUBLIC_DEF}
  )

  target_compile_options(${ARG_NAME}
    PRIVATE
      ${ARG_TARGET_COMPILE_PRIVATE_OPT}
    PUBLIC
      ${ARG_TARGET_COMPILE_PUBLIC_OPT}
  )

  if(NOT DEFINED ARG_FOLDER_VIEW)
    set(ARG_FOLDER_VIEW ${ARG_NAME})
  endif()

  set_target_properties(${ARG_NAME} PROPERTIES FOLDER ${ARG_FOLDER_VIEW})
endfunction()
