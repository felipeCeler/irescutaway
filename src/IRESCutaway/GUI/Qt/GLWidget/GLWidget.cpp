#include <GUI/Qt/GLWidget/GLWidget.hpp>

//#include <QtGui>

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

	isIRESOpen_ = 0;
	time_step   = 0;
	property_type_ = 0;

	wall_ = 0;
	line_ = 0;

	zoom_angle_ = 45.0f;
	fovy_       = 45.0f;
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
        accumulateCutawayGenerationTime_ = 0.0f;
        accumulateRenderingCutawayTime_ = 0.0f;
        accumulateSSAOBlurTime_ = 0.0f;
        accumulateMeanFilterTime_ = 0.0f;

        renderPass_ = 5000;

        cutawayGenerationTimes_.resize(renderPass_);
        renderingCutawayTimes_.resize(renderPass_);
        SSAOBlurCutawayTimes_.resize(renderPass_);
        MeanFilterTimes_.resize(renderPass_);

	renderPass_ = 0;

	accumulateRenderingTimes = 0.0f;

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
        farPlane_  = 15.0f;

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

        isply_          = false;

        /// From libQGLViewer
        AnimationInitializer( );

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

void GLWidget::changePropertyRangeStatic ( const double& minRange, const double& maxRange, int property_index )
{

	min_range_static_ = minRange;
	max_range_static_ = maxRange;

	std::size_t total = reservoir_model_.iresFaces_.size();

	primaries_face = 0;
	primaries_cuboid = 0;

////	for ( std::size_t i = 0; i < reservoir_model_.cuboidStatic.size(); i++)
////	{
////	        if ( (minRange < reservoir_model_.cuboidStatic[i]) && ( reservoir_model_.cuboidStatic[i] <= maxRange) )
////	        {
////	                primaries++;
////	        }
////	}

        for ( std::size_t i = 0; i < reservoir_model_.number_of_blocks_; i++)
        {
                if ( (minRange < reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[i]) && ( reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[i] <= maxRange) )
                {
                        reservoir_model_.indexCuboids[primaries_cuboid] = i;
                        primaries_cuboid++;
                }
        }

        for ( std::size_t i = 0; i < reservoir_model_.iresFaces_.size(); i++)
        {
                if ( (minRange < reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[reservoir_model_.iresFaces_[i].id]) && ( reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[reservoir_model_.iresFaces_[i].id] <= maxRange) )
                {
                        reservoir_model_.indexFaces[primaries_face] = i;
                        primaries_face++;
                }
        }

        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, reservoir_model_.indexBufferFace);
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, primaries_face * sizeof(reservoir_model_.indexFaces[0]), &reservoir_model_.indexFaces[0], GL_STATIC_DRAW );

        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, reservoir_model_.indexBufferCuboid);
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, primaries_cuboid * sizeof(reservoir_model_.indexCuboids[0]), &reservoir_model_.indexCuboids[0], GL_STATIC_DRAW );

        glFinish();

        std::cout << "primaries++" << primaries_cuboid << std::endl;
        std::cout << "primaries++" << primaries_face << std::endl;

	emit primariesPorcentage(QString::number(((float)primaries_face/(float)total)*100));

	reservoir_model_.current_static = property_index;

    update();

}

