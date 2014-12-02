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

//	glEnable(GL_LINE_SMOOTH);
//	glEnable ( GL_MULTISAMPLE );
//
//	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);

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

	wall_ = 0;
	line_ = 0;

	zoom_angle_ = 45.0f;
	orthoZoom   = 1.0f;

	lights.push_back ( Eigen::Vector3f ( 0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f (-0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 0.0 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 1.0 , 0.0 ) );

	light_elements = new float[12];

        for ( std::size_t i = 0; i < lights.size ( ); ++i )
        {
                for ( int j = 0; j < 3; ++j )
                {
                        light_elements[i * 3 + j] = lights[i][j];
                }
        }


	draw_secondary = 1;
	draw_primary = 0;

	isPaperDemo_	      = 1;
	isIRESCutaway_        = 0;
	isIRESCutawayStatic_  = 0;
	isIRESCutawayDynamic_ = 0;

	cluster = 0;
        cutawayPass_ = 0.0;
        accumulateCutawayGenerationTime_ = 0.0;
        accumulateRenderingCutawayTime_ = 0.0;

	renderingPass = 0.0f;
	accumulateRenderingTimes = 0.0f;

	delta_time.start();

	updateTimer_.setSingleShot ( false );
	connect ( &updateTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( gameLooping ( ) ) );
	updateTimer_.start(20);

	fpsTimer_.setSingleShot ( false );
	connect ( &fpsTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( fpsCounter( ) ) );
	fpsTimer_.start ( 1000 );

	freezeView_ = 0;

	volume_width = 0.0f;
	volume_height = 0.0f;

	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;

	zoom_start_ = 0;
	zoom_increment_ = 1.01;

        borderLinesSize_ = 0;
        meanFilterSize_  = 0;

	trackball_ = new Trackball();

	nearPlane_ = 1.0f;
        farPlane_  = 5.0f;

	isPerspective_ = true;

	trackball_->initOpenGLMatrices();

	trackball_->initializeBuffers();

	enable_blend_ = 0;

	depthFBO = new Framebuffer( width() , height(), 4 );

	normalCutawayID_ = 0;
	verticesCutawayID_ = 1;
	normalsSmoothID_ = 2;
	verticesSmoothID_ =  3;

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

	glGenVertexArrays (1, &vertexArray_Dummy);
	        glGenBuffers(1,&vertexBuffer_Dummy);

	glBindVertexArray(vertexArray_Dummy);
	        glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer_Dummy);
	        glBufferData ( GL_ARRAY_BUFFER , 1, 0 , GL_STATIC_DRAW );

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);

        displacement            = Eigen::Vector3f ( -3.0 , -3.0 , -3.0 );
        max_displacement        = Eigen::Vector3f ( 3.0 , 3.0 , 3.0 );

       ///  -- SSAO - TUCANO

        depthTextureID = 0;
        normalTextureID = 1;
        colorTextureID = 2;
        blurTextureID = 3;

        numberOfSamples = 64;
        noise_size = 64;

        radius = 20;

        blurRange = 3;
        intensity = 20;
        max_dist = 0.15;

        apply_blur = true;
        displayAmbientPass = false;

        noise_scale = Eigen::Vector2f::Zero();

        generateKernel();
        generateNoiseTexture();

        fboSSAO = new Framebuffer( width() , height(), 4 );

        quadSSAO = new Mesh();
        quadSSAO->createQuad();


       // Animation Controls
                //Timer Animation

                fps             = 0;
                videoSequence   = 0;
                frames.resize ( 4000 );
                // Quaternion Slerp

                sourcePosition_ = Eigen::Quaternionf ( 0.0 , 1.0 , 0.0 , 0.0 );
                sourcePosition_.normalize ( );

                targetPosition_ = Eigen::Quaternionf ( 0.0 , 0.0 , 0.0 , 1.0 );
                targetPosition_.normalized ( );


                number_of_takes_ = 0;
                take_index_ = 0;

                takes_.resize(10,Animation());

                time_steps_     = 0.0;

                play_           = false;
                isply_          = false;

}

GLWidget::~GLWidget( )
{
        delete[] light_elements;
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

	/// @Noob - http://math.hws.edu/graphicsnotes/c3/s5.html near plane can be negative on orthographic avoiding clipping by it.

	if ( isPerspective_ )
	{
	        trackball_->usePerspectiveMatrix  ( 45.0f , aspect , nearPlane_ , farPlane_);
	}
	else
	{
	        trackball_->useOrthographicMatrix ( -1.0f*aspect , 1.0f*aspect , -1.0f , 1.0 , 0.1f , 500.0f );
	}


	if (depthFBO)
	{
	        delete depthFBO;
	}

	if (fboSSAO)
	{
	        delete fboSSAO;
	}

	std::cout << trackball_->getProjectionMatrix() << std::endl;

	depthFBO = new Framebuffer( width , height, 4 );
	depthFBO->clearAttachments();

        fboSSAO = new Framebuffer( width , height, 4 );
        fboSSAO->clearAttachments();

	meanFilter->resize(width, height);

	glFinish();
}

void GLWidget::paintEvent   ( QPaintEvent * )
{

        QPainter p; // used for text overlay

        p.begin ( this );

        p.setWindow(0,0,width(),height());
        p.setRenderHint ( QPainter::Antialiasing );

        p.beginNativePainting();
        paintGL();
        p.endNativePainting();

        //  Reset OpenGL state for overlays.
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LINE_SMOOTH );

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

void GLWidget::paintGL ( )
{

        //  Reset OpenGL state for overlays.
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_LINE_SMOOTH );

        if ( buttonRelease_ )
        {
                processMultiKeys ( );
        }

        drawBackGround ( );

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

        // std::cout << " 1 shot " <<  trackball_->getQuaternion().coeffs() <<  std::endl;

        if (renderingPass > 200.0)
        {
                averageFPS = accumulateRenderingTimes / renderingPass;
                renderingPass = 0.0f;
                accumulateRenderingTimes = 0.0f;
        }else
        {
                renderingPass            += 1.0;
                accumulateRenderingTimes += renderTime.elapsed();
        }

        emit fpsChanged( QString::number( 1000/averageFPS ) );

        fps++;

        // .save("frame"+QString::number(videoSequence)+".tif","TIFF",100);

//        if ( play_ )
//        {
//                if ( videoSequence <= 3998 )
//                {
//                        frames[videoSequence] = QImage(width(),height(),QImage::Format_ARGB32_Premultiplied);
//                        glReadPixels(0, 0, width(), height(), GL_BGRA, GL_UNSIGNED_BYTE, frames[videoSequence].bits());
//                        frames[videoSequence].rgbSwapped();
//                        videoSequence++;
//                }
//                else
//                {
//                        play_ = false;
//
//                }
//
//        }




}


