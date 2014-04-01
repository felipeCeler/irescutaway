#include <QtGui/QtGui>

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
	/// GLEW OpenGL
	//Glew Initialization:

	GLenum glewInitResult = glewInit();

	//Check Glew Initialization:
	if (GLEW_OK != glewInitResult) {
		cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
		exit(EXIT_FAILURE);
	}
	/// GLEW OpenGL

	buttonRelease_ = false;

	/// OpenGL Stuffs
	glEnable ( GL_DEPTH_TEST );
	glDepthMask(GL_TRUE);

	glEnable ( GL_TEXTURE_2D );

	glEnable(GL_LINE_SMOOTH);
	glEnable ( GL_MULTISAMPLE );

	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);

	glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
	glDisable(GL_BLEND);

	setMinimumSize ( 800 , 800 );
	setSizePolicy ( QSizePolicy::MinimumExpanding , QSizePolicy::MinimumExpanding );
	/// Key event to GLWidget not o MainWindow ! | @QtDocumentation
	setFocus ( );
	/// If mouse tracking is enabled, the widget receives mouse move events even if no buttons are pressed. | @QtDocmentation
	setMouseTracking ( true );
	setFocusPolicy ( Qt::StrongFocus );
	setAttribute ( Qt::WA_NoSystemBackground );
	setAutoFillBackground ( false );
	setAcceptDrops(true);

	isIRESOpen_ = 0;
	time_step   = 0;

	zoom_angle_ = 45.0f;
	orthoZoom   = 1.0f;

	lights.push_back ( Eigen::Vector3f ( 0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f (-0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 0.0 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 1.0 , 0.0 ) );


	draw_secondary = 1;
	draw_primary = 0;

	isRawModel_	      = 1;
	isIRESCutaway_        = 0;
	isTextureViewer_      = 0;
	isFullModel_	      = 0;

	cluster = 0;

	//Timer Animation

	fps = 0;

	renderingPass = 0.0f;
	accumulateRenderingTimes = 0.0f;

	delta_time.start();

	updateTimer_.setSingleShot ( false );
	connect ( &updateTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( gameLooping ( ) ) );
	updateTimer_.start( 0 );

	fpsTimer_.setSingleShot ( false );
	connect ( &fpsTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( fpsCounter( ) ) );
	fpsTimer_.start ( 1000 );

	freezeView_ = 0;

	volume_width = 0.0f;
	volume_height = 0.0f;

        borderLinesSize_ = 0;
        meanFilterSize_  = 0;

	trackball_ = new Trackball();

	trackball_->initOpenGLMatrices();

	trackball_->initializeBuffers();

	enable_blend_ = 0;

	depthFBO = new Framebuffer( width() , height(), 2 );

	meanFilter = new MeanFilter( "Gaussian blur");
	meanFilter->resize(width(), height());

	loadShaders ( );

	// TODO xtoon
	picture = new Mesh( );
	picture->createQuad( );

	reservoir_model_.createBuffers( );

	dynamic_property_index = 0;

	// BenchMarking

	trackball_->decreaseZoom( 5.05 );

	glGenVertexArrays (1, &vertexArray_box);
	glGenBuffers(1,&vertex_box);

	std::vector<Celer::Vector4<float> > vertices(8,Celer::Vector4<float>(0.0,0.0,0.0,1.0));


	glBindVertexArray(vertexArray_box);
	        glBindBuffer(GL_ARRAY_BUFFER,vertex_box);
	        glBufferData ( GL_ARRAY_BUFFER , vertices.size( ) * sizeof(vertices[0]) , &vertices[0] , GL_STATIC_DRAW );

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);
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



		// TODO this is just the beginning
		// Create a texture as attachment
		if ( xtoon_texture_ != 0 )
		{
			glDeleteTextures ( 1 , &xtoon_texture_ );
		}
		else
		{
			glGenTextures ( 1 , &xtoon_texture_ );
		}

		glBindTexture ( GL_TEXTURE_2D, xtoon_texture_ );
		// Set basic parameters
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST );
		// Allocate memory
		glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGBA , image.width() , image.height() , 0 , GL_RGBA , GL_UNSIGNED_BYTE , imageOpenGL.bits() );

		qDebug() << " ID Texture " << xtoon_texture_ << " w x h " << image.width() << " x " << image.height();

		glBindTexture ( GL_TEXTURE_2D, 0 );

		qDebug() << "Soltou";

	}

}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
		event->accept ( );
}

