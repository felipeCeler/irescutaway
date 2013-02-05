#include <QtGui/QtGui>

#include <limits>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>

#include <GUI/Qt/GLWidget/GLWidget.hpp>
// Se quiser usar QPainter Ver exemplo no QT demo - Manda Qt em wave animation !!!


GLWidget::GLWidget ( const QGLFormat& format , QWidget* parent , const QGLWidget* shareWidget , Qt::WindowFlags f ) :
	QGLWidget ( format , parent , shareWidget , f )
{

}

GLWidget::GLWidget (  QWidget* parent , const QGLWidget* shareWidget , Qt::WindowFlags f ) :
	QGLWidget ( parent , shareWidget , f )
{

}

/// OpenGL
void GLWidget::initializeGL ( )
{
	/// Celer OpenGL
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File GLWidget.cpp line 29" );
	/// Celer OpenGL


	buttonRelease_ = false;

	/// OpenGL Stuffs
	glEnable ( GL_DEPTH_TEST );
	glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
	glEnable(GL_TEXTURE_2D);

	timer_.setSingleShot ( false );
	connect ( &timer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( gameLooping ( ) ) );
	connect ( &timer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( animate ( ) ) );
	timer_.start ( 0 );


	setMinimumSize ( 640 , 480 );
	setSizePolicy ( QSizePolicy::MinimumExpanding , QSizePolicy::MinimumExpanding );
	/// Key event to GLWidget not o MainWindow ! | @QtDocumentation
	setFocus ( );
	/// If mouse tracking is enabled, the widget receives mouse move events even if no buttons are pressed. | @QtDocmentation
	setMouseTracking ( true );
	setFocusPolicy ( Qt::StrongFocus );
	setAttribute ( Qt::WA_NoSystemBackground );
	setAutoFillBackground ( false );
	setAcceptDrops(true);
	

	LoadShaders ( );

	cube_.creatBuffers ( );

	camera_.setPosition ( cube_.box.center ( ) );
	camera_.setOffset ( 3.0f * cube_.box.diagonal ( ) );

	camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );
	cameraStep_ = 0.01f;

}

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );
	camera_.setWindowSize ( width , height );


	camera_.setAspectRatio ( width  , height  );
	camera_.setPerspectiveProjectionMatrix ( 60 , camera_.aspectRatio ( ) , 100.0 , 10.0*cube_.box.diagonal() );

	centerX_ = static_cast<float> ( width * 0.5 );
	centerY_ = static_cast<float> ( height * 0.5 );
}

/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.

/// Real Looping
void GLWidget::paintGL ( )
{
	TridimensionalSetUp ( );
}

