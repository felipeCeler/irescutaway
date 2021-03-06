#include <GUI/Qt/GLWidget/GLWidget.hpp>


// Se quiser usar QPainter Ver exemplo no QT demo - Manda Qt em wave animation !!!
GLWidget::GLWidget (  QWidget* parent ) : QOpenGLWidget ( parent )
{

}
/// OpenGL
void GLWidget::initializeGL ( )
{
	/// GLEW OpenGL
	/// Glew Initialization:
	glewExperimental = GL_TRUE;
	GLenum glewInitResult = glewInit ( );;

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
	/// If mouse tracking is bindd, the widget receives mouse move events even if no buttons are pressed. | @QtDocmentation
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
        accumulateRenderingPrimaryTime_ = 0.0f;
        accumulateSSAOBlurTime_ = 0.0f;
        accumulateMeanFilterTime_ = 0.0f;

        renderPass_ = 5000;

        cutawayGenerationTimes_.resize(renderPass_);
        renderingCutawayTimes_.resize(renderPass_);
        renderingPrimaryTimes_.resize(renderPass_);
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

	nearPlane_ = 1.0f;
        farPlane_  = 15.0f;

	isPerspective_ = true;

	enable_blend_ = 0;

	depthFBO = new Tucano::Framebuffer( width() , height(), 5 );

	normalCutawayID_ = 0;
	verticesCutawayID_ = 1;
	normalsSmoothID_ = 2;
	verticesSmoothID_ =  3;
	silhouetteID_	  = 4;

	meanFilter = new Effects::MeanFilter();

	justWireFrame = 0;

	loadShaders ( );

	/// TODO xtoon
	picture = new Mesh( );
	picture->createQuad( );

	reservoir_model_.createBuffers( );

	dynamic_property_index = 0;

	/// BenchMarking

	camera.setPerspectiveMatrix ( 60.0 , (float) this->width ( ) / (float) this->height ( ) , 0.1f , 100.0f );
	camera.decreaseZoom( 3.05 );

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

        fboSSAO = new Framebuffer( width() , height(), 4 );

        quadSSAO = new Mesh();
        quadSSAO->createQuad();

        isply_          = false;

        ply_primary_.init();

        /// From libQGLViewer
        AnimationInitializer( );

}

GLWidget::~GLWidget( )
{
        delete[] light_elements;
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

	int invalid = 0;

        for ( std::size_t i = 0; i < reservoir_model_.cuboidStatic.size(); i+=4 )
        {
                if ( ( minRange < reservoir_model_.cuboidStatic[i+property_index] ) && ( reservoir_model_.cuboidStatic[i+property_index] <= maxRange ) )
                {
                        reservoir_model_.indexCuboids[primaries_cuboid] = i/4;
                        primaries_cuboid++;
                }
                else
                {
                        invalid++;
                }

        }
//
        for ( std::size_t i = 0; i < reservoir_model_.iresFaces_.size(); i++)
        {
                if ( (minRange < reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[reservoir_model_.iresFaces_[i].id]) && ( reservoir_model_.static_porperties[reservoir_model_.static_indices[0]].values_[reservoir_model_.iresFaces_[i].id] <= maxRange) )
                {
                        reservoir_model_.indexFaces[primaries_face] = i;
                        primaries_face++;
                }
        }
//
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, reservoir_model_.indexBufferFace);
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, primaries_face * sizeof(reservoir_model_.indexFaces[0]), &reservoir_model_.indexFaces[0], GL_STATIC_DRAW );

        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, reservoir_model_.indexBufferCuboid);
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, primaries_cuboid * sizeof(reservoir_model_.indexCuboids[0]), &reservoir_model_.indexCuboids[0], GL_STATIC_DRAW );
//
        glFinish();

        std::cout << "primaries++ Cuboid" << primaries_cuboid << std::endl;
        std::cout << "primaries++" << primaries_face << std::endl;
        std::cout << "invalid" << invalid << std::endl;

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

	camera.setViewport ( Eigen::Vector2f ( (float) width , (float) height ) );


	if ( isPerspective_ )
	{
	        camera.setTrackballPerspectiveMatrix( 45.0f , aspect_ , nearPlane_ , farPlane_);
	}
	else
	{
	        camera.setTrackballOrthographicMatrix(-1.0f*aspect_ , 1.0f*aspect_ , -1.0f , 1.0 , 0.1f , 500.0f );
	}


	if (depthFBO)
	{
	        delete depthFBO;
	}

	if (fboSSAO)
	{
	        delete fboSSAO;
	}


	depthFBO = new Framebuffer( width , height, 5 );
	depthFBO->clearAttachments();

        fboSSAO = new Framebuffer( width , height, 4 );
        fboSSAO->clearAttachments();

        viewport_[0] = width;
        viewport_[1] = height;


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

        renderTime.start ( );

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

	cutawayGenerationTime_.restart ();

	IRESCutawaySurfaceStatic_->bind( );

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
	IRESCutawaySurfaceStatic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
	IRESCutawaySurfaceStatic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
	IRESCutawaySurfaceStatic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

	IRESCutawaySurfaceStatic_->setUniform ("freeze", freezeView_ );
	IRESCutawaySurfaceStatic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_ );

	reservoir_model_.drawIndexCuboids (primaries_cuboid);//reservoir_model_.drawCuboid ( );

	IRESCutawaySurfaceStatic_->unbind( );

        glFinish();
        accumulateCutawayGenerationTime_ += (float)cutawayGenerationTime_.elapsed();

	depthFBO->bindRenderBuffers(normalsSmoothID_, verticesSmoothID_);

        glDisable(GL_DEPTH_TEST);

        renderingMeanFilterTime_.start();
        meanFilter->setKernel(this->meanFilterSize_);
        meanFilter->renderTexture( depthFBO->getTexture(normalCutawayID_),viewport_);
        meanFilter->renderTexture( depthFBO->getTexture(verticesCutawayID_),viewport_);
        depthFBO->unbind();

        glFinish( );
        accumulateMeanFilterTime_ += (float)renderingMeanFilterTime_.elapsed();
        glEnable(GL_DEPTH_TEST);


	glDrawBuffer(GL_BACK);

	glFinish();
	glFlush();

}

