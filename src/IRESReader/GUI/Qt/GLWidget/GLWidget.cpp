#include <QtGui/QtGui>

#include <limits>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iterator>

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

	ires_cornerPoint_test_.openIRES( "/media/d/Workspace/IRESReader/Files/Ires/emerald.ires" );


	std::cout << "Vertices       :" << ires_cornerPoint_test_.vertices.size ( ) << std::endl;
	std::cout << "Blocks Indices :" << ires_cornerPoint_test_.blocks.size ( )  << std::endl;


	for (int i = 0 ; i < 8 ; i++)
	{
		std::cout << " xx id " << ires_cornerPoint_test_.blocks[i*4]   << " " << ires_cornerPoint_test_.vertices[i*4] ;
		std::cout << " id " << ires_cornerPoint_test_.blocks[i*4+1] << " " << ires_cornerPoint_test_.vertices[i*4+1] ;
		std::cout << " id " << ires_cornerPoint_test_.blocks[i*4+2] << " " << ires_cornerPoint_test_.vertices[i*4+2] ;
		std::cout << " id " << ires_cornerPoint_test_.blocks[i*4+3] << " " << ires_cornerPoint_test_.vertices[i*4+3] ;
	}

	for ( int i = 0; i < ires_cornerPoint_test_.blocks.size( ); i+=8 )
	{
		int index [] =
		{
		    // Top Face
		    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+2],/* 0 - 5*/
		    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+0],
		    // Bottom Face
		    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+6],/* 6 - 11 */
		    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+4],
		    // Front Face
		    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+7],/* 12 - 17*/
		    ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],
		    // Back Face
		    ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+5],/* 18 - 23*/
		    ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+1],
		    // Right Face
		    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+6],/*24 - 29*/
		    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+0],
		    // Left Face
		    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],/*30 - 35*/
		    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+2],
		};


		Celer::Vector3<double> vertices [] =
		{
		    // Top Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[0]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[1]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[2]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[3]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[4]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[5]]),
		    // Bottom Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[6]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[7]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[8]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[9]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[10]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[11]]),
		    // Bottom Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[12]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[13]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[14]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[15]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[16]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[17]]),
		    // Bottom Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[18]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[19]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[20]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[21]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[22]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[23]]),
		    // Bottom Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[24]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[25]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[26]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[27]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[28]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[29]]),
		    // Bottom Face
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[30]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[31]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[32]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[33]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[34]]),
		    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[35]])

	       };


		Celer::Vector3<double> top_face_normal 	  = ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[1]] ) ^ ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[2]] );
		Celer::Vector3<double> bottom_face_normal = ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[7]] ) ^ ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[8]] );
		Celer::Vector3<double> front_face_normal  = ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[13]] ) ^ ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[14]] );
		Celer::Vector3<double> back_face_normal   = ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[19]] ) ^ ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[20]] );
		Celer::Vector3<double> right_face_normal  = ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[25]] ) ^ ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[26]] );
		Celer::Vector3<double> left_face_normal   = ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[31]] ) ^ ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[32]] );

		Celer::Vector3<double> normals [] =
		{
		    // Top Face
	            top_face_normal,
	            top_face_normal,
	            top_face_normal,
	            top_face_normal,
	            top_face_normal,
	            top_face_normal,
		    // Bottom Face
	            bottom_face_normal,
	            bottom_face_normal,
	            bottom_face_normal,
	            bottom_face_normal,
	            bottom_face_normal,
	            bottom_face_normal,
		    // Front Face
	            front_face_normal,
	            front_face_normal,
	            front_face_normal,
	            front_face_normal,
	            front_face_normal,
	            front_face_normal,
		    // Back Face
	            back_face_normal,
	            back_face_normal,
	            back_face_normal,
	            back_face_normal,
	            back_face_normal,
	            back_face_normal,
		    // Right Face
	            right_face_normal,
	            right_face_normal,
	            right_face_normal,
	            right_face_normal,
	            right_face_normal,
	            right_face_normal,
		    // Left Face
	            left_face_normal,
	            left_face_normal,
	            left_face_normal,
	            left_face_normal,
	            left_face_normal,
	            left_face_normal

	       };

		std::copy ( index , index + 36 , std::back_inserter ( list_of_indices ) );
		std::copy ( vertices , vertices + 36 , std::back_inserter ( list_of_vertices ) );
		std::copy ( normals , normals + 36 , std::back_inserter ( list_of_normals ) );

	}  // end of looping

	for (int i = 0 ; i < 36 ; i++)
	{
		std::cout << " id " << list_of_indices[i]   << " " << list_of_vertices[i] ;

	}

	glGenVertexArrays ( 1 , &vertexArray);
	glBindVertexArray(vertexArray);

		/// Requesting Vertex Buffers to the GPU
