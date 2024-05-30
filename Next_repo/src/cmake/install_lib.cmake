############################ helper for installing 3rd party libs
#

macro(install_eCAL)
    set(oneValueArgs PATH COMPONENT)
    set(multiValueArgs)
    set(options)
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED M_INST_PATH)
        message(FATAL_ERROR "there is no path given for the install")
    endif()
    if(NOT DEFINED M_INST_COMPONENT)
        message(FATAL_ERROR "there is no component given for the install")
    endif()

    find_package(eCAL REQUIRED)
    if(MSVC)
        message("====================================================================")
        message("Installing binaries for eCAL from ${eCAL_DIR} to ${M_INST_PATH}")
        message("====================================================================")
        install(
            DIRECTORY ${eCAL_DIR}/../../../bin/
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${M_INST_PATH}
        )
    else()
        message("====================================================================")
        message("Installing binaries for eCAL from ${eCAL_DIR} to ${M_INST_PATH}/../lib")
        message("====================================================================")
        install(
          DIRECTORY ${eCAL_DIR}/../../
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${M_INST_PATH}/../lib
        )
    endif(MSVC)

endmacro()

macro(install_SPDLog)
    set(oneValueArgs COMPONENT)
    set(multiValueArgs)
    set(options)
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED M_INST_COMPONENT)
        message(FATAL_ERROR "there is no component given for the install")
    endif()

    find_package(spdlog 1.6.0 REQUIRED)

    if(MSVC)
        message("====================================================================")
        message("Installing includes for SPDLog from ${spdlog_DIR}/../../../include/ to ${CMAKE_INSTALL_INCLUDEDIR}")
        message("====================================================================")
        install(
            DIRECTORY ${spdlog_DIR}/../../../include/
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
        if(EXISTS ${spdlog_DIR}/../../spdlog.lib)
                message("====================================================================")
          message("Installing librarys for SPDLog from ${spdlog_DIR}/../../spdlog.lib to ${CMAKE_INSTALL_LIBDIR}")
          message("====================================================================")
          install(
            FILES ${spdlog_DIR}/../../spdlog.lib
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
          )
        endif()
        if(EXISTS ${spdlog_DIR}/../../spdlogd.lib)
                message("====================================================================")
          message("Installing librarys for SPDLog from ${spdlog_DIR}/../../spdlogd.lib to ${CMAKE_INSTALL_LIBDIR}")
          message("====================================================================")
          install(
            FILES ${spdlog_DIR}/../../spdlogd.lib
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
          )
        endif()
    else()
        message("====================================================================")
        message("Installing includes for SPDLog from ${spdlog_DIR}/../../../include/ to ${CMAKE_INSTALL_INCLUDEDIR}")
        message("====================================================================")
        install(
            DIRECTORY ${spdlog_DIR}/../../../include/
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )

        if(EXISTS ${spdlog_DIR}/../../../lib/libspdlogd.a)
            message("====================================================================")
            message("Installing librarys for SPDLog from ${spdlog_DIR}/../../../lib/libspdlogd.a to ${CMAKE_INSTALL_LIBDIR}")
            message("====================================================================")
            install(
                FILES ${spdlog_DIR}/../../../lib/libspdlogd.a
                COMPONENT ${M_INST_COMPONENT}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
            )
        endif()
        if(EXISTS ${spdlog_DIR}/../../../lib/libspdlog.a)
            message("====================================================================")
            message("Installing librarys for SPDLog from ${spdlog_DIR}/../../../lib/libspdlog.a to ${CMAKE_INSTALL_LIBDIR}")
            message("====================================================================")
            install(
                FILES ${spdlog_DIR}/../../../lib/libspdlog.a
                COMPONENT ${M_INST_COMPONENT}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
            )
        endif()
    endif(MSVC)

endmacro()

macro(install_mts_sentinel)
    set(oneValueArgs PATH COMPONENT INSTALL_FOR_NEXT_SDK)
    set(multiValueArgs)
    set(options)
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED M_INST_PATH)
        message(FATAL_ERROR "there is no path given for the install")
    endif()
    if(NOT DEFINED M_INST_COMPONENT)
        message(FATAL_ERROR "there is no component given for the install")
    endif()
    if (MSVC OR MSYS OR MINGW)
        message("====================================================================")
        message("Installing binaries for mts_sentinel to ${M_INST_PATH}/")
        message("====================================================================")
        install(
            IMPORTED_RUNTIME_ARTIFACTS mts_sentinel::sentinel mts_sentinel::msdia
            RUNTIME_DEPENDENCY_SET santinel_set
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${M_INST_PATH}/
        )
    else()
        message("====================================================================")
        message("Installing binaries for mts_sentinel to ${M_INST_PATH}/")
        message("====================================================================")
        install(
            IMPORTED_RUNTIME_ARTIFACTS mts_sentinel::sentinel
            RUNTIME_DEPENDENCY_SET santinel_set
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${M_INST_PATH}/
        )
        install(
            IMPORTED_RUNTIME_ARTIFACTS mts_sentinel::client
            RUNTIME_DEPENDENCY_SET santinel_set
            COMPONENT ${M_INST_COMPONENT}
            DESTINATION ${M_INST_PATH}/../lib/
        )

        install_extra_libs_linux(
            LIB_LIST mts_sentinel::client
            COMPONENT ${M_INST_COMPONENT}
        )
    endif()