void GLWidget::drawPrimaryStaticSilhouette  ( ) const // Draw only primary   Cells
{

	depthFBO->bindRenderBuffer(silhouetteID_);

        SSAOIRESPrimaryStaticSilhouette_->bind ( );

        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );
        /// Shader Intensity
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "saturation_" , this->saturationPrimaries_);
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "luminance_"  , this->luminancePrimaries_);

        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "min_range_static" , min_range_static_ );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "max_range_static" , max_range_static_ );

        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "paper" , 1.0 );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "box_min" , ply_primary_.box.box_min ( ).x , ply_primary_.box.box_min ( ).y , ply_primary_.box.box_min ( ).z );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "box_max" , ply_primary_.box.box_max ( ).x , ply_primary_.box.box_max ( ).y , ply_primary_.box.box_max ( ).z );

        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "min_property_static" , reservoir_model_.static_min[reservoir_model_.current_static] );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "max_property_static" , reservoir_model_.static_max[reservoir_model_.current_static] );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "property_index" , reservoir_model_.current_static );

        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESPrimaryStaticSilhouette_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawIndexFaces(primaries_face);//reservoir_model_.drawFaces ( )

        SSAOIRESPrimaryStaticSilhouette_->unbind ( );

        depthFBO->unbind();

        glDrawBuffer(GL_BACK);

}

void GLWidget::drawPrimaryStatic  ( ) const // Draw only primary   Cells
{

        SSAOIRESPrimaryStatic_->bind ( );

        //SSAOIRESPrimaryStatic_->setUniform ("silhouette" , depthFBO->bindAttachment(silhouetteID_));

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

        SSAOIRESPrimaryStatic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESPrimaryStatic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESPrimaryStatic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawIndexFaces(primaries_face);//reservoir_model_.drawFaces ( );

        SSAOIRESPrimaryStatic_->unbind ( );

        //depthFBO->unbind();

}