void GLWidget::flush()
{

        for (std::size_t index = 0; index < videoSequence; index++)
        {
                frames[index].save("frame"+QString::number(index)+".tif","TIFF",100);
        }

        QProcess _process;
        _process.start(QString("echo"), QStringList("-la"));
        _process.waitForFinished();


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

	depthFBO->bindRenderBuffers(normalCutawayID_,verticesCutawayID_);

	glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	IRESCutawaySurfaceStatic_->enable( );

	IRESCutawaySurfaceStatic_->setUniform("min_range", min_range  );
	IRESCutawaySurfaceStatic_->setUniform("max_range", max_range  );
	IRESCutawaySurfaceStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
	IRESCutawaySurfaceStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
	IRESCutawaySurfaceStatic_->setUniform("property_index", reservoir_model_.current_static );

	IRESCutawaySurfaceStatic_->setUniform("paper", 1.0  );
	IRESCutawaySurfaceStatic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
	IRESCutawaySurfaceStatic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

	IRESCutawaySurfaceStatic_->setUniform( "x" , volume_width );
	IRESCutawaySurfaceStatic_->setUniform( "y" , volume_height );
	IRESCutawaySurfaceStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	IRESCutawaySurfaceStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	IRESCutawaySurfaceStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	IRESCutawaySurfaceStatic_->setUniform ("freeze", freezeView_ );
	IRESCutawaySurfaceStatic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

	reservoir_model_.drawCuboid ( );

	IRESCutawaySurfaceStatic_->disable( );

	depthFBO->bindRenderBuffers(normalsSmoothID_, verticesSmoothID_);


        glDisable(GL_DEPTH_TEST);
        meanFilter->renderTexture( depthFBO->bindAttachment(normalCutawayID_),depthFBO->bindAttachment(verticesCutawayID_),meanFilterSize_);

        depthFBO->unbindAll();

        glEnable(GL_DEPTH_TEST);


	glDrawBuffer(GL_BACK);

	glFinish();
	glFlush();

}

void GLWidget::drawPrimaryStatic  ( ) const // Draw only primary   Cells
{

//        IRESPrimaryStatic_->enable( );
//
//        IRESPrimaryStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
//        IRESPrimaryStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//        IRESPrimaryStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
//
//        IRESPrimaryStatic_->setUniform("min_range", min_range  );
//        IRESPrimaryStatic_->setUniform("max_range", max_range  );
//
//        IRESPrimaryStatic_->setUniform("paper", 1.0  );
//        IRESPrimaryStatic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
//        IRESPrimaryStatic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );
//
//        IRESPrimaryStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//        IRESPrimaryStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//        IRESPrimaryStatic_->setUniform("property_index", reservoir_model_.current_static );
//
//        IRESPrimaryStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        IRESPrimaryStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        IRESPrimaryStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        reservoir_model_.drawFaces();
//
//        IRESPrimaryStatic_->disable( );


        SSAOIRESPrimaryStatic_->enable ( );

        SSAOIRESPrimaryStatic_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStatic_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStatic_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        SSAOIRESPrimaryStatic_->setUniform ( "min_range" , min_range );
        SSAOIRESPrimaryStatic_->setUniform ( "max_range" , max_range );

        SSAOIRESPrimaryStatic_->setUniform ( "paper" , 1.0 );
        SSAOIRESPrimaryStatic_->setUniform ( "box_min" , ply_primary_.box.box_min ( ).x , ply_primary_.box.box_min ( ).y , ply_primary_.box.box_min ( ).z );
        SSAOIRESPrimaryStatic_->setUniform ( "box_max" , ply_primary_.box.box_max ( ).x , ply_primary_.box.box_max ( ).y , ply_primary_.box.box_max ( ).z );

        SSAOIRESPrimaryStatic_->setUniform ( "min_property" , reservoir_model_.static_min[reservoir_model_.current_static] );
        SSAOIRESPrimaryStatic_->setUniform ( "max_property" , reservoir_model_.static_max[reservoir_model_.current_static] );
        SSAOIRESPrimaryStatic_->setUniform ( "property_index" , reservoir_model_.current_static );

        SSAOIRESPrimaryStatic_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESPrimaryStatic_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESPrimaryStatic_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawFaces ( );

        SSAOIRESPrimaryStatic_->disable ( );

        fboSSAO->unbindAll();
        //fboSSAO->clearDepth();


        // THIRD PASS
//        blurShader->enable();
//
//        blurShader->setUniform("blurTexture", fboSSAO->bindAttachment(blurTextureID));
//        blurShader->setUniform("blurRange", blurRange);
//
//        quadSSAO->render();
//
//        blurShader->disable();
//
//        fboSSAO->unbindAll();

}

