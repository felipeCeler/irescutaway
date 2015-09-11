
project(CelerScene)

set( CelerScene_SOURCES Camera.cpp Frustum.cpp )
set( CelerScene_HEADERS Camera.hpp Frustum.hpp )

add_library( CelerScene STATIC  ${CelerScene_SOURCES} ${CelerScene_HEADERS}  )