void GLWidget::changePropertyRangeDynamic ( const double& minRange, const double& maxRange)
{

        min_range_dynamic_ = minRange;
        max_range_dynamic_ = maxRange;

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

	aspect_ = (float) width / height;

	/// @Noob - http://math.hws.edu/graphicsnotes/c3/s5.html near plane can be negative on orthographic avoiding clipping by it.

	if ( isPerspective_ )
	{
	        trackball_->usePerspectiveMatrix  ( 45.0f , aspect_ , nearPlane_ , farPlane_);
	}
	else
	{
	        trackball_->useOrthographicMatrix ( -1.0f*aspect_ , 1.0f*aspect_ , -1.0f , 1.0 , 0.1f , 500.0f );
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

        pen.setWidth ( 2 );
        p.setPen ( pen );

        int left = rect.left ( ) + rect.width ( ) + 10;

        float minmax = reservoir_model_.static_max[reservoir_model_.current_static] - reservoir_model_.static_min[reservoir_model_.current_static];

        float minStatic = 1.0-((min_range_static_-reservoir_model_.static_min[reservoir_model_.current_static])/minmax);

        float maxStatic = 1.0-((max_range_static_-reservoir_model_.static_min[reservoir_model_.current_static])/minmax);

        // Min
        p.drawText ( rect.left ( ) -30 , rect.top ( ) + rect.height ( )*minStatic +10 , "Min" );
        p.drawLine ( width ( ) - relativeWidth - 4 , ( height ( ) - relativeHeight + 1 - relativeSpace*6*(1.0-minStatic)  ),width ( ) - relativeWidth+14, ( height ( ) - relativeHeight + 1 - relativeSpace*6* (1.0-minStatic)  ));        // Max
        // Max
        p.drawText ( rect.left ( ) -30 , rect.top ( ) + rect.height ( )*maxStatic  , "Max" );
        p.drawLine ( width ( ) - relativeWidth - 4 , ( height ( ) - relativeHeight + 1 - relativeSpace*6*(1.0-maxStatic)  ),width ( ) - relativeWidth+14 , ( height ( ) - relativeHeight + 1 - relativeSpace*6* (1.0-maxStatic)  ));

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

        if (renderingPass > 2.0)
        {
                averageFPS = accumulateRenderingTimes / renderingPass;
                renderingPass = 0.0f;
                accumulateRenderingTimes = 0.0f;
        }else
        {
                renderingPass            += 1.0;
                accumulateRenderingTimes += renderTime.elapsed();
        }

        emit fpsChanged( QString::number( averageFPS ) );

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


/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.
/// Real Looping

void GLWidget::drawIRESCutawayStaticSurface ( )
{

	glDepthFunc ( GL_GREATER );
	glClearDepth ( 0.0 );

	/// OFFSCREEN Renderer
	/// DRAW COLOR_BUFFER -->  glDrawBuffers(2,[normalCutawayID_,verticesCutawayID_]);
	depthFBO->clearAttachments( );
	depthFBO->bindRenderBuffers(normalCutawayID_,verticesCutawayID_);

	glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	cutawayGenerationTime_.start ();

	IRESCutawaySurfaceStatic_->enable( );

	IRESCutawaySurfaceStatic_->setUniform("min_range_static", min_range_static_  );
	IRESCutawaySurfaceStatic_->setUniform("max_range_static", max_range_static_  );
	IRESCutawaySurfaceStatic_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
	IRESCutawaySurfaceStatic_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
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

	reservoir_model_.drawCuboid ( );//reservoir_model_.drawIndexCuboids (primaries_cuboid);

	IRESCutawaySurfaceStatic_->disable( );

        glFinish();
        accumulateCutawayGenerationTime_ += (float)cutawayGenerationTime_.elapsed();

	depthFBO->bindRenderBuffers(normalsSmoothID_, verticesSmoothID_);

        glDisable(GL_DEPTH_TEST);

        renderingMeanFilterTime_.start();
        meanFilter->renderTexture( depthFBO->bindAttachment(normalCutawayID_),depthFBO->bindAttachment(verticesCutawayID_),meanFilterSize_);
        depthFBO->unbindAll();

        glFinish( );
        accumulateMeanFilterTime_ += (float)renderingMeanFilterTime_.elapsed();
        glEnable(GL_DEPTH_TEST);


	glDrawBuffer(GL_BACK);

	glFinish();
	glFlush();

}

void GLWidget::drawPrimaryStatic  ( ) const // Draw only primary   Cells
{

        SSAOIRESPrimaryStatic_->enable ( );

        SSAOIRESPrimaryStatic_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStatic_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStatic_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );
        /// Shader Intensity
        SSAOIRESPrimaryStatic_->setUniform ( "saturation_" , this->saturationPrimaries_);
        SSAOIRESPrimaryStatic_->setUniform ( "luminance_"  , this->luminancePrimaries_);

        SSAOIRESPrimaryStatic_->setUniform ( "min_range_static" , min_range_static_ );
        SSAOIRESPrimaryStatic_->setUniform ( "max_range_static" , max_range_static_ );

        SSAOIRESPrimaryStatic_->setUniform ( "paper" , 1.0 );
        SSAOIRESPrimaryStatic_->setUniform ( "box_min" , ply_primary_.box.box_min ( ).x , ply_primary_.box.box_min ( ).y , ply_primary_.box.box_min ( ).z );
        SSAOIRESPrimaryStatic_->setUniform ( "box_max" , ply_primary_.box.box_max ( ).x , ply_primary_.box.box_max ( ).y , ply_primary_.box.box_max ( ).z );

        SSAOIRESPrimaryStatic_->setUniform ( "min_property_static" , reservoir_model_.static_min[reservoir_model_.current_static] );
        SSAOIRESPrimaryStatic_->setUniform ( "max_property_static" , reservoir_model_.static_max[reservoir_model_.current_static] );
        SSAOIRESPrimaryStatic_->setUniform ( "property_index" , reservoir_model_.current_static );

        SSAOIRESPrimaryStatic_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESPrimaryStatic_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESPrimaryStatic_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawFaces ( );

        SSAOIRESPrimaryStatic_->disable ( );

}

void GLWidget::drawSecondaryStatic  ( ) const  // Draw only secondary Cells
{

        /// @TODO According with John McDonald from nvidia at http://youtu.be/-bCeNzgiJ8I?t=31m42s,
        /// this command is costly than just bind a Texture

        // Interior Cells
        SSAOIRESCutawayStatic_->enable( );

        SSAOIRESCutawayStatic_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStatic_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        /// Shader Intensity
        SSAOIRESCutawayStatic_->setUniform ( "saturation_" , this->saturationSecondaries_);
        SSAOIRESCutawayStatic_->setUniform ( "luminance_"  , this->luminanceSecondaries_);

        SSAOIRESCutawayStatic_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );

        SSAOIRESCutawayStatic_->setUniform ( "isPerspective_" , isPerspective_ );
        SSAOIRESCutawayStatic_->setUniform ( "nearPlane_" , nearPlane_ );
        SSAOIRESCutawayStatic_->setUniform ( "farPlane_" , farPlane_ );

        SSAOIRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStatic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStatic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStatic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces( );//reservoir_model_.drawIndexFaces(reservoir_model_.faceCount);//reservoir_model_.drawFaces( );

        SSAOIRESCutawayStatic_->disable( );

        // Surface Faces

        SSAOIRESCutawayStaticShell_->enable( );

        SSAOIRESCutawayStaticShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStaticShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        /// Shader Intensity
        SSAOIRESCutawayStaticShell_->setUniform ( "saturation_" , this->saturationShell_);
        SSAOIRESCutawayStaticShell_->setUniform ( "luminance_"  , this->luminanceShell_);

        SSAOIRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
        SSAOIRESCutawayStaticShell_->setUniform("faults", reservoir_model_.showFault );
        SSAOIRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );

        SSAOIRESCutawayStaticShell_->setUniform ( "isPerspective_" , isPerspective_ );
        SSAOIRESCutawayStaticShell_->setUniform ( "nearPlane_" , nearPlane_ );
        SSAOIRESCutawayStaticShell_->setUniform ( "farPlane_" , farPlane_ );

        SSAOIRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStaticShell_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStaticShell_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESCutawayStaticShell_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawFaces();//reservoir_model_.drawIndexFaces(reservoir_model_.faceCount);

        SSAOIRESCutawayStaticShell_->disable( );

        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayStatic (  )
{

//      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {
//
                if ( draw_cutaway_surface_ )
                {
                        drawIRESCutawayStaticSurface ( );
                }
                else
                {
                        depthFBO->clearAttachments( );
                }

                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc(GL_LESS);
                glClearDepth(1.0f);
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                // SSAO -- C&G

                /// OFFSCREEN Renderer
                /// DRAW COLOR_BUFFER -->  glDrawBuffers(2,[depthTextureID, normalTextureID, colorTextureID]);
                fboSSAO->clearAttachments();
                fboSSAO->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

                        renderingCutawayTime_.start();
                        if ( draw_secondary )
                        {
                                drawSecondaryStatic( );

                        }
                        glFinish();

                        if ( draw_primary )
                        {
                                drawPrimaryStatic( );

                        }
                        accumulateRenderingCutawayTime_ += (float)renderingCutawayTime_.elapsed();
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


                //glDrawBuffer(GL_BACK);

                glDisable(GL_DEPTH_TEST);

                fboSSAO->bindRenderBuffer(blurTextureID);

                /// SEGUNDO PASSO
                renderingSSAOBlurTime_.start();
                ssaoShader_->enable();

                glActiveTexture(GL_TEXTURE0 + 7);
                glBindTexture(GL_TEXTURE_2D, noiseTexture);

                ssaoShader_->setUniform("lightViewMatrix", trackball_->getProjectionMatrix().data(),4,GL_FALSE,1);

                ssaoShader_->setUniform("noiseScale", noise_scale.x(),noise_scale.y());
                ssaoShader_->setUniform("kernel", kernel, 2, numberOfSamples);

                ssaoShader_->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
                ssaoShader_->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
                ssaoShader_->setUniform("colorTexture", fboSSAO->bindAttachment(colorTextureID));
                ssaoShader_->setUniform("displayAmbientPass", displayAmbientPass);

                ssaoShader_->setUniform("radius", radius);
                ssaoShader_->setUniform("intensity", (float)intensity);
                ssaoShader_->setUniform("max_dist", max_dist);
                ssaoShader_->setUniform("noiseTexture", 7);

                //Second pass mesh rendering:
                quadSSAO->render();

                ssaoShader_->disable();
                glBindTexture(GL_TEXTURE_2D, 0);
                fboSSAO->unbindAll ( );
                fboSSAO->clearDepth ( );

                glDrawBuffer(GL_BACK);

                blurShader_->enable();

                blurShader_->setUniform("blurTexture", fboSSAO->bindAttachment(blurTextureID));
                blurShader_->setUniform("blurRange", blurRange);

                quadSSAO->render();

                glFinish( );
                accumulateSSAOBlurTime_ += (float)renderingSSAOBlurTime_.elapsed();

                blurShader_->disable();
                fboSSAO->unbindAll( );
                fboSSAO->clearDepth( );

                glEnable(GL_DEPTH_TEST);

//                if (play_)
//                {
                        if ( cutawayPass_ == 1.0f )
                        {
                                emit cutawayGenerationTime ( QString::number ( ( accumulateCutawayGenerationTime_/cutawayPass_ ) ) );
                                emit renderingCutawayTime ( QString::number ( ( accumulateRenderingCutawayTime_/cutawayPass_ ) ) );
                                emit renderingSSAOBlurTime ( QString::number ( ( accumulateSSAOBlurTime_/cutawayPass_ ) ) );
                                emit renderingMeanFilterTime ( QString::number ( ( accumulateMeanFilterTime_/cutawayPass_ ) ) );

//                                cutawayGenerationTimes_[renderPass_] = accumulateCutawayGenerationTime_/cutawayPass_;
//                                renderingCutawayTimes_[renderPass_] = accumulateRenderingCutawayTime_/cutawayPass_;
//                                SSAOBlurCutawayTimes_[renderPass_] = accumulateSSAOBlurTime_/cutawayPass_;
//                                MeanFilterTimes_[renderPass_] = accumulateMeanFilterTime_/cutawayPass_;

                                std::cout << "accumulateCutawayGenerationTime_" << accumulateCutawayGenerationTime_/cutawayPass_ << std::endl ;
                                std::cout << "accumulateRenderingCutawayTime_" << accumulateRenderingCutawayTime_/cutawayPass_ << std::endl ;

                                accumulateCutawayGenerationTime_ = 0.0f;
                                accumulateRenderingCutawayTime_ = 0.0f;
                                accumulateSSAOBlurTime_ = 0.0f;
                                accumulateMeanFilterTime_ = 0.0f;
                                cutawayPass_ = 0.0f;
                                renderPass_++;
                        }
                        else
                        {
                                cutawayPass_ += 1.0f;
                        }
//                }else
//                {
//
//                }
        }

}


