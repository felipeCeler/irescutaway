project(CelerMath)


set( CelerMath_SOURCES Math.cpp Vector2.cpp Vector3.cpp Vector4.cpp 
 Quaternion.cpp Color.cpp Matrix3x3.cpp Matrix4x4.cpp EigenSystem.cpp )
 
set( CelerMath_HEADERS Math.hpp Vector2.hpp Vector3.hpp Vector4.hpp 
 Quaternion.hpp Color.hpp Matrix3x3.hpp Matrix4x4.hpp EigenSystem.hpp )

add_library( CelerMath STATIC  ${CelerMath_SOURCES} ${CelerMath_HEADERS} )


