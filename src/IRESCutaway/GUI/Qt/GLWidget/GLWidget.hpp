#ifndef MYGLFRAME_H
#define MYGLFRAME_H


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

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFramebufferObject>

// Standard Library
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>
#include <functional>


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

		// TODO Physics
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


		void NoCutaway          ( );
		void BurnsCutaway       ( );
		void RawCutaway         ( int cluster );
		void IRESCutaway      ( );
		void updateCluster      ( int i ) { cluster = i; updateGL();}

		void setNoCutawayVisibility     ( bool visibility ) { isNoCutaway      = visibility; updateGL();}
		void setBurnsCutawayVisibility  ( bool visibility ) { isBurnsApproach  = visibility; updateGL();}
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

	// About Scene
	Celer::Camera<float> camera_;
	float cameraStep_;
	float scrollStep_;
	float centerX_;
	float centerY_;

	// Uniform Buffers
	GLuint  uniformBuffer_globalMatrices_;

	struct Transformation
	{
			Celer::Matrix4x4<float> ModelMatrix;
			Celer::Matrix4x4<float> ViewMatrix;
			Celer::Matrix4x4<float> ProjectionMatrix;
 	};

	Transformation transformationMatrices_;

	bool getVertices(unsigned int , float * vertices );

	Celer::OpenGL::ShaderManager	debugNormal;

	QPoint lastPos;
	/// Avoid camera_ movement issues
	QSet<int> keysPresseds_;
	bool 	  buttonRelease_;

	GLuint vertexArrayScreen;
	GLuint screen_buffer;
	GLuint texture_buffer;

	bool isIRESOpen_;

	// Cube in Interleaved VertexBuffer
	// Use same layout location as vertexArray

	// 1 Vertex Array
	GLuint vertexArray_cube_interleaved;
	// 1 Vertex Buffer
	GLuint vertexBuffer_cube_interleaved;

	struct CubeData
	{
		Celer::Vector4<float> vertices[8];
		Celer::Vector4<float> color;
		Celer::Vector4<float> IJK;
		Celer::Vector4<float> focus;
		Celer::Vector4<float> centroid;
	};

	std::vector<CubeData>           cube_interleaved;
	Celer::OpenGL::ShaderManager    cube_interleaved_shader;

	// Hong Kong Cutaway
	Celer::OpenGL::ShaderManager    cutawayVolumes;


	// Cut Volume Stuffs
	GLuint                          uniformBuffer_cutVolumes_;


	// Vector4<int> to mach std140 on shader. I'm still confusing.
	// @link http://www.gamedev.net/topic/577950-glsl-uniform-block-std140-layout-question/
	// @link http://stackoverflow.com/questions/15750788/glsl-layout-std140-padding-dilemma
	struct CutVolume
	{
		Celer::Vector4<float> center_points[63];
		Celer::Vector4<int>   size;
	};

	CutVolume cutVolume_;

	Celer::BoundingBox3<float> 		   box;
	std::list  <Celer::BoundingBox3<GLfloat> > boxes;
	std::vector<Celer::BoundingBox3<GLfloat> > cutVolumes;
	std::vector<Celer::Vector3<GLfloat> >      box_vertices;


	GLuint vertexArray_box;
	GLuint vertexBuffer_box;

	// From Image Cutaway.

	float theta;

	float pm_sz;

	QGLFramebufferObject*   fboStep[2];

	// Burns Approach
	Celer::OpenGL::ShaderManager	BurnsJFAInitializing430;
	Celer::OpenGL::ShaderManager	BurnsJFAStep430;
	Celer::OpenGL::ShaderManager	BurnsCutaway430Wireframe;

	// BoundingBox Visualization
	Celer::OpenGL::ShaderManager	cutVolume;
	Celer::OpenGL::ShaderManager    BoundingBoxInitialization;
	Celer::OpenGL::ShaderManager    BoundingBoxDebug;
	Celer::OpenGL::ShaderManager    BoundingBoxCutaway;

	// No cutaway Visualization
	Celer::OpenGL::ShaderManager	primary;
	Celer::OpenGL::ShaderManager	secondary;

	QImage fbo;
	float  angle;
	float  zoom_angle_;
	int cluster;


	bool  isNoCutaway;
	bool  isBurnsApproach;
	bool  isRawApproach;
	bool  isIRESApproach;

	bool draw_secondary;
	bool draw_primary;

	int cont;

	int max_I_;
	int min_I_;
	int max_J_;
	int min_J_;
	int max_K_;
	int min_K_;

	/// Celer Gaming Looping 0.1
	QTimer fpsTimer_;
	QTimer updateTimer_;
	QElapsedTimer delta_time;
	int fps;

	bool freezeView_;

	Celer::Vector3<float> new_z;

	Celer::Vector3<float> new_x;

	Celer::Vector3<float> new_y;

	float volume_width;
	float volume_height;

	Celer::Matrix4x4<float> lookatCamera;


};

#endif