void GLWidget::drawIRESCutawayDynamicSurface ( )
{

        glDepthFunc ( GL_GREATER );
        glClearDepth ( 0.0 );

        /// OFFSCREEN Renderer
        /// DRAW COLOR_BUFFER -->  glDrawBuffers(2,[normalCutawayID_,verticesCutawayID_]);
        depthFBO->clearAttachments( );
        depthFBO->bindRenderBuffers(normalCutawayID_,verticesCutawayID_);

        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        IRESCutawaySurfaceDynamic_->enable( );

//        IRESCutawaySurfaceDynamic_->setUniform( "v0" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v1" , ply_primary_.box.box_min().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v2" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_min().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v3" , ply_primary_.box.box_max().x, ply_primary_.box.box_max().y,ply_primary_.box.box_max().z);
//
//        IRESCutawaySurfaceDynamic_->setUniform( "v4" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v5" , ply_primary_.box.box_max().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v6" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_min().z);
//        IRESCutawaySurfaceDynamic_->setUniform( "v7" , ply_primary_.box.box_min().x, ply_primary_.box.box_min().y,ply_primary_.box.box_max().z);


//        IRESCutawaySurfaceDynamic_->setUniform("move_x", move_x);
//        IRESCutawaySurfaceDynamic_->setUniform("move_y", move_y);
//        IRESCutawaySurfaceDynamic_->setUniform("move_z", move_z);
//        IRESCutawaySurfaceDynamic_->setUniform( "x" , volume_width );
//        IRESCutawaySurfaceDynamic_->setUniform( "y" , volume_height );
//        IRESCutawaySurfaceDynamic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawaySurfaceDynamic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//        IRESCutawaySurfaceDynamic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//        IRESCutawaySurfaceDynamic_->setUniform ("freeze", freezeView_ );
//        IRESCutawaySurfaceDynamic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

        IRESCutawaySurfaceDynamic_->setUniform("num_lights", (GLint) lights.size ( )  );
        IRESCutawaySurfaceDynamic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        IRESCutawaySurfaceDynamic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        /// Shader Intensity
        IRESCutawaySurfaceDynamic_->setUniform ( "saturation_" , this->saturationPrimaries_);
        IRESCutawaySurfaceDynamic_->setUniform ( "luminance_"  , this->luminancePrimaries_);

        IRESCutawaySurfaceDynamic_->setUniform("paper", 1.0  );
        IRESCutawaySurfaceDynamic_->setUniform("property_type", property_type_  );
        IRESCutawaySurfaceDynamic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        IRESCutawaySurfaceDynamic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        IRESCutawaySurfaceDynamic_->setUniform("move_x", move_x  );
        IRESCutawaySurfaceDynamic_->setUniform("move_y", move_y  );
        IRESCutawaySurfaceDynamic_->setUniform("move_z", move_z  );

        IRESCutawaySurfaceDynamic_->setUniform( "x" , volume_width );
        IRESCutawaySurfaceDynamic_->setUniform( "y" , volume_height );

        IRESCutawaySurfaceDynamic_->setUniform("min_range_static", min_range_static_  );
        IRESCutawaySurfaceDynamic_->setUniform("max_range_static", max_range_static_  );

        IRESCutawaySurfaceDynamic_->setUniform("min_range_dynamic", min_range_dynamic_  );
        IRESCutawaySurfaceDynamic_->setUniform("max_range_dynamic", max_range_dynamic_  );

        IRESCutawaySurfaceDynamic_->setUniform ( "max_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        IRESCutawaySurfaceDynamic_->setUniform ( "min_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );

        IRESCutawaySurfaceDynamic_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESPrimaryDynamic_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
        IRESCutawaySurfaceDynamic_->setUniform("property_index", reservoir_model_.current_static );

        IRESCutawaySurfaceDynamic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        IRESCutawaySurfaceDynamic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
        IRESCutawaySurfaceDynamic_->setUniform ("freeze", freezeView_ );
        IRESCutawaySurfaceDynamic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );


