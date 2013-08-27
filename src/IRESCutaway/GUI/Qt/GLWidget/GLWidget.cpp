#include <QtGui/QtGui>

#include <GUI/Qt/GLWidget/GLWidget.hpp>
// Se quiser usar QPainter Ver exemplo no QT demo - Manda Qt em wave animation !!!

GLWidget::GLWidget ( const QGLFormat& format , QWidget* parent , const QGLWidget* shareWidget , Qt::WindowFlags f ) :
	QGLWidget ( format , parent , shareWidget , f ) , reservoir_model_eclipse(true)
{

}

GLWidget::GLWidget (  QWidget* parent , const QGLWidget* shareWidget , Qt::WindowFlags f ) :
	QGLWidget ( parent , shareWidget , f ) , reservoir_model_eclipse(true)
{

}
/// OpenGL
void GLWidget::initializeGL ( )
{
	/// Celer OpenGL
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File GLWidget.cpp method initializeGL ( )" );
	/// Celer OpenGL

	buttonRelease_ = false;

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


	loadShaders ( );

	isIRESOpen_ = 0;

	scrollStep_ = 45.0f;
	zoom_angle_ = 45.0f;
	orthoZoom   = 1.0f;

	modelMatrix_ = Celer::Matrix4x4<float> ( 1.0f,0.0f,0.0f,0.0,
					         0.0f,1.0f,0.0f,0.0,
					         0.0f,0.0f,1.0f,0.0,
					         0.0f,0.0f,0.0f,1.0);

	angle = static_cast<float>(1.0/std::tan(scrollStep_ * Celer::Math::kDeg2Rad));


	QGLFramebufferObjectFormat format;
	format.setSamples(4);

	format.setAttachment(QGLFramebufferObject::Depth);
	format.setInternalTextureFormat ( GL_RGBA32F );

	format.setTextureTarget ( GL_TEXTURE_2D );


	fboStep[0] 	  = new  QGLFramebufferObject ( width() , height() , format );
	fboStep[1] 	  = new  QGLFramebufferObject ( width() , height() , format );


	// Cube in Interleaved
	glGenVertexArrays ( 1, &vertexArray_cube_interleaved );
		glGenBuffers  ( 1, &vertexBuffer_cube_interleaved );

        int size_of_vertice = sizeof(Celer::Vector4<float>);
        int size_of_struct  =  sizeof(CubeData);

        ///http://www.opengl.org/wiki/Vertex_Specification
        for ( int location = 0 ; location < 12 ; location++)
        {
                glEnableVertexAttribArray(location);
                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
        }
	/// ---

	// Uniform Buffer Global Matrix

	glGenBuffers(1, &uniformBuffer_globalMatrices_);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer_globalMatrices_);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(transformationMatrices_), 0, GL_STREAM_DRAW);
	// Like glActiveTexture + glBindTexture() combo, we need a slot in the opengl
	// context to share the uniform buffer information. The same value need to be
	// bind to the glsl program. In this case slot Zero.
	// The glUniformBlockBinding() is like the glUniform1i() for textures.
	// The glBindBufferBase is like the glBindTexture() for textures.
	// @link http://www.opengl.org/discussion_boards/showthread.php/175577-Uniform-Buffer-confusion
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer_globalMatrices_);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Uniform CutVolumes
        glGenBuffers(1, &uniformBuffer_cutVolumes_);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer_cutVolumes_);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(cutVolume_), 0, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer_cutVolumes_);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

	draw_secondary = 1;
	draw_primary = 0;

	isRawApproach	      = 0;
	isIRESApproach        = 0;

	cluster = 0;

	glGenVertexArrays ( 1, &vertexArray_box);
		glGenBuffers  ( 1, &vertexBuffer_box);

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
}

bool GLWidget::isIRESOpen ( ) const
{
	return isIRESOpen_;
}

