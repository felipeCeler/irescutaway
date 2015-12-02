cmake_minimum_required (VERSION 2.6)

project(IresReader)

# where cmake will search for FINDME.cmake 
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )


message (${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# C++ flags - Ires needs c++0x version or high.   
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")


find_package(Eigen3 REQUIRED)

include_directories(
        #Eigen
        ${EIGEN_INCLUDE_DIR}
)

set( Ires_SOURCES ires/ires.cpp
                  ires/kdtreeires.cpp
                  ires/reader.cpp 
                  ires/writer.cpp 
                  ires/snappy/snappy.cc
                  ires/snappy/snappy-c.cc
                  ires/snappy/snappy-sinksource.cc
                  ires/snappy/snappy-stubs-internal.cc )
                  
set( Ires_HEADERS ires/ires.h 
                  ires/kdtreeires.h
                  ires/primitivesires.h
                  ires/snappy/snappy.h
                  ires/snappy/snappy-c.h
                  ires/snappy/snappy-internal.h
                  ires/snappy/snappy-sinksource.h
                  ires/snappy/snappy-stubs-internal.h
                  ires/snappy/snappy-stubs-public.h )

add_library( ${PROJECT_NAME} STATIC  ${Ires_SOURCES} ${Ires_HEADERS}  )



