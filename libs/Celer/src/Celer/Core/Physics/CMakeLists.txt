project(CelerPhysics)


set( CelerPhysics_SOURCES BoundingBox3.cpp OrientedBoundingBox3.cpp)
 
set( CelerPhysics_HEADERS BoundingBox3.hpp OrientedBoundingBox3.hpp)

add_library( CelerPhysics STATIC  ${CelerPhysics_SOURCES} ${CelerPhysics_HEADERS} )

target_link_libraries(CelerPhysics CelerMath)

