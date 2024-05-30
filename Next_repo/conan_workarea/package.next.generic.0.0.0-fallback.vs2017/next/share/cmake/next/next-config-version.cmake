# This file defines version for next
# the <project>-config-version.cmake is special file which is
# used to check version of the project
#

# Some ideas borrowed from excellent article
# https://foonathan.net/blog/2016/03/03/cmake-install.html
# written by Jonathan Müller a.k.a foonathan

set(PACKAGE_VERSION "0.0.0")

if("${PACKAGE_FIND_VERSION_COUNT}" GREATER 0)

    if("${PACKAGE_FIND_VERSION_MAJOR}" EQUAL "0")

        if("${PACKAGE_FIND_VERSION_MINOR}" EQUAL "0")

            if("${PACKAGE_FIND_VERSION_PATCH}" EQUAL "0")
                set(PACKAGE_VERSION_EXACT TRUE)
            else()
                if("${PACKAGE_FIND_VERSION_PATCH}" LESS "0")
                    set(PACKAGE_VERSION_COMPATIBLE TRUE)
                else()
                    set(PACKAGE_VERSION_UNSUITABLE TRUE)
                endif()
            endif()
        elseif("${PACKAGE_FIND_VERSION_MINOR}" LESS "0")

            if("${PACKAGE_FIND_VERSION_PATCH}" LESS "0")
                set(PACKAGE_VERSION_COMPATIBLE TRUE)
            else()
                set(PACKAGE_VERSION_UNSUITABLE TRUE)
            endif()
        else()
            set(PACKAGE_VERSION_UNSUITABLE TRUE)
        endif()
    else()
        set(PACKAGE_VERSION_UNSUITABLE TRUE)
    endif()
else()
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
endif()
