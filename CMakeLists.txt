cmake_minimum_required (VERSION 2.6)

project (IRESReader)

MESSAGE(STATUS "Platform : "${CMAKE_SYSTEM})

# where cmake will search for FINDME.cmake 
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/Files/CMake)

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/ )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/lib/ )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/lib/ )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/ )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin/ )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin/ )
   
# Do not remember what's the meaning of this  !
add_definitions(${QT_DEFINITIONS} -Wall)

## - Libraris
add_subdirectory(libs/Celer/src)
## - Project Sources
add_subdirectory(src/${PROJECT_NAME})