void GLWidget::drawSecondaryStatic  ( ) const  // Draw only secondary Cells
{

        // Interior Cells

//        IRESCutawayStatic_->enable( );
//
//        // @TODO Accord with John McDonald from nvidia at http://youtu.be/-bCeNzgiJ8I?t=31m42s,
//        // this command is costly than just bind a Texture
//
//        IRESCutawayStatic_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
//        IRESCutawayStatic_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );
//
//        IRESCutawayStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//        IRESCutawayStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//        IRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );
//
//        IRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
//        IRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//        IRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
//
//        IRESCutawayStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawayStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawayStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        reservoir_model_.drawFaces( );
//
//        IRESCutawayStatic_->disable( );
//
//        // Surface Faces
//
//        IRESCutawayStaticShell_->enable( );
//
//        IRESCutawayStaticShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
//        IRESCutawayStaticShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );
//
//        IRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//        IRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//        IRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
//        IRESCutawayStaticShell_->setUniform("faults", reservoir_model_.showFault );
//        IRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );
//
//        IRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
//        IRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//        IRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
//
//        IRESCutawayStaticShell_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawayStaticShell_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawayStaticShell_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        reservoir_model_.drawFaces();
//
//        IRESCutawayStaticShell_->disable( );
//
//        depthFBO->unbindAttachments();

        SSAOIRESCutawayStatic_->enable( );

        // @TODO Accord with John McDonald from nvidia at http://youtu.be/-bCeNzgiJ8I?t=31m42s,
        // this command is costly than just bind a Texture

        SSAOIRESCutawayStatic_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStatic_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        SSAOIRESCutawayStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );

        SSAOIRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces( );

        SSAOIRESCutawayStatic_->disable( );

        // Surface Faces

        SSAOIRESCutawayStaticShell_->enable( );

        SSAOIRESCutawayStaticShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStaticShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        SSAOIRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
        SSAOIRESCutawayStaticShell_->setUniform("faults", reservoir_model_.showFault );
        SSAOIRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );

        SSAOIRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStaticShell_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStaticShell_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStaticShell_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        SSAOIRESCutawayStaticShell_->disable( );

        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayStatic (  )
{

//      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {
//
                cutawayGenerationTime_.start ();



                if ( draw_cutaway_surface_ )
                {
                        drawIRESCutawayStaticSurface ( );
                }
                else
                {
//                        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
//                        glDepthFunc ( GL_LESS );
//                        glClearDepth ( 1.0f );
//                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//                        depthFBO->bind ( );
////
////                        glDrawBuffer ( GL_COLOR_ATTACHMENT0 );
////
////                        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
////                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
////
////                        glDepthFunc ( GL_GREATER );
////                        glClearDepth ( 0.0 );
////
////                        depthFBO->bind ( );
//
//                        glDrawBuffer ( GL_COLOR_ATTACHMENT0 );
//
//                        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
//                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//                        glBindVertexArray ( vertexArray_Dummy );
//                        glDrawArrays ( GL_POINTS , 0 , 1 );
//                        glBindVertexArray ( 0 );
//
//                        glDrawBuffer ( GL_COLOR_ATTACHMENT0 + 1 );
//
//                        meanFilter->renderTexture ( depthFBO->bindAttachment ( 0 ) , meanFilterSize_ );
//
//                        depthFBO->unbindAll ( );
//
//                        glDrawBuffer ( GL_BACK );
//                        depthFBO->unbindAll ( );
//
//                        glDrawBuffer ( GL_BACK );
                }


                glFinish();
                accumulateCutawayGenerationTime_ += (float)cutawayGenerationTime_.elapsed();



                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc(GL_LESS);
                glClearDepth(1.0f);
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                // SSAO -- C&G

                fboSSAO->clearAttachments();
                fboSSAO->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

                        renderingCutawayTime_.start();
                        if ( draw_secondary )
                        {
                                drawSecondaryStatic( );

                        }
                        glFinish();
                        accumulateRenderingCutawayTime_ += (float)renderingCutawayTime_.elapsed();
                        if ( draw_primary )
                        {
                                drawPrimaryStatic( );

                        }

                glDrawBuffer(GL_BACK);

                glDisable(GL_DEPTH_TEST);

                /// SEGUNDO PASSO

                ssaoShader->enable();

                glActiveTexture(GL_TEXTURE0 + 7);
                glBindTexture(GL_TEXTURE_2D, noiseTexture);

                ssaoShader->setUniform("lightViewMatrix", trackball_->getProjectionMatrix().data(),4,GL_FALSE,1);

                ssaoShader->setUniform("noiseScale", noise_scale.x(),noise_scale.y());
                ssaoShader->setUniform("kernel", kernel, 2, numberOfSamples);

                ssaoShader->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
                ssaoShader->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
                ssaoShader->setUniform("colorTexture", fboSSAO->bindAttachment(colorTextureID));
                ssaoShader->setUniform("displayAmbientPass", displayAmbientPass);

                ssaoShader->setUniform("radius", radius);
                ssaoShader->setUniform("intensity", (float)intensity);
                ssaoShader->setUniform("max_dist", max_dist);
                ssaoShader->setUniform("noiseTexture", 7);

                //Second pass mesh rendering:
                quadSSAO->render();

                ssaoShader->disable();
                glBindTexture(GL_TEXTURE_2D, 0);
                fboSSAO->unbindAll();
                //fboSSAO->clearDepth();

                glEnable(GL_DEPTH_TEST);


                if ( reservoir_model_.showBorderLine )
                {

                        glLineWidth( (float) borderLinesSize_ );
                        BorderLines_->enable ( );

                        BorderLines_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                        BorderLines_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                        BorderLines_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                        reservoir_model_.drawFaces ( );

                        BorderLines_->disable ( );
                }


                if (cutawayPass_ >= 100.0)
                {

                        emit cutawayGenerationTime( QString::number( 1000.0f/(accumulateCutawayGenerationTime_*0.01f) ) ) ;
                        emit renderingCutawayTime( QString::number( 1000.0f/(accumulateRenderingCutawayTime_*0.01f) ) ) ;
                        accumulateCutawayGenerationTime_ = 0.0f;
                        accumulateRenderingCutawayTime_ = 0.0f;
                        cutawayPass_ = 0.0f;

                }else
                {
                        cutawayPass_ += 1.0f;
                }

        }

}


