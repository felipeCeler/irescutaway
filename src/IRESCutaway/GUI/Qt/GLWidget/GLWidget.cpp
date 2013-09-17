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

	dynamic_ = false;

	/// OpenGL Stuffs
	glEnable ( GL_DEPTH_TEST );
	glDepthMask(GL_TRUE);

	glEnable ( GL_TEXTURE_2D );

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

	zoom_angle_ = 45.0f;
	orthoZoom   = 1.0f;

	lights.push_back ( Eigen::Vector3f ( 0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( -0.5 , 0.5 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 0.0 , 1.0 ) );
	lights.push_back ( Eigen::Vector3f ( 0.0 , 1.0 , 0.0 ) );


	// Cuboid
	glGenVertexArrays ( 1, &vertexArray_cuboid );
		glGenBuffers  ( 1, &vertexBuffer_cuboid ); 	     // Geometry
		glGenBuffers  ( 1, &vertexBuffer_cube_color  );      // Property Color
		glGenBuffers  ( 1, &vertexBuffer_cube_IJK  );        // Cube IJK
		glGenBuffers  ( 1, &vertexBuffer_cube_properties  ); // Cube Property

	// Face Features
	glGenVertexArrays ( 1, &vertexArray_faceFeature );
		glGenBuffers ( 1, &vertexBuffer_faceFeature );    // Geometry
		glGenBuffers ( 1, &vertexBuffer_faceColor );      // Property Color
		glGenBuffers ( 1, &vertexBuffer_faceIJK );        // Face IJK
		glGenBuffers ( 1, &vertexBuffer_faceProperties ); // Face Properties

	draw_secondary = 1;
	draw_primary = 0;

	isRawApproach	      = 0;
	isIRESApproach        = 0;

	cluster = 0;

	//Timer Animation

	fps = 0;

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

	trackball_ = new Trackball();

	trackball_->initOpenGLMatrices();

	trackball_->initializeBuffers();

	enable_blend_ = 0;

	depthFBO = new Framebuffer( width() , height(), 2 );

	meanFilter = new MeanFilter( "Gaussian blur");
	meanFilter->resize(width(), height());

	properties_name[0] = "Bubble Point Pressure";
	properties_name[1] = "Pressure";
	properties_name[2] = "Porosity";
	properties_name[3] = "Modified Block Volume";

	loadShaders ( );

}

bool GLWidget::isIRESOpen ( ) const
{
	return isIRESOpen_;
}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{

	min_range = minRange;
	max_range = maxRange;

	current_property = property_index;

	dynamic_ = true;

    updateGL();

}

void GLWidget::changeProperty ( int property_index )
{

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = min_value[property_index];
	float max = max_value[property_index];

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	current_property = property_index;

}