//		glGenBuffers ( 1 , &vertices_buffer );
//			glBindBuffer ( GL_ARRAY_BUFFER , vertices_buffer );
//			glBufferData ( GL_ARRAY_BUFFER , list_of_vertices.size( ) * sizeof(list_of_vertices[0]) , &list_of_vertices , GL_STATIC_DRAW );

		glGenBuffers ( 1 , &vertices_buffer );
			glBindBuffer ( GL_ARRAY_BUFFER , vertices_buffer );
			glBufferData ( GL_ARRAY_BUFFER , ires_cornerPoint_test_.vertices.size( ) * sizeof(ires_cornerPoint_test_.vertices[0]) , &ires_cornerPoint_test_.vertices[0] , GL_STATIC_DRAW );

		/// Requesting Vertex Buffers to the GPU
		glGenBuffers ( 1 , &normal_buffer );
			glBindBuffer ( GL_ARRAY_BUFFER , normal_buffer );
			glBufferData ( GL_ARRAY_BUFFER , list_of_normals.size( ) * sizeof(list_of_normals[0]) , &list_of_normals[0] , GL_STATIC_DRAW );

		/// Requesting Indices
		glGenBuffers ( 1 , &indices_buffer);
			glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, indices_buffer );
			glBufferData ( GL_ELEMENT_ARRAY_BUFFER , list_of_indices.size() * sizeof(list_of_indices[0]) , &list_of_indices[0] , GL_STATIC_DRAW );

	// Set up generic attributes pointers
	glBindBuffer ( GL_ARRAY_BUFFER, vertices_buffer );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);


	glBindVertexArray(0);


	cube_.creatBuffers ();

	std::cout << " Size: " << list_of_vertices.size() << std::endl;

	box.fromPointCloud( ires_cornerPoint_test_.vertices.begin(),ires_cornerPoint_test_.vertices.begin()+8 );

	camera_.setPosition ( box.center ( ) );
	camera_.setTarget ( box.center ( ) );
	std::cout << box.diagonal ( );
	camera_.setOffset ( 1.0 * box.diagonal ( ) );

	std::cout  << camera_.position();

	//camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );

	cameraStep_ = 100.0f;

}

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );
	camera_.setWindowSize ( width , height );


	camera_.setAspectRatio ( width  , height  );
	camera_.setPerspectiveProjectionMatrix ( 60 , camera_.aspectRatio ( ) , 1.0 , 1000.0*cube_.box.diagonal() );

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
		
//		cube_.draw ( );

		// 1rst attribute buffer : vertices

		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
//		// Draw the triangle !
		glDrawElements(GL_TRIANGLES, 12 , GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		

//		glBindVertexArray (vertexArray);			//VAO
//			//Vertices:
//			glEnableVertexAttribArray ( 0 );
//			glBindBuffer ( GL_ARRAY_BUFFER , vertices_buffer );
//			glVertexAttribPointer ( 0 , 3 , GL_DOUBLE , GL_FALSE , 0 , NULL );
//
//			glDrawArrays( GL_TRIANGLES , 0, 12);
//
//		glDisableVertexAttribArray ( 0 );
//		glBindVertexArray ( 0 );
//		glBindBuffer ( GL_ARRAY_BUFFER , 0 );


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

	//std::cout  << camera_.position();

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

