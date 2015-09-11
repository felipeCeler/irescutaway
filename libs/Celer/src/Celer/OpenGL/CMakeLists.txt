project(CelerOpenGL)

set(CelerOpenGL_MAJOR_VERSION 0)
set(CelerOpenGL_MINOR_VERSION 1)
set(CelerOpenGL_PATCH_VERSION 0)
set(CelerOpenGL_VERSION
  ${CelerOpenGL_MAJOR_VERSION}.${CelerOpenGL_MINOR_VERSION}.${CelerOpenGL_PATCH_VERSION})
 
set(CelerOpenGL_SRCS
    OpenGLContext.cpp
    Texture.cpp
    PixelBuffer.cpp
    FrameBuffer.cpp
    Shader.cpp
    ShaderManager.cpp
)

set(CelerOpenGL_HEADERS
    OpenGLContext.hpp
    Texture.hpp
    PixelBuffer.hpp
    FrameBuffer.hpp
    Shader.hpp
    ShaderManager.hpp
)

message(STATUS "Compiling ${CMAKE_PROJECT_NAME} version ${CelerOpenGL_VERSION}")
message(STATUS "Using cmake version ${CMAKE_VERSION}")
     
add_library( CelerOpenGL STATIC  ${CelerOpenGL_SRCS} ${CelerOpenGL_HEADERS} )
     
target_link_libraries(CelerOpenGL CelerBase ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} )
