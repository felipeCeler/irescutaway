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

#include <QtGui/QMdiSubWindow>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QMimeData>


#include <IRESClasses/CornerPointGrid_test.cpp>


class GLWidget: public QGLWidget
{
		Q_OBJECT

	public:
		// FIXME - Putting this as public, but I'm going to search for a better sulution to how feed other
		//	   widget with data form another one.
		Ires ires_cornerPoint_test_;
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
		void TridimensionalSetUp ( );
		void TwodimensionalSetUp ( );
		void cutawaySetup ( );
		void LoadShaders ( );
		void openIRES ( const std::string& filename );
		bool isIresWasOpenedSucessufully () const;
		void changeProperty ( int property_index );
		void changePropertyRange ( const double& min, const double& max, int property_index );

	protected:
		void dragEnterEvent ( QDragEnterEvent *event );
		void dragMoveEvent ( QDragMoveEvent *event );
		void dragLeaveEvent ( QDragLeaveEvent *event );
		void dropEvent ( QDropEvent *event );

	signals:

        void changed(const QMimeData *mimeData = 0);


private:

	// About Scene
	Celer::Camera<float> camera_;
	float cameraStep_;
	float scrollStep_;
	float centerX_;
	float centerY_;

	QPoint lastPos;
	/// Avoid camera_ movement issues
	QSet<int> keysPresseds_;
	bool buttonRelease_;
	/// Celer Gaming Looping 0.1
	QTimer timer_;

	/// Celer OpenGL Frameworks
	Celer::OpenGL::ShaderManager	manager;
	Celer::OpenGL::Texture2D 	facePhoto;

	Cube  cube_;

	GLuint vertexArray;
	GLuint vertices_buffer;
	GLuint normal_buffer;
	GLuint indices_buffer;
	GLuint color_buffer;
	GLuint screen_buffer;
	GLuint texture_buffer;

	bool ires_has_been_open_sucessefully;

	std::vector<int> 			list_of_indices;
	std::vector<Celer::Vector3<GLdouble> > 	list_of_normals;
	std::vector<Celer::Vector3<GLdouble> > 	list_of_vertices;
	std::vector<Celer::Vector4<GLfloat> >   list_of_colors;
	Celer::BoundingBox3<double> 		box;


	// From Image Cutaway.

	QGLFramebufferObject*   fboInitialization;
	Celer::OpenGL::ShaderManager	jumpFloodInitialization;

	QGLFramebufferObject*   fboStep[2];
	Celer::OpenGL::ShaderManager	jumpFloodingStep;


	GLuint primary_vertexArray;
	Celer::OpenGL::ShaderManager	primary;
	GLuint secundary_vertexArray;
	Celer::OpenGL::ShaderManager	secundary;

	QImage fbo;
	float angle;

};

#endif
