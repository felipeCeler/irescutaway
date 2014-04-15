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

	isPaperDemo_	      = 1;
	isIRESCutaway_        = 0;
	isIRESCutawayStatic_  = 0;
	isIRESCutawayDynamic_ = 0;

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

	justWireFrame = 0;

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

        videoSequence = 0;
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

    update();

}

void GLWidget::changeProperty ( const int property_index )
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
	update();
}

void GLWidget::changeMaxJ ( const int& value )
{
	max_J_ = value;
	std::cout << " min_J " << min_J_ << std::endl;
	std::cout << " max_J " << max_J_ << std::endl;
	update();
}

void GLWidget::changeMaxK ( const int& value )
{
	max_K_ = value;
	std::cout << " min_K " << min_K_ << std::endl;
	std::cout << " max_K " << max_K_ << std::endl;
	update();
}

void GLWidget::changeMinI ( const int& value )
{
	min_I_ = value;
	std::cout << " min_I " << min_I_ << std::endl;
	std::cout << " max_I " << max_I_ << std::endl;
	update();
}

void GLWidget::changeMinJ ( const int& value )
{
	min_J_ = value;
	std::cout << " min_J " << min_J_ << std::endl;
	std::cout << " max_J " << max_J_ << std::endl;
	update();
}

void GLWidget::changeMinK ( const int& value )
{
	min_K_ = value;
	std::cout << " min_K " << min_K_ << std::endl;
	std::cout << " max_K " << max_K_ << std::endl;
	update();
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

	float aspect = (float) width / height;

	trackball_->useOrthographicMatrix ( -1.0f*aspect , 1.0f*aspect , -1.0f , 1.0 , 0.1 , 500.0 );
	//trackball_->usePerspectiveMatrix  ( 60.0f , (float) width / float ( height ) , 0.1 , 500 );

	if (depthFBO)
		delete depthFBO;


	depthFBO = new Framebuffer( width , height, 2 );

	meanFilter->resize(width, height);
}

void GLWidget::paintGL ( )
{

        //  Reset OpenGL state for overlays.
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_LINE_SMOOTH );

        if ( buttonRelease_ )
        {
                processMultiKeys ( );
        }

        QTime renderTime;

        renderTime.start();

        if ( isIRESOpen_ )
        {
                if ( isPaperDemo_ )
                {
                        PaperDemo( );                // F9
                }
                else if ( isIRESCutaway_ )
                {
                        drawIRESModel( );            // F10
                }
                else if ( isIRESCutawayStatic_ )
                {
                        IRESCutawayStatic ( );       // F11
                }
                else // isIRESCutawayDynamic_
                {
                        IRESCutawayDynamic ( );      // F12
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
                        " Average : " + QString::number(static_cast<int>(renderTime.elapsed())) +
                        " fps :" + QString::number( 1000/averageFPS ) );

        fps++;

        //QGLWidget::grabFrameBuffer().save("frame"+QString::number(videoSequence)+".png","png",100);

        videoSequence++;


        //  Reset OpenGL state for overlays.
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LINE_SMOOTH );


        QPainter p; // used for text overlay

        p.begin ( this );
        p.setRenderHint ( QPainter::Antialiasing );
        // save the GL state set for QPainter
        //draw the overlayed text using QPainter

        unsigned int relativeWidth = 80;
        unsigned int relativeHeight = 40;

        unsigned int relativeSpace = 30;

        unsigned int size_W = 15;
        unsigned int size_H = 30;

        QPen pen;
        pen.setCapStyle ( Qt::RoundCap );
        pen.setJoinStyle ( Qt::RoundJoin );
        pen.setColor ( qRgb ( 0 , 0 , 0 ) );
        pen.setWidth ( 1 );
        p.setPen ( pen );

        QGradient gradient_ ( QLinearGradient ( 0.0 , 0.0 , 0.0 , 1.0 ) );

        gradient_.setCoordinateMode ( QGradient::ObjectBoundingMode );

        gradient_.setColorAt ( 0.0 , QColor(127,0,0) );
        gradient_.setColorAt ( 2.0/15.0 , QColor(255,0,0) );
        gradient_.setColorAt ( 6.0/15.0 , QColor(255,255,0) );

        gradient_.setColorAt ( 9.0/15.0 , QColor(0,255,255) );
        gradient_.setColorAt ( 14.0/15.0 , QColor(0,0,255) );
        gradient_.setColorAt ( 1.0 , QColor(0,0,127) );

        QRect rect ( width ( ) - relativeWidth , ( height ( ) - relativeHeight - relativeSpace * 6 ) , size_W , size_H * 6 );

        p.setBrush ( gradient_ );
        p.drawRect ( width ( ) - relativeWidth , ( height ( ) - relativeHeight - relativeSpace * 6 ) , size_W , size_H * 6 );
        int left = rect.left ( ) + rect.width ( ) + 10;

        p.drawText ( left , rect.top ( ) , "100.0%" );
        p.drawText ( left , rect.top ( ) + rect.height ( ) / 2 , "50.0%" );
        p.drawText ( left , rect.top ( ) + rect.height ( ) + 10 , "0.0%" );

        p.end();

}


