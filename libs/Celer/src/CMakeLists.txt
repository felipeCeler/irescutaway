project (CelerLibraries)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
        # This->Project
        ${CMAKE_CURRENT_SOURCE_DIR} 
        #OpenGL and GLEW
        ${GLEW_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        )


## Base classes
add_subdirectory(Celer/Base)

## Core Math Library - Just Vectors, Quaternions and Matrices Classes.
add_subdirectory(Celer/Core/Geometry/Math)

## Core Physics Library - Just Some Bounding Volume and their usage.
add_subdirectory(Celer/Core/Physics)

## Scene Class - Camera, Light and Everything about a Scene.
add_subdirectory(Celer/Scene)

## OpenGL Wrappers
add_subdirectory(Celer/OpenGL)