void GLWidget::TridimensionalSetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	camera_.setViewByMouse ( );

	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}

	camera_.computerViewMatrix( );


	glActiveTexture(GL_TEXTURE0);

 	if ( cube_.isCreated ( ) )
	{

		manager.active ( );

		glUniform3fv( manager.uniforms_["lightDirection"].location,0,camera_.position());

		glUniformMatrix4fv ( manager.uniforms_["modelViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
		glUniformMatrix4fv ( manager.uniforms_["projectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
		
		cube_.draw ( );

		// 1rst attribute buffer : vertices

//		glBindVertexArray(vertexArray);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
//		// Draw the triangle !
//		glDrawElements(GL_TRIANGLES, 36 , GL_UNSIGNED_INT, 0);
//
//		glBindVertexArray(0);
		
		manager.deactive ( );
		
	}

}

void GLWidget::TwodimensionalSetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void GLWidget::gameLooping ( )
{
	updateGL ( );
}

void GLWidget::animate ( )
{

}

void GLWidget::LoadShaders ( )
{
	qDebug ( ) << "ATIM !!";
	QDir shadersDir = QDir ( qApp->applicationDirPath ( ) );

	qDebug () << "Directory " << shadersDir.path ();
	shadersDir.cdUp ();
	qDebug () << "Directory " << shadersDir.path ();
	manager.create((shadersDir.path ()+"/share/Shaders/Primary.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/Primary.frag").toStdString());
}



/// KeyInput
void GLWidget::processMultiKeys ( )
{
foreach( int key , keysPresseds_)
{
	if ( key == Qt::Key_Q )
	{
		camera_.moveUpward ( cameraStep_ );

	}
	if ( key == Qt::Key_E )
	{
		camera_.moveUpward ( -cameraStep_ );

	}
	else if ( key == Qt::Key_W )
	{
		camera_.moveForward ( cameraStep_ );

	}
	if ( key == Qt::Key_S )
	{
		camera_.moveForward ( -cameraStep_ );

	}
	if ( key == Qt::Key_A )
	{
		camera_.strafeRight ( -cameraStep_ );

	}
	if ( key == Qt::Key_D )
	{
		camera_.strafeRight ( cameraStep_ );

	}
	if ( key == Qt::Key_R )
	{

		camera_.reset ( );

	}
	if ( key == Qt::Key_Plus )
	{

		if ( cameraStep_ < 2.0 )
		cameraStep_ += 0.01;

	}
	if ( key == Qt::Key_Minus )
	{

		if ( cameraStep_ > 0.0 )
		cameraStep_ -= 0.01;
	}
}
}

void GLWidget::keyPressEvent ( QKeyEvent * event )
{
	buttonRelease_ = true;
	keysPresseds_ += event->key ( );
}

void GLWidget::keyReleaseEvent ( QKeyEvent * event )
{
	buttonRelease_ = false;
	keysPresseds_ -= event->key ( );
}
/// MouseInput
void GLWidget::mousePressEvent ( QMouseEvent *event )
{
	event->accept ( );

		if ( event->button ( ) == Qt::LeftButton )
		{
			camera_.lockMouse ( true );
			centerX_ = static_cast<float> ( event->x ( ) );
			centerY_ = static_cast<float> ( event->y ( ) );
		}

		if ( event->button ( ) == Qt::RightButton )
		{
			lastPos = event->pos ( );
		}

}

void GLWidget::mouseReleaseEvent ( QMouseEvent *event )
{
	event->accept ( );

	if ( event->button ( ) == Qt::LeftButton )
	{
		camera_.lockMouse ( false );

	}
	else if ( event->button ( ) == Qt::RightButton )
	{

	}

}

void GLWidget::mouseMoveEvent ( QMouseEvent *event )
{

	if ( event->buttons ( ) & Qt::LeftButton )
	{
		//camera_.SetMouseInfo(event->x(),event->y());
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		pitch = - ( static_cast<float> ( event->x ( ) ) - centerX_ ) * 0.2;
		heading = - ( static_cast<float> ( event->y ( ) ) - centerY_ ) * 0.2;

		camera_.rotate ( pitch , heading , roll );

		/*! Três coisas :
		 *  - event->pos() retorna coordenadas x e y relativa a widget que recebeu o evento.
		 *  - mapToGlobla mapei as coordenadas da widget para coordenada global da tela.
		 *  - QCurso::setPos() posiciona o mouse em coordenada global.
		 *
		 *  Tudo o que eu queria para implementar a First Person Camera !
		 */
		QCursor::setPos ( mapToGlobal ( QPoint ( static_cast<int> ( centerX_ ) , static_cast<int> ( centerY_ ) ) ) );
	}

}

void GLWidget::wheelEvent ( QWheelEvent *event )
{
	event->accept ( );
	scrollStep_ += event->delta ( ) / 120.0;
}

void GLWidget::dragEnterEvent(QDragEnterEvent *event)
{

	setBackgroundRole(QPalette::Highlight);

	event->acceptProposedAction();

	emit changed(event->mimeData());

	qDebug() << "Entrou";
}
/// Then, we invoke acceptProposedAction() on event, setting the drop action to the one proposed.
/// Lastly, we emit the changed() signal, with the data that was dropped and its MIME type information as a parameter.
/// For dragMoveEvent(), we just accept the proposed QDragMoveEvent object, event, with acceptProposedAction().
void GLWidget::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}
///The DropArea class's implementation of dropEvent() extracts the event's mime data and displays it accordingly.
void GLWidget::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	/// The mimeData object can contain one of the following objects: an image, HTML text, plain text, or a list of URLs.

	if (event->mimeData()->hasUrls())
	{

		QList<QUrl> urlList = mimeData->urls();
		QString text;
		for (int i = 0; i < urlList.size() && i < 32; ++i) {
			QString url = urlList.at(i).path();
			text += url;// + QString("\n");
		}
		qDebug() << text;

		QImage image 			= QImage(text);
		QImage imageOpenGL = QGLWidget::convertToGLFormat( image );


		facePhoto.create(image.width(), image.height() );

		qDebug() << " ID Texture " << facePhoto.id() << " w x h " << image.width() << " x " << image.height();

		facePhoto.loadFromRawData(imageOpenGL.bits());

		facePhoto.bind();

		qDebug() << "Soltou";

	}


}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
		event->accept ( );
}