void GLWidget::drawIRESCutawayDynamicSurface ( )
{

        glDepthFunc ( GL_GREATER );
        glClearDepth ( 0.0 );


        depthFBO->bind( );

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        //glDepthMask(GL_FALSE);

//        glDisable(GL_DEPTH_TEST);

        IRESCutawaySurfaceDynamic_->enable( );

        IRESCutawaySurfaceDynamic_->setUniform( "v0" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v1" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v2" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v3" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);

        IRESCutawaySurfaceDynamic_->setUniform( "v4" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v5" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v6" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        IRESCutawaySurfaceDynamic_->setUniform( "v7" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);


        IRESCutawaySurfaceDynamic_->setUniform("move_x", move_x);
        IRESCutawaySurfaceDynamic_->setUniform("move_y", move_y);
        IRESCutawaySurfaceDynamic_->setUniform("move_z", move_z);
        IRESCutawaySurfaceDynamic_->setUniform( "x" , volume_width );
        IRESCutawaySurfaceDynamic_->setUniform( "y" , volume_height );
        IRESCutawaySurfaceDynamic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        IRESCutawaySurfaceDynamic_->setUniform ("freeze", freezeView_ );
        IRESCutawaySurfaceDynamic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        glBindVertexArray(vertexArray_Dummy);
                glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        IRESCutawaySurfaceDynamic_->disable( );

        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

        glDisable(GL_DEPTH_TEST);
        meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

        depthFBO->unbindAll();

        glEnable(GL_DEPTH_TEST);
        glDrawBuffer(GL_BACK);

        //glDepthMask(GL_TRUE);
}

void GLWidget::drawPrimaryDynamic ( ) const
{


        IRESPrimaryDynamic_->enable( );

        IRESPrimaryDynamic_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESPrimaryDynamic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESPrimaryDynamic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        IRESPrimaryDynamic_->setUniform("min_range", min_range  );
        IRESPrimaryDynamic_->setUniform("max_range", max_range  );

        IRESPrimaryDynamic_->setUniform("paper", 1.0  );
        IRESPrimaryDynamic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        IRESPrimaryDynamic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        IRESPrimaryDynamic_->setUniform("move_x", move_x  );
        IRESPrimaryDynamic_->setUniform("move_y", move_y  );
        IRESPrimaryDynamic_->setUniform("move_z", move_z  );

        IRESPrimaryDynamic_->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESPrimaryDynamic_->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
//
//        IRESPrimaryDynamic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//        IRESPrimaryDynamic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//        IRESPrimaryDynamic_->setUniform("property_index", reservoir_model_.current_static );

        IRESPrimaryDynamic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryDynamic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESPrimaryDynamic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawCuboid ( );

        IRESPrimaryDynamic_->disable( );

}

void GLWidget::drawSecondaryDynamic ( ) const
{

        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glDepthFunc ( GL_LESS );
        glClearDepth ( 1.0f );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        //drawBackGround ( );


        // Interior Cells

        IRESCutawayDynamicCrust_->enable( );

        IRESCutawayDynamicCrust_->setUniform( "normals" , depthFBO->bindAttachment(1) );

        IRESCutawayDynamicCrust_->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESCutawayDynamicCrust_->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        IRESCutawayDynamicCrust_->setUniform ( "property_index" , reservoir_model_.current_static );
        IRESCutawayDynamicCrust_->setUniform ( "time_step" , time_step );

        IRESCutawayDynamicCrust_->setUniform ( "Wall" , wall_ );
        IRESCutawayDynamicCrust_->setUniform ( "Line" , line_ );
        IRESCutawayDynamicCrust_->setUniform ( "isPerspective_" , isPerspective_ );
        IRESCutawayDynamicCrust_->setUniform ( "nearPlane_" , nearPlane_ );
        IRESCutawayDynamicCrust_->setUniform ( "farPlane_" , farPlane_ );

        IRESCutawayDynamicCrust_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        IRESCutawayDynamicCrust_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        IRESCutawayDynamicCrust_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        IRESCutawayDynamicCrust_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicCrust_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicCrust_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawCuboid ( );

        IRESCutawayDynamicCrust_->disable( );


        IRESCutawayDynamicShell_->enable( );

        IRESCutawayDynamicShell_->setUniform ( "normals" , depthFBO->bindAttachment(1) );

        IRESCutawayDynamicShell_->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESCutawayDynamicShell_->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        IRESCutawayDynamicShell_->setUniform ( "faults" , reservoir_model_.showFault );

        IRESCutawayDynamicShell_->setUniform ( "isPerspective_" , isPerspective_ );
        IRESCutawayDynamicShell_->setUniform ( "nearPlane_" , nearPlane_ );
        IRESCutawayDynamicShell_->setUniform ( "farPlane_" , farPlane_ );
        IRESCutawayDynamicShell_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        IRESCutawayDynamicShell_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        IRESCutawayDynamicShell_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        IRESCutawayDynamicShell_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicShell_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        IRESCutawayDynamicShell_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawFaces( );

        IRESCutawayDynamicShell_->disable( );


        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayDynamic ( )
{

        //      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {

                if ( draw_cutaway_surface_ )
                {
                        drawIRESCutawayDynamicSurface ( );
                }else
                {
                        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                        glDepthFunc ( GL_LESS );
                        glClearDepth ( 1.0f );
                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//                        depthFBO->bind( );
//
//                        glDrawBuffer(GL_COLOR_ATTACHMENT0);
//
//                        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
//                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//                        glDepthFunc ( GL_GREATER );
//                        glClearDepth ( 0.0 );
//
//                        depthFBO->bind( );

                        glDrawBuffer(GL_COLOR_ATTACHMENT0);

                        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


                        glBindVertexArray(vertexArray_Dummy);
                                glDrawArrays(GL_POINTS,0,1);
                        glBindVertexArray(0);


                        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

                        meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

                        depthFBO->unbindAll();


                        glDrawBuffer(GL_BACK);
                        depthFBO->unbindAll();


                        glDrawBuffer(GL_BACK);
                }


                glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
                glDepthFunc ( GL_LESS );
                glClearDepth ( 1.0f );
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                if ( draw_secondary )
                {
                        drawSecondaryDynamic( );
                }

                if ( draw_primary )
                {
                        if (isply_)
                                PaperPly();
                        else
                                drawPrimaryDynamic();
                }

                if ( reservoir_model_.showBorderLine )
                {

                        glLineWidth( (float) borderLinesSize_ );
                        BorderLines_->enable ( );

                        BorderLines_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                        BorderLines_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                        BorderLines_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                        reservoir_model_.drawFaces ( );

                        BorderLines_->disable ( );
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

        // Surface Faces

        RawModel_->enable( );

        RawModel_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        RawModel_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        RawModel_->setUniform("property_index", reservoir_model_.current_static );
        RawModel_->setUniform("faults", reservoir_model_.showFault );
        RawModel_->setUniform("justWireFrame", justWireFrame );
        RawModel_->setUniform("displacement", displacement[0],displacement[1],displacement[2] );

        RawModel_->setUniform("num_lights", (GLint) lights.size ( )  );
        RawModel_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        RawModel_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        RawModel_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        RawModel_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        RawModel_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        RawModel_->disable( );


        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->enable ( );

                BorderLines_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                BorderLines_->disable ( );
        }

//	rawShellLCG->enable();
//	rawShellLCG->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
//	rawShellLCG->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
//	rawShellLCG->setUniform("property_index", reservoir_model_.current_static );
//
//        rawShellLCG->setUniform("num_lights", (GLint) lights.size ( )  );
//        rawShellLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//        rawShellLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
//
//        rawShellLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        rawShellLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        rawShellLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        reservoir_model_.drawInternalFaces( );
//
//        rawShellLCG->disable();


}

void GLWidget::drawBackGround ( ) const
{
	/// FIXME Conditions  - Just the model opened.

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	BackGround_->enable();

	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture ( GL_TEXTURE_2D, xtoon_texture_ );
	BackGround_->setUniform("imageTexture", 3);
	BackGround_->setUniform("viewportSize", width(), height() );

	picture->render();

	BackGround_->disable();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void GLWidget::PaperDemo() const
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
                        PaperSecondary( );
                }
                if ( draw_primary )
                {

                        PaperPly();//PaperPrimary( );

                }

                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_BLEND);
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

                DummyQuad_->enable();

                DummyQuad_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
                DummyQuad_->setUniform("displacement", displacement[0],displacement[1],displacement[2] );
                DummyQuad_->setUniform("max_displacement", max_displacement[0],max_displacement[1],max_displacement[2] );
                DummyQuad_->setUniform("num_lights", (GLint) lights.size ( )  );
                DummyQuad_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
                DummyQuad_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
                DummyQuad_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
                DummyQuad_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

                glBindVertexArray( vertexArray_Dummy );
                glDrawArrays( GL_POINTS, 0, 1 );
                glBindVertexArray( 0);

                DummyQuad_->disable();
                glDisable(GL_BLEND);
        }
}

