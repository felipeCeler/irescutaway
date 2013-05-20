#ifndef MYGLFRAME_H
#define MYGLFRAME_H


///
#include "Cube.hpp"
/// Celer::Libraries
	/// OpenGL
#include <Celer/OpenGLFramework/OpenGLContext.hpp>
#include <Celer/OpenGLFramework/GLTexture.hpp>
#include <Celer/OpenGLFramework/Shader.hpp>
#include <Celer/OpenGLFramework/ShaderManager.hpp>
	/// Math

#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Vector4.hpp>
	/// Scene
#include <Celer/Scene/Camera.hpp>

#include <QtOpenGL/QGLWidget>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>

// Standard Library
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>


#include <QtGui/QMdiSubWindow>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QMimeData>


#include <IRES/CornerPointGrid.hpp>



class GLWidget: public QGLWidget
{
		Q_OBJECT


	public:
		// FIXME - Putting this as public, but I'm going to search for a better sulution to how feed other
		//	   widget with data form another one.

		IRES::CornerPointGrid reservoir_model_;
		// From QGLWidget
		explicit GLWidget ( const QGLFormat& format , QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		explicit GLWidget ( QWidget* parent = 0 , const QGLWidget* shareWidget = 0 , Qt::WindowFlags f = 0 );
		void initializeGL ( );
		void resizeGL ( int width , int height );

		void paintGL ( );
		void processMultiKeys ( );
		void mousePressEvent ( QMouseEvent *event );
		void mouseMoveEvent ( QMouseEvent *event );
		void mouseReleaseEvent ( QMouseEvent *event );
		void wheelEvent ( QWheelEvent *e );
		void keyPressEvent ( QKeyEvent * event );
		void keyReleaseEvent ( QKeyEvent * event );

	public slots:

		void gameLooping ( );
		void animate ( );
		void LoadShaders ( );
		void CutVolumeGenerator ( );
		void openIRES ( const std::string& filename );
		void openIRES2 ( const std::string& filename );
		bool isIresWasOpenedSucessufully () const;
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


		void NoCutawaySetUp ( );
		void BurnsCutawaySetup ( );
		void BoundingVolumeCutawaySetup ( );

		void setNoCutawayVisibility     ( bool visibility ) { isNoCutaway 	   = visibility; updateGL(); }
		void setBoundingBoxVisibility   ( bool visibility ) { isBoudingBoxApproach = visibility; updateGL();}
		void setBurnsApproachVisibility ( bool visibility ) { isBurnsApproach      = visibility; updateGL();}



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

	Cube cube_;

	Celer::OpenGL::ShaderManager    wireframe;
	Celer::OpenGL::ShaderManager    orientedBoxApproach;

	QPoint lastPos;
	/// Avoid camera_ movement issues
	QSet<int> keysPresseds_;
	bool buttonRelease_;
	/// Celer Gaming Looping 0.1
	QTimer timer_;

	/// Celer OpenGL Frameworks
	Celer::OpenGL::ShaderManager	textureViewer;
	Celer::OpenGL::Texture2D 	facePhoto;


	GLuint vertexArray;
	GLuint vertexArrayScreen;
	GLuint screen_buffer;
	GLuint texture_buffer;

	// Reservoir Attributes on GPU
	GLuint reservoir_vertices_buffer;
	GLuint reservoir_vertices_location;
	GLuint reservoir_normal_buffer;
	GLuint reservoir_normal_location;
	GLuint reservoir_color_buffer;
	GLuint reservoir_color_location;
	GLuint reservoir_renderFlag_buffer;
	GLuint reservoir_renderFlag_location;
	GLuint reservoir_IJK_buffer;
	GLuint reservoir_IJK_location;

	GLuint reservoir_indices_buffer;


	bool ires_has_been_open_sucessefully;

	std::vector<int> 			reservoir_list_of_indices;
	std::vector<Celer::Vector4<GLfloat> > 	reservoir_list_of_normals;
	std::vector<Celer::Vector4<GLfloat> > 	reservoir_list_of_vertices;
	std::vector<Celer::Vector4<GLfloat> >   reservoir_list_of_colors;
	std::vector<Celer::Vector4<GLint> >     reservoir_list_of_IJKs;
	std::vector<Celer::Vector4<GLfloat> >   reservoir_list_of_renderFlag;


	Celer::BoundingBox3<double> 		   box;
	std::list  <Celer::BoundingBox3<GLfloat> > boxes;
	std::vector<Celer::BoundingBox3<GLfloat> > cutVolumes;
	std::vector<Celer::Vector4<GLfloat> >      box_vertices;

	GLuint vertexArray_box;
	GLuint vertexBuffer_box;

	// From Image Cutaway.

	float theta;

	float pm_sz;

	QGLFramebufferObject*   fboStep[2];

	// Burns Approach
	Celer::OpenGL::ShaderManager	BurnsJFAInitializing430;
	Celer::OpenGL::ShaderManager	BurnsJFAStep430;
	Celer::OpenGL::ShaderManager	BurnsCutaway430;
	Celer::OpenGL::ShaderManager	BurnsCutaway430Wireframe;


	// BoudingBox Visualization
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

	bool  isBurnsApproach;
	bool  isBoudingBoxApproach;
	bool  isNoCutaway;

	bool draw_secondary;
	bool draw_primary;

	int max_I_;
	int min_I_;
	int max_J_;
	int min_J_;
	int max_K_;
	int min_K_;

};

#endif
