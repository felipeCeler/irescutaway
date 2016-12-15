project(Tucano)

find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
        #Eigen3
        ${EIGEN3_INCLUDE_DIR}
        #OpenGL and GLEW
        ${GLEW_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        )
 
set( _SRCS_
    Camera.cpp
    FlyCamera.cpp
    FrameBuffer.cpp
    GLTexture.cpp
    Shader.cpp
    TextureManager.cpp
    Trackball.cpp
    )

set( _HEADERS_
    Camera.hpp
    FlyCamera.hpp
    FrameBuffer.hpp
    GLTexture.hpp
    Shader.hpp
    TextureManager.hpp
    Trackball.hpp
    Misc.hpp
    )

# C++ flags
## -Wno-unused-local-typedefs -Wunused-local-typedefs - For Eigen Boring Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-local-typedefs -Wunused-local-typedefs")
     
add_library( Tucano STATIC  ${_SRCS_} ${_HEADERS_} )
     
target_link_libraries(Tucano  ${OPENGL_LIBRARIES} ${GLEW_GLEW_LIBRARY} )
        