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

		void drawPrimary ( );
		void drawSecondary (  );

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
	Celer::OpenGL::ShaderManager	textureViewer;
	Celer::OpenGL::Texture2D 	facePhoto;

	GLuint screen_buffer;
	GLuint texture_buffer;

	GLuint vertexArray;
		// Primary Attributes
		GLuint primary_vertices_buffer;
		GLuint primary_vertices_location;
		GLuint primary_normal_buffer;
		GLuint primary_normal_location;
		GLuint primary_color_buffer;
		GLuint primary_color_location;
		GLuint primary_renderFlag_buffer;
		GLuint primary_renderFlag_location;
		GLuint primary_indices_buffer;
		//Secundary Attributes
		GLuint secondary_vertices_buffer;
		GLuint secondary_vertices_location;
		GLuint secondary_normal_buffer;
		GLuint secondary_normal_location;
		GLuint secondary_color_buffer;
		GLuint secondary_color_location;
		GLuint secondary_renderFlag_buffer;
		GLuint secondary_renderFlag_location;
		GLuint secondary_indices_buffer;

	bool draw_secondary;
	bool draw_primary;

	bool ires_has_been_open_sucessefully;

	std::vector<int> 			secondary_list_of_indices;
	std::vector<Celer::Vector3<GLdouble> > 	secondary_list_of_normals;
	std::vector<Celer::Vector3<GLdouble> > 	secondary_list_of_vertices;
	std::vector<Celer::Vector4<GLfloat> >   secondary_list_of_colors;
	std::vector<GLfloat>    		secondary_list_of_renderFlag;


	std::vector<int> 			primary_list_of_indices;
	std::vector<Celer::Vector3<GLdouble> > 	primary_list_of_normals;
	std::vector<Celer::Vector3<GLdouble> > 	primary_list_of_vertices;
	std::vector<Celer::Vector4<GLfloat> >   primary_list_of_colors;
	std::vector<GLfloat>    		primary_list_of_renderFlag;


	Celer::BoundingBox3<double> 		box;

	// From Image Cutaway.

	float theta;

	float pm_sz;

	QGLFramebufferObject*   fboInitialization;
	Celer::OpenGL::ShaderManager	jumpFloodInitialization;

	QGLFramebufferObject*   fboStep[2];
	Celer::OpenGL::ShaderManager	jumpFloodingStep;


	QGLShaderProgram *JumpFloodingStep;
	QGLShader *VertexShaderStep, *FragmentShaderStep;


	Celer::OpenGL::ShaderManager	primary;
	Celer::OpenGL::ShaderManager	secondary;
	Celer::OpenGL::ShaderManager	cutaway;
	Celer::OpenGL::ShaderManager	cutawayWireframe;

	QImage fbo;
	float angle;
	float zoom_angle_;

};

#endif