bool GLWidget::isIRESOpen ( ) const
{
	return isIRESOpen_;
}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{

	min_range = minRange;
	max_range = maxRange;

	reservoir_model_.current_static = property_index;

    updateGL();

}

void GLWidget::changeProperty ( int property_index )
{

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = reservoir_model_.static_min[property_index];
	float max = reservoir_model_.static_max[property_index];

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	reservoir_model_.current_static = property_index;

}
void GLWidget::changeIJK ( const int& min_i, const int& max_i,
                           const int& min_j, const int& max_j,
                           const int& min_k, const int& max_k )
{
	min_I_ = min_i;
	max_I_ = max_i;
	min_J_ = min_j;
	max_J_ = max_j;
	min_K_ = min_k;
	max_K_ = max_k;

	std::cout << " min_I " << min_I_ << std::endl;
	std::cout << " max_I " << max_I_ << std::endl;
	std::cout << " min_J " << min_J_ << std::endl;
	std::cout << " max_J " << max_J_ << std::endl;
	std::cout << " min_K " << min_K_ << std::endl;
	std::cout << " max_K " << max_K_ << std::endl;

}

void GLWidget::changeMaxI ( const int& value )
{
	max_I_ = value;
	std::cout << " min_I " << min_I_ << std::endl;
	std::cout << " max_I " << max_I_ << std::endl;
	updateGL();
}

void GLWidget::changeMaxJ ( const int& value )
{
	max_J_ = value;
	std::cout << " min_J " << min_J_ << std::endl;
	std::cout << " max_J " << max_J_ << std::endl;
	updateGL();
}

void GLWidget::changeMaxK ( const int& value )
{
	max_K_ = value;
	std::cout << " min_K " << min_K_ << std::endl;
	std::cout << " max_K " << max_K_ << std::endl;
	updateGL();
}

void GLWidget::changeMinI ( const int& value )
{
	min_I_ = value;
	std::cout << " min_I " << min_I_ << std::endl;
	std::cout << " max_I " << max_I_ << std::endl;
	updateGL();
}

void GLWidget::changeMinJ ( const int& value )
{
	min_J_ = value;
	std::cout << " min_J " << min_J_ << std::endl;
	std::cout << " max_J " << max_J_ << std::endl;
	updateGL();
}

void GLWidget::changeMinK ( const int& value )
{
	min_K_ = value;
	std::cout << " min_K " << min_K_ << std::endl;
	std::cout << " max_K " << max_K_ << std::endl;
	updateGL();
}

void GLWidget::openIRES_v2( const std::string& filename )
{

	reservoir_model_.openIRES( filename );

	if ( reservoir_model_.isOpen( ) )
	{
		isIRESOpen_ = 1;
	}
	else
	{
	        isIRESOpen_ = 0;
	}

}

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );

	trackball_->useOrthographicMatrix ( -1.0f , 1.0 , -1.0f , 1.0 , 0.1 , 500.0 );
	//trackball_->usePerspectiveMatrix  ( 60.0f , (float) width / float ( height ) , 0.1 , 500 );

	if (depthFBO)
		delete depthFBO;


	depthFBO = new Framebuffer( width , height, 2 );

	meanFilter->resize(width, height);
}
/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.
/// Real Looping