/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.
/// Real Looping

void GLWidget::drawIRESCutawayStaticSurface ( ) const
{

	glDepthFunc ( GL_GREATER );
	glClearDepth ( 0.0 );

	depthFBO->bind( );

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	IRESCutawaySurfaceStatic->enable( );

	IRESCutawaySurfaceStatic->setUniform("min_range", min_range  );
	IRESCutawaySurfaceStatic->setUniform("max_range", max_range  );
	IRESCutawaySurfaceStatic->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	IRESCutawaySurfaceStatic->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	IRESCutawaySurfaceStatic->setUniform("property_index", reservoir_model_.current_static );

	IRESCutawaySurfaceStatic->setUniform("paper", 1.0  );
	IRESCutawaySurfaceStatic->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
	IRESCutawaySurfaceStatic->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

	IRESCutawaySurfaceStatic->setUniform( "x" , volume_width );
	IRESCutawaySurfaceStatic->setUniform( "y" , volume_height );
	IRESCutawaySurfaceStatic->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	IRESCutawaySurfaceStatic->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	IRESCutawaySurfaceStatic->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	IRESCutawaySurfaceStatic->setUniform ("freeze", freezeView_ );
	IRESCutawaySurfaceStatic->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

	reservoir_model_.drawCuboid ( );

	IRESCutawaySurfaceStatic->disable( );

	glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

	meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

	depthFBO->unbindAll();


	glDrawBuffer(GL_BACK);


}

void GLWidget::drawPrimaryStatic  ( ) const // Draw only primary   Cells
{

        GLfloat light_elements[12];
        for ( std::size_t i = 0; i < lights.size ( ); ++i )
        {
                for ( int j = 0; j < 3; ++j )
                {
                        light_elements[i * 3 + j] = lights[i][j];
                }
        }

        IRESPrimaryStatic_->enable( );

        IRESPrimaryStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESPrimaryStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESPrimaryStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESPrimaryStatic_->setUniform("min_range", min_range  );
        IRESPrimaryStatic_->setUniform("max_range", max_range  );

        IRESPrimaryStatic_->setUniform("paper", 1.0  );
        IRESPrimaryStatic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        IRESPrimaryStatic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        IRESPrimaryStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        IRESPrimaryStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        IRESPrimaryStatic_->setUniform("property_index", reservoir_model_.current_static );

        IRESPrimaryStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        IRESPrimaryStatic_->disable( );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                borderLinesLCG->enable ( );

                borderLinesLCG->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                borderLinesLCG->disable ( );
        }
}

void GLWidget::drawSecondaryStatic  ( ) const  // Draw only secondary Cells
{

        GLfloat light_elements[12];
        for ( std::size_t i = 0; i < lights.size ( ); ++i )
        {
                for ( int j = 0; j < 3; ++j )
                {
                        light_elements[i * 3 + j] = lights[i][j];
                }
        }

        // Surface Faces

        IRESCutawayStaticShell_->enable( );

        IRESCutawayStaticShell_->setUniform ( "normals" , depthFBO->bindAttachment(1) );

        IRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        IRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        IRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
        IRESCutawayStaticShell_->setUniform("faults", reservoir_model_.showFault );
        IRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );

        IRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESCutawayStaticShell_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStaticShell_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStaticShell_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        IRESCutawayStaticShell_->disable( );

        // Interior Cells

        // Interior Cells

        depthFBO->unbindAttachments();

        IRESCutawayStatic_->enable( );

        IRESCutawayStatic_->setUniform( "normals" , depthFBO->bindAttachment(1) );

        IRESCutawayStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        IRESCutawayStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        IRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );

        IRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESCutawayStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawInternalFaces( );

        IRESCutawayStatic_->disable( );

        depthFBO->unbindAttachments();

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                borderLinesLCG->enable ( );

                borderLinesLCG->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                borderLinesLCG->disable ( );
        }

}