endmacro()

function(install_runtime)
    set(oneValueArgs TARGET_NAME COMPONENT)
    set(multiValueArgs FILES DIRECTORY EXTRA_INSTALL_LIBS DESTINATION)
    set(options "optional")
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(DEFINED M_INST_TARGET_NAME)
      install(
        TARGETS ${M_INST_TARGET_NAME}
        COMPONENT ${M_INST_COMPONENT}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      )
      install(
        RUNTIME_DEPENDENCY_SET ${M_INST_TARGET_NAME}
        COMPONENT ${M_INST_COMPONENT}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
      )

      if(WIN32)
        install(
          FILES $<TARGET_PDB_FILE:${M_INST_TARGET_NAME}>
          COMPONENT ${M_INST_COMPONENT}
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          OPTIONAL
        )
        install(
          FILES $<TARGET_RUNTIME_DLLS:${M_INST_TARGET_NAME}>
          COMPONENT ${M_INST_COMPONENT}
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          OPTIONAL
        )
        install_mts_sentinel(
          COMPONENT ${M_INST_COMPONENT}
          PATH $<TARGET_FILE_DIR:${M_INST_TARGET_NAME}>
        )
        add_custom_command(TARGET ${M_INST_TARGET_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${M_INST_TARGET_NAME}> $<TARGET_FILE_DIR:${M_INST_TARGET_NAME}>
          COMMAND_EXPAND_LISTS
        )
      endif()
    endif()

    if(DEFINED M_INST_DESTINATION)
      install(
        DIRECTORY ${M_INST_DIRECTORY}
        COMPONENT ${M_INST_COMPONENT}
        DESTINATION ${M_INST_DESTINATION}
      )
    else()
      install(
        DIRECTORY ${M_INST_DIRECTORY}
        COMPONENT ${M_INST_COMPONENT}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      )
    endif()

    install_mts_sentinel(
      COMPONENT ${M_INST_COMPONENT}
      PATH ${CMAKE_INSTALL_BINDIR}
    )

    install(
      FILES ${M_INST_FILES}
      COMPONENT ${M_INST_COMPONENT}
      DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endfunction()

function(install_extra_libs_linux)
    set(oneValueArgs COMPONENT)
    set(multiValueArgs LIB_LIST EXTRA_LIB_LIST)
    set(options "optional")
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(UNIX)
      list(APPEND additional_libs "") # init empty list
      list(APPEND libs_to_check "")

      if(DEFINED M_INST_EXTRA_LIB_LIST)
        get_target_property(libraries ${M_INST_EXTRA_LIB_LIST} INTERFACE_LINK_LIBRARIES)

        foreach(library ${libraries})
            get_target_property(target_type ${library} TYPE)
            if (target_type STREQUAL "SHARED_LIBRARY")
              # CsZ: Magno libraries should be installed using the following generator expressions
              install(FILES $<TARGET_SONAME_FILE:${library}> COMPONENT ${M_INST_COMPONENT} DESTINATION ${CMAKE_INSTALL_LIBDIR})
              install(FILES $<TARGET_LINKER_FILE:${library}> COMPONENT ${M_INST_COMPONENT} DESTINATION ${CMAKE_INSTALL_LIBDIR})              
            endif()
        endforeach()
      endif()

      list(APPEND libs_to_check ${M_INST_LIB_LIST})

      # now looking for libs and their symlinks (e.g. libMyLib.so and libMyLib.so.1.0.0 are needed)
      foreach(cur_lib ${libs_to_check})
        get_target_property(cur_lib_configuration ${cur_lib} IMPORTED_CONFIGURATIONS)
        get_target_property(cur_lib_location ${cur_lib} IMPORTED_LOCATION_${cur_lib_configuration})
        get_filename_component(cur_lib_lib_dir ${cur_lib_location} DIRECTORY)
        get_filename_component(cur_lib_lib_filename ${cur_lib_location} NAME_WE)
        set(cur_lib_to_check "${cur_lib_lib_dir}/${cur_lib_lib_filename}.so") # stick directory and file name together
        if(NOT EXISTS ${cur_lib_to_check})
          message(WARNING "unable to get path of ${cur_lib}")
        else()
          message("${cur_lib} found at ${cur_lib_to_check}")
          file(GLOB new_files "${cur_lib_to_check}*")
          list(APPEND additional_libs ${new_files})
        endif()
      endforeach()

      install(FILES
        ${additional_libs}
        COMPONENT ${M_INST_COMPONENT}
        DESTINATION lib
      )
    endif()
endfunction()

function(install_extra_libs_windows)
    set(oneValueArgs TARGET_NAME COMPONENT)
    set(multiValueArgs LIB_LIST)
    set(options "optional")
    cmake_parse_arguments(M_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(WIN32)
      get_target_property(libraries ${M_INST_LIB_LIST} INTERFACE_LINK_LIBRARIES)
      foreach(lib ${libraries})
        get_target_property(target_type ${lib} TYPE)
        if(target_type STREQUAL "SHARED_LIBRARY")
          # copy into the build folder (e.g. for debugging)
          add_custom_command(TARGET ${M_INST_TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${M_INST_TARGET_NAME}>
          )
          # installing into package folder
          install(IMPORTED_RUNTIME_ARTIFACTS ${lib} COMPONENT ${M_INST_COMPONENT} DESTINATION ${CMAKE_INSTALL_BINDIR})
        endif()
      endforeach()
    endif()
endfunction()

function(install_lib)
    set(oneValueArgs libname targetname)
    set(multiValueArgs "")
    set(options "optional")
    cmake_parse_arguments(INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED INST_targetname)
        message(FATAL_ERROR "there is no name given for the target")
    endif()

    if (NOT TARGET ${INST_libname})
        if(INST_optional)
            message(WARNING "library \"${INST_libname}\" does not exist; ignored")
            continue()
        else()
            message(FATAL_ERROR "library \"${INST_libname}\" does not exist")
        endif()
    endif()

    message("====================================================================")
    message("Installing library ${INST_libname} for target ${INST_targetname}")
    message("====================================================================")
    get_target_property(target_type ${INST_libname} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY")
        if(MSVC)
            # copy into the build folder (e.g. for debugging)
            add_custom_command(TARGET ${INST_targetname} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${INST_libname}> $<TARGET_FILE_DIR:${INST_targetname}>
            )
            # installing into bin folder
            install(IMPORTED_RUNTIME_ARTIFACTS ${INST_libname} COMPONENT TARGET_COMPONENT DESTINATION ${CMAKE_INSTALL_BINDIR})
        else(MSVC)
            # installing into lib folder
            install(IMPORTED_RUNTIME_ARTIFACTS ${INST_libname} COMPONENT TARGET_COMPONENT DESTINATION ${CMAKE_INSTALL_LIBDIR})
        endif(MSVC)
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        if(MSVC)
            # copy into the build folder (e.g. for debugging)
            add_custom_command(TARGET ${INST_targetname} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${INST_libname}> $<TARGET_FILE_DIR:${INST_targetname}>
            )
            # installing into lib folder
            install(IMPORTED_RUNTIME_ARTIFACTS ${INST_libname} COMPONENT TARGET_COMPONENT DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else(MSVC)
            install(IMPORTED_RUNTIME_ARTIFACTS ${INST_libname} COMPONENT TARGET_COMPONENT DESTINATION ${CMAKE_INSTALL_LIBDIR})
        endif(MSVC)
    else(target_type)
         message(FATAL_ERROR "function only used to install static and shared librarys")
    endif(target_type)
endfunction(install_lib)
