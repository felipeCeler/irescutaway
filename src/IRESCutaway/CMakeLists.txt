# Teste
project(${CMAKE_PROJECT_NAME}_EXE)

set(QT_USE_QTOPENGL true)
set(QT_USE_QTGUI true)
set(QT_WRAP_CPP true)
set(QT_WRAP_UI true)

# find needed packages
find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen REQUIRED)

INCLUDE(${QT_USE_FILE})


include_directories(
		##- Celer Library
		${CMAKE_SOURCE_DIR}/libs/Celer/src
		##- Ires Library
		${CMAKE_SOURCE_DIR}/libs/Ires
		## - LCG/include
		${CMAKE_SOURCE_DIR}/libs/LCG
        ##- Qt     
        ${QT_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTOPENGL_INCLUDE_DIR}
        ##- OpenGL 
        ${GLEW_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ## - Eigen
        ${EIGEN_INCLUDE_DIR}
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
    						 IRES/CornerPointGrid.cpp    						 
    						 IRES/Block.cpp
    						 Loaders/Ply/PlyLoader.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/trackball.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/framebuffer.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/shader.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/shaderlib/mesh.cpp
    						 ${CMAKE_SOURCE_DIR}/libs/LCG/samples/imgproc/meanfilter.cpp
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

QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS})
QT4_WRAP_UI (UI_HEADERS ${UI_SOURCES})
QT4_ADD_RESOURCES(RCC_HEADERS ${RCC_SOURCES})

IF(WIN32)
	ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS} ${MOC_SOURCES} ${UI_HEADERS} ${RCC_HEADERS})
ELSE(WIN32)
	ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS} ${MOC_SOURCES} ${UI_HEADERS} ${RCC_HEADERS})	
ENDIF(WIN32)
              
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} IresReader CelerMath CelerPhysics CelerScene CelerOpenGL)

# Do a better solution. This one is Ungly !!!
SET (SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/GUI/Qt/RCC/Shaders")
             
SET (DESTINATION "${CMAKE_BINARY_DIR}/build/share/Shaders")

ADD_CUSTOM_TARGET (CopyShaders ALL
                   DEPENDS ${SOURCE}
                   COMMENT "Copying Shaders: ${SOURCE} ${DESTINATION}" )
                 
ADD_CUSTOM_COMMAND (
                TARGET CopyShaders
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE} ${DESTINATION} )   

