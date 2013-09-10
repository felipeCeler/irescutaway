#ifndef GLWIDGET_H
#define GLWIDGET_H

///
#include "Cube.hpp"

/// Ires Library
#include <ires/ires.h>
/// Celer::Libraries
	/// OpenGL
#include <Celer/OpenGL/OpenGLContext.hpp>
#include <Celer/OpenGL/Texture.hpp>
#include <Celer/OpenGL/Shader.hpp>
#include <Celer/OpenGL/ShaderManager.hpp>
	/// Math
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Vector4.hpp>
	/// Scene
#include <Celer/Scene/Camera.hpp>
/// LCG Libary
#include <shaderlib/trackball.hpp>
#include <shaderlib/framebuffer.hpp>
#include <samples/imgproc/meanfilter.hpp>

/// C++ Standard Library
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>
#include <functional>

/// Qt 4.8
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMdiSubWindow>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QMimeData>
#include <QtCore/QTime>
#include <QtCore/QElapsedTimer>

#include <IRES/CornerPointGrid.hpp>


class GLWidget: public QGLWidget
{
	Q_OBJECT

	public:
		// FIXME - Putting this as public, but I'm going to search for a better sulution to how feed other
		//	   widget with data form another one.

		IRES::CornerPointGrid reservoir_model_;
		ires::Ires	      reservoir_model_eclipse;
		// From QGLWidget
		explicit GLWidget ( const QGLFormat& format , QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		explicit GLWidget ( QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		void initializeGL ( );
		void resizeGL ( int width , int height );

		void paintGL ( );
		//void timerEvent(QTimerEvent*);
		void processMultiKeys ( );
		void mousePressEvent ( QMouseEvent *event );
		void mouseMoveEvent ( QMouseEvent *event );
		void mouseReleaseEvent ( QMouseEvent *event );
		void wheelEvent ( QWheelEvent *e );
		void keyPressEvent ( QKeyEvent * event );
		void keyReleaseEvent ( QKeyEvent * event );

	public slots:

		/// TODO Physics
		void gameLooping ( );
		void fpsCounter ( );

		void loadShaders ( );
		void openIRESCharles  ( const std::string& filename );
		bool isIRESOpen () const;

		void cutVolumeGenerator ( );
		void changeProperty ( int property_index );
		void changePropertyRange ( const double& min, const double& max, int property_index );

		void changeIJK ( const int& min_i, const int& max_i, const int& min_j, const int& max_j, const int& min_k, const int& max_k );

		void changeMaxI ( const int& value);
		void changeMaxJ ( const int& value);
		void changeMaxK ( const int& value);

		void changeMinI ( const int& value);
		void changeMinJ ( const int& value);
		void changeMinK ( const int& value);

		void setPrimaryVisibility ( bool );
		void setSecondaryVisibility ( bool );

		// Draw Functions

		void drawCutawaySurface ( );
		void drawSecundary ( );
		void drawPrimary   ( );

		bool dynamic() { return dynamic_; }

		void NoCutaway          ( );
		void RawCutaway         ( int cluster );
		void IRESCutaway      	( );
		void updateCluster      ( int i ) { cluster = i; updateGL();}

		void setNoCutawayVisibility     ( bool visibility ) { isNoCutaway      = visibility; updateGL();}
		void setRawCutawayVisibility    ( bool visibility ) { isRawApproach    = visibility; updateGL();}
		void setIRESCutawayVisibility   ( bool visibility ) {   isIRESApproach = visibility; updateGL();}

		void IRESv1_to_IRESv2 ( const std::string& filename );

		void CameraTrackball ( );
		void CameraFly ( );
		void freezeView ( );

	protected:
		void dragEnterEvent ( QDragEnterEvent *event );
		void dragMoveEvent  ( QDragMoveEvent *event );
		void dragLeaveEvent ( QDragLeaveEvent *event );
		void dropEvent      ( QDropEvent *event );

	signals:

        void changed(const QMimeData *mimeData = 0);

private:

	bool getVertices(unsigned int , float * vertices );


	QPoint lastPos;
	/// Avoid camera_ movement issues
	QSet<int> keysPresseds_;
	bool 	  buttonRelease_;

	bool isIRESOpen_;


	Celer::BoundingBox3<float> 		   box;
	std::list  <Celer::BoundingBox3<GLfloat> > boxes;
	std::vector<Celer::BoundingBox3<GLfloat> > cutVolumes;
	std::vector<Celer::Vector3<GLfloat> >      box_vertices;

	GLuint vertexArray_box;
	GLuint vertexBuffer_box;

	struct CutBox
	{
		Celer::Vector4<float> center;
		Celer::Vector4<float> axis[3];
		Celer::Vector4<float> extent;
		Celer::Vector4<float> aperture;
	};

	std::vector<CutBox> cutBoxs;

	GLuint vertexArray_cutBox;
	GLuint vertexBuffer_cutBox;

	// BoundingBox Visualization
	Celer::OpenGL::ShaderManager    BoundingBoxInitialization;
	Celer::OpenGL::ShaderManager    BoundingBoxDebug;
	Celer::OpenGL::ShaderManager    BoundingBoxCutaway;

	// No cutaway Visualization
	Celer::OpenGL::ShaderManager	primary;
	Celer::OpenGL::ShaderManager	secondary;

	Celer::OpenGL::ShaderManager	debugNormal;

	Celer::OpenGL::ShaderManager shell;

	QImage fbo;
	float  angle;
	float  zoom_angle_;
	int    cluster;

	/// -- ANIMATION  Stuffs ----------------------------------------------
	// Celer Gaming Looping 0.1 -> For animation
	QTimer fpsTimer_;
	QTimer updateTimer_;
	QElapsedTimer delta_time;
	int fps;

	/// -- RENDERING Stuffs  ----------------------------------------------
	bool  isNoCutaway;
	bool  isRawApproach;
	bool  isIRESApproach;

	bool draw_secondary;
	bool draw_primary;

	// Used in ires v1 to ires v2 conversion.
	int blockIndex_;

	int max_I_;
	int min_I_;
	int max_J_;
	int min_J_;
	int max_K_;
	int min_K_;

	// lights
	std::vector < Eigen::Vector3f > lights;

	/// normal x y z + vertice .z
	Framebuffer * depthFBO;

	/// Smooth
	MeanFilter * meanFilter;

	// Cube in Interleaved VertexBuffer
	// Use same layout location as vertexArray

	// 1 Vertex Array
	GLuint vertexArray_cuboid;
	// 1 Vertex Buffer with 8 vertex which define a Cuboid
	GLuint vertexBuffer_cuboid;

	struct Cuboid
	{
		Celer::Vector4<float> vertices[8];
	};

	std::vector<Cuboid>           cuboids;

	std::vector<Celer::Vector4<float> > cubeColor;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_cube_color;
	std::vector<Celer::Vector4<float> > cubeIJK;
	/// vec4 (I, J, K , 0 );
		GLuint vertexBuffer_cube_IJK;
	std::vector<Celer::Vector4<float> > cubeFocus;
	/// vec4 (Primary/Secundary , Actived  ,0.0 , 0.0);
		GLuint vertexBuffer_cube_Focus;

	/// -- Shell / Fracture Geometry

	// Face in Interleaved VertexBuffer
	// Use same layout location as vertexArray

	// 1 Vertex Array
	GLuint vertexArray_faceFeature;
	// 1 Vertex Buffer
	GLuint vertexBuffer_faceFeature;

	struct FaceFeature
	{
		Celer::Vector4<float> vertices[4];
	};

	std::vector<Celer::Vector4<float> > facesFeatureColors;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_faceColor;
	std::vector<Celer::Vector4<float> > facesFeatureIJK;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_faceIJK;
	std::vector<Celer::Vector4<float> > facesFeatureType;
	/// vec4 ( isShell, isFault, 0.0 , 0.0 );
		GLuint vertexBuffer_faceType;

	std::vector<FaceFeature>        facesFeature;

        bool enable_blend_;

	/// -- SCENE Stuffs ---------------------------------------------------

	/// -- Aperture of Cutaway
	float volume_width;
	float volume_height;

	// Stores the freezed camera (view matrix)
	Eigen::Matrix4f freeze_viewmatrix_;
	bool freezeView_;

	// trackball
	Trackball * trackball_;

	float orthoZoom;
	Celer::Matrix4x4<float> modelMatrix_;

	// Uniform Buffers
	GLuint  uniformBuffer_globalMatrices_;

	struct Transformation
	{
			Celer::Matrix4x4<float> ModelMatrix;
			Celer::Matrix4x4<float> ViewMatrix;
			Celer::Matrix4x4<float> ProjectionMatrix;
 	};

	Transformation transformationMatrices_;

	bool dynamic_;

	// LCG procudure
        Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);

        // Old Celer Setup
	Celer::Camera<float> camera_;
	float cameraStep_;
	float scrollStep_;
	float centerX_;
	float centerY_;

};

#endif
