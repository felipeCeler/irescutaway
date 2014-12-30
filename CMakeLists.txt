cmake_minimum_required (VERSION 2.6)

project (IRESCutaway)

message(STATUS "Platform : "${CMAKE_SYSTEM})

# where cmake will search for FINDME.cmake 
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/Files/CMake)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/lib/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/lib/ )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin/ )

# C++ flags
## -Wno-unused-local-typedefs -Wunused-local-typedefs - For Eigen Boring Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wunused-local-typedefs -Wno-unused-local-typedefs")
set(CMAKE_BUILD_TYPE Debug)

IF(MSVC)
   ADD_DEFINITIONS(/arch:SSE2)
ENDIF(MSVC)

# End C++ flags
   
# I do not remember what's the meaning of this  !
# add_definitions(${QT_DEFINITIONS} -Wall)

## - Libraris
add_subdirectory(libs/Ires)
## - Celer Libraris
add_subdirectory(libs/Celer/src)
## - Project Sources
add_subdirectory(src/${PROJECT_NAME})