void GLWidget::cutVolumeGenerator( )
{

	Celer::BoundingBox3<GLfloat> box;

	cutVolumes.clear();
	box_vertices.clear();

	std::cout << " number of boxes initial " << boxes.size() << std::endl;

	while ( boxes.size ( ) != 0 )
	{

		box = boxes.front( );

		boxes.pop_front( );

		std::list<Celer::BoundingBox3<GLfloat> >::iterator box_iterator = boxes.begin ( );

		int cont = 0;

		while ( ( box_iterator != boxes.end ( ) )  )
		{

			if ( box.intersect( *box_iterator ) )
			{
				box 	     = box + (*box_iterator);
				box_iterator = boxes.erase ( box_iterator );
				cont++;
			}
			else
			{
				++box_iterator;
			}
		}

		cutVolumes.push_back( box );
	}


	box_vertices.resize( cutVolumes.size( ) );
	int cont = 0;
	int cutVolumeIndex = 0;

	for ( std::vector<Celer::BoundingBox3<GLfloat> >::iterator it = cutVolumes.begin(); it != cutVolumes.end();++it)
	{

		Celer::Vector3<GLfloat> v0 ( it->box_max ( ).x , it->box_max ( ).y , it->box_max ( ).z );
		Celer::Vector3<GLfloat> v1 ( it->box_max ( ).x , it->box_max ( ).y , it->box_min ( ).z );
		Celer::Vector3<GLfloat> v2 ( it->box_min ( ).x , it->box_max ( ).y , it->box_min ( ).z );
		Celer::Vector3<GLfloat> v3 ( it->box_min ( ).x , it->box_max ( ).y , it->box_max ( ).z );

		Celer::Vector3<GLfloat> v4 ( it->box_max ( ).x , it->box_min ( ).y , it->box_max ( ).z );
		Celer::Vector3<GLfloat> v5 ( it->box_min ( ).x , it->box_min ( ).y , it->box_max ( ).z );
		Celer::Vector3<GLfloat> v6 ( it->box_min ( ).x , it->box_min ( ).y , it->box_min ( ).z );
		Celer::Vector3<GLfloat> v7 ( it->box_max ( ).x , it->box_min ( ).y , it->box_min ( ).z );


		Celer::Vector3<GLfloat> topNormal 	= ((v0 - v1) ^ (v0 - v3)).norm();
		//std::cout << topNormal    << std::endl;
		Celer::Vector3<GLfloat> bottomNormal 	= ((v4 - v5) ^ (v4 - v7)).norm();
		//std::cout << bottomNormal << std::endl;
		Celer::Vector3<GLfloat> frontNormal 	= ((v0 - v3) ^ (v0 - v4)).norm();
		//std::cout << frontNormal  << std::endl;
		Celer::Vector3<GLfloat> backmNormal 	= ((v1 - v7) ^ (v1 - v2)).norm();
		//std::cout << backmNormal  << std::endl;
		Celer::Vector3<GLfloat> rightNormal 	= ((v0 - v4) ^ (v0 - v1)).norm();
		//std::cout << rightNormal  << std::endl;
		Celer::Vector3<GLfloat> leftNormal 	= ((v2 - v6) ^ (v2 - v3)).norm();
		//std::cout << leftNormal   << std::endl;

		// Care about the type: GL_DOUBLE or GL_FLOAT
		Celer::Vector4<GLfloat> vertices[] =
		{
                        // X Y Z
			//  Top Face
			v0,v1,v3,v2,
			// Bottom Face
			v4,v5,v7,v6,
			// Front Face
			v0,v3,v4,v5,
			// Back Face
			v2,v1,v6,v7,
			// Right Face
			v0,v4,v1,v7,
			// Left Face
			v2,v6,v3,v5
		};

		box_vertices[cont] = it->center( );

		if ( cont < 63 )
		{
		        cutVolume_.center_points[cutVolumeIndex] = Celer::Vector4<float>(it->center( ),1.0);
		        cutVolumeIndex++;
		}
		cont++;
	}

	cutVolume_.size = Celer::Vector4<int> (cutVolumeIndex,0,0,0);


	glBindVertexArray ( vertexArray_box );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_box );
		glBufferData ( GL_ARRAY_BUFFER , box_vertices.size( ) * sizeof ( box_vertices[0] ) , &box_vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 0 , 0 );
	glBindVertexArray ( 0 );

	glBindBuffer ( GL_UNIFORM_BUFFER , uniformBuffer_cutVolumes_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(this->cutVolume_), &this->cutVolume_ );
	glBindBuffer ( GL_UNIFORM_BUFFER , 0);

}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{

    boxes.clear ( );

//	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::vector<Celer::Vector4<GLfloat> > colors ( 24 );
	std::vector<Celer::Vector4<GLfloat> > renderFlags ( 24 );

	Celer::Vector4<GLfloat> color;

	int index = 0;
	for ( int i = 0; i < reservoir_model_.header_.number_of_Blocks; i++)
	{
		if ( reservoir_model_.blocks[i].valid )
		{

			float normalized_color = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ - min ) / ( max - min );

			float regularValue = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ );

			Celer::Vector4<GLfloat> color (1.0,1.0,1.0,1.0);
			Celer::Vector4<GLfloat> focus (1.0,1.0,0.0,0.0);

			float fourValue = 4 * normalized_color;
			float red   = (std::min)(fourValue - 1.5, -fourValue + 4.5);
			float green = (std::min)(fourValue - 0.5, -fourValue + 3.5);
			float blue  = (std::min)(fourValue + 0.5, -fourValue + 2.5);

			red 	= (std::max)(0.0f, (std::min)(red, 1.0f));
			green 	= (std::max)(0.0f, (std::min)(green, 1.0f));
			blue 	= (std::max)(0.0f, (std::min)(blue, 1.0f));

			color = Celer::Vector4<GLfloat> ( red , green , blue , 1.0f );
//
			colors = std::vector<Celer::Vector4<GLfloat> > ( 24 , color );

			if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
			{
				focus = Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f );
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f ) );
				box.reset();
				box.fromPointCloud ( reservoir_model_.blocks[i].vertices.begin ( ) , reservoir_model_.blocks[i].vertices.end ( ) );
				boxes.push_back( box );

			}
			else
			{
				focus = Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f );
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f ) );
			}

			cube_interleaved[index].color = color;
			cube_interleaved[index].focus = focus;
			index++;
		}
		else
		{
			continue;
		}
	}

	// Loop over the boxes
	cutVolumeGenerator();

	std::cout << " number of boxes " << cutVolume_.size << std::endl;

	// Cube Interleaved
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_interleaved);
	glBufferData ( GL_ARRAY_BUFFER , cube_interleaved.size( ) * sizeof(cube_interleaved[0]) , &cube_interleaved[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);

}

