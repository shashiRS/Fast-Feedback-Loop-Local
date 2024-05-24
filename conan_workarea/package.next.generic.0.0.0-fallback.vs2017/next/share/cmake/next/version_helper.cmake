cmake_minimum_required( VERSION 3.0.0 )

include( CMakeParseArguments )

function(get_git_version)
    find_package( Git )
        if( NOT GIT_FOUND )
            message( FATAL_ERROR "[get_git_version] Git not found" )
    endif( NOT GIT_FOUND )

    execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --short=8 HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    # Check whether we got any revision (which isn't always the case, e.g. when someone downloaded a zip
    # file from Github instead of a checkout)
    if ("${GIT_REVISION}" STREQUAL "")
        set(GIT_REVISION "N/A")
        set(GIT_TAG "N/A")
        set(GIT_BRANCH "N/A")
        set(GIT_COMMIT_HASH "N/A")
    else()
         message(STATUS "Git revision ${GIT_REVISION}")
        # Get the current working branch
        execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
         message(STATUS "Git branch ${GIT_BRANCH}")
        # Get the latest commit hash
        execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)    
          message(STATUS "Git commit hash ${GIT_COMMIT_HASH}")
        # Get last tag from git
        execute_process(COMMAND "${GIT_EXECUTABLE}" describe --abbrev=0 --tags WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        if ("${GIT_TAG}" STREQUAL "")
            # If there is no git tag
            set(GIT_TAG "N/A")
        endif()
        message(STATUS "Git tag ${GIT_TAG}")
    endif()

    set( PROJECT_GIT_HASH ${GIT_COMMIT_HASH} PARENT_SCOPE  )

    set( PROJECT_SEMANTIC_VERSION ${GIT_TAG} PARENT_SCOPE  )

endfunction()

# Create version information for the target
#
# Mandatory arguments:
#  TARGET_NAME - name of the target
# Optional arguments:
#  VERSION_FILE - if this is missing the default is used
#  RC_FILE - if this is missing the default is used
# example:
#  to link against Next, for the target "MY_TARGET":
#    link_against_next(TARGET MY_TARGET)

function(create_version_info)
  # Parse arguments
  set(options)
  set(oneValueArgs
    RC_FILE
    VERSION_FILE
    TARGET_NAME
  )
  set(multiValueArgs )
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # make git tag and commit hash available to caller
  get_git_version()

  if(NOT DEFINED ARG_TARGET_NAME)
    message(FATAL_ERROR "No target name given.")
  endif()
  
  get_target_property(tgt_binary_dir ${ARG_TARGET_NAME} BINARY_DIR)
  if("${tgt_binary_dir}" STREQUAL "tgt_binary_dir-NOTFOUND")
    message(FATAL_ERROR "Target ${ARG_TARGET_NAME} or its binary dir not found.")
  endif()
  # add the path of the configure file as include path
  target_include_directories(${ARG_TARGET_NAME}
    PRIVATE
      $<BUILD_INTERFACE:${tgt_binary_dir}>
  )
  
  set(OUT_RC_FILE ${tgt_binary_dir}/version_info.rc)
  if(NOT DEFINED ARG_RC_FILE)
    message("No RC file given. Using default")
    if (next_FOUND)
        #if this is called from outside Next repo
        set(ARG_RC_FILE ${next_PACKAGE_PATH}/next/share/cmake/next/version_info/version_info.rc.in)
    else()
        set(ARG_RC_FILE ${CMAKE_SOURCE_DIR}/src/cmake/version_info/version_info.rc.in)
    endif()
  endif()
  configure_file(${ARG_RC_FILE} "${OUT_RC_FILE}")
  
  set(OUT_HEADER_FILE ${tgt_binary_dir}/bin_version.h)
  if(NOT DEFINED ARG_VERSION_FILE)
    message("No version file given. Using default")
    if (next_FOUND)
        #if this is called from outside Next repo
        set(ARG_VERSION_FILE ${next_PACKAGE_PATH}/next/share/cmake/next/version_info/version.h.in)
    else()
        set(ARG_VERSION_FILE ${CMAKE_SOURCE_DIR}/src/cmake/version_info/version.h.in)
    endif()
  endif()
  configure_file(${ARG_VERSION_FILE} "${OUT_HEADER_FILE}")
  
  if(WIN32)
    # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
    set_source_files_properties(${OUT_RC_FILE} PROPERTIES LANGUAGE RC)
    # Add it to the target
    target_sources(${ARG_TARGET_NAME}
      PRIVATE
        ${OUT_HEADER_FILE}
        ${OUT_RC_FILE}
    )
    endif(WIN32)
endfunction()