//        glBindVertexArray(vertexArray_Dummy);
//                glDrawArrays(GL_POINTS,0,1);
//        glBindVertexArray(0);

        reservoir_model_.drawCuboid ( );

        IRESCutawaySurfaceDynamic_->disable( );

        depthFBO->bindRenderBuffers(normalsSmoothID_, verticesSmoothID_);

        glDisable(GL_DEPTH_TEST);

        meanFilter->renderTexture( depthFBO->bindAttachment(normalCutawayID_),depthFBO->bindAttachment(verticesCutawayID_),meanFilterSize_);

        depthFBO->unbindAll();

        glEnable(GL_DEPTH_TEST);
        glDrawBuffer(GL_BACK);

}

void GLWidget::drawPrimaryDynamic ( ) const
{


        SSAOIRESPrimaryDynamic_->enable( );

        SSAOIRESPrimaryDynamic_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESPrimaryDynamic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESPrimaryDynamic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        /// Shader Intensity
        SSAOIRESPrimaryDynamic_->setUniform ( "saturation_" , this->saturationPrimaries_);
        SSAOIRESPrimaryDynamic_->setUniform ( "luminance_"  , this->luminancePrimaries_);

        SSAOIRESPrimaryDynamic_->setUniform("paper", 1.0  );
        SSAOIRESPrimaryDynamic_->setUniform("property_type", property_type_  );
        SSAOIRESPrimaryDynamic_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        SSAOIRESPrimaryDynamic_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        SSAOIRESPrimaryDynamic_->setUniform("move_x", move_x  );
        SSAOIRESPrimaryDynamic_->setUniform("move_y", move_y  );
        SSAOIRESPrimaryDynamic_->setUniform("move_z", move_z  );

        SSAOIRESPrimaryDynamic_->setUniform("min_range_static", min_range_static_  );
        SSAOIRESPrimaryDynamic_->setUniform("max_range_static", max_range_static_  );

        SSAOIRESPrimaryDynamic_->setUniform("min_range_dynamic", min_range_dynamic_  );
        SSAOIRESPrimaryDynamic_->setUniform("max_range_dynamic", max_range_dynamic_  );

        SSAOIRESPrimaryDynamic_->setUniform ( "max_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        SSAOIRESPrimaryDynamic_->setUniform ( "min_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );

        SSAOIRESPrimaryDynamic_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESPrimaryDynamic_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESPrimaryDynamic_->setUniform("property_index", reservoir_model_.current_static );

        SSAOIRESPrimaryDynamic_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESPrimaryDynamic_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        SSAOIRESPrimaryDynamic_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawCuboid ( );

        SSAOIRESPrimaryDynamic_->disable( );

}

