project(CelerBase)

set( CelerBase_SOURCES Exception.cpp)
set( CelerBase_HEADERS Exception.hpp Base.hpp)

add_library( CelerBase STATIC  ${CelerBase_SOURCES} ${CelerBase_HEADERS}  )