void GLWidget::changeProperty ( int property_index )
{


	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	std::vector<Celer::Vector4<GLfloat> > colors;

	int index = 0;

	for ( int i = 0; i < reservoir_model_.header_.number_of_Blocks; i++)
	{
		if ( reservoir_model_.blocks[i].valid )
		{

			float normalized_color = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ - min ) / ( max - min );

			Celer::Vector4<GLfloat> color(1.0,1.0,1.0,1.0);

			// @see Implementing a Continuous "Jet" Colormap Function in GLSL - @link http://www.metastine.com/?p=7

			float fourValue = 4 * normalized_color;
			float red   = (std::min)(fourValue - 1.5, -fourValue + 4.5);
			float green = (std::min)(fourValue - 0.5, -fourValue + 3.5);
			float blue  = (std::min)(fourValue + 0.5, -fourValue + 2.5);

			red 	= (std::max)(0.0f, (std::min)(red, 1.0f));
			green 	= (std::max)(0.0f, (std::min)(green, 1.0f));
			blue 	= (std::max)(0.0f, (std::min)(blue, 1.0f));

			color = Celer::Vector4<GLfloat> ( red , green , blue , 1.0f );


			colors = std::vector<Celer::Vector4<GLfloat> > ( 24 , color );

			cube_interleaved[index].color = color;
			index++;

		}
		else
		{
			continue;
		}
	}

	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_interleaved);
	glBufferData ( GL_ARRAY_BUFFER , cube_interleaved.size( ) * sizeof(cube_interleaved[0]) , &cube_interleaved[0] , GL_STATIC_DRAW );
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

bool GLWidget::getVertices( unsigned int blockIndex, float * vertices )
{

	if ( (blockIndex >= 0) && ( blockIndex < reservoir_model_.blocks.size())  )
	{
		// TODO View CubeRendering_and_Orientation.svg
		//      Tying to translate from Charles Ires 2 Version to CelerSystem

		if ( reservoir_model_.blocks[blockIndex].valid  )
		{
			vertices[0] = reservoir_model_.blocks[blockIndex].vertices[5].x;
			vertices[1] = reservoir_model_.blocks[blockIndex].vertices[5].y;
			vertices[2] = reservoir_model_.blocks[blockIndex].vertices[5].z;

			vertices[3] = reservoir_model_.blocks[blockIndex].vertices[4].x;
			vertices[4] = reservoir_model_.blocks[blockIndex].vertices[4].y;
			vertices[5] = reservoir_model_.blocks[blockIndex].vertices[4].z;

			vertices[6] = reservoir_model_.blocks[blockIndex].vertices[6].x;
			vertices[7] = reservoir_model_.blocks[blockIndex].vertices[6].y;
			vertices[8] = reservoir_model_.blocks[blockIndex].vertices[6].z;

			vertices[9]  = reservoir_model_.blocks[blockIndex].vertices[7].x;
			vertices[10] = reservoir_model_.blocks[blockIndex].vertices[7].y;
			vertices[11] = reservoir_model_.blocks[blockIndex].vertices[7].z;

			vertices[12] = reservoir_model_.blocks[blockIndex].vertices[0].x;
			vertices[13] = reservoir_model_.blocks[blockIndex].vertices[0].y;
			vertices[14] = reservoir_model_.blocks[blockIndex].vertices[0].z;

			vertices[15] = reservoir_model_.blocks[blockIndex].vertices[2].x;
			vertices[16] = reservoir_model_.blocks[blockIndex].vertices[2].y;
			vertices[17] = reservoir_model_.blocks[blockIndex].vertices[2].z;

			vertices[18] = reservoir_model_.blocks[blockIndex].vertices[3].x;
			vertices[19] = reservoir_model_.blocks[blockIndex].vertices[3].y;
			vertices[20] = reservoir_model_.blocks[blockIndex].vertices[3].z;

			vertices[21] = reservoir_model_.blocks[blockIndex].vertices[1].x;
			vertices[22] = reservoir_model_.blocks[blockIndex].vertices[1].y;
			vertices[23] = reservoir_model_.blocks[blockIndex].vertices[1].z;

			cont = blockIndex;

			return true;

		}else
		{

			return false;
		}

	}

	return false;

}

