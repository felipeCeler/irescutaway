# Teste
project(${CMAKE_PROJECT_NAME}_EXE)

## It asks for Qt5_DIR
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)


#INCLUDE(${QT_USE_FILE})


include_directories(
		##- Celer Library
		${CMAKE_SOURCE_DIR}/libs/Celer/src
		##- Ires Library
		${CMAKE_SOURCE_DIR}/libs/Ires
		## - LCG/include
		${CMAKE_SOURCE_DIR}/libs/LCG
		## - Tucano/include
		${CMAKE_SOURCE_DIR}/libs
		## - Tucano/include
		${CMAKE_SOURCE_DIR}/libs/tucano/src
        ##- OpenGL 
        ${GLEW_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ## - Eigen
        ${EIGEN_INCLUDE_DIR}
        ## - Qt5
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5OpenGL_INCLUDE_DIRS}
        )

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

set( ${PROJECT_NAME}_SOURCES App/main.cpp 
    						 GUI/Qt/MainWindow/MainWindow.cpp
    						 GUI/Qt/AboutWidget/AboutWidgetIRESReader.cpp
    						 GUI/Qt/GLWidget/GLWidget.cpp
    						 GUI/Qt/GLWidget/GLAnimation.cpp
    						 IRES/CornerPointGrid.cpp    						 
    						 IRES/Block.cpp
    						 Loaders/Ply/PlyLoader.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/trackball.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/framebuffer.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/shader.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/mesh.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/samples/imgproc/meanfilter.cpp
    						 ## Tucano
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/tucano.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/texture.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/texturemanager.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/mesh.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/model.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/framebuffer.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/effect.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/bufferobject.hpp
    						 ${CMAKE_SOURCE_DIR}/libs/tucano/src/utils/ppmimporter.hpp
    						 )
	
set( ${PROJECT_NAME}_HEADERS GUI/Qt/MainWindow/MainWindow.hpp
                             GUI/Qt/AboutWidget/AboutWidgetIRESReader.hpp
                             GUI/Qt/GLWidget/GLWidget.hpp
                             IRES/CornerPointGrid.hpp
                             IRES/Block.hpp
                             Loaders/Ply/PlyLoader.hpp
                             ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/trackball.hpp
                             ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/framebuffer.hpp
                             ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/shader.hpp
                             ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/mesh.hpp
                             ${CMAKE_SOURCE_DIR}/libs/LCG/samples/imgproc/meanfilter.hpp
                             )	

set( MOC_HEADERS GUI/Qt/MainWindow/MainWindow.hpp
                 GUI/Qt/AboutWidget/AboutWidgetIRESReader.hpp
                 GUI/Qt/GLWidget/GLWidget.hpp  )
                 
set( UI_SOURCES GUI/Qt/UI/${CMAKE_PROJECT_NAME}.ui GUI/Qt/AboutWidget/AboutWidgetIRESReader.ui)

set(RCC_SOURCES GUI/Qt/RCC/Resources.qrc)

QT5_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS})
QT5_WRAP_UI (UI_HEADERS ${UI_SOURCES})
QT5_ADD_RESOURCES(RCC_HEADERS ${RCC_SOURCES})

#QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS})
#QT4_WRAP_UI (UI_HEADERS ${UI_SOURCES})
#QT4_ADD_RESOURCES(RCC_HEADERS ${RCC_SOURCES})

IF(WIN32)
	ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS} ${MOC_SOURCES} ${UI_HEADERS} ${RCC_HEADERS})
ELSE(WIN32)
	ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS} ${MOC_SOURCES} ${UI_HEADERS} ${RCC_HEADERS})	
ENDIF(WIN32)
              
#TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} IresReader CelerMath CelerPhysics CelerScene CelerOpenGL)

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} Qt5::Gui  Qt5::OpenGL Qt5::Widgets Qt5::Core ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} IresReader CelerMath CelerPhysics CelerScene CelerOpenGL)

# Do a better solution. This one is Ungly !!!
SET (SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/GUI/Qt/RCC/Shaders")
             
SET (DESTINATION "${CMAKE_BINARY_DIR}/build/share/Shaders")

ADD_CUSTOM_TARGET (CopyShaders ALL
                   DEPENDS ${SOURCE}
                   COMMENT "Copying Shaders: ${SOURCE} ${DESTINATION}" )
                 
ADD_CUSTOM_COMMAND (
                TARGET CopyShaders
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE} ${DESTINATION} )   