void GLWidget::drawCutawaySurface ( )
{

	glDepthFunc ( GL_GREATER );
	glClearDepth ( 0.0 );

	depthFBO->bind( );

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	BoundingBoxInitializationLCG->enable( );

	BoundingBoxInitializationLCG->setUniform("min_range", min_range  );
	BoundingBoxInitializationLCG->setUniform("max_range", max_range  );
	BoundingBoxInitializationLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	BoundingBoxInitializationLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	BoundingBoxInitializationLCG->setUniform("property_index", reservoir_model_.current_static );

	BoundingBoxInitializationLCG->setUniform( "x" , volume_width );
	BoundingBoxInitializationLCG->setUniform( "y" , volume_height );
	BoundingBoxInitializationLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxInitializationLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxInitializationLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	BoundingBoxInitializationLCG->setUniform ("freeze", freezeView_ );
	BoundingBoxInitializationLCG->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

	reservoir_model_.drawCuboid ( );

	BoundingBoxInitializationLCG->disable( );

	glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

	meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

	depthFBO->unbindAll();


	glDrawBuffer(GL_BACK);

}

void GLWidget::drawSecondary ( )
{
	GLfloat light_elements[12];
	for ( std::size_t i = 0; i < lights.size ( ); ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			light_elements[i * 3 + j] = lights[i][j];
		}
	}

	shellLCG->enable( );

	shellLCG->setUniform ( "normals" , depthFBO->bindAttachment(1) );

	shellLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	shellLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	shellLCG->setUniform("property_index", reservoir_model_.current_static );
	shellLCG->setUniform("faults", reservoir_model_.showFault );

	shellLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	shellLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	shellLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	shellLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	shellLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	shellLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	reservoir_model_.drawFace();

	shellLCG->disable( );

	if ( enable_blend_ )
	{
		glDisable(GL_BLEND);
	}

	depthFBO->unbindAttachments();

	BoundingBoxCutawayLCG->enable( );

	BoundingBoxCutawayLCG->setUniform( "normals" , depthFBO->bindAttachment(1) );

	BoundingBoxCutawayLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	BoundingBoxCutawayLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	BoundingBoxCutawayLCG->setUniform("property_index", reservoir_model_.current_static );
//	BoundingBoxCutawayLCG->setUniform("min_property", reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step]  );
//	BoundingBoxCutawayLCG->setUniform("max_property", reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step]  );
	BoundingBoxCutawayLCG->setUniform("property_index", reservoir_model_.current_static );
	BoundingBoxCutawayLCG->setUniform("time_step", time_step );

	BoundingBoxCutawayLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	BoundingBoxCutawayLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	BoundingBoxCutawayLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	BoundingBoxCutawayLCG->setUniform( "min_IJK" , min_I_ , min_J_ , min_K_ );
	BoundingBoxCutawayLCG->setUniform( "max_IJK" , max_I_ , max_J_ , max_K_ );

	BoundingBoxCutawayLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxCutawayLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxCutawayLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	reservoir_model_.drawCuboid ( );

	BoundingBoxCutawayLCG->enable( );

	depthFBO->unbindAttachments();

	if ( reservoir_model_.showBorderLine )
	{

	        glLineWidth( (float) borderLinesSize_ );
		borderLinesLCG->enable ( );

		borderLinesLCG->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
		borderLinesLCG->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
		borderLinesLCG->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

		reservoir_model_.drawFace ( );

		borderLinesLCG->disable ( );
	}
}

void GLWidget::drawPrimaryBoudingBox ( )
{

}

void GLWidget::drawPrimary( )
{

	GLfloat light_elements[12];
	for ( std::size_t i = 0; i < lights.size ( ); ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			light_elements[i * 3 + j] = lights[i][j];
		}
	}

	primaryLCG->enable( );

	primaryLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	primaryLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	primaryLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	primaryLCG->setUniform("min_range", min_range  );
	primaryLCG->setUniform("max_range", max_range  );

	primaryLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	primaryLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	primaryLCG->setUniform("property_index", reservoir_model_.current_static );

	primaryLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	primaryLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	primaryLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	reservoir_model_.drawCuboid ( );

	primaryLCG->disable( );


}

void GLWidget::drawFullModel ( )
{
	GLfloat light_elements[12];
	for ( std::size_t i = 0; i < lights.size ( ); ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			light_elements[i * 3 + j] = lights[i][j];
		}
	}

	glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	drawBackGround( );


        glBindVertexArray ( reservoir_model_.vertexArrayCuboids );

        glBindBuffer(GL_ARRAY_BUFFER, reservoir_model_.cuboidDynamicIds[dynamic_property_index][time_step]);
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray ( 0 );

	secondaryLCG->enable( );