void GLWidget::IRESv1_to_IRESv2( const std::string& filename )
{

	reservoir_model_.openIRES( filename );

	ires::Ires new_reservoir_file_(true);

	std::function<bool (unsigned int , float * )> fn = std::bind(&GLWidget::getVertices, this, std::placeholders::_1, std::placeholders::_2);

	cont = 0;

	new_reservoir_file_.buildVertexBlockLists(reservoir_model_.header_.number_of_Blocks_in_I_Direction,
						  reservoir_model_.header_.number_of_Blocks_in_J_Direction,
						  reservoir_model_.header_.number_of_Blocks_in_K_Direction, fn);


	std::vector<std::string> names;
	names.resize(reservoir_model_.static_porperties.size( ));
	std::vector<float> values;

	for ( std::size_t i = 0 ; i < reservoir_model_.static_porperties.size( ) ; i++)
	{
		names[i] = reservoir_model_.static_porperties[i].name;
		std::copy ( reservoir_model_.static_porperties[i].values_.begin() 	, reservoir_model_.static_porperties[i].values_.end() , std::back_inserter ( values  ) );
	}

	new_reservoir_file_.setStaticProps(names,values);

	std::cout << " Block Index " << cont  << std::endl;

	new_reservoir_file_.setHeaderData("sapphire-208000", ires::Date(ires::Date::Year(2013), ires::Date::Month(6), ires::Date::Day(10)), 0);

	bool result = new_reservoir_file_.writeFile("sapphire-208000.ires2");

	std::cout << "result" << result << std::endl;

	if (result )
	{
		new_reservoir_file_.readFile( "sapphire-208000.ires2.ires" );
	}

	std::size_t i = cont;

	std::vector<std::string > new_names;
	std::vector<float> new_values;

	if ( reservoir_model_.blocks[i].valid )
	{

		float v[24];
		new_reservoir_file_.getBlockVertices(i, v);

		new_reservoir_file_.getStaticPropertyNames(new_names);
		new_reservoir_file_.getStaticPropertyValues(0 , new_values );

		std::cout << " Static " <<  new_names.size() << std::endl;


		for ( std::size_t i = 0 ; i < new_names.size( ); i++)
		{
			std::cout << " names " << new_names[i] << std::endl;
		}


		for  ( std::size_t j = 0 ; j <  reservoir_model_.static_porperties.size() ; j++)
		{
			new_reservoir_file_.getStaticPropertyValues(j , new_values );
			std::cout << " Ires 1.0 " << reservoir_model_.static_porperties[j].name << " : " << reservoir_model_.static_porperties[j].values_[cont] << std::endl;
			std::cout << " Ires 2.0 " << new_names[j] <<  " : " << new_values[cont] << std::endl;

		}

	}

}

