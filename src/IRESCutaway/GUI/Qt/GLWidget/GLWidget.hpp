#ifndef GLWIDGET_H
#define GLWIDGET_H

/// Ires Library
#include <ires/ires.h>
/// Celer::Libraries
	/// OpenGL
#include <GL/glew.h>
#include <IRES/CornerPointGrid.hpp>

/// LCG Libary
#include <shaderlib/trackball.hpp>
#include <shaderlib/framebuffer.hpp>
#include <samples/imgproc/meanfilter.hpp>

/// C++ Standard Library
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <iterator>
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


class GLWidget: public QGLWidget
{
	Q_OBJECT

	public:
		// FIXME - Putting this as public, but I'm going to search for a better sulution to how feed other
		//	   widget with data form another one.

		IRES::CornerPointGrid reservoir_model_;

		// GPU load Properties

		std::string properties_name[4];
		std::size_t indices[4];
		float min_value[4];
		float max_value[4];

		float min_range;
		float max_range;

		int current_property;

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
		void loadProperties     ( );
		void updateCluster      ( int i ) { cluster = i; updateGL();}

		void setNoCutawayVisibility     ( bool visibility ) { isNoCutaway      = visibility; updateGL();}
		void setRawCutawayVisibility    ( bool visibility ) { isRawApproach    = visibility; updateGL();}
		void setIRESCutawayVisibility   ( bool visibility ) {   isIRESApproach = visibility; updateGL();}

		void freezeView ( );

	protected:
		void dragEnterEvent ( QDragEnterEvent *event );
		void dragMoveEvent  ( QDragMoveEvent *event );
		void dragLeaveEvent ( QDragLeaveEvent *event );
		void dropEvent      ( QDropEvent *event );

	signals:

        void changed(const QMimeData *mimeData = 0);

private:

	QPoint lastPos;
	/// Avoid camera_ movement issues
	QSet<int> keysPresseds_;
	bool 	  buttonRelease_;

	bool isIRESOpen_;

        Shader*                         BoundingBoxInitializationLCG;
        Shader*                         BoundingBoxDebugLCG;
        Shader*                         BoundingBoxCutawayLCG;

        Shader*                         secondaryLCG;

        Shader*                         primaryLCG;
        Shader*                         shellLCG;
        Shader*                         rawShellLCG;

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
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f> >  lights;

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


	std::vector<float>           cuboids;
	GLint			     cuboids_size;

	std::vector<float> cubeColor;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_cube_color;
	std::vector<float> cubeIJK;
	/// vec4 (I, J, K , 0 );
		GLuint vertexBuffer_cube_IJK;
	std::vector<float> cubeFocus;
	/// vec4 (Primary/Secondary , Active  ,0.0 , 0.0);
		GLuint vertexBuffer_cube_Focus;

	std::vector<float> cubeProperties;
	// Four property x = Bubble Point Pressure
	//               y = Pressure
	//	         z = Porosity
	//               w = Modified Block Volume
		GLuint vertexBuffer_cube_properties;

	/// -- Shell / Fracture Geometry

	// Face in Interleaved VertexBuffer
	// Use same layout location as vertexArray

	// 1 Vertex Array
	GLuint vertexArray_faceFeature;
	// 1 Vertex Buffer
	GLuint vertexBuffer_faceFeature;


	std::vector<float> facesFeatureColors;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_faceColor;
	std::vector<float> facesFeatureIJK;
	/// vec4 (R, G, B , 0 );
		GLuint vertexBuffer_faceIJK;
	std::vector<float> facesFeatureType;
	/// vec4 ( isShell, isFault, 0.0 , 0.0 );
		GLuint vertexBuffer_faceType;

	std::vector<float> facesFeatureProperties;
	// Four property x = Bubble Point Pressure
	//               y = Pressure
	//	         z = Porosity
	//               w = Modified Block Volume
			GLuint vertexBuffer_faceProperties;

	std::vector<float>      facesFeature;
	GLint 			facesFeature_size;

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

	bool dynamic_;

	// LCG procudure
        Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);


        // TODO NPR Stuffs - Mario's request
        GLuint xtoon_texture_;
        Mesh * picture;

        Shader * xtoon_texture_viewer;
};

#endif