void GLWidget::drawSecondaryDynamic ( ) const
{

        // Interior Cells

        SSAOIRESCutawayDynamic_->enable( );

        SSAOIRESCutawayDynamic_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayDynamic_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        SSAOIRESCutawayDynamic_->setUniform("min_range_static", min_range_static_  );
        SSAOIRESCutawayDynamic_->setUniform("max_range_static", max_range_static_  );

        SSAOIRESCutawayDynamic_->setUniform("min_range_dynamic", min_range_dynamic_  );
        SSAOIRESCutawayDynamic_->setUniform("max_range_dynamic", max_range_dynamic_  );

        SSAOIRESCutawayDynamic_->setUniform ( "max_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        SSAOIRESCutawayDynamic_->setUniform ( "min_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );

        SSAOIRESCutawayDynamic_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayDynamic_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayDynamic_->setUniform("property_index", reservoir_model_.current_static );
        SSAOIRESCutawayDynamic_->setUniform("property_type", property_type_  );

        /// Shader Intensity
        SSAOIRESCutawayDynamic_->setUniform ( "saturation_" , this->saturationSecondaries_);
        SSAOIRESCutawayDynamic_->setUniform ( "luminance_"  , this->luminanceSecondaries_);

        SSAOIRESCutawayDynamic_->setUniform ( "Wall" , wall_ );
        SSAOIRESCutawayDynamic_->setUniform ( "Line" , line_ );
        SSAOIRESCutawayDynamic_->setUniform ( "isPerspective_" , isPerspective_ );
        SSAOIRESCutawayDynamic_->setUniform ( "nearPlane_" , nearPlane_ );
        SSAOIRESCutawayDynamic_->setUniform ( "farPlane_" , farPlane_ );

        SSAOIRESCutawayDynamic_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        SSAOIRESCutawayDynamic_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        SSAOIRESCutawayDynamic_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayDynamic_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESCutawayDynamic_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
        SSAOIRESCutawayDynamic_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );

        reservoir_model_.drawFaces();

        SSAOIRESCutawayDynamic_->disable( );