void GLWidget::openIRESCharles( const std::string& filename )
{

	//reservoir_model_.openIRES( filename );

	reservoir_model_.openIRES_Version_2( filename );

	if ( reservoir_model_.blocks.size( ) > 0 )
	{

		cube_interleaved.clear();

		CubeData cube_temp;

		cube_interleaved.resize(reservoir_model_.blocks.size( ));

		int index = 0;
		int I = 0;
		int J = 0;
		int K = 0;

		for ( std::size_t i = 0; i < reservoir_model_.blocks.size( ) ; i++)
		{

			if ( reservoir_model_.blocks[i].valid )
			{

				I = reservoir_model_.header_v2_.numI;
				J = reservoir_model_.header_v2_.numJ;
				K = reservoir_model_.header_v2_.numK;


				cube_temp.vertices[4] = reservoir_model_.blocks[i].vertices[0];
				cube_temp.vertices[5] = reservoir_model_.blocks[i].vertices[1];
				cube_temp.vertices[7] = reservoir_model_.blocks[i].vertices[2];
				cube_temp.vertices[6] = reservoir_model_.blocks[i].vertices[3];

				cube_temp.vertices[0] = reservoir_model_.blocks[i].vertices[4];
				cube_temp.vertices[3] = reservoir_model_.blocks[i].vertices[5];
				cube_temp.vertices[1] = reservoir_model_.blocks[i].vertices[6];
				cube_temp.vertices[2] = reservoir_model_.blocks[i].vertices[7];

				cube_temp.color  = Celer::Vector4<float> (1.0,0.0,0.0,1.0);

				if ( ((reservoir_model_.blocks[i].IJK[0].x > 0 ) && (reservoir_model_.blocks[i].IJK[0].x < I )) ||
				     ((reservoir_model_.blocks[i].IJK[0].y > 0 ) && (reservoir_model_.blocks[i].IJK[0].y < J )) ||
				     ((reservoir_model_.blocks[i].IJK[0].z > 0 ) && (reservoir_model_.blocks[i].IJK[0].z < K )) )
				{
					// Internal Cell;
					cube_temp.IJK    = Celer::Vector4<float> ( reservoir_model_.blocks[i].IJK[0].x , reservoir_model_.blocks[i].IJK[0].y , reservoir_model_.blocks[i].IJK[0].z , 0.0f );

				}else
				{
					// Surface  Cell;
					cube_temp.IJK    = Celer::Vector4<float> ( reservoir_model_.blocks[i].IJK[0].x , reservoir_model_.blocks[i].IJK[0].y , reservoir_model_.blocks[i].IJK[0].z , 1.0f );

				}

				cube_temp.focus  = Celer::Vector4<float> ( 1.0f, 1.0f ,0.0f ,1.0f );

				cube_temp.centroid = ( cube_temp.vertices[0] + cube_temp.vertices[1] +
						       cube_temp.vertices[2] + cube_temp.vertices[3] +
						       cube_temp.vertices[4] + cube_temp.vertices[5] +
						       cube_temp.vertices[6] + cube_temp.vertices[7]
						       ) ;
				// 1/8 = 0.125
				cube_temp.centroid *=  0.125 ;

				cube_interleaved[index] = cube_temp;

				index++;

			}
			else
			{

			}
		}

		cube_interleaved.resize(  index  );

		changeProperty(0);

		max_I_ = 0;
		min_I_ = 0;
		max_J_ = 0;
		min_J_ = 0;
		max_K_ = 0;
		min_K_ = 0;


		camera_.setPosition ( reservoir_model_.box_v2.center ( ) );
		camera_.setTarget ( reservoir_model_.box_v2.center ( ) );
		std::cout << reservoir_model_.box_v2.diagonal ( );
		camera_.setOffset ( 3.0 * reservoir_model_.box_v2.diagonal ( ) );

		std::cout << camera_.position ( );

		camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );

		cameraStep_ = 0.001f;

		glBindVertexArray ( vertexArray_cube_interleaved );

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_interleaved);
                        glBufferData ( GL_ARRAY_BUFFER , cube_interleaved.size( ) * sizeof(cube_interleaved[0]) , &cube_interleaved[0] , GL_STATIC_DRAW );

                        int size_of_vertice = sizeof(Celer::Vector4<float>);
                        int size_of_struct  =  sizeof(CubeData);

                        //http://www.opengl.org/wiki/Vertex_Specification
                        for ( int location = 0 ; location < 12 ; location++)
                        {
                                glEnableVertexAttribArray(location);
                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
                        }

                glBindVertexArray(0);




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
	camera_.setWindowSize ( width , height );


	camera_.setAspectRatio ( width  , height  );
	camera_.setPerspectiveProjectionMatrix ( 0 , camera_.aspectRatio ( ) , 0.1 , 500 );
    camera_.setOrthographicProjectionMatrix( -1.0f, (float)1.0 , -1.0f, (float)1.0, 0.0, 1.0 );

	centerX_ = static_cast<float> ( width * 0.5 );
	centerY_ = static_cast<float> ( height * 0.5 );



	if ( fboStep[0] )
		delete fboStep[0];

	if ( fboStep[1] )
		delete fboStep[1];

	QGLFramebufferObjectFormat format;
	format.setAttachment(QGLFramebufferObject::Depth);

	format.setTextureTarget ( GL_TEXTURE_2D );

	format.setInternalTextureFormat ( GL_RGBA32F );

	fboStep[0] = new QGLFramebufferObject ( width , height , format );
	fboStep[1] = new QGLFramebufferObject ( width , height , format );



}
/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.
/// Real Looping
void GLWidget::paintGL ( )
{

	camera_.setViewByMouse ( );

	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}

	camera_.computerViewMatrix( );

	camera_.setPerspectiveProjectionMatrix ( zoom_angle_ , camera_.aspectRatio ( ) , 0.1 , 500 );

	glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBuffer_globalMatrices_);

	transformationMatrices_.ModelMatrix      = ~camera_.viewMatrix ( );
	transformationMatrices_.ViewMatrix       = ~camera_.viewMatrix ( );
	transformationMatrices_.ProjectionMatrix = ~camera_.perspectiveProjectionMatrix( );

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(this->transformationMatrices_), &this->transformationMatrices_ );

	if(!freezeView_)
	{
		if (cutVolumes.size() > 0)
		{
			new_z = camera_.position() - cutVolumes[cluster].center();

			new_z.normalize();

			new_x = new_z ^ camera_.UpVector();

			new_x.normalize();

			new_y = new_z ^ new_x;

			new_y.normalize();

			lookatCamera = Celer::Matrix4x4<float>(new_x, new_y, new_z);
		}
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

        if ( draw_secondary && (cube_interleaved.size() != 0) )
        {
                secondary.active ( );

                glUniform3fv ( secondary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
                glUniform3f  ( secondary.uniforms_["min_IJK"].location , (float)min_I_,(float)min_J_,(float)min_K_ );
                glUniform3f  ( secondary.uniforms_["max_IJK"].location , (float)max_I_, (float)max_J_, (float)max_K_ );
                glUniform2f  ( secondary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

                glBindVertexArray ( vertexArray_cube_interleaved );
                glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
                glBindVertexArray ( 0 );

                secondary.deactive ( );
        }

        if ( draw_primary && (cube_interleaved.size() != 0)  )
        {
                primary.active ( );

                glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

                glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

                glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
                glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

                glBindVertexArray ( vertexArray_cube_interleaved );
                glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
                glBindVertexArray ( 0 );

                primary.deactive ( );
        }
}


void GLWidget::RawCutaway ( int cluster )
{

//	/// FIXME Conditions  - Primary and Secondary well defined.

 	if ( isIRESOpen_ )
	{

 		if ( cutVolumes.size( ) > 0)
 		{

            glDepthFunc ( GL_GREATER);
            glClearDepth ( 0.0 );

			fboStep[0]->bind ( );

			glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			BoundingBoxInitialization.active ( );

            glUniform1f ( BoundingBoxInitialization.uniforms_["x"].location , volume_width );
            glUniform1f ( BoundingBoxInitialization.uniforms_["y"].location , volume_height );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_x"].location , 1 , new_x );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_y"].location , 1 , new_y );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_z"].location , 1 , new_z );
			glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , modelMatrix_ );
			glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//            glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
            glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix() );

			//VAO
			glBindVertexArray ( vertexArray_box );
			glDrawArrays ( GL_POINTS , 0 , cutVolume_.size.x);
			glBindVertexArray ( 0 );

			BoundingBoxInitialization.deactive ( );

			fboStep[0]->release ( );

 		}

		glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
		glDepthFunc(GL_LESS);
		glClearDepth(1.0f);
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if ( draw_secondary )
		{

			BoundingBoxCutaway.active ( );
 			glActiveTexture ( GL_TEXTURE0 );

 			glBindTexture ( GL_TEXTURE_2D , fboStep[0]->texture ( ) );

 			glUniform1i ( BoundingBoxCutaway.uniforms_["normals"].location , 0 );

 			glUniform3fv ( BoundingBoxCutaway.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

 			glUniform3i ( BoundingBoxCutaway.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
 			glUniform3i ( BoundingBoxCutaway.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );

			glUniform2f ( BoundingBoxCutaway.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , modelMatrix_ );
			glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
            //glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
            glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix() );

			glBindVertexArray ( vertexArray_cube_interleaved );
			glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
			glBindVertexArray ( 0 );

			BoundingBoxCutaway.deactive ( );


		}
		if ( draw_primary )
		{

//            if ( cutVolumes.size ( ) > 0 )
//            {

//                BoundingBoxDebug.active ( );
//                glActiveTexture ( GL_TEXTURE0 );
//                glBindTexture ( GL_TEXTURE_2D , fboStep[1]->texture ( ) );
//                // FIXME Throw an Exception when std::map doesnt find A VARIABLE !!!
//                glUniform1i ( BoundingBoxDebug.uniforms_["normals"].location , 0 );

//                                glUniform1f ( BoundingBoxDebug.uniforms_["x"].location , volume_width );
//                                glUniform1f ( BoundingBoxDebug.uniforms_["y"].location , volume_height );

//                glUniform3fv ( BoundingBoxDebug.uniforms_["new_x"].location , 1 , new_x );
//                glUniform3fv ( BoundingBoxDebug.uniforms_["new_y"].location , 1 , new_y );
//                glUniform3fv ( BoundingBoxDebug.uniforms_["new_z"].location , 1 , new_z );
//                glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
//                glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//                glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//                //VAO
//                glBindVertexArray ( vertexArray_box );
//                glDrawArrays ( GL_POINTS , 0 , cutVolume_.size.x );
//                glBindVertexArray ( 0 );
//                BoundingBoxDebug.deactive ( );
//            }


 			primary.active ( );

 			glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( primary.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , modelMatrix_ );
			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
            //glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
            glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix ( ) );

			glBindVertexArray ( vertexArray_cube_interleaved );
			glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
			glBindVertexArray ( 0 );

			primary.deactive ( );

		}
	}

}