//	secondaryLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//	secondaryLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//	secondaryLCG->setUniform("property_index", reservoir_model_.current_static );

	std::cout << "--- " << 	reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] << std::endl;
	std::cout << "--- " << 	reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] << std::endl;

	secondaryLCG->setUniform("max_property", reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step]  );
	secondaryLCG->setUniform("min_property", reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step]  );
	secondaryLCG->setUniform("property_index", reservoir_model_.current_static );
	secondaryLCG->setUniform("time_step", time_step );

	secondaryLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	secondaryLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	secondaryLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	secondaryLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	secondaryLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	secondaryLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	reservoir_model_.drawCuboid ( );

	secondaryLCG->enable( );

}

void GLWidget::paintGL ( )
{

	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}

        QTime renderTime;

        renderTime.start();

        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if ( isIRESOpen_ )
	{
		if ( isRawModel_ )
		{
			drawRawModel( );
		}
		else if ( isIRESCutaway_ )
		{
			IRESCutaway ( );
		}
		else if ( isFullModel_ )
		{
			drawFullModel( );
		}else
		{
		        PaperDemo( );
		}

		//trackball_->render();
	}

        glFinish( );

        //std::cout << " 1 shot " <<  1000/(float)renderTime.elapsed() << std::endl;

        if (renderingPass > 100.0)
        {
                averageFPS = accumulateRenderingTimes / renderingPass;
                renderingPass = 0.0f;
                accumulateRenderingTimes = 0.0f;
        }else
        {
                renderingPass            += 1.0;
                accumulateRenderingTimes += renderTime.elapsed();
        }

        emit fpsChanged("Resolution :"+QString::number(width())+" x "+QString::number(height())+
                        " fps : " + QString::number(static_cast<int>(renderTime.elapsed())) +
                        " Average :" + QString::number( averageFPS ) );

	fps++;


}

////Timer
//void GLWidget::timerEvent( QTimerEvent *event )
//{
//
//
//
//}

void GLWidget::drawRawModel ( )
{
        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround( );

	GLfloat light_elements[12];
	for ( std::size_t i = 0; i < lights.size ( ); ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			light_elements[i * 3 + j] = lights[i][j];
		}
	}

	rawShellLCG->enable( );

        glActiveTexture(GL_TEXTURE0+3);
        glBindTexture ( GL_TEXTURE_2D, xtoon_texture_ );
        rawShellLCG->setUniform("xtoon_texture", 3  );
        rawShellLCG->setUniform("viewportSize", width(), height() );

	rawShellLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	rawShellLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	rawShellLCG->setUniform("property_index", reservoir_model_.current_static );

	rawShellLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	rawShellLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	rawShellLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	rawShellLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	rawShellLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	rawShellLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	reservoir_model_.drawFace();

	rawShellLCG->disable( );

}

void GLWidget::IRESCutaway (  )
{

//	/// FIXME Conditions  - Primary and Secondary well defined.

 	if ( isIRESOpen_ )
	{

 		drawCutawaySurface( );

		glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
		glDepthFunc(GL_LESS);
		glClearDepth(1.0f);
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		drawBackGround( );

		if ( draw_secondary )
		{
			drawSecondary( );
		}
		if ( draw_primary )
		{
			drawPrimary( );
		}
	}

}

void GLWidget::drawBackGround ( )
{
	/// FIXME Conditions  - Just the model opened.

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	BackGround->enable();

	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture ( GL_TEXTURE_2D, xtoon_texture_ );
	BackGround->setUniform("imageTexture", 3);
	BackGround->setUniform("viewportSize", width(), height() );

	picture->render();

	BackGround->disable();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}


void GLWidget::PaperDemo()
{
        if ( isIRESOpen_ )
        {
                PaperDrawCutawaySurface( );

                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc(GL_LESS);
                glClearDepth(1.0f);
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                if ( draw_secondary )
                {
                        drawSecondary( );
                }
                if ( draw_primary )
                {
                        PaperPly( );

                }
        }
}