void GLWidget::loadProperties( )
{

	// Four property x = Bubble Point Pressure
	//               y = Pressure
	//	         z = Porosity
	//               w = Modified Block Volume

	for ( std::size_t property_index = 0; property_index < reservoir_model_.static_porperties.size( ); property_index++ )
	{
		if ( properties_name[0].compare( reservoir_model_.static_porperties[property_index].name ) == 0 )
		{
			std::cout << "Bubble -> " << reservoir_model_.static_porperties[property_index].name << std::endl;
			min_value[0] =  reservoir_model_.static_porperties[property_index].min_;
			max_value[0] =  reservoir_model_.static_porperties[property_index].max_;
			indices[0] = property_index;
		}
		if ( properties_name[1].compare( reservoir_model_.static_porperties[property_index].name ) == 0 )
		{
			std::cout << "Pressure -> " << reservoir_model_.static_porperties[property_index].name << std::endl;
			min_value[1] =  reservoir_model_.static_porperties[property_index].min_;
			max_value[1] =  reservoir_model_.static_porperties[property_index].max_;
			indices[1] = property_index;
		}
		if ( properties_name[2].compare( reservoir_model_.static_porperties[property_index].name ) == 0 )
		{
			std::cout << "Porosity -> " << reservoir_model_.static_porperties[property_index].name << std::endl;
			min_value[2] =  reservoir_model_.static_porperties[property_index].min_;
			max_value[2] =  reservoir_model_.static_porperties[property_index].max_;
			indices[2] = property_index;
		}
		if ( properties_name[3].compare( reservoir_model_.static_porperties[property_index].name ) == 0 )
		{
			std::cout << "Volume -> " << reservoir_model_.static_porperties[property_index].name << std::endl;
			min_value[3] =  reservoir_model_.static_porperties[property_index].min_;
			max_value[3] =  reservoir_model_.static_porperties[property_index].max_;
			indices[3] = property_index;
		}
	}

	current_property = 0;

	int index = 0;

	for ( std::size_t i = 0; i < reservoir_model_.blocks.size( ); i++)
	{
		if ( reservoir_model_.blocks[i].valid )
		{
			cubeProperties[index]   = reservoir_model_.blocks[i].static_porperties[indices[0]].value_;
			cubeProperties[index+1] = reservoir_model_.blocks[i].static_porperties[indices[1]].value_;
			cubeProperties[index+2] = reservoir_model_.blocks[i].static_porperties[indices[2]].value_;
			cubeProperties[index+3] = reservoir_model_.blocks[i].static_porperties[indices[3]].value_;
			index += 4;
		}
		else
		{
			continue;
		}
	}

	for ( std::size_t shell_index = 0; shell_index < reservoir_model_.list_of_block_id.size() ; shell_index++ )
	{
		facesFeatureProperties[shell_index] = cubeProperties[reservoir_model_.list_of_block_id[shell_index]];
	}

	// Cuboid
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_properties);
	glBufferData ( GL_ARRAY_BUFFER , cubeProperties.size( ) * sizeof(cubeProperties[0]) , &cubeProperties[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	// FaceFeature
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceProperties );
	glBufferData ( GL_ARRAY_BUFFER , facesFeatureProperties.size( ) * sizeof(facesFeatureProperties[0]) , &facesFeatureProperties[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);


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

void GLWidget::openIRESCharles( const std::string& filename )
{

	reservoir_model_.openIRES_Version_2( filename );

	if ( reservoir_model_.blocks.size( ) > 0 )
	{
		cuboids.clear();
		cubeColor.clear( );
		cubeIJK.clear( );
		cubeFocus.clear( );

		facesFeature.clear();

		// Geometry
		facesFeature.resize ( reservoir_model_.list_of_block_id.size ( ) * 16 );
		// Attributes
		facesFeatureColors.resize ( reservoir_model_.list_of_block_id.size ( ) * 4 );
		facesFeatureIJK.resize ( reservoir_model_.list_of_block_id.size ( ) * 4 );
		facesFeatureType.resize ( reservoir_model_.list_of_block_id.size ( ) * 4  );
		facesFeatureProperties.resize ( reservoir_model_.list_of_block_id.size ( ) * 4 );

		for ( std::size_t i = 0; i < reservoir_model_.list_of_block_id.size( ) ; i++)
		{
			facesFeature[i] = reservoir_model_.list_of_vertex_geometry_a[i*3];
			facesFeature[i+1] = reservoir_model_.list_of_vertex_geometry_a[i*3+1];
			facesFeature[i+2] = reservoir_model_.list_of_vertex_geometry_a[i*3+2];
			facesFeature[i+3] = 1.0f;

			facesFeature[i+4] = reservoir_model_.list_of_vertex_geometry_b[i*3];
			facesFeature[i+5] = reservoir_model_.list_of_vertex_geometry_b[i*3+1];
			facesFeature[i+6] = reservoir_model_.list_of_vertex_geometry_b[i*3+2];
			facesFeature[i+7] = 1.0f;

			facesFeature[i+8] = reservoir_model_.list_of_vertex_geometry_c[i*3];
			facesFeature[i+9] = reservoir_model_.list_of_vertex_geometry_c[i*3+1];
			facesFeature[i+10] = reservoir_model_.list_of_vertex_geometry_c[i*3+2];
			facesFeature[i+11] = 1.0f;

			facesFeature[i+12] = reservoir_model_.list_of_vertex_geometry_d[i*3];
			facesFeature[i+13] = reservoir_model_.list_of_vertex_geometry_d[i*3+1];
			facesFeature[i+14] = reservoir_model_.list_of_vertex_geometry_d[i*3+2];
			facesFeature[i+15] = 1.0f;

			facesFeatureColors[i]   = 1.0f;
			facesFeatureColors[i+1] = 0.0f;
			facesFeatureColors[i+2] = 0.0f;
			facesFeatureColors[i+3] = 1.0f;

			facesFeatureIJK[i]   = 1.0f;
			facesFeatureIJK[i+1] = 0.0f;
			facesFeatureIJK[i+2] = 0.0f;
			facesFeatureIJK[i+3] = 1.0f;

			facesFeatureType[i]   = reservoir_model_.list_of_block_flag[i];
			facesFeatureType[i+1] = reservoir_model_.list_of_block_flag[i];
			facesFeatureType[i+2] = reservoir_model_.list_of_block_flag[i];
			facesFeatureType[i+3] = 1.0f;
		}

		std::cout << " Face Interleaved = " << reservoir_model_.list_of_block_id.size() << std::endl;

		cuboids.resize        ( reservoir_model_.blocks.size( ) * 32 );
		cubeColor.resize      ( reservoir_model_.blocks.size( ) * 4  );
		cubeIJK.resize        ( reservoir_model_.blocks.size( ) * 4  );
		cubeFocus.resize      ( reservoir_model_.blocks.size( ) * 4  );
		cubeProperties.resize ( reservoir_model_.blocks.size( ) * 4  );

		int index = 0;

		for ( std::size_t i = 0; i < reservoir_model_.blocks.size( ) ; i++)
		{

			if ( reservoir_model_.blocks[i].valid )
			{
				// @link http://www.cplusplus.com/reference/iterator/back_inserter/
				std::copy ( reservoir_model_.blocks[i].vertices.begin(), reservoir_model_.blocks[i].vertices.end(), std::back_inserter(cuboids) );

				cubeColor[index]   = 1.0f;
				cubeColor[index+1] = 0.0f;
				cubeColor[index+2] = 0.0f;
				cubeColor[index+3] = 1.0f;

				cubeIJK[index]   = reservoir_model_.blocks[i].IJK[0];
				cubeIJK[index+1] = reservoir_model_.blocks[i].IJK[1];
				cubeIJK[index+2] = reservoir_model_.blocks[i].IJK[2];
				cubeIJK[index+3] = reservoir_model_.blocks[i].IJK[3];

				cubeFocus[index]   = 1.0f;
				cubeFocus[index+1] = 0.0f;
				cubeFocus[index+2] = 0.0f;
				cubeFocus[index+3] = 1.0f;

				index += 4;

			}
			else
			{

			}
		}

		cubeColor.resize(index);
		cubeFocus.resize(index);
		cubeIJK.resize(index);

		changeProperty(0);

		max_I_ = 0;
		min_I_ = 0;
		max_J_ = 0;
		min_J_ = 0;
		max_K_ = 0;
		min_K_ = 0;


		// trackball_->translateModelMatrix( Eigen::Vector3f(reservoir_model_.box_v2.center ( ).x,reservoir_model_.box_v2.center ( ).y,reservoir_model_.box_v2.center ( ).z));

		glBindVertexArray ( vertexArray_cuboid );

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cuboid );
                        glBufferData ( GL_ARRAY_BUFFER , cuboids.size( ) * sizeof(cuboids[0]) , &cuboids[0] , GL_STATIC_DRAW );

                        int size_of_vertice = 4 * sizeof(float);
                        int size_of_struct  = 8 * size_of_vertice;

                        //http://www.opengl.org/wiki/Vertex_Specification
                        for ( int location = 0 ; location < 8 ; location++)
                        {
                                glEnableVertexAttribArray(location);
                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
                        }

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_color);
                        glBufferData ( GL_ARRAY_BUFFER , cubeColor.size( ) * sizeof(cubeColor[0]) , &cubeColor[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(8);
                        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_IJK);
                        glBufferData ( GL_ARRAY_BUFFER , cubeIJK.size( ) * sizeof(cubeIJK[0]) , &cubeIJK[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(9);
                        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_Focus);
                        glBufferData ( GL_ARRAY_BUFFER , cubeFocus.size( ) * sizeof(cubeFocus[0]) , &cubeFocus[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(10);
                        glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_properties);
                        glBufferData ( GL_ARRAY_BUFFER , cubeProperties.size( ) * sizeof(cubeProperties[0]) , &cubeProperties[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(11);
                        glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 0, 0);


                glBindVertexArray(0);

                /// FacesFeatures

		glBindVertexArray ( vertexArray_faceFeature );

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceFeature );
                        glBufferData ( GL_ARRAY_BUFFER , facesFeature.size( ) * sizeof(facesFeature[0]) , &facesFeature[0] , GL_STATIC_DRAW );

                        int size_of_vertice_face = 4 * sizeof(float);
                        int size_of_struct_face  = 4 * sizeof(size_of_vertice_face);

                        //http://www.opengl.org/wiki/Vertex_Specification
                        for ( int location = 0 ; location < 4 ; location++)
                        {
                                glEnableVertexAttribArray(location);
                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct_face , reinterpret_cast<void*>(size_of_vertice_face * location));
                        }

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceType);
                        glBufferData ( GL_ARRAY_BUFFER , facesFeatureType.size( ) * sizeof(facesFeatureType[0]) , &facesFeatureType[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(4);
                        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceColor);
                        glBufferData ( GL_ARRAY_BUFFER , facesFeatureColors.size( ) * sizeof(facesFeatureColors[0]) , &facesFeatureColors[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(5);
                        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceIJK);
                        glBufferData ( GL_ARRAY_BUFFER , facesFeatureIJK.size( ) * sizeof(facesFeatureIJK[0]) , &facesFeatureIJK[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(6);
                        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceProperties);
                        glBufferData ( GL_ARRAY_BUFFER , facesFeatureProperties.size( ) * sizeof(facesFeatureProperties[0]) , &facesFeatureProperties[0] , GL_STATIC_DRAW );

                        glEnableVertexAttribArray(7);
                        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);

                glBindVertexArray(0);

		isIRESOpen_ = 1;

		loadProperties();
	}
	else
	{
	        isIRESOpen_ = 0;
	}

}

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );

        trackball_->useOrthographicMatrix(-1.0f, 1.0 , -1.0f, 1.0, 0.0, 500.0);

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
		BoundingBoxInitializationLCG->setUniform("min_property", min_value[current_property]  );
		BoundingBoxInitializationLCG->setUniform("max_property", max_value[current_property]  );
		BoundingBoxInitializationLCG->setUniform("property_index", current_property );

		BoundingBoxInitializationLCG->setUniform( "x" , volume_width );
		BoundingBoxInitializationLCG->setUniform( "y" , volume_height );
		BoundingBoxInitializationLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
		BoundingBoxInitializationLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
		BoundingBoxInitializationLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

		BoundingBoxInitializationLCG->setUniform ("freeze", freezeView_ );
		BoundingBoxInitializationLCG->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

		glBindVertexArray ( vertexArray_cuboid );
		glDrawArrays ( GL_POINTS , 0 , cuboids.size ( ) );
		glBindVertexArray ( 0 );

		BoundingBoxInitializationLCG->disable( );


		glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

		meanFilter->renderTexture( depthFBO->bindAttachment(0));

		depthFBO->unbindAll();


		glDrawBuffer(GL_BACK);

}

void GLWidget::drawSecundary ( )
{
	if ( enable_blend_ )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	}

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

	shellLCG->setUniform("min_property", min_value[current_property]  );
	shellLCG->setUniform("max_property", max_value[current_property]  );
	shellLCG->setUniform("property_index", current_property );

	shellLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	shellLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	shellLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	shellLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	shellLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	shellLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	glBindVertexArray ( vertexArray_faceFeature );
	glDrawArrays ( GL_POINTS , 0 , facesFeature.size ( ) );
	glBindVertexArray ( 0 );

	shellLCG->disable( );

	if ( enable_blend_ )
	{
		glDisable(GL_BLEND);
	}

	depthFBO->unbindAttachments();

	BoundingBoxCutawayLCG->enable( );

	BoundingBoxCutawayLCG->setUniform( "normals" , depthFBO->bindAttachment(1) );

	BoundingBoxCutawayLCG->setUniform("min_property", min_value[current_property]  );
	BoundingBoxCutawayLCG->setUniform("max_property", max_value[current_property]  );
	BoundingBoxCutawayLCG->setUniform("property_index", current_property );

	BoundingBoxCutawayLCG->setUniform("num_lights", (GLint) lights.size ( )  );
	BoundingBoxCutawayLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
	BoundingBoxCutawayLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

	BoundingBoxCutawayLCG->setUniform( "min_IJK" , min_I_ , min_J_ , min_K_ );
	BoundingBoxCutawayLCG->setUniform( "max_IJK" , max_I_ , max_J_ , max_K_ );

	BoundingBoxCutawayLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxCutawayLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	BoundingBoxCutawayLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	glBindVertexArray ( vertexArray_cuboid );
	glDrawArrays ( GL_POINTS , 0 , cuboids.size ( ) );
	glBindVertexArray ( 0 );

	BoundingBoxCutawayLCG->enable( );

	depthFBO->unbindAttachments();
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
	primaryLCG->setUniform("min_property", min_value[current_property]  );
	primaryLCG->setUniform("max_property", max_value[current_property]  );
	primaryLCG->setUniform("property_index", current_property );

	primaryLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
	primaryLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
	primaryLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

	glBindVertexArray ( vertexArray_cuboid );
	glDrawArrays ( GL_POINTS , 0 , cuboids.size ( ) );
	glBindVertexArray ( 0 );


	primaryLCG->disable( );


}