void GLWidget::IRESCutaway ( )
{
//        /// FIXME Conditions  - Just the model opened.
//
        glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        if ( isIRESOpen_ )
        {
                if ( draw_secondary )
                {
                        if ( cutVolumes.size ( ) > 0 )
                        {
                                glDepthFunc ( GL_GREATER);
                                glClearDepth ( 0.0 );

                                fboStep[0]->bind ( );

                                glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
                                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                                BoundingBoxInitialization.active ( );

                                glUniform1f ( BoundingBoxInitialization.uniforms_["x"].location , volume_width );
                                glUniform1f ( BoundingBoxInitialization.uniforms_["y"].location , volume_height );
                                glUniform3fv ( BoundingBoxInitialization.uniforms_["new_x"].location , 1 , new_x );
                                glUniform3fv ( BoundingBoxInitialization.uniforms_["new_y"].location , 1 , new_y );
                                glUniform3fv ( BoundingBoxInitialization.uniforms_["new_z"].location , 1 , new_z );
                                glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
                                glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
                                glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
                                //VAO
                                glBindVertexArray ( vertexArray_box );
                                glDrawArrays ( GL_POINTS , 0 , cutVolume_.size.x);
                                glBindVertexArray ( 0 );

                                BoundingBoxInitialization.deactive ( );

                                fboStep[0]->release ( );


                                glDepthFunc(GL_LESS);
                                glClearDepth( 1.0f );

                                glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
                                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
                                cutawayVolumes.active();

                                glActiveTexture ( GL_TEXTURE0 );
                                glBindTexture ( GL_TEXTURE_2D , fboStep[0]->texture ( ) );

                                glUniform1i ( cutawayVolumes.uniforms_["depthBuffer"].location , 0 );
                                glUniform1i ( cutawayVolumes.uniforms_["verticeBuffer"].location , 1 );

                                glUniform1f ( cutawayVolumes.uniforms_["x"].location , volume_width );
                                glUniform1f ( cutawayVolumes.uniforms_["y"].location , volume_height );

                                glUniform3fv ( cutawayVolumes.uniforms_["new_x"].location , 1 ,  new_x );
                                glUniform3fv ( cutawayVolumes.uniforms_["new_y"].location , 1 ,  new_y );
                                glUniform3fv ( cutawayVolumes.uniforms_["new_z"].location , 1 ,  new_z );

                                glUniform2f ( cutawayVolumes.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

                                glBindVertexArray ( vertexArray_cube_interleaved );
                                glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
                                glBindVertexArray ( 0 );

                                cutawayVolumes.deactive();

                        }
                        else
                        {

                                glDepthFunc(GL_LESS);
                                glClearDepth( 1.0f );

                                glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
                                glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                                cutawayVolumes.active ( );

                                glUniform3fv ( cutawayVolumes.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
                                glUniform3i  ( cutawayVolumes.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
                                glUniform3i  ( cutawayVolumes.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );
                                glUniform2f  ( cutawayVolumes.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

                                glBindVertexArray ( vertexArray_cube_interleaved );
                                glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
                                glBindVertexArray ( 0 );

                                cutawayVolumes.deactive ( );


                        }


                }
                if ( draw_primary )
                {

                        primary.active ( );

                        glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

                        glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

                        glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
                        glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

                        glBindVertexArray ( vertexArray_cube_interleaved );
                        glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
                        glBindVertexArray ( 0 );

                        primary.deactive ( );

//                      glEnable ( GL_BLEND );
//                      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//
//                      cutVolume.active ( );
//
//                      glUniform3fv ( cutVolume.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//                      glUniform2f ( cutVolume.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//
//                      glUniformMatrix4fv ( cutVolume.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//                      glUniformMatrix4fv ( cutVolume.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//                      glBindVertexArray ( vertexArray_box );
//                      glDrawArrays ( GL_LINES_ADJACENCY , 0 , box_vertices.size() );
//                      glBindVertexArray ( 0 );
//
//                      cutVolume.deactive ( );
//                      glDisable ( GL_BLEND );


                }

        }

}

void GLWidget::gameLooping ( )
{

	//std::cout << "fps :" << v  << std::endl;

	updateGL();

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
	qDebug ( ) << "ATIM !!";
	QDir shadersDir = QDir ( qApp->applicationDirPath ( ) );

	#if defined(_WIN32) || defined(_WIN64) // Windows Directory Style
	/* Do windows stuff */
	QString shaderDirectory ("D:\\Workspace\\IRESCutaway\\src\\IRESCutaway\\GUI\\Qt\\RCC\\Shaders\\");
	#elif defined(__linux__)               // Linux Directory Style
	/* Do linux stuff */
    QString shaderDirectory ("/home/ricardomarroquim/devel/irescutaway/src/IRESCutaway/GUI/Qt/RCC/Shaders/");
    //QString shaderDirectory ("/media/d/Workspace/IRESCutaway/src/IRESCutaway/GUI/Qt/RCC/Shaders/");
	#else
	/* Error, both can't be defined or undefined same time */
	#endif

	qDebug () << "Directory " << shadersDir.path ();
	shadersDir.cdUp ();
	qDebug () << "Directory " << shadersDir.path ();



	primary.create("primary",(shaderDirectory + "Primary.vert").toStdString(),
				 (shaderDirectory + "Primary.geom").toStdString(),
				 (shaderDirectory + "Primary.frag").toStdString());

	secondary.create("secondary",(shaderDirectory + "Secondary.vert").toStdString(),
			             (shaderDirectory + "Secondary.geom").toStdString(),
				     (shaderDirectory + "Secondary.frag").toStdString());

	// BoudingBox Approach
	BoundingBoxInitialization.create ("BoundingBoxApproach",(shaderDirectory + "BoundingBoxApproach.vert").toStdString(),
								(shaderDirectory + "BoundingBoxApproach.geom").toStdString(),
								(shaderDirectory + "BoundingBoxApproach.frag").toStdString());

	BoundingBoxDebug.create ("BoundingBoxApproach Debug",(shaderDirectory + "BoundingBoxApproach.vert").toStdString(),
							     (shaderDirectory + "BoundingBoxApproach.geom").toStdString(),
							     (shaderDirectory + "BoundingBoxApproachDebug.frag").toStdString());

	BoundingBoxCutaway.create ("BoundingBoxCutaway",(shaderDirectory + "BoundingBoxCutaway.vert").toStdString(),
							(shaderDirectory + "BoundingBoxCutaway.geom").toStdString(),
							(shaderDirectory + "BoundingBoxCutaway.frag").toStdString());


	debugNormal.create ("DebugNormal",  (shaderDirectory + "DebugNormal.vert").toStdString(),
			   (shaderDirectory + "DebugNormal.geom").toStdString(),
			   (shaderDirectory + "DebugNormal.frag").toStdString());


	cutawayVolumes.create("cutawayVolumes", (shaderDirectory + "cutawayVolumes.vert").toStdString(),
			     (shaderDirectory   + "cutawayVolumes.geom").toStdString(),
			     (shaderDirectory   + "cutawayVolumes.frag").toStdString());


	// Uniform Buffer Usage @link http://www.arcsynthesis.org/gltut/Positioning/Tut07%20Shared%20Uniforms.html
	// For earch program where the uniform is defined, build a link to the Uniform Block bind.

	glUniformBlockBinding(secondary.id( ), secondary.uniform_blocks_["GlobalMatrices"].index, 0);
	glUniformBlockBinding(cutawayVolumes.id( ), cutawayVolumes.uniform_blocks_["GlobalMatrices"].index, 0);
	// CutVolume Uniform Binds

	glUniformBlockBinding(cutawayVolumes.id( ), cutawayVolumes.uniform_blocks_["CutVolumes"].index, 1);
	glUniformBlockBinding(BoundingBoxInitialization.id( ), BoundingBoxInitialization.uniform_blocks_["CutVolumes"].index, 1);
	glUniformBlockBinding(BoundingBoxDebug.id( ), BoundingBoxDebug.uniform_blocks_["CutVolumes"].index, 1);

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
			cameraStep_ += 0.0001;

		}
		if ( key == Qt::Key_Minus )
		{

			if ( cameraStep_ > 0.0 )
			cameraStep_ -= 0.0001;
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
		qDebug() << " Hello ";
	}
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

		/*! Tr�s coisas :
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

	if ( QApplication::keyboardModifiers ( ) == Qt::ShiftModifier )
	{
		zoom_angle_ += event->delta ( ) / 120.0;

		orthoZoom += event->delta ( ) / 1200.0;

		modelMatrix_[0][0] = orthoZoom;
		modelMatrix_[1][1] = orthoZoom;
		modelMatrix_[2][2] = orthoZoom;

		qDebug ( ) << orthoZoom;
	}
	else
	{
		scrollStep_ += event->delta ( ) / 120.0;
		angle = static_cast<float> ( 1.0 / std::tan ( scrollStep_ * Celer::Math::kDeg2Rad ) );
		qDebug ( ) << scrollStep_;
	}


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


		qDebug() << "Soltou";

	}


}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
		event->accept ( );
}

void GLWidget::setPrimaryVisibility( bool visibility )
{
	if ( cube_interleaved.size() > 0 )
		draw_primary = visibility;
	updateGL();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( cube_interleaved.size() > 0 )
		draw_secondary = visibility;
	updateGL();
}


void GLWidget::CameraTrackball()
{
	camera_.setBehavior( Celer::Camera<float>::REVOLVE_AROUND_MODE);
}

void GLWidget::CameraFly()
{
	camera_.setBehavior( Celer::Camera<float>::FIRST_PERSON );
}

void GLWidget::freezeView( )
{
	freezeView_ = !freezeView_;

}