void GLWidget::IRESCutawayStatic (  ) const
{

//      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {

                drawIRESCutawayStaticSurface( );

                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc(GL_LESS);
                glClearDepth(1.0f);
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                if ( draw_secondary )
                {
                        drawSecondaryStatic( );
                }
                if ( draw_primary )
                {
                        drawPrimaryStatic( );

                }
        }

}


void GLWidget::drawIRESCutawayDynamicSurface ( ) const
{

        glDepthFunc ( GL_GREATER );
        glClearDepth ( 0.0 );

        depthFBO->bind( );

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


        IRESCutawaySurfaceDynamic->enable( );

        IRESCutawaySurfaceDynamic->setUniform( "v0" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
        IRESCutawaySurfaceDynamic->setUniform( "v1" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic->setUniform( "v2" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic->setUniform( "v3" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);

        IRESCutawaySurfaceDynamic->setUniform( "v4" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);
        IRESCutawaySurfaceDynamic->setUniform( "v5" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic->setUniform( "v6" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic->setUniform( "v7" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);


        IRESCutawaySurfaceDynamic->setUniform("move", (float) borderLinesSize_ );
        IRESCutawaySurfaceDynamic->setUniform( "x" , volume_width );
        IRESCutawaySurfaceDynamic->setUniform( "y" , volume_height );
        IRESCutawaySurfaceDynamic->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        IRESCutawaySurfaceDynamic->setUniform ("freeze", freezeView_ );
        IRESCutawaySurfaceDynamic->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        glBindVertexArray(vertexArray_box);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        IRESCutawaySurfaceDynamic->disable( );


        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

        meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

        depthFBO->unbindAll();


        glDrawBuffer(GL_BACK);

}

void GLWidget::drawPrimaryDynamic ( ) const
{

        GLfloat light_elements[12];
        for ( std::size_t i = 0; i < lights.size ( ); ++i )
        {
                for ( int j = 0; j < 3; ++j )
                {
                        light_elements[i * 3 + j] = lights[i][j];
                }
        }

        IRESPrimaryDynamic->enable( );

        IRESPrimaryDynamic->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESPrimaryDynamic->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESPrimaryDynamic->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESPrimaryDynamic->setUniform("min_range", min_range  );
        IRESPrimaryDynamic->setUniform("max_range", max_range  );

        IRESPrimaryDynamic->setUniform("paper", 1.0  );
        IRESPrimaryDynamic->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        IRESPrimaryDynamic->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        IRESPrimaryDynamic->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESPrimaryDynamic->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
//
//        IRESPrimaryDynamic->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//        IRESPrimaryDynamic->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//        IRESPrimaryDynamic->setUniform("property_index", reservoir_model_.current_static );

        IRESPrimaryDynamic->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryDynamic->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryDynamic->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawCuboid ( );

        IRESPrimaryDynamic->disable( );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                borderLinesLCG->enable ( );

                borderLinesLCG->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                borderLinesLCG->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                borderLinesLCG->disable ( );
        }

}

void GLWidget::drawSecondaryDynamic () const
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
        glDepthFunc ( GL_LESS );
        glClearDepth ( 1.0f );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround ( );

        IRESCutawayDynamicShell->enable ( );

        IRESCutawayDynamicShell->setUniform ( "normals" , depthFBO->bindAttachment(1) );
//      secondaryLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//      secondaryLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//      secondaryLCG->setUniform("property_index", reservoir_model_.current_static );

        std::cout << "--- " << reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] << std::endl;
        std::cout << "--- " << reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] << std::endl;

        IRESCutawayDynamicShell->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESCutawayDynamicShell->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        IRESCutawayDynamicShell->setUniform ( "time_step" , time_step );

        IRESCutawayDynamicShell->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        IRESCutawayDynamicShell->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        IRESCutawayDynamicShell->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        IRESCutawayDynamicShell->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicShell->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicShell->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawFaces( );

        IRESCutawayDynamicShell->disable( );


        depthFBO->unbindAttachments();

        IRESCutawayDynamicCrust->enable( );

        IRESCutawayDynamicCrust->setUniform( "normals" , depthFBO->bindAttachment(1) );

//      secondaryLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//      secondaryLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//      secondaryLCG->setUniform("property_index", reservoir_model_.current_static );

        std::cout << "--- " <<  reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] << std::endl;
        std::cout << "--- " <<  reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] << std::endl;

        IRESCutawayDynamicCrust->setUniform("max_property", reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step]  );
        IRESCutawayDynamicCrust->setUniform("min_property", reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step]  );
        IRESCutawayDynamicCrust->setUniform("property_index", reservoir_model_.current_static );
        IRESCutawayDynamicCrust->setUniform("time_step", time_step );

        IRESCutawayDynamicCrust->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawayDynamicCrust->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawayDynamicCrust->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESCutawayDynamicCrust->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayDynamicCrust->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayDynamicCrust->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawCuboid ( );

        IRESCutawayDynamicCrust->disable( );

        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayDynamic ( ) const
{

        //      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {

                drawIRESCutawayDynamicSurface ( );

                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc ( GL_LESS );
                glClearDepth ( 1.0f );
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround ( );

                if ( draw_secondary )
                {
                        drawSecondaryDynamic( );
                }
                if ( draw_primary )
                {
                        drawPrimaryDynamic( );
                }
        }

}