void GLWidget::paintGL ( )
{


	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}


	if ( isRawApproach )
	{
		RawCutaway ( cluster );
	}
	else if ( isIRESApproach )
	{
		IRESCutaway ( );
	}else
	{
		NoCutaway ( );
	}

	//trackball_->render();

	fps++;


}

////Timer
//void GLWidget::timerEvent( QTimerEvent *event )
//{
//
//
//
//}

void GLWidget::NoCutaway ( )
{
        glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        if ( draw_secondary && (cuboids.size() != 0) )
        {

        	GLfloat light_elements[12];
        	for ( std::size_t i = 0; i < lights.size ( ); ++i )
        	{
        		for ( int j = 0; j < 3; ++j )
        		{
        			light_elements[i * 3 + j] = lights[i][j];
        		}
        	}

        	rawShellLCG->enable( );

        	rawShellLCG->setUniform("min_property", min_value[current_property]  );
        	rawShellLCG->setUniform("max_property", max_value[current_property]  );
        	rawShellLCG->setUniform("property_index", current_property );

        	rawShellLCG->setUniform("num_lights", (GLint) lights.size ( )  );
        	rawShellLCG->setUniform("lights[0]", light_elements,3, (GLint) lights.size ( )  );
        	rawShellLCG->setUniform("WIN_SCALE", (float) width ( ) , (float) height ( ) );

        	rawShellLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
        	rawShellLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
        	rawShellLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

        	glBindVertexArray ( vertexArray_faceFeature );
        	glDrawArrays ( GL_POINTS , 0 , facesFeature.size ( ) );
        	glBindVertexArray ( 0 );

        	rawShellLCG->disable( );

        }

        if ( draw_primary && (cuboids.size() != 0)  )
        {
        	drawPrimary();
        }
}

void GLWidget::RawCutaway ( int cluster )
{

//	/// FIXME Conditions  - Primary and Secondary well defined.

 	if ( isIRESOpen_ )
	{

 		drawCutawaySurface( );

		glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
		glDepthFunc(GL_LESS);
		glClearDepth(1.0f);
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if ( draw_secondary )
		{
			drawSecundary( );
		}
		if ( draw_primary )
		{
			drawPrimary( );

		}
	}

}

void GLWidget::IRESCutaway ( )
{
//        /// FIXME Conditions  - Just the model opened.
//
        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLWidget::gameLooping ( )
{

	//std::cout << "fps :" << v  << std::endl;

     //updateGL();

    //    fps++;



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

        rawShellLCG->initialize( );

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

void GLWidget::dragEnterEvent(QDragEnterEvent *event)
{

	setBackgroundRole(QPalette::Highlight);

	event->acceptProposedAction();

	emit changed(event->mimeData());

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

	}


}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
		event->accept ( );
}

void GLWidget::setPrimaryVisibility( bool visibility )
{
	if ( cuboids.size() > 0 )
		draw_primary = visibility;
	updateGL();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( cuboids.size() > 0 )
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
