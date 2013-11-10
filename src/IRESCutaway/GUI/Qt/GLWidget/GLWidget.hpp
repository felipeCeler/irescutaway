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

		// From QGLWidget
		explicit GLWidget ( const QGLFormat& format , QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		explicit GLWidget ( QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		void initializeGL ( );
		void resizeGL     ( int width , int height );

		void paintGL      ( );
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
		void drawBackGround ( );

		void loadShaders ( );
		void openIRES_v2 ( const std::string& filename );
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
		void drawCutawaySurface    ( );	//
		void drawSecondary         ( ); // Draw only secondary Cells
		void drawPrimary           ( );	// Draw only primary   Cells
		void drawPrimaryBoudingBox ( ); // Draw only primary   Cells with its bounding box
		void drawFullModel         ( );

		void drawRawModel       ( );
		void textureViewer      ( );
		void IRESCutaway      	( );

		void showFault		( bool visibility );
		void showBorderLines    ( bool visibility );

		void setTextureViewerVisibility ( bool visibility ) { isTextureViewer_ = visibility; updateGL(); }
		void setRawModelVisibility      ( bool visibility ) { isRawModel_      = visibility; updateGL(); }
		void setIRESCutawayVisibility   ( bool visibility ) { isIRESCutaway_   = visibility; updateGL(); }
		void setIRESFullModelVisibility ( bool visibility ) { isFullModel_     = visibility; updateGL(); }

		void freezeView ( );

		void changeTimeStep        ( int step  ) { this->time_step = step; std::cout << " ... " << step << std::endl; updateGL(); };
		void changeDynamicProperty ( int index ) { reservoir_model_.loadDynamicProperties( index ); dynamic_property_index = index; updateGL(); };

	protected:
		void dragEnterEvent ( QDragEnterEvent *event );
		void dragMoveEvent  ( QDragMoveEvent *event );
		void dragLeaveEvent ( QDragLeaveEvent *event );
		void dropEvent      ( QDropEvent *event );

	signals:

        void changed(const QMimeData *mimeData = 0);

	public:
		IRES::CornerPointGrid reservoir_model_;

		float min_range;
		float max_range;
	private:

		/// Avoid camera_ movement issues
		QPoint lastPos;
		QSet<int> keysPresseds_;
		bool 	  buttonRelease_;

		Shader*                         BackGround;

		bool isIRESOpen_;

		Shader*                         BoundingBoxInitializationLCG;
		Shader*                         BoundingBoxDebugLCG;
		Shader*                         BoundingBoxCutawayLCG;

		Shader*                         secondaryLCG;
		Shader*                         primaryLCG;
		Shader* 			primaryDebugLCG;

		Shader*                         shellLCG;
		Shader*                         rawShellLCG;
		Shader* 			borderLinesLCG;

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
		bool isTextureViewer_;
		bool isRawModel_;
		bool isIRESCutaway_;
		bool isFullModel_;

		bool draw_secondary;
		bool draw_primary;

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

		bool enable_blend_;

		/// -- SCENE Stuffs ---------------------------------------------------

		/// -- Aperture of Cutaway
		float volume_width;
		float volume_height;

		int time_step;
		int dynamic_property_index;

		// Stores the freezed camera (view matrix)
		Eigen::Matrix4f freeze_viewmatrix_;
		bool freezeView_;

		Trackball * trackball_;

		float orthoZoom;

		// LCG procudure
		Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);

		// TODO NPR Stuffs - Mario's request
		GLuint xtoon_texture_;
		Mesh * picture;

		Shader * xtoon_texture_viewer;
};

#endif