//        SSAOIRESCutawayDynamicShell_->enable( );
//
//        SSAOIRESCutawayDynamicShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
//        SSAOIRESCutawayDynamicShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );
//
//        SSAOIRESCutawayDynamicShell_->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "faults" , reservoir_model_.showFault );
//
//        /// Shader Intensity
//        SSAOIRESCutawayDynamicShell_->setUniform ( "saturation_" , this->saturationShell_);
//        SSAOIRESCutawayDynamicShell_->setUniform ( "luminance_"  , this->luminanceShell_);
//
//        SSAOIRESCutawayDynamicShell_->setUniform ( "isPerspective_" , isPerspective_ );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "nearPlane_" , nearPlane_ );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "farPlane_" , farPlane_ );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );
//
//        SSAOIRESCutawayDynamicShell_->setUniform ( "ModelMatrix" , trackball_->getModelMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "ViewMatrix" , trackball_->getViewMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
//        SSAOIRESCutawayDynamicShell_->setUniform ( "ProjectionMatrix" , trackball_->getProjectionMatrix ( ).data ( ) , 4 , GL_FALSE , 1 );
//
//        reservoir_model_.drawFaces( );
//
//        SSAOIRESCutawayDynamicShell_->disable( );


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
                        depthFBO->clearAttachments();
                }


                glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
                glDepthFunc ( GL_LESS );
                glClearDepth ( 1.0f );
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                drawBackGround( );

                /// OFFSCREEN Renderer
                 /// DRAW COLOR_BUFFER -->  glDrawBuffers(2,[depthTextureID, normalTextureID, colorTextureID]);
                 fboSSAO->clearAttachments();
                 fboSSAO->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

                         if ( draw_secondary )
                         {
                                 drawSecondaryDynamic( );
                         }

//                         if ( draw_primary )
//                         {
//                                 if (isply_)
//                                         PaperPly();
//                                 else
//                                         drawPrimaryDynamic();
//                         }

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


                 //glDrawBuffer(GL_BACK);

                 glDisable(GL_DEPTH_TEST);

                 fboSSAO->bindRenderBuffer(blurTextureID);

                 /// SEGUNDO PASSO
                 ssaoShader_->enable();

                 glActiveTexture(GL_TEXTURE0 + 7);
                 glBindTexture(GL_TEXTURE_2D, noiseTexture);

                 ssaoShader_->setUniform("lightViewMatrix", trackball_->getProjectionMatrix().data(),4,GL_FALSE,1);

                 ssaoShader_->setUniform("noiseScale", noise_scale.x(),noise_scale.y());
                 ssaoShader_->setUniform("kernel", kernel, 2, numberOfSamples);

                 ssaoShader_->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
                 ssaoShader_->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
                 ssaoShader_->setUniform("colorTexture", fboSSAO->bindAttachment(colorTextureID));
                 ssaoShader_->setUniform("displayAmbientPass", displayAmbientPass);

                 ssaoShader_->setUniform("radius", radius);
                 ssaoShader_->setUniform("intensity", (float)intensity);
                 ssaoShader_->setUniform("max_dist", max_dist);
                 ssaoShader_->setUniform("noiseTexture", 7);

                 //Second pass mesh rendering:
                 quadSSAO->render();

                 ssaoShader_->disable();
                 glBindTexture(GL_TEXTURE_2D, 0);
                 fboSSAO->unbindAll ( );
                 fboSSAO->clearDepth ( );

                 glDrawBuffer(GL_BACK);

                 blurShader_->enable();

                 blurShader_->setUniform("blurTexture", fboSSAO->bindAttachment(blurTextureID));
                 blurShader_->setUniform("blurRange", blurRange);

                 quadSSAO->render();

                 blurShader_->disable();
                 fboSSAO->unbindAll( );
                 fboSSAO->clearDepth( );

                 glEnable(GL_DEPTH_TEST);

        }


}

