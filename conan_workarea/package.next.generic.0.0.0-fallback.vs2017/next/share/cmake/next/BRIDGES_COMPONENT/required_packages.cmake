
# Code generated by <bricks_cmake_groups>/cmake/post/implicit_dependencies/generate.py; DO NOT EDIT!

# try importing package
if (next_FIND_QUIETLY)
    find_package(mtscpp QUIET)
else()
    find_package(mtscpp)
endif()

if (NOT (mtscpp_FOUND))

    # depending package cannot be imported
    set(next_BRIDGES_COMPONENT_FOUND 0)
    set(
        next_BRIDGES_COMPONENT_NOT_FOUND_MESSAGE
        "Importing required package mtscpp failed: ${mtscpp_NOT_FOUND_MESSAGE}"
    )
endif()