void GLWidget::PaperDrawCutawaySurface()
{

        glDepthFunc ( GL_GREATER );
        glClearDepth ( 0.0 );

        depthFBO->bind( );

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        BoundingBoxInitializationLCG->enable( );

        BoundingBoxInitializationLCG->setUniform("min_range", min_range  );
        BoundingBoxInitializationLCG->setUniform("max_range", max_range  );
        BoundingBoxInitializationLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        BoundingBoxInitializationLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        BoundingBoxInitializationLCG->setUniform("property_index", reservoir_model_.current_static );

        BoundingBoxInitializationLCG->setUniform( "x" , volume_width );
        BoundingBoxInitializationLCG->setUniform( "y" , volume_height );
        BoundingBoxInitializationLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BoundingBoxInitializationLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BoundingBoxInitializationLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        BoundingBoxInitializationLCG->setUniform ("freeze", freezeView_ );
        BoundingBoxInitializationLCG->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        reservoir_model_.drawCuboid ( );

        BoundingBoxInitializationLCG->disable( );


        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

        meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

        depthFBO->unbindAll();


        glDrawBuffer(GL_BACK);

}

void GLWidget::PaperPly ( )
{
        /// FIXME Conditions - Just the model opened.

        GLfloat light_elements[12];
        for ( std::size_t i = 0; i < lights.size ( ); ++i )
        {
                for ( int j = 0; j < 3; ++j )
                {
                        light_elements[i * 3 + j] = lights[i][j];
                }
        }

        BurnsPrimary->enable();

        BurnsPrimary->setUniform("num_lights", (GLint) lights.size ( )  );
        BurnsPrimary->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        BurnsPrimary->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimary->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimary->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        ply_primary_.Draw ( );

        BurnsPrimary->disable();

}

void GLWidget::gameLooping ( )
{

     //std::cout << "fps :" << v  << std::endl;

     //updateGL();

     fps++;

}

void GLWidget::fpsCounter ( )
{

        //std::cout << "fps :" << fps  << std::endl;

	fps = 0;

	delta_time.restart();
}