////Timer
//void GLWidget::timerEvent( QTimerEvent *event )
//{
//
//
//
//}


/// F10
void GLWidget::drawIRESModel ( )
{
        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround( );

        // Surface Faces

        RawModel_->enable( );

        /// Shader Intensity
        RawModel_->setUniform ( "saturation_" , this->saturationPrimaries_);
        RawModel_->setUniform ( "luminance_"  , this->luminancePrimaries_);

        RawModel_->setUniform("paper", 1.0  );
        RawModel_->setUniform("property_type", property_type_  );
        RawModel_->setUniform("box_min", ply_primary_.box.box_min().x,ply_primary_.box.box_min().y,ply_primary_.box.box_min().z );
        RawModel_->setUniform("box_max", ply_primary_.box.box_max().x,ply_primary_.box.box_max().y,ply_primary_.box.box_max().z );

        RawModel_->setUniform("move_x", move_x  );
        RawModel_->setUniform("move_y", move_y  );
        RawModel_->setUniform("move_z", move_z  );

        RawModel_->setUniform("min_range_static", min_range_static_  );
        RawModel_->setUniform("max_range_static", max_range_static_  );

        RawModel_->setUniform("min_range_dynamic", min_range_dynamic_  );
        RawModel_->setUniform("max_range_dynamic", max_range_dynamic_  );

        RawModel_->setUniform ("max_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        RawModel_->setUniform ("min_property_dynamic" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );

        RawModel_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
        RawModel_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
        RawModel_->setUniform("property_index", reservoir_model_.current_static );

        RawModel_->setUniform("justWireFrame", justWireFrame );
        RawModel_->setUniform("faults", reservoir_model_.showFault );

        RawModel_->setUniform("num_lights", (GLint) lights.size ( )  );
        RawModel_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        RawModel_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        RawModel_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        RawModel_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        RawModel_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        reservoir_model_.drawIndexFaces(primaries_face);

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

        BurnsPrimary_->setUniform("min_range_static", min_range_static_  );
        BurnsPrimary_->setUniform("max_range_static", max_range_static_  );

        BurnsPrimary_->setUniform("min_range_dynamic", min_range_dynamic_  );
        BurnsPrimary_->setUniform("max_range_dynamic", max_range_dynamic_  );

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

        BurnsPly_->enable();

        BurnsPly_->setUniform("move_x", move_x );
        BurnsPly_->setUniform("move_y", move_y );
        BurnsPly_->setUniform("move_z", move_z );

        BurnsPly_->setUniform("move", (float) borderLinesSize_ );
        BurnsPly_->setUniform("num_lights", (GLint) lights.size ( )  );
        BurnsPly_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        BurnsPly_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        BurnsPly_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        BurnsPly_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        ply_primary_.Draw();

        BurnsPly_->disable();

}


void GLWidget::reloadShaders ( )
{
        // Effects --
        meanFilter->reloadShaders();

        BorderLines_->reloadShaders ( );
        xtoon_texture_viewer->reloadShaders ( );
        BackGround_->reloadShaders ( );
        // Paper Demo
        RawModel_->reloadShaders ( );

        BurnsPrimary_->reloadShaders ( );
        BurnsPrimarySetup_->reloadShaders ( );
        BurnsSecondary_->reloadShaders ( );
        DummyQuad_->reloadShaders ( );

        // ! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic_->reloadShaders ( );

                SSAOIRESPrimaryStatic_->reloadShaders();
                SSAOIRESCutawayStatic_->reloadShaders();
                SSAOIRESCutawayStaticShell_->reloadShaders();

        // ! DYNAMIC VIEWER F12 Dynamic Properties
                IRESCutawaySurfaceDynamic_->reloadShaders ( );

                SSAOIRESPrimaryDynamic_->reloadShaders();
                SSAOIRESCutawayDynamic_->reloadShaders();
                SSAOIRESCutawayDynamicShell_->reloadShaders();


        BurnsPly_->reloadShaders ( );

        // SSAO
        ssaoShader_->reloadShaders ( );
        blurShader_->reloadShaders ( );
        deferredShader->reloadShaders ( );

        blurShader_->reloadShaders ( );
        ssaoShader_->reloadShaders();

}

void GLWidget::loadShaders ( )
{
        //! Binary absolute location
	QDir shadersDir = QDir ( qApp->applicationDirPath ( ) );

	//! Debug Verion: to load the update shaders
        qDebug () << "Directory " << shadersDir.path ();
        shadersDir.cdUp ();
        shadersDir.cdUp ();
        shadersDir.cdUp ();
        qDebug () << "Directory " << shadersDir.path ();

	#if defined(_WIN32) || defined(_WIN64) // Windows Directory Style
	/* Do windows stuff */
	        QString shaderDirectory (shadersDir.path ()+"\\src\\IRESCutaway\\GUI\\Qt\\RCC\\Shaders\\");
	#elif defined(__linux__)               // Linux Directory Style
	/* Do linux stuff */
	        QString shaderDirectory (shadersDir.path ()+"/src/IRESCutaway/GUI/Qt/RCC/Shaders/");
	#else
	/* Error, both can't be defined or undefined same time */
	        std::cout <<  "Operate System not supported !"
                halt();
	#endif

        //! Effects --
                BorderLines_ = new Shader ("Border Lines",
                                          (shaderDirectory + "BorderLines.vert").toStdString(),
                                          (shaderDirectory + "BorderLines.frag").toStdString(),
                                          (shaderDirectory + "BorderLines.geom").toStdString(),1);
                BorderLines_->initialize();

                xtoon_texture_viewer = new Shader ("xtoon_texture",
                                                  (shaderDirectory + "xtoon_texture.vert").toStdString(),
                                                  (shaderDirectory + "xtoon_texture.frag").toStdString(),"",1);
                xtoon_texture_viewer->initialize( );

                meanFilter->setShadersDir( shaderDirectory.toStdString() );
                meanFilter->initialize( );

                BackGround_ = new Shader  ("BackGround", (shaderDirectory + "BackGround.vert").toStdString(),
                                         (shaderDirectory + "BackGround.frag").toStdString(),"",1);
                BackGround_->initialize( );
        //! Paper Demo

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

       //! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic_ = new Shader ("IRESCutawaySurfaceStatic",
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.vert").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.frag").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.geom").toStdString(),1);
                IRESCutawaySurfaceStatic_->initialize( );


                IRESCutawaySurfaceDynamic_ = new Shader ("IRESCutawaySurfaceDynamic",
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.geom").toStdString(),1);
                IRESCutawaySurfaceDynamic_->initialize();




                BurnsPly_ = new Shader ("BurnsPly",
                                       (shaderDirectory + "Demo/BurnsPrimary430.vert").toStdString(),
                                       (shaderDirectory + "Demo/BurnsPrimary430.frag").toStdString(),
                                        "",1);
                BurnsPly_->initialize( );

        //! SSAO

                /// The per pixel AO computation shader

                ssaoShader_ = new Shader ("ssaoShader",
                                         (shaderDirectory + "SSAO/ssao.vert").toStdString(),
                                         (shaderDirectory + "SSAO/ssao.frag").toStdString(),
                                            "",1);
                ssaoShader_->initialize();


                deferredShader = new Shader ("deferredShader", (shaderDirectory + "SSAO/viewspacebuffer.vert").toStdString(),
                                                             (shaderDirectory + "SSAO/viewspacebuffer.frag").toStdString(),
                                                             "",1);
                deferredShader->initialize();

                blurShader_ = new Shader ("blurShader", (shaderDirectory + "SSAO/gaussianblurfilter.vert").toStdString(),
                                                       (shaderDirectory + "SSAO/gaussianblurfilter.frag").toStdString(),
                                                             "",1);
                blurShader_->initialize();

                //! SSAO STATIC

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

                //! SSAO Dynamic

                SSAOIRESPrimaryDynamic_ = new Shader ("IRESPrimaryDynamic",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.geom").toStdString(),1);

                SSAOIRESPrimaryDynamic_->initialize();

                SSAOIRESCutawayDynamic_ = new Shader ("SSAOIRESCutawayDynamic",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.geom").toStdString(),1);

                SSAOIRESCutawayDynamic_->initialize();

                SSAOIRESCutawayDynamicShell_ = new Shader ("SSAOIRESCutawayDynamicShell_",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.geom").toStdString(),1);

                SSAOIRESCutawayDynamicShell_->initialize();




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
                        trackball_->usePerspectiveMatrix  ( fovy_ , aspect , nearPlane_ , farPlane_ );
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
//	        if  ( event->delta ( ) > 0 )
//	        {
//                        if ( fovy_ < 90.0f )
//                        {
//                                fovy_ += 1.0;
//                        }
//	        }else
//	        {
//	                if ( fovy_ > 0.0f )
//	                {
//	                        fovy_ -= 1.0;
//	                }
//	        }
//
//	        trackball_->usePerspectiveMatrix  ( fovy_ , aspect_ , nearPlane_ , farPlane_ );
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

void GLWidget::freezeView ( )
{
        freezeView_ = !freezeView_;
        if ( freezeView_ )
        {
                freeze_viewmatrix_ = trackball_->getViewMatrix ( ).matrix ( );
        }
}