void GLWidget::PaperDrawCutawaySurface() const
{


        glDepthFunc ( GL_GREATER );
        glClearDepth ( 0.0 );

        depthFBO->bind( );

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


        BurnsPrimarySetup_->enable( );

        BurnsPrimarySetup_->setUniform( "v0" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
        BurnsPrimarySetup_->setUniform( "v1" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup_->setUniform( "v2" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup_->setUniform( "v3" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);

        BurnsPrimarySetup_->setUniform( "v4" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);
        BurnsPrimarySetup_->setUniform( "v5" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup_->setUniform( "v6" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
        BurnsPrimarySetup_->setUniform( "v7" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);


        BurnsPrimarySetup_->setUniform("move_x", move_x );
        BurnsPrimarySetup_->setUniform("move_y", move_y );
        BurnsPrimarySetup_->setUniform("move_z", move_z );
        BurnsPrimarySetup_->setUniform( "x" , volume_width );
        BurnsPrimarySetup_->setUniform( "y" , volume_height );
        BurnsPrimarySetup_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimarySetup_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimarySetup_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        BurnsPrimarySetup_->setUniform ("freeze", freezeView_ );
        BurnsPrimarySetup_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        glBindVertexArray(vertexArray_Dummy);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        BurnsPrimarySetup_->disable( );


        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

        meanFilter->renderTexture( depthFBO->bindAttachment(0),meanFilterSize_);

        depthFBO->unbindAll();


        glDrawBuffer(GL_BACK);

}

void GLWidget::PaperSecondary( ) const
{

        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround( );

        // Surface Faces

        BurnsSecondary_->enable( );

        BurnsSecondary_->setUniform("min_StaticProperty", reservoir_model_.static_min[reservoir_model_.current_static]  );
        BurnsSecondary_->setUniform("max_StaticProperty", reservoir_model_.static_max[reservoir_model_.current_static]  );
        BurnsSecondary_->setUniform("property_index", reservoir_model_.current_static );
        BurnsSecondary_->setUniform("min_DynamicProperty", reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        BurnsSecondary_->setUniform("max_DynamicProperty", reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        BurnsSecondary_->setUniform("property_type", 0.0 );


        BurnsSecondary_->setUniform("faults", reservoir_model_.showFault );
        BurnsSecondary_->setUniform("Wireframe", reservoir_model_.showWireFrame );
        BurnsSecondary_->setUniform("displacement", displacement[0],displacement[1],displacement[2] );

        BurnsSecondary_->setUniform("num_lights", (GLint) lights.size ( )  );
        BurnsSecondary_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        BurnsSecondary_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        BurnsSecondary_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsSecondary_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsSecondary_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();

        BurnsSecondary_->disable( );


        glEnable(GL_BLEND);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->enable ( );

                BorderLines_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                BorderLines_->disable ( );
        }
        glDisable(GL_BLEND);


}

void GLWidget::PaperPrimary( ) const
{

        BurnsPrimary_->enable( );

        BurnsPrimary_->setUniform("num_lights", (GLint) lights.size ( )  );
        BurnsPrimary_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        BurnsPrimary_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        BurnsPrimary_->setUniform("min_range", min_range  );
        BurnsPrimary_->setUniform("max_range", max_range  );

        BurnsPrimary_->setUniform("paper", 1.0  );
        BurnsPrimary_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        BurnsPrimary_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        BurnsPrimary_->setUniform("min_StaticProperty", reservoir_model_.static_min[reservoir_model_.current_static]  );
        BurnsPrimary_->setUniform("max_StaticProperty", reservoir_model_.static_max[reservoir_model_.current_static]  );
        BurnsPrimary_->setUniform("property_index", reservoir_model_.current_static );
        BurnsPrimary_->setUniform("min_DynamicProperty", reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        BurnsPrimary_->setUniform("max_DynamicProperty", reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        BurnsPrimary_->setUniform("property_type", 0.0 );

        BurnsPrimary_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimary_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsPrimary_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawCuboid();

        BurnsPrimary_->disable( );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->enable ( );

                BorderLines_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
                BorderLines_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

                reservoir_model_.drawFaces ( );

                BorderLines_->disable ( );
        }

}

void GLWidget::PaperPly ( ) const
{
        /// FIXME Conditions - Just the model opened.

//        BurnsPly_->enable();
//
//        BurnsPly_->setUniform("move_x", move_x );
//        BurnsPly_->setUniform("move_y", move_y );
//        BurnsPly_->setUniform("move_z", move_z );
//
//        BurnsPly_->setUniform("move", (float) borderLinesSize_ );
//        BurnsPly_->setUniform("num_lights", (GLint) lights.size ( )  );
//        BurnsPly_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//        BurnsPly_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        BurnsPly_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        BurnsPly_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        ply_primary_.Draw();
//
//        BurnsPly_->disable();


        // Bind buffer to store coord, normal and color information
        fboSSAO->clearAttachments();
        fboSSAO->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

        deferredShader->enable();
        deferredShader->setUniform("projectionMatrix", trackball_->getProjectionMatrix().data(),4,GL_FALSE,1);
        deferredShader->setUniform("modelMatrix",trackball_->getModelMatrix().data(),4,GL_FALSE,1);
        deferredShader->setUniform("viewMatrix", trackball_->getViewMatrix().data(),4,GL_FALSE,1);
        deferredShader->setUniform("lightViewMatrix", trackball_->getViewMatrix().data(),4,GL_FALSE,1);
//      deferredShader->setUniform("has_color", mesh->hasAttribute("in_Color"));

        ply_primary_.Draw();

        deferredShader->disable();
        fboSSAO->unbindAll();
        fboSSAO->clearDepth();

        glDrawBuffer(GL_BACK);

        /// SEGUNDO PASSO

        ssaoShader->enable();

        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        ssaoShader->setUniform("lightViewMatrix", trackball_->getProjectionMatrix().data(),4,GL_FALSE,1);

        ssaoShader->setUniform("noiseScale", noise_scale.x(),noise_scale.y());
        ssaoShader->setUniform("kernel", kernel, 2, numberOfSamples);

        ssaoShader->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
        ssaoShader->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
        ssaoShader->setUniform("colorTexture", fboSSAO->bindAttachment(colorTextureID));
        ssaoShader->setUniform("displayAmbientPass", displayAmbientPass);

        ssaoShader->setUniform("radius", radius);
        ssaoShader->setUniform("intensity", (float)intensity);
        ssaoShader->setUniform("max_dist", max_dist);
        ssaoShader->setUniform("noiseTexture", 7);

        //Second pass mesh rendering:
        quadSSAO->render();

        ssaoShader->disable();
        glBindTexture(GL_TEXTURE_2D, 0);
        fboSSAO->unbindAll();
        fboSSAO->clearDepth();

        // THIRD PASS
        blurShader->enable();

        blurShader->setUniform("blurTexture", fboSSAO->bindAttachment(blurTextureID));
        blurShader->setUniform("blurRange", blurRange);

        quadSSAO->render();

        blurShader->disable();

        fboSSAO->unbindAll();

}


float GLWidget::clamp(float x, float a, float b)

{

    return x < a ? a : (x > b ? b : x);

}

float GLWidget::smoothstep(float x)
{

        if (x > 1)
            x = 1.0;
       else if (x < 0)
            x = 0.0;
        // Evaluate polynomial
      return (x*x*x) * (x * (x * 6 - 15) + 10);

}

void GLWidget::gameLooping ( )
{

        if ( zoom_start_ )
        {
                if ( zoom_increment_ > 0)
                        trackball_->increaseZoom ( 1.01 );
                else
                        trackball_->decreaseZoom( 1.01 );
                update();
        }

        //
        if ( play_ )
        {

                if ( time_steps_ <= 1.0 )
                {
//                        std::cout << "fps :" << time_steps_  << std::endl;
                        trackball_->setQuaternion(this->squad(sourcePosition_,sourcePositionTangent_,targetPositionTangent_,targetPosition_,time_steps_));
                        //trackball_->setQuaternion(this->slerp(sourcePosition_,targetPosition_,time_steps_,false));

                        //std::cout << " mTagent " << takes_[take_index_].keyframes_[takes_[take_index_].nextKeyframe_].coeffs() << std::endl;

                        //time_steps_ += time_interval_;

                        time_steps_ += time_interval_;

                        update();

                }
                else if ( takes_[take_index_].nextKeyframe_ < takes_[take_index_].number_of_keyframes_-1 )
                {
//                        std::cout << " nextKeyFrame " << nextKeyframe_ << std::endl;
//
//                        std::cout << " mTagent " << takes_[take_index_].tangents_[nextKeyframe_].coeffs() << std::endl;
//                        std::cout << " Rot " << keyframes_[nextKeyframe_].coeffs() << std::endl;


                        takes_[take_index_].nextKeyframe_++;

                        sourcePosition_ = targetPosition_;
                        targetPosition_ = takes_[take_index_].keyframes_[takes_[take_index_].nextKeyframe_];

                        sourcePositionTangent_ = targetPositionTangent_;
                        targetPositionTangent_ = takes_[take_index_].tangents_[takes_[take_index_].nextKeyframe_];

                        float angular = sourcePosition_.angularDistance(targetPosition_);

                        std::cout << "Angular Distance : " << angular << std::endl;

                        time_interval_ = 0.01;
                                                                                                                                                        ;

                        time_steps_ = 0.0f*time_interval_;

                        std::cout << " Angular Distance " << angular << std::endl;

//                        sourcePosition_.normalize();
//                        targetPosition_.normalize();
//
//                        sourcePositionTangent_.normalize();
//                        targetPositionTangent_.normalize();

                }
                else
                {

                        play_ = false;
                        takes_[take_index_].nextKeyframe_ = 0;
//                        std::cout << " Angular Distance " << angular << std::endl;
                }

        }


     fps++;

}

void GLWidget::fpsCounter ( )
{

        //std::cout << "fps :" << fps  << std::endl;

	fps = 0;

	delta_time.restart();
}

void GLWidget::reloadShaders ( )
{
        // Effects --
        BorderLines_->reloadShaders ( );
        xtoon_texture_viewer->reloadShaders ( );
        BackGround_->reloadShaders ( );
        // Paper Demo
        RawShell_->reloadShaders ( );
        RawModel_->reloadShaders ( );

        BurnsPrimary_->reloadShaders ( );
        BurnsPrimarySetup_->reloadShaders ( );
        BurnsSecondary_->reloadShaders ( );
        DummyQuad_->reloadShaders ( );

        // ! DYNAMIC VIEWER F11 Static PropertieswayStatic"
        IRESCutawaySurfaceStatic_->reloadShaders ( );
        IRESCutawayStaticShell_->reloadShaders ( );
        IRESCutawayStatic_->reloadShaders ( );
        IRESPrimaryStatic_->reloadShaders ( );

        // ! DYNAMIC VIEWER F12 Dynamic Properties

        IRESCutawayDynamic_->reloadShaders ( );
        IRESCutawaySurfaceDynamic_->reloadShaders ( );
        IRESCutawayDynamicCrust_->reloadShaders ( );
        IRESCutawayDynamicShell_->reloadShaders ( );
        IRESPrimaryDynamic_->reloadShaders ( );

        BurnsPly_->reloadShaders ( );

        // SSAO

        ssaoShader->reloadShaders ( );
        blurShader->reloadShaders ( );
        deferredShader->reloadShaders ( );
        SSAOIRESPrimaryStatic_->reloadShaders();
        SSAOIRESCutawayStatic_->reloadShaders();
        SSAOIRESCutawayStaticShell_->reloadShaders();

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
                BorderLines_ = new Shader ("Border Lines", (shaderDirectory + "BorderLines.vert").toStdString(),
                                            (shaderDirectory + "BorderLines.frag").toStdString(),
                                            (shaderDirectory + "BorderLines.geom").toStdString(),1);
                BorderLines_->initialize();

                xtoon_texture_viewer = new Shader  ("xtoon_texture", (shaderDirectory + "xtoon_texture.vert").toStdString(),
                                                              (shaderDirectory + "xtoon_texture.frag").toStdString(),"",1);
                xtoon_texture_viewer->initialize( );

                meanFilter->setShadersDir( shaderDirectory.toStdString() );
                meanFilter->initialize( );

                BackGround_ = new Shader  ("BackGround", (shaderDirectory + "BackGround.vert").toStdString(),
                                         (shaderDirectory + "BackGround.frag").toStdString(),"",1);
                BackGround_->initialize( );
        // Paper Demo


                RawShell_ = new Shader ("Shell",
                                         (shaderDirectory + "RawShell.vert").toStdString(),
                                         (shaderDirectory + "RawShell.frag").toStdString(),
                                          "",1);

                RawShell_->initialize( );

                RawModel_ = new Shader ("Raw Model",
                                         (shaderDirectory + "RawModel.vert").toStdString(),
                                         (shaderDirectory + "RawModel.frag").toStdString(),
                                         (shaderDirectory + "RawModel.geom").toStdString(),1);

                RawModel_->initialize( );

                BurnsPrimary_ = new Shader ("BurnsPrimary",
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.vert").toStdString(),
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.frag").toStdString(),
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.geom").toStdString(),1);


                BurnsPrimary_->initialize( );

                BurnsPrimarySetup_ = new Shader ("BurnsPrimarySetup",
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.vert").toStdString(),
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.frag").toStdString(),
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.geom").toStdString(),1);
                BurnsPrimarySetup_->initialize( );

                BurnsSecondary_ = new Shader ("BurnsSecondary",
                                         (shaderDirectory + "Demo/BurnsSecondary.vert").toStdString(),
                                         (shaderDirectory + "Demo/BurnsSecondary.frag").toStdString(),
                                         (shaderDirectory + "Demo/BurnsSecondary.geom").toStdString(),1);

                BurnsSecondary_->initialize( );

                DummyQuad_ = new Shader ("DummyQuad",
                                        (shaderDirectory + "Demo/DummyQuad.vert").toStdString(),
                                        (shaderDirectory + "Demo/DummyQuad.frag").toStdString(),
                                        (shaderDirectory + "Demo/DummyQuad.geom").toStdString(),1);
                DummyQuad_->initialize( );

       // ! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic_ = new Shader ("IRESCutawaySurfaceStatic",
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.vert").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.frag").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.geom").toStdString(),1);
                IRESCutawaySurfaceStatic_->initialize( );

                IRESCutawayStaticShell_ = new Shader ("IRESCutawayStaticShell",
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.vert").toStdString(),
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.frag").toStdString(),
                                                     (shaderDirectory + "Static/IRESCutawayStaticShell.geom").toStdString(),1);
                IRESCutawayStaticShell_->initialize();

                IRESCutawayStatic_ = new Shader ("IRESCutawayStatic",
                                                (shaderDirectory + "Static/IRESCutawayStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/IRESCutawayStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/IRESCutawayStatic.geom").toStdString()
                                                 ,1);
                IRESCutawayStatic_->initialize();

                IRESPrimaryStatic_ = new Shader ("IRESPrimaryStatic",
                                                (shaderDirectory + "Static/IRESPrimaryStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/IRESPrimaryStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/IRESPrimaryStatic.geom").toStdString(),1);

                IRESPrimaryStatic_->initialize();


        // ! DYNAMIC VIEWER F12 Dynamic Properties

                IRESCutawayDynamic_ = new Shader ("IRESCutawayDynamic",
                                                (shaderDirectory + "Dynamic/IRESCutawayDynamic.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/IRESCutawayDynamic.frag").toStdString(),
                                                "",1);
                IRESCutawayDynamic_->initialize();

                IRESCutawaySurfaceDynamic_ = new Shader ("IRESCutawaySurfaceDynamic",
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.geom").toStdString(),1);
                IRESCutawaySurfaceDynamic_->initialize();

                IRESCutawayDynamicCrust_ = new Shader ("IRESCutawayDynamicCrust",
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicCrust.geom").toStdString(),1);
                IRESCutawayDynamicCrust_->initialize();


                IRESCutawayDynamicShell_ = new Shader ("IRESCutawayDynamicShell",
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawayDynamicShell.geom").toStdString(),1);
                IRESCutawayDynamicShell_->initialize();

                IRESPrimaryDynamic_ = new Shader ("IRESPrimaryDynamic",
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESPrimaryDynamic.geom").toStdString(),1);
                IRESPrimaryDynamic_->initialize();


                BurnsPly_ = new Shader ("BurnsPly",
                                       (shaderDirectory + "Demo/BurnsPrimary430.vert").toStdString(),
                                       (shaderDirectory + "Demo/BurnsPrimary430.frag").toStdString(),
                                        "",1);
                BurnsPly_->initialize( );

                // SSAO

                /// The per pixel AO computation shader

                ssaoShader = new Shader ("ssaoShader", (shaderDirectory + "SSAO/ssao.vert").toStdString(),
                                                         (shaderDirectory + "SSAO/ssao.frag").toStdString(),
                                            "",1);
                ssaoShader->initialize();

                ///
                deferredShader = new Shader ("deferredShader", (shaderDirectory + "SSAO/viewspacebuffer.vert").toStdString(),
                                                             (shaderDirectory + "SSAO/viewspacebuffer.frag").toStdString(),
                                                             "",1);
                deferredShader->initialize();

                ///
                blurShader = new Shader ("blurShader", (shaderDirectory + "SSAO/guassianblurfilter.vert").toStdString(),
                                                         (shaderDirectory + "SSAO/guassianblurfilter.frag").toStdString(),
                                                             "",1);
                blurShader->initialize();

                /// SSAO Primary

                SSAOIRESPrimaryStatic_ = new Shader ("IRESPrimaryStatic",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.geom").toStdString(),1);

                SSAOIRESPrimaryStatic_->initialize();

                SSAOIRESCutawayStatic_ = new Shader ("SSAOIRESCutawayStatic_",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.geom").toStdString(),1);

                SSAOIRESCutawayStatic_->initialize();

                SSAOIRESCutawayStaticShell_ = new Shader ("SSAOIRESCutawayStaticShell_",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.geom").toStdString(),1);

                SSAOIRESCutawayStaticShell_->initialize();




}
/// KeyInput
void GLWidget::processMultiKeys ( )
{

	foreach( int key , keysPresseds_)
	{

		//std::cout  << camera_.position();

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
		if ( key == Qt::Key_T)
		{
		        if ( displacement[0] < max_displacement[0])
		                displacement[0] += 0.1;
		}
		if ( key == Qt::Key_Y)
		{
                        if ( displacement[0] > -max_displacement[0])
                                displacement[0] -= 0.1;
		}
                if ( key == Qt::Key_G)
                {
                        if ( displacement[1] < max_displacement[1])
                        displacement[1] += 0.1;
                }
                if ( key == Qt::Key_H)
                {
                        if ( displacement[1] > -max_displacement[1])
                        displacement[1] -= 0.1;
                }
                if ( key == Qt::Key_V)
                {
                        if ( displacement[2] < max_displacement[2])
                        displacement[2] += 0.1;
                }
                if ( key == Qt::Key_B)
                {
                        if ( displacement[2] > -max_displacement[2])
                        displacement[2] -= 0.1;
                }
	}
}

void GLWidget::keyPressEvent ( QKeyEvent * event )
{

	buttonRelease_ = true;
	keysPresseds_ += event->key ( );

	if ( event->key() == Qt::Key_F5 )
	{
		reloadShaders();
	}
	else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_W) )
	{
	        move_y += 0.1;
	}
        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_S) )
        {
                move_y -= 0.1;
        }
        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_D) )
        {
                move_x += 0.1;
        }
        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_A) )
        {
                move_x -= 0.1;
        }
        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_Q) )
        {
                move_z += 0.1;
        }
        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_E) )
        {
                move_z -= 0.1;
        }

        else if ( (event->modifiers() == Qt::ControlModifier ) && (event->key() == Qt::Key_S) )
        {
                flush();
        }
	else if ( (event->modifiers() == Qt::ShiftModifier ) && (event->key() == Qt::Key_R) )
	{
                trackball_->reset();
                trackball_->decreaseZoom( 5.05 );
                //  Animation
                        takes_[take_index_].number_of_keyframes_    = 0;
                        takes_[take_index_].nextKeyframe_           = 0;
                        play_                   = false;
                        time_steps_             = 0.0;
                        videoSequence           = 0;

                update();
	}
        else if ( (event->modifiers() == Qt::ShiftModifier ) && (event->key() == Qt::Key_P) )
        {
                isPerspective_ = !isPerspective_;

                float aspect = (float) width() / height();

                if ( isPerspective_ )
                        trackball_->usePerspectiveMatrix  ( 45.0f , aspect , nearPlane_ , farPlane_ );
                else
                        trackball_->useOrthographicMatrix ( -1.0f*aspect , 1.0f*aspect , -1.0f , 1.0 , 0.1f , 500.0f );

                /// Clear All color/depth buffer from the current framebuffer
                depthFBO->clearAttachments();
                fboSSAO->clearAttachments();

        }
        else if ( event->key() == Qt::Key_1)
        {
                take_index_ = 0;
                std::cout << " take_index_ :" << take_index_ << std::endl;
        }
        else if ( event->key() == Qt::Key_2)
        {
                take_index_ = 1;
                std::cout << " take_index_ :" << take_index_ << std::endl;
        }
        else if ( event->key() == Qt::Key_3)
        {
                take_index_ = 2;
                std::cout << " take_index_ :" << take_index_ << std::endl;
        }
        else if ( event->key() == Qt::Key_4)
        {
                take_index_ = 3;
                std::cout << " take_index_ :" << take_index_ << std::endl;
        }
	else if ( event->key() == Qt::Key_7)
	{
	        position_one = trackball_->getViewMatrix();
	}
        else if ( event->key() == Qt::Key_8)
        {
                position_two = trackball_->getViewMatrix();
        }
        else if ( event->key() == Qt::Key_9)
        {
                trackball_->setViewMatrix( position_one );
        }
        else if ( event->key() == Qt::Key_0)
        {
                trackball_->setViewMatrix( position_two );
        }
	else if ( event->key() == Qt::Key_P )
        {
	        setPlay(take_index_);

        }
        else if ( event->key() == Qt::Key_Q )
        {
//                sourcePosition_ = takes_[i].keyframes_[0];
//                targetPosition_ = takes_[i].keyframes_[1];
//
//                sourcePositionTangent_ = takes_[i].tangents_[0];
//                targetPositionTangent_ = takes_[i].tangents_[1];
//
//                std::cout << " Quaternion :" << takes_[i].keyframes_[0].coeffs() << std::endl;
//                std::cout << " Quaternion :" << takes_[i].keyframes_[1].coeffs() << std::endl;

        }
        else if ( event->key() == Qt::Key_W )
        {
//                sourcePosition_ = takes_[i].keyframes_[1];
//                targetPosition_ = takes_[i].keyframes_[2];
//
//                sourcePositionTangent_ = takes_[i].tangents_[1];
//                targetPositionTangent_ = takes_[i].tangents_[2];
//
//                std::cout << " Quaternion :" << takes_[i].keyframes_[1].coeffs() << std::endl;
//                std::cout << " Quaternion :" << takes_[i].keyframes_[2].coeffs() << std::endl;

        }
        else if ( event->key() == Qt::Key_Space)
        {
                takes_[take_index_].keyframes_[takes_[take_index_].number_of_keyframes_] = (trackball_->getQuaternion());

                takes_[take_index_].number_of_keyframes_++;

                std::cout << " Size :" << takes_[take_index_].number_of_keyframes_ << std::endl;
                std::cout << " Quaternion :" << takes_[take_index_].keyframes_[takes_[take_index_].number_of_keyframes_-1].coeffs() << std::endl;
        }
        else if ( event->key() == Qt::Key_L)
        {
                isply_ = !isply_;
        }else if ( event->key() == Qt::Key_Z)
        {
                if (wall_ == 0)
                        wall_ = 1;
                else
                        wall_ = 0;
        }else if ( event->key() == Qt::Key_X)
        {
                if (line_ == 0)
                        line_ = 1;
                else
                        line_ = 0;
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
	event->ignore();
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
	if ( event->button ( ) == Qt::MiddleButton )
	{
	        zoom_start_ = true;
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
	if ( event->button ( ) == Qt::MiddleButton )
	{
	          zoom_start_ = false;
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
			//trackball_->increaseZoom ( 1.05 );
		        zoom_increment_ = 1.01;
		}
		else
		{
			//trackball_->decreaseZoom ( 1.05 );
			zoom_increment_ = -1.01;
		}

		orthoZoom += event->delta ( ) / 1200.0;
	}
	else
	{
	        int numDegrees = event->delta() / 8;
	        int numSteps = numDegrees / 15;

	        //if  ( ( event->delta ( ) > 0.0 ) && ( event->delta ( ) > 90.0 ) )
	        {
	                qDebug() << numDegrees;
	        }
	}
	event->accept();

    update();

}

void GLWidget::showFault	  ( bool visibility )
{
	if ( visibility )
		reservoir_model_.showFault = 1;
	else
		reservoir_model_.showFault = 0;
}

void GLWidget::showWireframe      ( bool visibility )
{
        if ( visibility )
                reservoir_model_.showWireFrame = 1;
        else
                reservoir_model_.showWireFrame = 0;
}

void GLWidget::showBorderLines    ( bool visibility )
{
	reservoir_model_.showBorderLine = visibility;;
}

void GLWidget::setPrimaryVisibility( bool visibility )
{
	if ( reservoir_model_.isOpen_ )
		draw_primary = visibility;

	std::cout << " Show Primary " << draw_primary << std::endl;
	update();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( reservoir_model_.isOpen_ )
		draw_secondary = visibility;
	update();
}

void GLWidget::setCutawaySurfaceVisibility( bool visibility )
{
        if ( reservoir_model_.isOpen_ )
                draw_cutaway_surface_ = visibility;

        std::cout << " Show Cutaway Surface " << draw_cutaway_surface_ << std::endl;
        update();
}

void GLWidget::freezeView( )
{
	freezeView_ = !freezeView_;
    if (freezeView_) {
        freeze_viewmatrix_ = trackball_->getViewMatrix().matrix();
    }

}