void GLWidget::loadShaders ( )
{
	QDir shadersDir = QDir ( qApp->applicationDirPath ( ) );

	#if defined(_WIN32) || defined(_WIN64) // Windows Directory Style
	/* Do windows stuff */
	QString shaderDirectory ("D:\\Workspace\\IRESCutaway\\src\\IRESCutaway\\GUI\\Qt\\RCC\\Shaders\\");
	#elif defined(__linux__)               // Linux Directory Style
	/* Do linux stuff */
        //QString shaderDirectory ("/home/ricardomarroquim/devel/irescutaway/src/IRESCutaway/GUI/Qt/RCC/Shaders/");
	QString shaderDirectory ("/media/d/Workspace/IRESCutaway/src/IRESCutaway/GUI/Qt/RCC/Shaders/");
	#else
	/* Error, both can't be defined or undefined same time */
	#endif

	qDebug () << "Directory " << shadersDir.path ();
	shadersDir.cdUp ();
	qDebug () << "Directory " << shadersDir.path ();


	meanFilter->setShadersDir( shaderDirectory.toStdString() );

	meanFilter->initialize( );

	BackGround = new Shader  ("BackGround", (shaderDirectory + "BackGround.vert").toStdString(),
				 (shaderDirectory + "BackGround.frag").toStdString(),"",1);

	BackGround->initialize( );

	// LCG
	// When New is calling, does the destroyer function is also called ?
	primaryLCG =  new Shader( "Primary",(shaderDirectory + "Primary.vert").toStdString(),
	                                    (shaderDirectory + "Primary.frag").toStdString(),
                                            (shaderDirectory + "Primary.geom").toStdString(),1);

	primaryLCG->initialize();


        secondaryLCG = new Shader("Secondary",(shaderDirectory + "Secondary.vert").toStdString(),
                                              (shaderDirectory + "Secondary.frag").toStdString(),
                                              (shaderDirectory + "Secondary.geom").toStdString(),1);

        secondaryLCG->initialize( );


        BoundingBoxInitializationLCG = new Shader ("BoundingBoxApproach",(shaderDirectory + "BoundingBoxApproach.vert").toStdString(),
                                                                (shaderDirectory + "BoundingBoxApproach.frag").toStdString(),
                                                                (shaderDirectory + "BoundingBoxApproach.geom").toStdString(),1);
        BoundingBoxInitializationLCG->initialize( );

        BoundingBoxDebugLCG = new Shader ("BoundingBoxApproach Debug",(shaderDirectory + "BoundingBoxApproach.vert").toStdString(),
                                                             (shaderDirectory + "BoundingBoxApproach.frag").toStdString(),
                                                             (shaderDirectory + "BoundingBoxApproachDebug.geom").toStdString(),1);

        BoundingBoxDebugLCG->initialize();

        BoundingBoxCutawayLCG = new Shader ("BoundingBoxCutaway",(shaderDirectory + "BoundingBoxCutaway.vert").toStdString(),
                                                        (shaderDirectory + "BoundingBoxCutaway.frag").toStdString(),
                                                        (shaderDirectory + "BoundingBoxCutaway.geom").toStdString(),1);
        BoundingBoxCutawayLCG->initialize();

        shellLCG = new Shader ("Shell", (shaderDirectory + "Shell.vert").toStdString(),
                              (shaderDirectory + "Shell.frag").toStdString(),
                              (shaderDirectory + "Shell.geom").toStdString(),1);

        shellLCG->initialize();

        rawShellLCG = new Shader ("Shell", (shaderDirectory + "RawShell.vert").toStdString(),
                              (shaderDirectory + "RawShell.frag").toStdString(),
                              (shaderDirectory + "RawShell.geom").toStdString(),1);

        borderLinesLCG = new Shader ("Border Lines", (shaderDirectory + "borderLines.vert").toStdString(),
                                    (shaderDirectory + "borderLines.frag").toStdString(),
        		            (shaderDirectory + "borderLines.geom").toStdString(),1);

        borderLinesLCG->initialize();

        rawShellLCG->initialize( );

        xtoon_texture_viewer = new Shader  ("xtoon_texture", (shaderDirectory + "xtoon_texture.vert").toStdString(),
        		                              (shaderDirectory + "xtoon_texture.frag").toStdString(),"",1);

        xtoon_texture_viewer->initialize( );


        BurnsPrimary = new Shader ("BurnsPrimary",(shaderDirectory + "BurnsPrimary430.vert").toStdString(),
                                                  (shaderDirectory + "BurnsPrimary430.frag").toStdString(),"",1);
        BurnsPrimary->initialize();

        BurnsPrimarySetup = new Shader ("BurnsPrimarySetup", (shaderDirectory + "BurnsPrimarySetup.vert").toStdString(),
                                    (shaderDirectory + "BurnsPrimarySetup.frag").toStdString(),
                                    (shaderDirectory + "BurnsPrimarySetup.geom").toStdString(),1);
        BurnsPrimarySetup->initialize();


}
/// KeyInput
void GLWidget::processMultiKeys ( )
{

	foreach( int key , keysPresseds_)
	{

		//std::cout  << camera_.position();

		if ( key == Qt::Key_R )
		{
			trackball_->reset();
			updateGL();
		}
		if( key == Qt::Key_Left)
		{
			if ( volume_width  > 0.0 )
				volume_width--;
		}
		if( key == Qt::Key_Right)
		{
			if ( volume_width  <= 30.0 )
				volume_width++;
		}
		if( key == Qt::Key_Up)
		{
			if ( volume_height  <= 30.0 )
				volume_height++;
		}
		if( key == Qt::Key_Down)
		{
			if ( volume_height  > 0.0 )
				volume_height--;
		}
	}
}

void GLWidget::keyPressEvent ( QKeyEvent * event )
{

	buttonRelease_ = true;
	keysPresseds_ += event->key ( );

	if ( event->key() == Qt::Key_F5 )
	{
		loadShaders();
	}
    updateGL();
}

