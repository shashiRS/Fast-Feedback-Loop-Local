set(CMAKE_INSTALL_RPATH_BACKUP ${CMAKE_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\};$\{ORIGIN\}/../lib")

set(POCKET_LZMA_LIB pocketlzma_lib)

add_library(${POCKET_LZMA_LIB} STATIC 
    pocketlzma.hpp
    pocketlzma_wrapper.hpp
    pocketlzma_wrapper.cpp
)

if(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=../lib")
endif(UNIX)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH_BACKUP})