////Timer
//void GLWidget::timerEvent( QTimerEvent *event )
//{
//
//
//
//}

void GLWidget::drawIRESModel ( )
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


        // Surface Faces

	IRESCutawayStaticShell_->enable( );

	IRESCutawayStaticShell_->setUniform ( "normals" , depthFBO->bindAttachment(1) );

	IRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	IRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	IRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
	IRESCutawayStaticShell_->setUniform("faults", reservoir_model_.showFault );
	IRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );

	IRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESCutawayStaticShell_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStaticShell_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStaticShell_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        IRESCutawayStaticShell_->disable( );

        // Interior Cells

        depthFBO->unbindAttachments();

	IRESCutawayStatic_->enable( );

	IRESCutawayStatic_->setUniform( "normals" , depthFBO->bindAttachment(1) );

        IRESCutawayStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        IRESCutawayStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        IRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );

        IRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESCutawayStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawayStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawInternalFaces( );

        IRESCutawayStatic_->disable( );

        depthFBO->unbindAttachments();

}


void GLWidget::drawBackGround ( ) const
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
                        drawSecondaryStatic( );
                }
                if ( draw_primary )
                {
                        drawPrimaryStatic( );

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


        BurnsPrimarySetup->enable( );

        BurnsPrimarySetup->setUniform( "v0" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
        BurnsPrimarySetup->setUniform( "v1" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup->setUniform( "v2" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup->setUniform( "v3" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);

        BurnsPrimarySetup->setUniform( "v4" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);
        BurnsPrimarySetup->setUniform( "v5" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup->setUniform( "v6" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup->setUniform( "v7" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);


        BurnsPrimarySetup->setUniform("move", (float) borderLinesSize_ );
        BurnsPrimarySetup->setUniform( "x" , volume_width );
        BurnsPrimarySetup->setUniform( "y" , volume_height );
        BurnsPrimarySetup->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimarySetup->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimarySetup->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        BurnsPrimarySetup->setUniform ("freeze", freezeView_ );
        BurnsPrimarySetup->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        glBindVertexArray(vertexArray_box);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        BurnsPrimarySetup->disable( );


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

        BurnsPrimary->setUniform("move", (float) borderLinesSize_ );
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

     //update();

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


        // Effects --
                borderLinesLCG = new Shader ("Border Lines", (shaderDirectory + "borderLines.vert").toStdString(),
                                            (shaderDirectory + "borderLines.frag").toStdString(),
                                            (shaderDirectory + "borderLines.geom").toStdString(),1);
                borderLinesLCG->initialize();

                xtoon_texture_viewer = new Shader  ("xtoon_texture", (shaderDirectory + "xtoon_texture.vert").toStdString(),
                                                              (shaderDirectory + "xtoon_texture.frag").toStdString(),"",1);
                xtoon_texture_viewer->initialize( );

                meanFilter->setShadersDir( shaderDirectory.toStdString() );
                meanFilter->initialize( );

                BackGround = new Shader  ("BackGround", (shaderDirectory + "BackGround.vert").toStdString(),
                                         (shaderDirectory + "BackGround.frag").toStdString(),"",1);
                BackGround->initialize( );


        rawShellLCG = new Shader ("Shell", (shaderDirectory + "RawShell.vert").toStdString(),
                              (shaderDirectory + "RawShell.frag").toStdString(),
                              (shaderDirectory + "RawShell.geom").toStdString(),1);

        rawShellLCG->initialize( );


        BurnsPrimary = new Shader ("BurnsPrimary",(shaderDirectory + "BurnsPrimary430.vert").toStdString(),
                                                  (shaderDirectory + "BurnsPrimary430.frag").toStdString(),"",1);
        BurnsPrimary->initialize();

        BurnsPrimarySetup = new Shader ("BurnsPrimarySetup", (shaderDirectory + "BurnsPrimarySetup.vert").toStdString(),
                                    (shaderDirectory + "BurnsPrimarySetup.frag").toStdString(),
                                    (shaderDirectory + "BurnsPrimarySetup.geom").toStdString(),1);
        BurnsPrimarySetup->initialize();

       // ! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic = new Shader ("IRESCutawaySurfaceStatic",
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.vert").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.frag").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.geom").toStdString(),1);
                IRESCutawaySurfaceStatic->initialize( );

                IRESCutawayStaticShell_ = new Shader ("IRESCutawayStaticShell",
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.vert").toStdString(),
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.frag").toStdString(),
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.geom").toStdString(),1);
                IRESCutawayStaticShell_->initialize();

                IRESCutawayStatic_ = new Shader ("IRESCutawayStatic",
                                                (shaderDirectory + "Static/IRESCutawayStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/IRESCutawayStatic.frag").toStdString(),"",1);
                IRESCutawayStatic_->initialize();

                IRESPrimaryStatic_ = new Shader ("IRESPrimaryStatic",
                                                (shaderDirectory + "Static/IRESPrimaryStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/IRESPrimaryStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/IRESPrimaryStatic.geom").toStdString(),1);

                IRESPrimaryStatic_->initialize();


        // ! DYNAMIC VIEWER F12 Dynamic Properties

                IRESCutawaySurfaceDynamic = new Shader ("IRESCutawaySurfaceDynamic",
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.geom").toStdString(),1);
                IRESCutawaySurfaceDynamic->initialize();

                IRESCutawayDynamicCrust = new Shader ("IRESCutawayDynamicCrust",
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.geom").toStdString(),1);
                IRESCutawayDynamicCrust->initialize();


                IRESCutawayDynamicShell = new Shader ("IRESCutawayDynamicShell",
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.geom").toStdString(),1);
                IRESCutawayDynamicShell->initialize();

                IRESPrimaryDynamic = new Shader ("IRESPrimaryDynamic",
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.geom").toStdString(),1);
                IRESPrimaryDynamic->initialize();
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
			update();
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
	else if ( event->key() == Qt::Key_R )
	{
	        trackball_->reset();
	        trackball_->decreaseZoom( 5.05 );
	}
	else
	{
	        event->ignore();
	}

    update();
}

void GLWidget::keyReleaseEvent ( QKeyEvent * event )
{

	buttonRelease_ = false;
	keysPresseds_ -= event->key ( );
    update();
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
    update();
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
    update();
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
    update();

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

    update();

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
	update();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( reservoir_model_.cuboids.size() > 0 )
		draw_secondary = visibility;
	update();
}

void GLWidget::freezeView( )
{
	freezeView_ = !freezeView_;
    if (freezeView_) {
        freeze_viewmatrix_ = trackball_->getViewMatrix().matrix();
    }

}