void GLWidget::keyReleaseEvent ( QKeyEvent * event )
{

	buttonRelease_ = false;
	keysPresseds_ -= event->key ( );
    updateGL();
}
/// MouseInput
void GLWidget::mousePressEvent ( QMouseEvent *event )
{

	event->accept ( );

	Eigen::Vector2i pos(event->x ( ),event->y ( ));

	Eigen::Vector2f positionInTrackballSystem;
	positionInTrackballSystem = convertToNormalizedDeviceCoordinates(pos);


	if ( event->button ( ) == Qt::LeftButton )
	{
		trackball_->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
		trackball_->beginRotation();
	}

	if ( event->button ( ) == Qt::RightButton )
	{
		lastPos = event->pos ( );
		trackball_->setInitialTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
		trackball_->beginTranslation();
	}
    updateGL();
}

void GLWidget::mouseReleaseEvent ( QMouseEvent *event )
{

	event->accept ( );

	if ( event->button ( ) == Qt::LeftButton )
	{

		trackball_->endRotation();

	}
	else if ( event->button ( ) == Qt::RightButton )
	{
		trackball_->endTranslation();
	}
    updateGL();
}

void GLWidget::mouseMoveEvent ( QMouseEvent *event )
{


	if ( event->buttons ( )  )
	{
		//Position vector in [0,viewportWidth] domain:
		Eigen::Vector2i pos(event->x ( ),event->y ( ));

		//Convert to Normalized Device Coordinates System:
		Eigen::Vector2f positionInTrackballSystem;
		positionInTrackballSystem = convertToNormalizedDeviceCoordinates(pos);

		//Camera Rotating:
		if(trackball_->isRotating() && (event->buttons ( ) & Qt::LeftButton ))  {
			Eigen::Vector3f initialPosition = trackball_->getInitialRotationPosition();
			if(positionInTrackballSystem != Eigen::Vector2f(initialPosition[0], initialPosition[1])) {
				trackball_->setFinalRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
				trackball_->rotateCamera();
				trackball_->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);

			}

		}

		if(trackball_->isTranslating() && (event->buttons ( ) & Qt::RightButton )) {
			Eigen::Vector2f initialPosition = trackball_->getInitialTranslationPosition();
			if(positionInTrackballSystem != Eigen::Vector2f(initialPosition[0], initialPosition[1])) {
				trackball_->setFinalTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
				trackball_->translateCamera();
				trackball_->setInitialTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			}
		}
	}
    updateGL();

}

Eigen::Vector2f GLWidget::convertToNormalizedDeviceCoordinates(Eigen::Vector2i position) {
	float x = (float)position[0]/(float)(width()/2.0);
	x = x-1;

	float y = (float)position[1]/(float)(height()/2.0);
	y = 1-y;

	Eigen::Vector2f ret(x,y);
	return ret;
}

void GLWidget::wheelEvent ( QWheelEvent *event )
{

	if ( QApplication::keyboardModifiers ( ) == Qt::ShiftModifier )
	{
		zoom_angle_ += event->delta ( ) / 120.0;

		if ( event->delta ( ) < 0.0 )
		{
			trackball_->increaseZoom ( 1.05 );
		}
		else
		{
			trackball_->decreaseZoom ( 1.05 );
		}

		orthoZoom += event->delta ( ) / 1200.0;
	}
	else
	{

	}

    updateGL();

}

void GLWidget::showFault	  ( bool visibility )
{
	if ( visibility )
		reservoir_model_.showFault = 0;
	else
		reservoir_model_.showFault = 1;
}

void GLWidget::showBorderLines    ( bool visibility )
{
	reservoir_model_.showBorderLine = visibility;;
}

void GLWidget::setPrimaryVisibility( bool visibility )
{
	if ( reservoir_model_.cuboids.size() > 0 )
		draw_primary = visibility;
	updateGL();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( reservoir_model_.cuboids.size() > 0 )
		draw_secondary = visibility;
	updateGL();
}

void GLWidget::freezeView( )
{
	freezeView_ = !freezeView_;
    if (freezeView_) {
        freeze_viewmatrix_ = trackball_->getViewMatrix().matrix();
    }

}
