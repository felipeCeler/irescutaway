project(Ires)

find_package(Eigen REQUIRED)

include_directories(
        #Eigen
        ${EIGEN_INCLUDE_DIR}
        )


set( Ires_SOURCES Ires/ires/ires.cpp 
                  Ires//ires/reader.cpp 
                  Ires/ires/writer.cpp 
                  Ires/ires/kdtree.cpp 
                  Ires/ires/snappy/snappy.cc
                  Ires/ires/snappy/snappy-c.cc
                  Ires/ires/snappy/snappy-sinksource.cc
                  Ires/ires/snappy/snappy-stubs-internal.cc )
                  
set( Ires_HEADERS Ires/ires/ires.h 
                  Ires/ires/common.h
                  Ires/ires/snappy/snappy.h
                  Ires/ires/snappy/snappy-c.h
                  Ires/ires/snappy/snappy-internal.h
                  Ires/ires/snappy/snappy-sinksource.h
                  Ires/ires/snappy/snappy-stubs-internal.h
                  Ires/ires/snappy/snappy-stubs-public.h )

add_library( IresReader STATIC  ${Ires_SOURCES} ${Ires_HEADERS}  )