void GLWidget::drawSecondaryStatic  ( ) const  // Draw only secondary Cells
{

        /// @TODO According with John McDonald from nvidia at http://youtu.be/-bCeNzgiJ8I?t=31m42s,
        /// this command is costly than just bind a Texture

        // Interior Cells
        SSAOIRESCutawayStatic_->bind( );

        SSAOIRESCutawayStatic_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStatic_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        /// Shader Intensity
        SSAOIRESCutawayStatic_->setUniform ( "saturation_" , this->saturationSecondaries_);
        SSAOIRESCutawayStatic_->setUniform ( "luminance_"  , this->luminanceSecondaries_);

        SSAOIRESCutawayStatic_->setUniform("min_range_static", min_range_static_  );
        SSAOIRESCutawayStatic_->setUniform("max_range_static", max_range_static_  );
        SSAOIRESCutawayStatic_->setUniform("min_property_static", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("max_property_static", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStatic_->setUniform("property_index", reservoir_model_.current_static );

        SSAOIRESCutawayStatic_->setUniform ("isPerspective_" , isPerspective_ );
        SSAOIRESCutawayStatic_->setUniform ("nearPlane_" , nearPlane_ );
        SSAOIRESCutawayStatic_->setUniform ("farPlane_" , farPlane_ );

        SSAOIRESCutawayStatic_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStatic_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStatic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESCutawayStatic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESCutawayStatic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawFaces( );//reservoir_model_.drawIndexFaces(reservoir_model_.faceCount);

        SSAOIRESCutawayStatic_->unbind( );

        // Surface Faces

        SSAOIRESCutawayStaticShell_->bind( );

        SSAOIRESCutawayStaticShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayStaticShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        /// Shader Intensity
        SSAOIRESCutawayStaticShell_->setUniform ( "saturation_" , this->saturationShell_);
        SSAOIRESCutawayStaticShell_->setUniform ( "luminance_"  , this->luminanceShell_);

        SSAOIRESCutawayStaticShell_->setUniform("min_property", reservoir_model_.static_min[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("max_property", reservoir_model_.static_max[reservoir_model_.current_static]  );
        SSAOIRESCutawayStaticShell_->setUniform("property_index", reservoir_model_.current_static );
        SSAOIRESCutawayStaticShell_->setUniform("faults" ,(GLint)(reservoir_model_.showFault) );
        SSAOIRESCutawayStaticShell_->setUniform("justWireFrame", justWireFrame );

        SSAOIRESCutawayStaticShell_->setUniform ( "isPerspective_" , isPerspective_ );
        SSAOIRESCutawayStaticShell_->setUniform ( "nearPlane_" , nearPlane_ );
        SSAOIRESCutawayStaticShell_->setUniform ( "farPlane_" , farPlane_ );

        SSAOIRESCutawayStaticShell_->setUniform("num_lights", (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        SSAOIRESCutawayStaticShell_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayStaticShell_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESCutawayStaticShell_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESCutawayStaticShell_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawFaces();//reservoir_model_.drawIndexFaces(reservoir_model_.faceCount);

        SSAOIRESCutawayStaticShell_->unbind( );

        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayStatic (  )
{

//      /// FIXME Conditions  - Primary and Secondary well defined.

        if ( isIRESOpen_ )
        {


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



                if ( reservoir_model_.showBorderLine )
                {
                	drawPrimaryStaticSilhouette();
                }


                drawBackGround( );

                if ( reservoir_model_.showBorderLine )
                {

                        glLineWidth( (float) borderLinesSize_ );
                        BorderLines_->bind ( );

                        BorderLines_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
                        BorderLines_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
                        BorderLines_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

                        reservoir_model_.drawFaces ( );

                        BorderLines_->unbind ( );
                }


                /// SSAO -- C&G
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
                        accumulateRenderingCutawayTime_ += (float)renderingCutawayTime_.elapsed();

                        renderingPrimaryTime_.start();
                        if ( draw_primary )
                        {
                                drawPrimaryStatic( );

                        }

                        glFinish();
                        accumulateRenderingPrimaryTime_ += (float)renderingPrimaryTime_.elapsed();

                glDisable(GL_DEPTH_TEST);

		/// SSAO -- Graphical Models - I hope
		fboSSAO->bindRenderBuffer(blurTextureID);

		/// Second Rendering Pass
		renderingSSAOBlurTime_.start();
		ssaoShaderTucanoGitlab_->bind();

		ssaoShaderTucanoGitlab_->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
		ssaoShaderTucanoGitlab_->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
		ssaoShaderTucanoGitlab_->setUniform("displayAmbientPass", displayAmbientPass);
		ssaoShaderTucanoGitlab_->setUniform("noise_texture", 7);

		ssaoShaderTucanoGitlab_->setUniform("radius", radius);
		ssaoShaderTucanoGitlab_->setUniform("intensity", (GLfloat)intensity);
		ssaoShaderTucanoGitlab_->setUniform("global_scale", (GLfloat)intensity);

		/// Second pass mesh rendering:
		quadSSAO->render();

		ssaoShaderTucanoGitlab_->unbind();

		glBindTexture(GL_TEXTURE_2D, 0);
		fboSSAO->unbind ( );
		fboSSAO->clearDepth ( );
		/// End of SSAO

		//glBindFramebuffer(context()->defaultFramebufferObject());
                glDrawBuffer(GL_BACK);
                makeCurrent();

		blurTucanoGitlab_->bind();

		blurTucanoGitlab_->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
		blurTucanoGitlab_->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
		blurTucanoGitlab_->setUniform("colorTexture", fboSSAO->bindAttachment(colorTextureID));
		blurTucanoGitlab_->setUniform("ssaoTexture", fboSSAO->bindAttachment(blurTextureID));
		blurTucanoGitlab_->setUniform("blurRange", blurRange);

		quadSSAO->render();

		glFinish( );
		accumulateSSAOBlurTime_ += (float)renderingSSAOBlurTime_.elapsed();

		blurTucanoGitlab_->unbind();

                fboSSAO->unbind( );
                fboSSAO->clearDepth( );


                glEnable(GL_DEPTH_TEST);

//                if (play_)
//                {
                        if ( cutawayPass_ == 1.0f )
                        {
                                emit cutawayGenerationTime ( QString::number ( ( accumulateCutawayGenerationTime_/cutawayPass_ ) ) );
                                emit renderingCutawayTime ( QString::number ( ( accumulateRenderingCutawayTime_/cutawayPass_ ) ) );
                                emit renderingPrimaryTime ( QString::number ( ( accumulateRenderingPrimaryTime_/cutawayPass_ ) ) );
                                emit renderingSSAOBlurTime ( QString::number ( ( accumulateSSAOBlurTime_/cutawayPass_ ) ) );
                                emit renderingMeanFilterTime ( QString::number ( ( accumulateMeanFilterTime_/cutawayPass_ ) ) );

                                cutawayGenerationTimes_[renderPass_] = accumulateCutawayGenerationTime_/cutawayPass_;
                                renderingCutawayTimes_[renderPass_] = accumulateRenderingCutawayTime_/cutawayPass_;
                                renderingPrimaryTimes_[renderPass_] = accumulateRenderingPrimaryTime_/cutawayPass_;
                                SSAOBlurCutawayTimes_[renderPass_] = accumulateSSAOBlurTime_/cutawayPass_;
                                MeanFilterTimes_[renderPass_] = accumulateMeanFilterTime_/cutawayPass_;

//                                std::cout << "accumulateCutawayGenerationTime_" << accumulateCutawayGenerationTime_/cutawayPass_ << std::endl ;
//                                std::cout << "accumulateRenderingCutawayTime_" << accumulateRenderingCutawayTime_/cutawayPass_ << std::endl ;

                                accumulateCutawayGenerationTime_ = 0.0f;
                                accumulateRenderingCutawayTime_ = 0.0f;
                                accumulateRenderingPrimaryTime_ = 0.0f;
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

        IRESCutawaySurfaceDynamic_->bind( );

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
        IRESCutawaySurfaceDynamic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        IRESCutawaySurfaceDynamic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        IRESCutawaySurfaceDynamic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        IRESCutawaySurfaceDynamic_->setUniform ("freeze", freezeView_ );
        IRESCutawaySurfaceDynamic_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_);

        glBindVertexArray(vertexArray_Dummy);
                glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        //reservoir_model_.drawCuboid ( );

        IRESCutawaySurfaceDynamic_->unbind( );

        depthFBO->bindRenderBuffers(normalsSmoothID_, verticesSmoothID_);

        glDisable(GL_DEPTH_TEST);

        meanFilter->renderTexture( depthFBO->getTexture(normalCutawayID_),viewport_);
        meanFilter->renderTexture( depthFBO->getTexture(verticesCutawayID_),viewport_);

        depthFBO->unbind();

        glEnable(GL_DEPTH_TEST);
}

void GLWidget::drawPrimaryDynamic ( ) const
{

        SSAOIRESPrimaryDynamic_->bind( );

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

        SSAOIRESPrimaryDynamic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESPrimaryDynamic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESPrimaryDynamic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawCuboid ( );

        SSAOIRESPrimaryDynamic_->unbind( );

}

void GLWidget::drawSecondaryDynamic ( ) const
{

        /// Interior Cells
        SSAOIRESCutawayDynamic_->bind( );

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

        SSAOIRESCutawayDynamic_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESCutawayDynamic_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESCutawayDynamic_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawCuboid();

        SSAOIRESCutawayDynamic_->unbind( );


        SSAOIRESCutawayDynamicShell_->bind( );

        SSAOIRESCutawayDynamicShell_->setUniform( "normal" , depthFBO->bindAttachment(normalsSmoothID_) );
        SSAOIRESCutawayDynamicShell_->setUniform( "vertex" , depthFBO->bindAttachment(verticesSmoothID_) );

        SSAOIRESCutawayDynamicShell_->setUniform ( "max_property" , reservoir_model_.dynamic_properties[dynamic_property_index].max_[time_step] );
        SSAOIRESCutawayDynamicShell_->setUniform ( "min_property" , reservoir_model_.dynamic_properties[dynamic_property_index].min_[time_step] );
        SSAOIRESCutawayDynamicShell_->setUniform ( "faults" , reservoir_model_.showFault );

        /// Shader Intensity
        SSAOIRESCutawayDynamicShell_->setUniform ( "saturation_" , this->saturationShell_);
        SSAOIRESCutawayDynamicShell_->setUniform ( "luminance_"  , this->luminanceShell_);

        SSAOIRESCutawayDynamicShell_->setUniform ( "isPerspective_" , isPerspective_ );
        SSAOIRESCutawayDynamicShell_->setUniform ( "nearPlane_" , nearPlane_ );
        SSAOIRESCutawayDynamicShell_->setUniform ( "farPlane_" , farPlane_ );
        SSAOIRESCutawayDynamicShell_->setUniform ( "num_lights" , (GLint) lights.size ( ) );
        SSAOIRESCutawayDynamicShell_->setUniform ( "lights[0]" , light_elements , 3 , (GLint) lights.size ( ) );
        SSAOIRESCutawayDynamicShell_->setUniform ( "WIN_SCALE" , (float) width ( ) , (float) height ( ) );

        SSAOIRESCutawayDynamicShell_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        SSAOIRESCutawayDynamicShell_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        SSAOIRESCutawayDynamicShell_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawFaces( );

        SSAOIRESCutawayDynamicShell_->unbind( );


        depthFBO->unbindAttachments();

}

void GLWidget::IRESCutawayDynamic ( )
{
        /// FIXME Conditions  - Primary and Secondary well defined.

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

                         if ( draw_primary )
                         {
				if ( isply_ )
				{
					PaperPly ( );
				}
				else
				{
					drawPrimaryDynamic ( );
				}
                         }

                         if ( reservoir_model_.showBorderLine )
                         {

                                 glLineWidth( (float) borderLinesSize_ );
                                 BorderLines_->bind ( );

                                 BorderLines_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
                                 BorderLines_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
                                 BorderLines_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

                                 reservoir_model_.drawFaces ( );

                                 BorderLines_->unbind ( );
                         }


                 glDisable(GL_DEPTH_TEST);


 		fboSSAO->bindRenderBuffer(blurTextureID);

 		/// Second Rendering Pass
 		renderingSSAOBlurTime_.start();
 		ssaoShaderTucanoGitlab_->bind();

 		ssaoShaderTucanoGitlab_->setUniform("coordsTexture", fboSSAO->bindAttachment(depthTextureID));
 		ssaoShaderTucanoGitlab_->setUniform("normalTexture", fboSSAO->bindAttachment(normalTextureID));
 		ssaoShaderTucanoGitlab_->setUniform("displayAmbientPass", displayAmbientPass);
 		noise_texture.bind(7);
 		ssaoShaderTucanoGitlab_->setUniform("noise_texture", 7);

 		ssaoShaderTucanoGitlab_->setUniform("radius", radius);
 		ssaoShaderTucanoGitlab_->setUniform("intensity", (GLfloat)intensity);
 		ssaoShaderTucanoGitlab_->setUniform("global_scale", (GLfloat)intensity);

 		/// Second pass mesh rendering:
 		quadSSAO->render();

 		ssaoShaderTucanoGitlab_->unbind();

 		glBindTexture(GL_TEXTURE_2D, 0);
 		fboSSAO->unbind ( );
 		fboSSAO->clearDepth ( );
 		noise_texture.unbind();
 		/// End of SSAO

		glDrawBuffer ( GL_BACK );
		makeCurrent ( );

		blurTucanoGitlab_->bind ( );

		blurTucanoGitlab_->setUniform ( "coordsTexture" , fboSSAO->bindAttachment ( depthTextureID ) );
		blurTucanoGitlab_->setUniform ( "normalTexture" , fboSSAO->bindAttachment ( normalTextureID ) );
		blurTucanoGitlab_->setUniform ( "colorTexture" , fboSSAO->bindAttachment ( colorTextureID ) );
		blurTucanoGitlab_->setUniform ( "ssaoTexture" , fboSSAO->bindAttachment ( blurTextureID ) );
		blurTucanoGitlab_->setUniform ( "blurRange" , blurRange );

		quadSSAO->render ( );

		glFinish ( );
		accumulateSSAOBlurTime_ += (float) renderingSSAOBlurTime_.elapsed ( );

		blurTucanoGitlab_->unbind ( );

		fboSSAO->unbind ( );
		fboSSAO->clearDepth ( );

		glEnable ( GL_DEPTH_TEST );


        }


}


/// F10
void GLWidget::drawIRESModel ( )
{
        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround( );

        fboSSAO->bind();

        fboSSAO->unbind();

        glDrawBuffer(GL_BACK);

        makeCurrent();

        // Surface Faces
        RawModel_->bind( );
        //std::cout << " RawModel_->getShaderProgram() " << RawModel_->getShaderProgram() << std::endl;

        RawModel_->setUniform("num_lights", (GLint) lights.size ( )  );
        RawModel_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        RawModel_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        RawModel_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        RawModel_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        RawModel_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawFaces();//drawIndexFaces(primaries_face);

        RawModel_->unbind( );


        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->bind ( );

                BorderLines_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
                BorderLines_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
                BorderLines_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

                reservoir_model_.drawFaces ( );

                BorderLines_->unbind ( );
        }

//	rawShellLCG->bind();
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
//        rawShellLCG->unbind();


}

void GLWidget::drawBackGround ( ) const
{
	/// FIXME Conditions  - Just the model opened.

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	BackGround_->bind();

	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture ( GL_TEXTURE_2D, xtoon_texture_ );
	BackGround_->setUniform("imageTexture", 3);
	BackGround_->setUniform("viewportSize", width(), height() );

	picture->render();

	BackGround_->unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void GLWidget::PaperDemo()
{
        if ( isIRESOpen_ )
        {
                //PaperDrawCutawaySurface( );

                glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
                glDepthFunc(GL_LESS);
                glClearDepth(1.0f);
                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                makeCurrent();
                drawBackGround( );

                if ( draw_secondary )
                {
                        PaperSecondary( );
                }
                if ( draw_primary )
                {
                	PaperPly();
                }

//                glActiveTexture(GL_TEXTURE0);
//                glEnable(GL_BLEND);
//                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//
//                DummyQuad_->bind();
//
//                DummyQuad_->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );
//                DummyQuad_->setUniform("displacement", displacement[0],displacement[1],displacement[2] );
//                DummyQuad_->setUniform("max_displacement", max_displacement[0],max_displacement[1],max_displacement[2] );
//                DummyQuad_->setUniform("num_lights", (GLint) lights.size ( )  );
//                DummyQuad_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
//                DummyQuad_->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
//                DummyQuad_->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
//                DummyQuad_->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
//
//                glBindVertexArray( vertexArray_Dummy );
//                glDrawArrays( GL_POINTS, 0, 1 );
//                glBindVertexArray( 0);
//
//                DummyQuad_->unbind();
//                glDisable(GL_BLEND);
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


        BurnsPrimarySetup_->bind( );

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
        BurnsPrimarySetup_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        BurnsPrimarySetup_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        BurnsPrimarySetup_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        BurnsPrimarySetup_->setUniform ("freeze", freezeView_ );
        BurnsPrimarySetup_->setUniform ("FreezeViewMatrix",freeze_viewmatrix_);

        glBindVertexArray(vertexArray_Dummy);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        BurnsPrimarySetup_->unbind( );


        glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

        meanFilter->renderTexture( depthFBO->getTexture(0),viewport_);

        depthFBO->unbind();


        glDrawBuffer(GL_BACK);

}

void GLWidget::PaperSecondary( ) const
{

        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        drawBackGround( );

        /// Surface Faces

        BurnsSecondary_->bind( );

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

        BurnsSecondary_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        BurnsSecondary_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        BurnsSecondary_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawFaces();

        BurnsSecondary_->unbind( );


        glEnable(GL_BLEND);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->bind ( );

                BorderLines_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
                BorderLines_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
                BorderLines_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

                reservoir_model_.drawFaces ( );

                BorderLines_->unbind ( );
        }
        glDisable(GL_BLEND);


}

void GLWidget::PaperPrimary( ) const
{

        BurnsPrimary_->bind( );

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

        BurnsPrimary_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        BurnsPrimary_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        BurnsPrimary_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        reservoir_model_.drawCuboid();

        BurnsPrimary_->unbind( );

        if ( reservoir_model_.showBorderLine )
        {

                glLineWidth( (float) borderLinesSize_ );
                BorderLines_->bind ( );

                BorderLines_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
                BorderLines_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
                BorderLines_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

                reservoir_model_.drawFaces ( );

                BorderLines_->unbind ( );
        }

}

void GLWidget::PaperPly ( )
{
        /// FIXME Conditions - Just the model opened.

        BurnsPly_->bind();

        BurnsPly_->setUniform("move_x", move_x );
        BurnsPly_->setUniform("move_y", move_y );
        BurnsPly_->setUniform("move_z", move_z );

        BurnsPly_->setUniform("move", (float) borderLinesSize_ );
        BurnsPly_->setUniform("num_lights", (GLint) lights.size ( )  );
        BurnsPly_->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );

        BurnsPly_->setUniform ( "ModelMatrix" , Eigen::Affine3f::Identity() );
        BurnsPly_->setUniform ( "ViewMatrix" , camera.getViewMatrix ( ) );
        BurnsPly_->setUniform ( "ProjectionMatrix" , camera.getProjectionMatrix ( ) );

        ply_primary_.Draw();

        BurnsPly_->unbind();

}


void GLWidget::reloadShaders ( )
{
        // Effects --
        meanFilter->reloadShaders();

        BorderLines_->reloadShaders ( );
        BackGround_->reloadShaders ( );
        // Paper Demo
        RawModel_->reloadShaders ( );

        BurnsPrimary_->reloadShaders ( );
        BurnsPrimarySetup_->reloadShaders ( );
        BurnsSecondary_->reloadShaders ( );
        DummyQuad_->reloadShaders ( );

        // ! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic_->reloadShaders ( );

                SSAOIRESPrimaryStaticSilhouette_->reloadShaders();
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
        ssaoShaderTucanoGitlab_->reloadShaders();
        blurTucanoGitlab_->reloadShaders();
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
                BorderLines_ = new Tucano::Shader ("Border Lines",
                                          (shaderDirectory + "BorderLines.vert").toStdString(),
                                          (shaderDirectory + "BorderLines.frag").toStdString(),
                                          (shaderDirectory + "BorderLines.geom").toStdString(),"","");
                BorderLines_->initialize();
                std::cout << " BorderLines_ " << BorderLines_->getShaderProgram() << std::endl;


                meanFilter->setShadersDir( shaderDirectory.toStdString() );
                meanFilter->initialize( );

                //std::cout << " meanFilter " << meanFilter->getShaderProgram() << std::endl;

                BackGround_ = new Tucano::Shader  ("BackGround", (shaderDirectory + "BackGround.vert").toStdString(),
                                         (shaderDirectory + "BackGround.frag").toStdString(),"","","");
                BackGround_->initialize( );

                std::cout << " BackGround_ " << BackGround_->getShaderProgram() << std::endl;

        //! Paper Demo

                RawModel_ = new Tucano::Shader ("Raw Model",
                                         (shaderDirectory + "RawModel.vert").toStdString(),
                                         (shaderDirectory + "RawModel.frag").toStdString(),
                                         (shaderDirectory + "RawModel.geom").toStdString(),"","");

                RawModel_->initialize( );
                std::cout << " RawModel_ " << RawModel_->getShaderProgram() << std::endl;

                BurnsPrimary_ = new Tucano::Shader ("BurnsPrimary",
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.vert").toStdString(),
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.frag").toStdString(),
                                      (shaderDirectory + "Demo/BurnsPrimaryStatic.geom").toStdString(),"","");


                BurnsPrimary_->initialize( );
                std::cout << " BurnsPrimary_ " << BurnsPrimary_->getShaderProgram() << std::endl;

                BurnsPrimarySetup_ = new Tucano::Shader ("BurnsPrimarySetup",
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.vert").toStdString(),
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.frag").toStdString(),
                                                (shaderDirectory + "Demo/BurnsPrimarySetup.geom").toStdString(),"","");
                BurnsPrimarySetup_->initialize( );
                std::cout << " BurnsPrimarySetup_ " << BurnsPrimarySetup_->getShaderProgram() << std::endl;

                BurnsSecondary_ = new Tucano::Shader ("BurnsSecondary",
                                         (shaderDirectory + "Demo/BurnsSecondary.vert").toStdString(),
                                         (shaderDirectory + "Demo/BurnsSecondary.frag").toStdString(),
                                         (shaderDirectory + "Demo/BurnsSecondary.geom").toStdString(),"","");

                BurnsSecondary_->initialize( );
                std::cout << " BurnsSecondary_ " << BurnsSecondary_->getShaderProgram() << std::endl;

                DummyQuad_ = new Tucano::Shader ("DummyQuad",
                                        (shaderDirectory + "Demo/DummyQuad.vert").toStdString(),
                                        (shaderDirectory + "Demo/DummyQuad.frag").toStdString(),
                                        (shaderDirectory + "Demo/DummyQuad.geom").toStdString(),"","");
                DummyQuad_->initialize( );
                std::cout << " DummyQuad_ " << DummyQuad_->getShaderProgram() << std::endl;
       //! DYNAMIC VIEWER F11 Static PropertieswayStatic"
                IRESCutawaySurfaceStatic_ = new Tucano::Shader ("IRESCutawaySurfaceStatic",
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.vert").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.frag").toStdString(),
                                                      (shaderDirectory + "Static/IRESCutawaySurfaceStatic.geom").toStdString(),"","");
                IRESCutawaySurfaceStatic_->initialize( );
                std::cout << " IRESCutawaySurfaceStatic_ " << IRESCutawaySurfaceStatic_->getShaderProgram() << std::endl;

                IRESCutawaySurfaceDynamic_ = new Tucano::Shader ("IRESCutawaySurfaceDynamic",
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.vert").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.frag").toStdString(),
                                                       (shaderDirectory + "Dynamic/IRESCutawaySurfaceDynamic.geom").toStdString(),"","");
                IRESCutawaySurfaceDynamic_->initialize();
                std::cout << " IRESCutawaySurfaceDynamic_ " << IRESCutawaySurfaceDynamic_->getShaderProgram() << std::endl;



                BurnsPly_ = new Tucano::Shader ("BurnsPly",
                                       (shaderDirectory + "Demo/BurnsPrimary430.vert").toStdString(),
                                       (shaderDirectory + "Demo/BurnsPrimary430.frag").toStdString(),
                                        "","","");
                BurnsPly_->initialize( );
                std::cout << " BurnsPly_ " << BurnsPly_->getShaderProgram() << std::endl;
                //! SSAO
                /// The per pixel AO computation shader

                ssaoShader_ = new Tucano::Shader ("ssaoShader",
                                         (shaderDirectory + "SSAO/ssao.vert").toStdString(),
                                         (shaderDirectory + "SSAO/ssao.frag").toStdString(),
                                            "","","");
                ssaoShader_->initialize();
                std::cout << " ssaoShader_ " << ssaoShader_->getShaderProgram() << std::endl;
                //! SSAO from tucano-gitlab
			ssaoShaderTucanoGitlab_ = new Tucano::Shader ("ssaoShader",
						 (shaderDirectory + "SSAO/ssao_tucano_gitlab.vert").toStdString(),
						 (shaderDirectory + "SSAO/ssao_tucano_gitlab.frag").toStdString(),
						    "","","");
			ssaoShaderTucanoGitlab_->initialize();
			std::cout << " ssaoShaderTucanoGitlab_ " << ssaoShaderTucanoGitlab_->getShaderProgram() << std::endl;

			blurTucanoGitlab_ = new Tucano::Shader ("ssaoShader",
						 (shaderDirectory + "SSAO/blur_tuncano_gitlab.vert").toStdString(),
						 (shaderDirectory + "SSAO/blur_tuncano_gitlab.frag").toStdString(),
						    "","","");
			blurTucanoGitlab_->initialize();
			std::cout << " ssaoShaderTucanoGitlab_ " << ssaoShaderTucanoGitlab_->getShaderProgram() << std::endl;

		        /// Tucano gitlab
		        Tucano::ImageImporter::loadPPMImage(&noise_texture, (shaderDirectory + "samples/random.ppm").toStdString());
		        noise_texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST );

                deferredShader = new Tucano::Shader ("deferredShader", (shaderDirectory + "SSAO/viewspacebuffer.vert").toStdString(),
                                                             (shaderDirectory + "SSAO/viewspacebuffer.frag").toStdString(),
                                                             "","","");
                deferredShader->initialize();
                std::cout << " deferredShader " << deferredShader->getShaderProgram() << std::endl;

                blurShader_ = new Tucano::Shader ("blurShader", (shaderDirectory + "SSAO/gaussianblurfilter.vert").toStdString(),
                                                       (shaderDirectory + "SSAO/gaussianblurfilter.frag").toStdString(),
                                                             "","","");
                blurShader_->initialize();
                std::cout << " blurShader_ " << blurShader_->getShaderProgram() << std::endl;
                //! SSAO STATIC


                SSAOIRESPrimaryStaticSilhouette_ = new Tucano::Shader ("IRESPrimaryStatic",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStaticSilhouette.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStaticSilhouette.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStaticSilhouette.geom").toStdString(),"","");

                SSAOIRESPrimaryStaticSilhouette_->initialize();

                SSAOIRESPrimaryStatic_ = new Tucano::Shader ("IRESPrimaryStatic",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESPrimaryStatic.geom").toStdString(),"","");

                SSAOIRESPrimaryStatic_->initialize();
                std::cout << " SSAOIRESPrimaryStatic_ " << SSAOIRESPrimaryStatic_->getShaderProgram() << std::endl;

                SSAOIRESCutawayStatic_ = new Tucano::Shader ("SSAOIRESCutawayStatic_",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStatic.geom").toStdString(),"","");

                SSAOIRESCutawayStatic_->initialize();
                std::cout << " SSAOIRESCutawayStatic_ " << SSAOIRESCutawayStatic_->getShaderProgram() << std::endl;

                SSAOIRESCutawayStaticShell_ = new Tucano::Shader ("SSAOIRESCutawayStaticShell_",
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.vert").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.frag").toStdString(),
                                                (shaderDirectory + "Static/SSAO/SSAOIRESCutawayStaticShell.geom").toStdString(),"","");

                SSAOIRESCutawayStaticShell_->initialize();
                std::cout << " SSAOIRESCutawayStaticShell_ " << SSAOIRESCutawayStaticShell_->getShaderProgram() << std::endl;

                //! SSAO Dynamic

                SSAOIRESPrimaryDynamic_ = new Tucano::Shader ("IRESPrimaryDynamic",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESPrimaryDynamic.geom").toStdString(),"","");

                SSAOIRESPrimaryDynamic_->initialize();
                std::cout << " SSAOIRESPrimaryDynamic_ " << SSAOIRESPrimaryDynamic_->getShaderProgram() << std::endl;

                SSAOIRESCutawayDynamic_ = new Tucano::Shader ("SSAOIRESCutawayDynamic",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamic.geom").toStdString(),"","");

                SSAOIRESCutawayDynamic_->initialize();
                std::cout << " SSAOIRESCutawayDynamic_ " << SSAOIRESCutawayDynamic_->getShaderProgram() << std::endl;

                SSAOIRESCutawayDynamicShell_ = new Tucano::Shader ("SSAOIRESCutawayDynamicShell_",
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.vert").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.frag").toStdString(),
                                                (shaderDirectory + "Dynamic/SSAO/SSAOIRESCutawayDynamicShell.geom").toStdString(),"","");

                SSAOIRESCutawayDynamicShell_->initialize();
                std::cout << " SSAOIRESCutawayDynamicShell_ " << SSAOIRESCutawayDynamicShell_->getShaderProgram() << std::endl;





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
                camera.reset();
                camera.decreaseZoom( 5.05 );
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
                        camera.setTrackballPerspectiveMatrix( fovy_ , aspect , nearPlane_ , farPlane_ );
                else
                        camera.setTrackballOrthographicMatrix( -1.0f*aspect , 1.0f*aspect , -1.0f , 1.0 , 0.1f , 500.0f );

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
	        position_one = camera.getViewMatrix();
	}
        else if ( event->key() == Qt::Key_8)
        {
                position_two = camera.getViewMatrix();
        }
        else if ( event->key() == Qt::Key_9)
        {
        	camera.setViewMatrix( position_one );
        }
        else if ( event->key() == Qt::Key_0)
        {
                camera.setViewMatrix( position_two );
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
                takes_[take_index_].keyframes_[takes_[take_index_].number_of_keyframes_] = (camera.getQuaternion());

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

	Eigen::Vector2f screen_pos ( event->x ( ) , event->y ( ) );
	if ( event->modifiers ( ) & Qt::ShiftModifier )
	{
		if ( event->button ( ) == Qt::LeftButton )
		{
			camera.translateCamera ( screen_pos );
		}
	}
	else
	{
		if ( event->button ( ) == Qt::LeftButton )
		{
			camera.rotateCamera ( screen_pos );
		}
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
		camera.endTranslation ( );
		camera.endRotation ( );
	}
	if ( event->button ( ) == Qt::MiddleButton )
	{
	          zoom_start_ = false;
	}

    update();
}

void GLWidget::mouseMoveEvent ( QMouseEvent *event )
{
	Eigen::Vector2f screen_pos ( event->x ( ) , event->y ( ) );
	if ( ( event->modifiers ( ) & Qt::ShiftModifier )  )
	{

		if ( event->buttons ( ) & Qt::LeftButton )
		{
			camera.translateCamera ( screen_pos );
		}

	}
	else
	{
		if ( event->buttons ( ) & Qt::LeftButton )
		{
			camera.rotateCamera ( screen_pos );
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

void GLWidget::changeTimeStep ( const int step )
{
	makeCurrent();
        std::cout << " ... " << step << std::endl;
        this->time_step = step;

        glBindVertexArray ( reservoir_model_.vertexArrayCuboids );

        glBindBuffer ( GL_ARRAY_BUFFER , reservoir_model_.cuboidDynamicIds[dynamic_property_index][time_step] );
        glEnableVertexAttribArray ( 9 );
        glVertexAttribPointer ( 9 , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );

        glBindVertexArray ( 0 );

        glBindVertexArray ( reservoir_model_.vertexArrayFaces );

        glBindBuffer ( GL_ARRAY_BUFFER , reservoir_model_.faceDynamicIds[dynamic_property_index][time_step] );
        glEnableVertexAttribArray ( 6 );
        glVertexAttribPointer ( 6 , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );

        glBindVertexArray ( 0 );

        update ( );
}

void GLWidget::changeDynamicProperty ( int index )
{
	makeCurrent();
        this->dynamic_property_index = index;

        glBindVertexArray ( reservoir_model_.vertexArrayCuboids );

        glBindBuffer ( GL_ARRAY_BUFFER , reservoir_model_.cuboidDynamicIds[dynamic_property_index][time_step] );
        glEnableVertexAttribArray ( 9 );
        glVertexAttribPointer ( 9 , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );

        glBindVertexArray ( 0 );

        glBindVertexArray ( reservoir_model_.vertexArrayFaces );

        glBindBuffer ( GL_ARRAY_BUFFER , reservoir_model_.faceDynamicIds[dynamic_property_index][time_step] );
        glEnableVertexAttribArray ( 6 );
        glVertexAttribPointer ( 6 , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );

        glBindVertexArray ( 0 );

        update ( );
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
                freeze_viewmatrix_ = camera.getViewMatrix ( );
        }
}
