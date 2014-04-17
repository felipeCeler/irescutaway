#ifndef GLWIDGET_H
#define GLWIDGET_H

/// Ires Library
#include <ires/ires.h>
/// Celer::Libraries
	/// OpenGL
#include <GL/glew.h>
#include <IRES/CornerPointGrid.hpp>
#include <Loaders/Ply/PlyLoader.hpp>

/// LCG Library
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
                //virtual void paintEvent   ( QPaintEvent * );

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
		void gameLooping    ( );
		void fpsCounter     ( );
		float benchmark      ( ){ return benchmark_;}
		void loadShaders ( );
		void openIRES_v2 ( const std::string& filename );
		bool isIRESOpen  ( ) const;

		void changeIJK ( const int& min_i, const int& max_i, const int& min_j, const int& max_j, const int& min_k, const int& max_k );

		void changeMaxI ( const int& value);
		void changeMaxJ ( const int& value);
		void changeMaxK ( const int& value);

		void changeMinI ( const int& value);
		void changeMinJ ( const int& value);
		void changeMinK ( const int& value);

		// Draw Functions
                void drawBackGround ( ) const;
		void showFault		( bool visibility );
		void showBorderLines    ( bool visibility );

		void setPaperDemoVisibility             ( bool visibility ) { isPaperDemo_ = visibility; updateGL(); }
		void setIRESCutawayVisibility           ( bool visibility ) { isIRESCutaway_      = visibility; updateGL(); }
		void setIRESCutawayStaticVisibility     ( bool visibility ) { isIRESCutawayStatic_   = visibility; updateGL(); }
		void setIRESCutawayDynamicVisibility    ( bool visibility ) { isIRESCutawayDynamic_     = visibility; updateGL(); }

		void borderLinesSize ( int size )
		{
		        borderLinesSize_        = size;
		        updateGL();
		}

		void meanFilterSize ( int size )
		{
		        meanFilterSize_         = size;
		        updateGL();
		}

		void freezeView ( );

		// ! Raw IRES model    VIEWER F9
                        void drawIRESModel       ( );
                // ! Paper Demo        VIEWER F10

                        void loadPly ( QString pFilename )
                        {
                                std::string filename = QFile::encodeName ( pFilename ).constData ( );
                                ply_primary_.Load ( filename.c_str ( ) );

                                if ( ply_primary_.open )
                                {
                                        std::cout << "Opened succefuly " << ply_primary_.TotalConnectedPoints << std::endl;
                                }
                        }
                        void PaperPly ( ) const;
                        void PaperDemo( );
                        void PaperDrawCutawaySurface( );
                        void PaperPrimary( );
		// ! STATIC  VIEWER F11 Static Properties
                        void IRESCutawayStatic                  ( ) ;
                        void drawIRESCutawayStaticSurface       ( ) const; //
                        void drawSecondaryStatic                ( ) const; // Draw only secondary Cells
                        void drawPrimaryStatic                  ( ) const; // Draw only primary   Cells

                        void setPrimaryVisibility   ( bool );
                        void setSecondaryVisibility ( bool );

                        void changeProperty      ( const int property_index );
                        void changePropertyRange ( const double& min, const double& max, int property_index );
		// ! DYNAMIC VIEWER F12 Dynamic Properties
                        void IRESCutawayDynamic                    ( ) const;
                        void drawIRESCutawayDynamicSurface         ( ) const;
                        void drawPrimaryDynamic                    ( ) const;
                        void drawSecondaryDynamic                  ( ) const;

                        void changeTimeStep        ( const int step  )
                        {
                                // ! Debug std::cout << " ... " << step << std::endl;
                                        this->time_step = step;

                                        glBindVertexArray ( reservoir_model_.vertexArrayCuboids );

                                        glBindBuffer(GL_ARRAY_BUFFER, reservoir_model_.cuboidDynamicIds[dynamic_property_index][time_step]);
                                        glEnableVertexAttribArray(9);
                                        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, 0, 0);

                                        glBindVertexArray ( 0 );


                                        glBindVertexArray ( reservoir_model_.vertexArrayFaces );

                                        glBindBuffer(GL_ARRAY_BUFFER, reservoir_model_.faceDynamicIds[dynamic_property_index][time_step]);
                                        glEnableVertexAttribArray(6);
                                        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, 0);

                                        glBindVertexArray ( 0 );

                                updateGL();
                        };
                        void changeDynamicProperty ( int index )
                        {
                                        dynamic_property_index = index;

                                        glBindVertexArray ( reservoir_model_.vertexArrayCuboids );

                                        glBindBuffer(GL_ARRAY_BUFFER, reservoir_model_.cuboidDynamicIds[dynamic_property_index][time_step]);
                                        glEnableVertexAttribArray(9);
                                        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, 0, 0);

                                        glBindVertexArray ( 0 );

                                        glBindVertexArray ( reservoir_model_.vertexArrayFaces );

                                        glBindBuffer(GL_ARRAY_BUFFER, reservoir_model_.faceDynamicIds[dynamic_property_index][time_step]);
                                        glEnableVertexAttribArray(6);
                                        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, 0);

                                        glBindVertexArray ( 0 );

                                updateGL();
                        };



        signals:
                void fpsChanged(const QString&);
                void cutawayGenerationTime(const QString&);
                void renderingCutawayTime(const QString&);

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

		// ! DYNAMIC VIEWER F10 Static Properties

		Shader*                         IRESCutawaySurfaceStatic;
		Shader*                         IRESCutawayStaticShell_;
                Shader*                         IRESCutawayStatic_;
                Shader*                         IRESPrimaryStatic_;

                Shader*                         BurnsPrimarySetup;
                Shader*                         BurnsPrimary;
                Shader*                         BurnsPly;
		Shader*                         rawShellLCG;
		Shader*                         rawModel_;
		Shader* 			borderLinesLCG;

		// ! DYNAMIC VIEWER F12 Dynamic Properties

                Shader*                         IRESCutawaySurfaceDynamic;
                Shader*                         IRESCutawayDynamicCrust;
                Shader*                         IRESCutawayDynamicShell;
                Shader*                         IRESPrimaryDynamic;

		QImage fbo;
		float  angle;
		float  zoom_angle_;
		int    cluster;

		/// -- ANIMATION  Stuffs ----------------------------------------------
		// Celer Gaming Looping 0.1 -> For animation
		QTimer fpsTimer_;
		QTimer updateTimer_;
		QElapsedTimer delta_time;
		QTime renderTime;

		QTime cutawayGenerationTime_;
		float accumulateCutawayGenerationTime_;
		QTime renderingCutawayTime_;
		float accumulateRenderingCutawayTime_;

		float cutawayPass_;

		int fps;
		float benchmark_;
		float renderingPass;
		float accumulateRenderingTimes;
		float averageFPS;


		/// -- RENDERING Stuffs  ----------------------------------------------
		bool isPaperDemo_;
		bool isIRESCutaway_;
		bool isIRESCutawayStatic_;
		bool isIRESCutawayDynamic_;

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

		float justWireFrame;

                int borderLinesSize_;
                int meanFilterSize_;

		int time_step;
		int dynamic_property_index;

		// Stores the froze camera (view matrix)
		Eigen::Matrix4f freeze_viewmatrix_;
		bool freezeView_;

		Eigen::Affine3f position_one;
		Eigen::Affine3f position_two;

		Trackball * trackball_;

		float orthoZoom;

		// LCG procedure
		Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);

		// TODO NPR Stuffs - Mario's request
		GLuint xtoon_texture_;
		Mesh * picture;

		Shader * xtoon_texture_viewer;

		// Ply Models
                Model_PLY ply_primary_;

                Model_PLY ply_secondary_;

                GLuint vertex_box;
                GLuint vertexArray_box;

                long unsigned int videoSequence;
};


#endif
