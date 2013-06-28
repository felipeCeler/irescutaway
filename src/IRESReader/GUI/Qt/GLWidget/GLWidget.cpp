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
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File GLWidget.cpp line 31" );
	/// Celer OpenGL

	buttonRelease_ = false;

	/// OpenGL Stuffs
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_TEXTURE_RECTANGLE );
	glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
	glDisable(GL_BLEND);


//	timer_.setSingleShot ( false );
//	connect ( &timer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( gameLooping ( ) ) );
//	connect ( &timer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( animate ( ) ) );
//	timer_.start ( 60 );

	//Timer Animation
	timerId = 0;
	t = 0.0;

	startTimer(30);


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

	ires_has_been_open_sucessefully = 0;

	scrollStep_ = 45.0f;
	zoom_angle_ = 45.0f;
	angle = static_cast<float>(1.0/std::tan(scrollStep_ * Celer::Math::kDeg2Rad));
	theta = angle;
	pm_sz = 0.0f;


	QGLFramebufferObjectFormat format;
	format.setSamples(4);

	//format.setAttachment(QGLFramebufferObject::Depth);
	format.setInternalTextureFormat ( GL_RGBA32F );
	format.setTextureTarget ( GL_TEXTURE_RECTANGLE );

	fboStep[0] 	  = new  QGLFramebufferObject ( width() , height() , format );
	fboStep[1] 	  = new  QGLFramebufferObject ( width() , height() , format );

	glGenVertexArrays ( 1 , &vertexArray);
		glGenBuffers ( 1, &reservoir_vertices_buffer );
		glGenBuffers ( 1, &reservoir_normal_buffer );
		glGenBuffers ( 1, &reservoir_color_buffer );
		glGenBuffers ( 1, &reservoir_renderFlag_buffer );
		glGenBuffers ( 1, &reservoir_IJK_buffer );
		glGenBuffers ( 1, &reservoir_indices_buffer );
		// Viewport
		glGenBuffers ( 1, &screen_buffer);
		glGenBuffers ( 1, &texture_buffer);

	/// Cube Triangle Adjacency
	glGenVertexArrays ( 1 , &vertexArray_Cube);
		glGenBuffers ( 1, &reservoir_vertices_triangles_adjacency_buffer );
		glGenBuffers ( 1, &reservoir_normal_triangles_adjacency_buffer );
		glGenBuffers ( 1, &reservoir_color_triangles_adjacency_buffer );
		glGenBuffers ( 1, &reservoir_focus_triangles_adjacency_buffer );
		glGenBuffers ( 1, &reservoir_IJK_triangles_adjacency_buffer );

	glGenVertexArrays ( 1 , &vertexArrayScreen );

	// Charles Ires v 2
	glGenVertexArrays ( 1, &vertexArray_Charles);
		glGenBuffers ( 1, &reservoir_vertices_charles_buffer );


	reservoir_vertices_location 	= 1;
	reservoir_normal_location 	= 2;
	reservoir_color_location 	= 3;
	reservoir_renderFlag_location 	= 4;
	reservoir_IJK_location 		= 5;

	// Cube in Geomtry Shader

	reservoir_vertices_triangles_adjacency_location = 1;
	reservoir_normal_triangles_adjacency_location 	= 2;
	reservoir_color_triangles_adjacency_location 	= 3;
	reservoir_focus_triangles_adjacency_location 	= 4;
	reservoir_IJK_triangles_adjacency_location 	= 5;

	/// ---

	draw_secondary = 1;
	draw_primary = 0;

	isBurnsApproach = 0;
	isBoudingBoxApproach = 0;
	cluster = 0;

	glGenVertexArrays (1, &vertexArray_box);
		glGenBuffers( 1, &vertexBuffer_box);


	cube_.createBuffers( Celer::Vector3<float>(0,0,0) );


	// Camera Physics
	max_velocity_ = 1.0f;
	velocity_ = 0.1f;
	acceleration_ = 0.001f;

	delta_time_ = 0.0f;

	last_time_ = 0.0f;
	current_time_ = 0.0f;

	clock_.start();
}

bool GLWidget::isIresWasOpenedSucessufully ( ) const
{
	return ires_has_been_open_sucessefully;
}

void GLWidget::CutVolumeGenerator( )
{

	Celer::BoundingBox3<GLfloat> box;

	cutVolumes.clear();

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


	for ( std::vector<Celer::BoundingBox3<GLfloat> >::iterator it = cutVolumes.begin(); it != cutVolumes.end();++it)
	{

		Celer::Vector3<GLfloat> v0 ( it->max ( ).x , it->max ( ).y , it->max ( ).z );
		Celer::Vector3<GLfloat> v1 ( it->max ( ).x , it->max ( ).y , it->min ( ).z );
		Celer::Vector3<GLfloat> v2 ( it->min ( ).x , it->max ( ).y , it->min ( ).z );
		Celer::Vector3<GLfloat> v3 ( it->min ( ).x , it->max ( ).y , it->max ( ).z );

		Celer::Vector3<GLfloat> v4 ( it->max ( ).x , it->min ( ).y , it->max ( ).z );
		Celer::Vector3<GLfloat> v5 ( it->min ( ).x , it->min ( ).y , it->max ( ).z );
		Celer::Vector3<GLfloat> v6 ( it->min ( ).x , it->min ( ).y , it->min ( ).z );
		Celer::Vector3<GLfloat> v7 ( it->max ( ).x , it->min ( ).y , it->min ( ).z );


		std::cout << "min -- " << it->min ( ) <<  std::endl;
		std::cout << "max == " << it->max ( ) << std::endl;



		Celer::Vector3<GLfloat> topNormal 	= ((v0 - v1) ^ (v0 - v3)).norm();
		//std::cout << topNormal << std::endl;
		Celer::Vector3<GLfloat> bottomNormal 	= ((v4 - v5) ^ (v4 - v7)).norm();
		//std::cout << bottomNormal << std::endl;
		Celer::Vector3<GLfloat> frontNormal 	= ((v0 - v3) ^ (v0 - v4)).norm();
		//std::cout << frontNormal << std::endl;
		Celer::Vector3<GLfloat> backmNormal 	= ((v1 - v7) ^ (v1 - v2)).norm();
		//std::cout << backmNormal << std::endl;
		Celer::Vector3<GLfloat> rightNormal 	= ((v0 - v4) ^ (v0 - v1)).norm();
		//std::cout << rightNormal << std::endl;
		Celer::Vector3<GLfloat> leftNormal 	= ((v2 - v6) ^ (v2 - v3)).norm();
		//std::cout << leftNormal << std::endl;


		// Care about the type: GL_DOUBLE or GL_FLOAT
		Celer::Vector3<GLfloat> vertices[] =
		{
		  // X Y Z
			//  Top Face
			v0,v1,v3,v2,
			// Bottom Face
			v4,v5,v7,v6,
			// Front Face
		  //v0,v3,v4,v5,
			// Back Face
			v2,v1,v6,v7,
			// Right Face
			v0,v4,v1,v7,
			// Left Face
			v2,v6,v3,v5
		};


		// Care about the type: GL_DOUBLE or GL_FLOAT
		Celer::Vector3<GLfloat> normals[] =
		{
		  // X Y Z
			//  Top Face
			topNormal,topNormal,topNormal,topNormal,
			// Bottom Face
			bottomNormal,bottomNormal,bottomNormal,bottomNormal,
			// Front Face
		      //frontNormal,frontNormal,frontNormal,frontNormal,
			// Back Face
			backmNormal,backmNormal,backmNormal,backmNormal,
			// Right Face
			rightNormal,rightNormal,rightNormal,rightNormal,
			// Left Face
			leftNormal,leftNormal,leftNormal,leftNormal
		};

		std::copy ( vertices, vertices + 24 , std::back_inserter ( box_vertices  ) );

	}

	glBindVertexArray ( vertexArray_box );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_box );
		glBufferData ( GL_ARRAY_BUFFER , box_vertices.size( ) * sizeof ( box_vertices[0] ) , &box_vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray ( 1 );
		glVertexAttribPointer ( 1 , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );
	glBindVertexArray ( 0 );

}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{


	reservoir_list_of_colors.clear ( );
	reservoir_list_of_renderFlag.clear ( );

	reservoir_list_of_triangles_adjacency_colors.clear();
	reservoir_list_of_triangles_adjacency_focus.clear();

	boxes.clear ( );
	cutVolumes.clear();

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::vector<Celer::Vector4<GLfloat> > colors ( 24 );
	std::vector<Celer::Vector4<GLfloat> > renderFlags ( 24 );

	Celer::Vector4<GLfloat> color;

	for ( int i = 0; i < reservoir_model_.header_.number_of_Blocks; i++)
	{
		if ( reservoir_model_.blocks[i].valid )
		{

			float normalized_color = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ - min ) / ( max - min );

			float regularValue = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ );

			Celer::Vector4<GLfloat> color(1.0,1.0,1.0,1.0);

			float fourValue = 4 * normalized_color;
			float red   = std::min(fourValue - 1.5, -fourValue + 4.5);
			float green = std::min(fourValue - 0.5, -fourValue + 3.5);
			float blue  = std::min(fourValue + 0.5, -fourValue + 2.5);

			red 	= std::max(0.0f, std::min(red, 1.0f));
			green 	= std::max(0.0f, std::min(green, 1.0f));
			blue 	= std::max(0.0f, std::min(blue, 1.0f));

			color = Celer::Vector4<GLfloat> ( red , green , blue , 1.0f );
//
			colors = std::vector<Celer::Vector4<GLfloat> > ( 24 , color );

			if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
			{
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f ) );

				box.fromPointCloud ( reservoir_model_.blocks[i].vertices.begin ( ) , reservoir_model_.blocks[i].vertices.end ( ) );

				boxes.push_back( box );


			}
			else
			{
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f ) );
			}

			std::copy ( colors.begin( ) 	, colors.end( )      , std::back_inserter ( reservoir_list_of_colors     ) );
			std::copy ( renderFlags.begin( ), renderFlags.end( ) , std::back_inserter ( reservoir_list_of_renderFlag ) );

			std::copy ( renderFlags.begin( ), renderFlags.begin( ) + 6, std::back_inserter ( reservoir_list_of_triangles_adjacency_focus ) );
			std::copy ( colors.begin( ) , colors.begin( ) + 6 , std::back_inserter ( reservoir_list_of_triangles_adjacency_colors ) );
		}
		else
		{
			continue;
		}
	}

	// Loop over the boxes
	CutVolumeGenerator();

	std::cout << " number of boxes " << cutVolumes.size( ) << std::endl;

	glBindVertexArray(vertexArray);

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_colors.size ( ) * sizeof ( reservoir_list_of_colors[0] ) , &reservoir_list_of_colors[0] , GL_STREAM_DRAW );

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_renderFlag_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_renderFlag.size ( ) * sizeof ( reservoir_list_of_renderFlag[0] ) , &reservoir_list_of_renderFlag[0] , GL_STREAM_DRAW );

	glBindVertexArray(0);

	glBindVertexArray(vertexArray_Cube);

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_triangles_adjacency_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_colors.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_colors[0] ) , &reservoir_list_of_triangles_adjacency_colors[0] , GL_STREAM_DRAW );

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_focus_triangles_adjacency_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_focus.size( ) * sizeof ( reservoir_list_of_triangles_adjacency_focus[0] ) , &reservoir_list_of_triangles_adjacency_focus[0] , GL_STREAM_DRAW );

	glBindVertexArray(0);

}

void GLWidget::changeProperty ( int property_index )
{

	reservoir_list_of_colors.clear ( );

	reservoir_list_of_triangles_adjacency_colors.clear();

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	std::size_t i = 0;

	std::vector<Celer::Vector4<GLfloat> > colors;

	for ( int i = 0; i < reservoir_model_.header_.number_of_Blocks; i++)
	{
		if ( reservoir_model_.blocks[i].valid )
		{

			float normalized_color = ( reservoir_model_.blocks[i].static_porperties[property_index].value_ - min ) / ( max - min );

			Celer::Vector4<GLfloat> color(1.0,1.0,1.0,1.0);

			// @see Implementing a Continuous "Jet" Colormap Function in GLSL - http://www.metastine.com/?p=7

			float fourValue = 4 * normalized_color;
			float red   = std::min(fourValue - 1.5, -fourValue + 4.5);
			float green = std::min(fourValue - 0.5, -fourValue + 3.5);
			float blue  = std::min(fourValue + 0.5, -fourValue + 2.5);

			red 	= std::max(0.0f, std::min(red, 1.0f));
			green 	= std::max(0.0f, std::min(green, 1.0f));
			blue 	= std::max(0.0f, std::min(blue, 1.0f));

			color = Celer::Vector4<GLfloat> ( red , green , blue , 1.0f );


			colors = std::vector<Celer::Vector4<GLfloat> > ( 24 , color );

			std::copy (colors.begin( ) 	, colors.end( ) , 	std::back_inserter ( reservoir_list_of_colors ) );
			std::copy (colors.begin( ) 	, colors.begin( ) + 6 , std::back_inserter ( reservoir_list_of_triangles_adjacency_colors ) );

		}
		else
		{
			continue;
		}
	}

	glBindVertexArray(vertexArray);

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_colors.size ( ) * sizeof ( reservoir_list_of_colors[0] ) , &reservoir_list_of_colors[0] , GL_STREAM_DRAW );

	glBindVertexArray(0);


	glBindVertexArray(vertexArray_Cube);

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_triangles_adjacency_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_colors.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_colors[0] ) , &reservoir_list_of_triangles_adjacency_colors[0] , GL_STREAM_DRAW );

	glBindVertexArray(0);



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

void GLWidget::IRES_v1_to_IRESv2( const std::string& filename )
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


		for ( int i = 0 ; i < new_names.size( ); i++)
		{
			std::cout << " names " << new_names[i] << std::endl;
		}


		for  ( int j = 0 ; j <  reservoir_model_.static_porperties.size() ; j++)
		{
			new_reservoir_file_.getStaticPropertyValues(j , new_values );
			std::cout << " Ires 1.0 " << reservoir_model_.static_porperties[j].name << " : " << reservoir_model_.static_porperties[j].values_[cont] << std::endl;
			std::cout << " Ires 2.0 " << new_names[j] <<  " : " << new_values[cont] << std::endl;

		}




	}

}

void GLWidget::openIRESCharles( const std::string& filename )
{

	reservoir_model_.openIRES_Version_2( filename );

	if ( reservoir_model_.blocks.size( ) > 0 )
	{
		reservoir_list_of_vertices.clear ( );
		reservoir_list_of_normals.clear ( );
		reservoir_list_of_colors.clear ( );
		reservoir_list_of_IJKs.clear ( );
		reservoir_list_of_renderFlag.clear( );
		reservoir_list_of_indices.clear ( );

		/// Triangle Adjacency
		reservoir_list_of_triangles_adjacency_vertices.clear();
		reservoir_list_of_triangles_adjacency_normals.clear();
		reservoir_list_of_triangles_adjacency_colors.clear();
		reservoir_list_of_triangles_adjacency_IJKs.clear();
		reservoir_list_of_triangles_adjacency_focus.clear();

		for ( std::size_t i = 0; i < reservoir_model_.blocks.size( ) ; i++)
		{

			if ( reservoir_model_.blocks[i].valid )
			{

				std::copy(reservoir_model_.blocks[i].vertices.begin(), reservoir_model_.blocks[i].vertices.end(),
						std::back_inserter(reservoir_list_of_vertices));
				std::copy(reservoir_model_.blocks[i].normals.begin(), reservoir_model_.blocks[i].normals.end(),
						std::back_inserter(reservoir_list_of_normals));
				std::copy(reservoir_model_.blocks[i].focus.begin(), reservoir_model_.blocks[i].focus.end(),
						 std::back_inserter(reservoir_list_of_renderFlag));
				std::copy(reservoir_model_.blocks[i].IJK.begin(), reservoir_model_.blocks[i].IJK.end(),
						 std::back_inserter(reservoir_list_of_IJKs));


				Celer::Vector4<float> vertices [] =
				{
				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[0].x,reservoir_model_.blocks[i].vertices[0].y,reservoir_model_.blocks[i].vertices[0].z,reservoir_model_.blocks[i].vertices[6].x),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[1].x,reservoir_model_.blocks[i].vertices[1].y,reservoir_model_.blocks[i].vertices[1].z,reservoir_model_.blocks[i].vertices[6].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[2].x,reservoir_model_.blocks[i].vertices[2].y,reservoir_model_.blocks[i].vertices[2].z,reservoir_model_.blocks[i].vertices[6].z),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[3].x,reservoir_model_.blocks[i].vertices[3].y,reservoir_model_.blocks[i].vertices[3].z,reservoir_model_.blocks[i].vertices[7].x),

				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[4].x,reservoir_model_.blocks[i].vertices[4].y,reservoir_model_.blocks[i].vertices[4].z,reservoir_model_.blocks[i].vertices[7].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].vertices[5].x,reservoir_model_.blocks[i].vertices[5].y,reservoir_model_.blocks[i].vertices[5].z,reservoir_model_.blocks[i].vertices[7].z)
				};

				Celer::Vector4<float> normals [] =
				{
				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[0].x,reservoir_model_.blocks[i].normals[0].y,reservoir_model_.blocks[i].normals[0].z,reservoir_model_.blocks[i].normals[6].x),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[1].x,reservoir_model_.blocks[i].normals[1].y,reservoir_model_.blocks[i].normals[1].z,reservoir_model_.blocks[i].normals[6].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[2].x,reservoir_model_.blocks[i].normals[2].y,reservoir_model_.blocks[i].normals[2].z,reservoir_model_.blocks[i].normals[6].z),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[3].x,reservoir_model_.blocks[i].normals[3].y,reservoir_model_.blocks[i].normals[3].z,reservoir_model_.blocks[i].normals[7].x),

				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[4].x,reservoir_model_.blocks[i].normals[4].y,reservoir_model_.blocks[i].normals[4].z,reservoir_model_.blocks[i].normals[7].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].normals[5].x,reservoir_model_.blocks[i].normals[5].y,reservoir_model_.blocks[i].normals[5].z,reservoir_model_.blocks[i].normals[7].z)
				};

				Celer::Vector4<float> focus [] =
				{
				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[0].x,reservoir_model_.blocks[i].focus[0].y,reservoir_model_.blocks[i].focus[0].z,reservoir_model_.blocks[i].focus[6].x),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[1].x,reservoir_model_.blocks[i].focus[1].y,reservoir_model_.blocks[i].focus[1].z,reservoir_model_.blocks[i].focus[6].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[2].x,reservoir_model_.blocks[i].focus[2].y,reservoir_model_.blocks[i].focus[2].z,reservoir_model_.blocks[i].focus[6].z),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[3].x,reservoir_model_.blocks[i].focus[3].y,reservoir_model_.blocks[i].focus[3].z,reservoir_model_.blocks[i].focus[7].x),

				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[4].x,reservoir_model_.blocks[i].focus[4].y,reservoir_model_.blocks[i].focus[4].z,reservoir_model_.blocks[i].focus[7].y),
				    Celer::Vector4<float>( reservoir_model_.blocks[i].focus[5].x,reservoir_model_.blocks[i].focus[5].y,reservoir_model_.blocks[i].focus[5].z,reservoir_model_.blocks[i].focus[7].z)
				};

				Celer::Vector4<int> IJK [] =
				{
				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[0].x,reservoir_model_.blocks[i].IJK[0].y,reservoir_model_.blocks[i].IJK[0].z,reservoir_model_.blocks[i].IJK[6].x),
				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[1].x,reservoir_model_.blocks[i].IJK[1].y,reservoir_model_.blocks[i].IJK[1].z,reservoir_model_.blocks[i].IJK[6].y),
				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[2].x,reservoir_model_.blocks[i].IJK[2].y,reservoir_model_.blocks[i].IJK[2].z,reservoir_model_.blocks[i].IJK[6].z),
				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[3].x,reservoir_model_.blocks[i].IJK[3].y,reservoir_model_.blocks[i].IJK[3].z,reservoir_model_.blocks[i].IJK[7].x),

				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[4].x,reservoir_model_.blocks[i].IJK[4].y,reservoir_model_.blocks[i].IJK[4].z,reservoir_model_.blocks[i].IJK[7].y),
				    Celer::Vector4<int>( reservoir_model_.blocks[i].IJK[5].x,reservoir_model_.blocks[i].IJK[5].y,reservoir_model_.blocks[i].IJK[5].z,reservoir_model_.blocks[i].IJK[7].z)
				};

				std::copy ( vertices , vertices + 6 , std::back_inserter( reservoir_list_of_triangles_adjacency_vertices ));
				std::copy ( normals  , normals + 6 , std::back_inserter( reservoir_list_of_triangles_adjacency_normals ));
				std::copy ( focus    , focus + 6 , std::back_inserter( reservoir_list_of_triangles_adjacency_focus ));
				std::copy ( IJK      , IJK + 6 , std::back_inserter( reservoir_list_of_triangles_adjacency_IJKs ));


			}
			else
			{

			}
		}

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

		camera_.setBehavior ( Celer::Camera<float>::FIRST_PERSON );

		cameraStep_ = 0.1f;


		glBindVertexArray(vertexArray);


		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_vertices_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_vertices.size ( ) * sizeof ( reservoir_list_of_vertices[0] ) , &reservoir_list_of_vertices[0] , GL_STATIC_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_vertices_location );
		glVertexAttribPointer ( reservoir_vertices_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_normal_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_normals.size ( ) * sizeof ( reservoir_list_of_normals[0] ) , &reservoir_list_of_normals[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_normal_location );
		glVertexAttribPointer ( reservoir_normal_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );


		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_colors.size ( ) * sizeof ( reservoir_list_of_colors[0] ) , &reservoir_list_of_colors[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_color_location );
		glVertexAttribPointer ( reservoir_color_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_renderFlag_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_renderFlag.size ( ) * sizeof ( reservoir_list_of_renderFlag[0] ) , &reservoir_list_of_renderFlag[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_renderFlag_buffer );
		glVertexAttribPointer ( reservoir_renderFlag_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		// FIXME glVertexAttribIPointer FOR INTEGERS!
		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_IJK_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_IJKs.size ( ) * sizeof ( reservoir_list_of_IJKs[0] ) , &reservoir_list_of_IJKs[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_IJK_buffer );
		glVertexAttribIPointer ( reservoir_IJK_location , 4 , GL_INT, 0 , 0 );

		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , reservoir_indices_buffer );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER , reservoir_list_of_indices.size ( ) * sizeof ( reservoir_list_of_indices[0] ) , &reservoir_list_of_indices[0] , GL_STATIC_DRAW );

		glBindVertexArray(0);



		glBindVertexArray(vertexArray_Cube);

		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_vertices_triangles_adjacency_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_vertices.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_vertices[0] ) , &reservoir_list_of_triangles_adjacency_vertices[0] , GL_STATIC_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_vertices_triangles_adjacency_location );
		glVertexAttribPointer ( reservoir_vertices_triangles_adjacency_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_normal_triangles_adjacency_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_normals.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_normals[0] ) , &reservoir_list_of_triangles_adjacency_normals[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_normal_triangles_adjacency_location );
		glVertexAttribPointer ( reservoir_normal_triangles_adjacency_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );


		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_triangles_adjacency_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_colors.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_colors[0] ) , &reservoir_list_of_triangles_adjacency_colors[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_color_triangles_adjacency_location );
		glVertexAttribPointer ( reservoir_color_triangles_adjacency_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_focus_triangles_adjacency_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangles_adjacency_focus.size ( ) * sizeof ( reservoir_list_of_triangles_adjacency_focus[0] ) , &reservoir_list_of_triangles_adjacency_focus[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_focus_triangles_adjacency_location );
		glVertexAttribPointer ( reservoir_focus_triangles_adjacency_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		// FIXME glVertexAttribIPointer FOR INTEGERS!
		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_IJK_triangles_adjacency_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_IJKs.size ( ) * sizeof ( reservoir_list_of_IJKs[0] ) , &reservoir_list_of_IJKs[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_IJK_triangles_adjacency_location );
		glVertexAttribIPointer ( reservoir_IJK_triangles_adjacency_location , 4 , GL_INT, 0 , 0 );

		glBindVertexArray(0);

		glBindVertexArray(vertexArrayScreen);


		GLfloat openGLScreenCoordinates[] =
		{
		       -1.0f,  1.0f,
			1.0f,  1.0f,
			1.0f, -1.0f,
		        1.0f, -1.0f,
		       -1.0f, -1.0f,
		       -1.0f,  1.0f
		 };

		GLfloat textureCoordinates[] =
		{
			0.0f,  0.0f,
			0.0f,  1.0f,
			1.0f,  0.0f,

			0.0f,  1.0f,
			1.0f,  1.0f,
			1.0f,  0.0f,
		};

		glBindBuffer ( GL_ARRAY_BUFFER , screen_buffer );
		glBufferData ( GL_ARRAY_BUFFER, 12 *  sizeof( openGLScreenCoordinates[0] ), openGLScreenCoordinates, GL_STATIC_DRAW);
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer ( GL_ARRAY_BUFFER , texture_buffer );
		glBufferData ( GL_ARRAY_BUFFER, 12 * sizeof( textureCoordinates[0] ), textureCoordinates , GL_STATIC_DRAW);
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);


		ires_has_been_open_sucessefully = 1;
	}
	else
	{
		ires_has_been_open_sucessefully = 0;
	}

}

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );
	camera_.setWindowSize ( width , height );


	camera_.setAspectRatio ( width  , height  );
	camera_.setPerspectiveProjectionMatrix ( 0 , camera_.aspectRatio ( ) , 0.1 , 500 );
	camera_.setOrthographicProjectionMatrix( 0.0f, (float)width , 0.0f, (float)height, -100.0, 100.0 );


	centerX_ = static_cast<float> ( width * 0.5 );
	centerY_ = static_cast<float> ( height * 0.5 );



	if ( fboStep[0] )
		delete fboStep[0];

	if ( fboStep[1] )
		delete fboStep[1];


	QGLFramebufferObjectFormat format;
	//format.setAttachment(QGLFramebufferObject::Depth);
	format.setTextureTarget ( GL_TEXTURE_RECTANGLE );
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


	current_time_ = clock_.elapsed();

	delta_time_ = current_time_ - last_time_;
	delta_time_ /= 1000.0f;

	if ( velocity_ < max_velocity_ )
		velocity_ = velocity_ + acceleration_ * (delta_time_);

	last_time_ = current_time_;


	float z = camera_.position().z + velocity_ ;

	std::cout << camera_.position() << delta_time_ << std::endl;

	camera_.setPosition( Celer::Vector3<float>(camera_.position().x,camera_.position().y, z ) );

	camera_.setPerspectiveProjectionMatrix ( zoom_angle_ , camera_.aspectRatio ( ) , 0.1 , 500 );

	if 	( isBurnsApproach )
	{
		camera_.setTarget( reservoir_model_.box_v2.center( ) );
		BurnsCutawaySetup ( );
	}
	else if ( isBoudingBoxApproach )
	{
		if ( cutVolumes.size() > 0)
			camera_.setTarget( cutVolumes[cluster].center() );
		camera_.setTarget( reservoir_model_.box_v2.center( ) );
		BoundingVolumeCutawaySetup ( cluster );
	}
	else
	{
		if ( cutVolumes.size() > 0)
			camera_.setTarget( cutVolumes[cluster].center() );
		camera_.setTarget( reservoir_model_.box_v2.center( ) );
		NoCutawaySetUp ( );
	}

}

//Timer
void GLWidget::timerEvent( QTimerEvent *event )
{

	clock_.restart();

	updateGL();
}

void GLWidget::BoundingVolumeCutawaySetup( int cluster )
{

	/// FIXME Conditions  - Primary and Secondary well defined.

	Celer::Vector3<float> new_z =  camera_.position() - cutVolumes[cluster].center ( );

	new_z.normalize();

	Celer::Vector3<float> new_x = new_z ^ camera_.UpVector();

	new_x.normalize();

	Celer::Vector3<float> new_y = new_z ^ new_x;

	new_y.normalize();

	Celer::Matrix4x4<float> lookatCamera ( new_x, new_y , new_z  );


 	if ( ires_has_been_open_sucessefully )
	{

 		if ( cutVolumes.size( ) > 0)
 		{

			BoundingBoxInitialization.active ( );

			fboStep[1]->bind ( );

			glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glUniform4fv ( BoundingBoxInitialization.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].min ( ) , 1.0f ) );
			glUniform4fv ( BoundingBoxInitialization.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].max ( ) , 1.0f ) );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["center_point"].location , 1 , cutVolumes[cluster].center ( ) );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_x"].location , 1 ,  new_x );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_y"].location , 1 ,  new_y );
			glUniform3fv ( BoundingBoxInitialization.uniforms_["new_z"].location , 1 ,  new_z );
			glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
			glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( BoundingBoxInitialization.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
			//VAO
			glBindVertexArray ( vertexArray );
			glDrawArrays ( GL_POINTS , 0 , 1 );
			glBindVertexArray ( 0 );

			fboStep[1]->release ( );

			BoundingBoxInitialization.deactive ( );

 		}


		glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
// 		glDisable(GL_BLEND);

		if ( draw_secondary )
		{

			BoundingBoxCutaway.active ( );
 			glActiveTexture ( GL_TEXTURE0 );
 			glBindTexture ( GL_TEXTURE_RECTANGLE , fboStep[1]->texture ( ) );

 			glUniform3fv ( BoundingBoxCutaway.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

 			glUniform3i ( BoundingBoxCutaway.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
 			glUniform3i ( BoundingBoxCutaway.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );

			glUniform2f ( BoundingBoxCutaway.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( BoundingBoxCutaway.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);

			BoundingBoxCutaway.deactive ( );


			debugNormal.active( );

			glUniformMatrix4fv ( debugNormal.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( debugNormal.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);

			debugNormal.deactive( );

		}
		if ( draw_primary )
		{

			if ( cutVolumes.size ( ) > 0 )
			{
				BoundingBoxDebug.active ( );

				new_z =  camera_.position() - cutVolumes[cluster].center();

				new_z.normalize();

				new_x = new_z ^ camera_.UpVector();

				new_x.normalize();

				new_y = new_z ^ new_x;

				new_y.normalize();

				lookatCamera = Celer::Matrix4x4<float>( new_x, new_y , new_z  );

				// FIXME Throw an Exception when std::map doesnt find A VARIABLE !!!
				glUniform4fv ( BoundingBoxDebug.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].min ( ) , 1.0f ) );
				glUniform4fv ( BoundingBoxDebug.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].max ( ) , 1.0f ) );
				glUniform3fv ( BoundingBoxDebug.uniforms_["center_point"].location , 1 , cutVolumes[cluster].center ( ));
				glUniform3fv ( BoundingBoxDebug.uniforms_["new_x"].location , 1 ,  new_x );
				glUniform3fv ( BoundingBoxDebug.uniforms_["new_y"].location , 1 ,  new_y );
				glUniform3fv ( BoundingBoxDebug.uniforms_["new_z"].location , 1 ,  new_z );
				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
				//VAO
				glBindVertexArray ( vertexArray );
				glDrawArrays ( GL_POINTS , 0 , 1 );
				glBindVertexArray ( 0 );

				BoundingBoxDebug.deactive ( );
			}

 			primary.active ( );

 			glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);

			primary.deactive ( );

		}

	}


//	wireframe.active();
//	glUniform3fv(wireframe.uniforms_["lightDirection"].location,1,camera_.position());
//	glUniform2f(wireframe.uniforms_["WIN_SCALE"].location,(float)width(),(float)height());
//	glUniformMatrix4fv(wireframe.uniforms_["ModelMatrix"].location,1,GL_TRUE, lookatCamera);
//	glUniformMatrix4fv(wireframe.uniforms_["ViewMatrix"].location,1,GL_TRUE, camera_.viewMatrix());
//	glUniformMatrix4fv(wireframe.uniforms_["ProjectionMatrix"].location,1,GL_TRUE, camera_.perspectiveProjectionMatrix() );
//	cube_.drawQuadStrips();
//	wireframe.deactive();


//	Celer::Vector3<float> new_z =  cutVolumes[0].center() - camera_.position();
//
//	new_z.normalize();
//
//	Celer::Vector3<float> new_x = new_z ^ camera_.UpVector();
//
//	new_x.normalize();
//
//	Celer::Vector3<float> new_y = new_z ^ new_x;
//
//	new_y.normalize();
//
//	Celer::Matrix4x4<float> lookatCamera ( new_x, new_y , new_z  );
//
//	orientedBoxApproach.active ( );
//	glBindVertexArray ( vertexArray );
//	glUniform4fv ( orientedBoxApproach.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[0].min ( ) , 1.0f ) );
//	glUniform4fv ( orientedBoxApproach.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[0].max ( ) , 1.0f ) );
//	glUniform3fv ( orientedBoxApproach.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//	glUniformMatrix4fv ( orientedBoxApproach.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
//	glUniformMatrix4fv ( orientedBoxApproach.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//	glUniformMatrix4fv ( orientedBoxApproach.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//	glDrawArrays ( GL_POINTS , 0 , 1 );
//	glBindVertexArray ( 0 );
//	orientedBoxApproach.deactive ( );


}

void GLWidget::NoCutawaySetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/// FIXME Conditions  - Just the model opened.

 	if ( ires_has_been_open_sucessefully )
	{

		if ( draw_secondary )
		{

			if ( cutVolumes.size ( ) > 0 )
			{


				Celer::Vector3<float> new_z =  camera_.position() - cutVolumes[cluster].center ( );

				new_z.normalize();

				Celer::Vector3<float> new_x = new_z ^ camera_.UpVector();

				new_x.normalize();

				Celer::Vector3<float> new_y = new_z ^ new_x;

				new_y.normalize();

				Celer::Matrix4x4<float> lookatCamera ( new_x, new_y , new_z );


				cube_in_GeometryShader.active ( );

				glUniform4fv ( cube_in_GeometryShader.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].min ( ) , 1.0f ) );
				glUniform4fv ( cube_in_GeometryShader.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].max ( ) , 1.0f ) );

				glUniform3fv ( cube_in_GeometryShader.uniforms_["center_point"].location , 1 , cutVolumes[cluster].center ( ));
				glUniform3fv ( cube_in_GeometryShader.uniforms_["new_x"].location , 1 ,  new_x );
				glUniform3fv ( cube_in_GeometryShader.uniforms_["new_y"].location , 1 ,  new_y );
				glUniform3fv ( cube_in_GeometryShader.uniforms_["new_z"].location , 1 ,  new_z );

				glUniform2f ( cube_in_GeometryShader.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
				glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
				glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

				glBindVertexArray ( vertexArray_Cube );
				glDrawArrays ( GL_TRIANGLES_ADJACENCY , 0 , reservoir_list_of_triangles_adjacency_vertices.size ( ) );
				glBindVertexArray ( 0 );

				cube_in_GeometryShader.deactive ( );


//				BoundingBoxDebug.active ( );
//
//				// FIXME Throw an Exception when std::map doesnt find A VARIABLE !!!
//				glUniform4fv ( BoundingBoxDebug.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].min ( ) , 1.0f ) );
//				glUniform4fv ( BoundingBoxDebug.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].max ( ) , 1.0f ) );
//				glUniform3fv ( BoundingBoxDebug.uniforms_["center_point"].location , 1 , cutVolumes[cluster].center ( ));
//				glUniform3fv ( BoundingBoxDebug.uniforms_["new_x"].location , 1 ,  new_x );
//				glUniform3fv ( BoundingBoxDebug.uniforms_["new_y"].location , 1 ,  new_y );
//				glUniform3fv ( BoundingBoxDebug.uniforms_["new_z"].location , 1 ,  new_z );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//				//VAO
//				glBindVertexArray ( vertexArray );
//				glDrawArrays ( GL_POINTS , 0 , 1 );
//				glBindVertexArray ( 0 );
//
//				BoundingBoxDebug.deactive ( );

			}else
			{

				secondary.active ( );

				glUniform3fv ( secondary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

				glUniform3i ( secondary.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
				glUniform3i ( secondary.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );

				glUniform2f ( secondary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

				glUniformMatrix4fv ( secondary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
				glUniformMatrix4fv ( secondary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

				glBindVertexArray(vertexArray);
				glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
				glBindVertexArray(0);

				secondary.deactive ( );
			}

//			debugNormal.active( );
//
//			glUniformMatrix4fv ( debugNormal.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//			glUniformMatrix4fv ( debugNormal.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//			glBindVertexArray(vertexArray);
//			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
//			glBindVertexArray(0);
//
//			debugNormal.deactive( );


		}
		if ( draw_primary )
		{

 			primary.active ( );

 			glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);

			primary.deactive ( );

			glEnable ( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			cutVolume.active ( );

			glUniform3fv ( cutVolume.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( cutVolume.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );

			glUniformMatrix4fv ( cutVolume.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( cutVolume.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray ( vertexArray_box );
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , box_vertices.size() );
			glBindVertexArray ( 0 );

			cutVolume.deactive ( );
			glDisable ( GL_BLEND );


		}

	}

}

void GLWidget::BurnsCutawaySetup ( )
{

	/// FIXME Conditions  - Primary and Secondary well defined.

	int i = 1;

	if ( ires_has_been_open_sucessefully  )
	{

		BurnsJFAInitializing430.active ( );

 		fboStep[1]->bind();

		glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUniformMatrix4fv ( BurnsJFAInitializing430.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
		glUniformMatrix4fv ( BurnsJFAInitializing430.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix()  );

		//VAO
		glBindVertexArray(vertexArray);
		glDrawArrays ( GL_TRIANGLES , 0 , reservoir_list_of_vertices.size());
		glBindVertexArray(0);

		fboStep[1]->release( );

		BurnsJFAInitializing430.deactive ( );


		// Do Jumping Flooding Algorithm
		int stepSize = ( width ( ) > height ( ) ? width ( ) : height ( ) ) * 0.5;
		bool ExitLoop = 0;


		BurnsJFAStep430.active( );


		glUniform2f ( BurnsJFAStep430.uniforms_["viewport"].location, (float)width ( ) , (float)height ( ) );

		pm_sz = ( camera_.nearPlane ( ) + camera_.farPlane ( ) ) / ( camera_.nearPlane ( ) - camera_.farPlane ( ) );


		camera_.setOrthographicProjectionMatrix ( 0.0 , GLfloat ( width ( ) ) , 0.0 , GLfloat ( height ( ) ) , -100.0 , 100.0 );

		while ( !ExitLoop )
		{
			//std::cout << " i " << stepSize << std::endl;
			i = ( i + 1 ) % 2;
			fboStep[i]->bind ( );
			glActiveTexture ( GL_TEXTURE0 );
			glEnable ( GL_TEXTURE_RECTANGLE );
			glBindTexture ( GL_TEXTURE_RECTANGLE , fboStep[ ( i + 1 ) % 2]->texture ( ) );


			glUniform1f ( BurnsJFAStep430.uniforms_["pm_sz"].location,pm_sz );
			glUniform1f ( BurnsJFAStep430.uniforms_["theta"].location,angle );
			glUniform1i ( BurnsJFAStep430.uniforms_["stepSize"].location,stepSize );
			glUniformMatrix4fv ( BurnsJFAStep430.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix() );

			glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
			glClear ( GL_COLOR_BUFFER_BIT );

			glBindVertexArray(vertexArrayScreen);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			stepSize *= 0.5;
			if ( stepSize < 1 )
				ExitLoop = true;
			glDisable ( GL_TEXTURE_RECTANGLE );
			fboStep[i]->release ( );
		}

		BurnsJFAStep430.deactive( ) ;


		glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if ( draw_secondary && (reservoir_list_of_vertices.size ( ) != 0) )
		{
			BurnsCutaway430Wireframe.active ( );

			glActiveTexture ( GL_TEXTURE0 );
			glEnable ( GL_TEXTURE_RECTANGLE );
			glBindTexture ( GL_TEXTURE_RECTANGLE , fboStep[i]->texture ( ) );

			glUniform3fv ( BurnsCutaway430Wireframe.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

 			glUniform3i ( BurnsCutaway430Wireframe.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
 			glUniform3i ( BurnsCutaway430Wireframe.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );

			glUniform2f ( BurnsCutaway430Wireframe.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
			glUniform1i ( BurnsCutaway430Wireframe.uniforms_["cutaway"].location , 1 );
			glUniformMatrix4fv ( BurnsCutaway430Wireframe.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( BurnsCutaway430Wireframe.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			// VAO
			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);
			glDisable ( GL_TEXTURE_RECTANGLE );

			BurnsCutaway430Wireframe.deactive ( );
		}

		if ( draw_primary && (reservoir_list_of_vertices.size ( ) != 0)  )
		{
			primary.active ( );

			glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
			glUniform1i ( primary.uniforms_["cutaway"].location , 1 );
			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);

			primary.deactive ( );
		}

	}
	else if ( ires_has_been_open_sucessefully )
	{

//		secondary.active ( );
//
//		glUniform3fv ( secondary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//		glUniform2f ( secondary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//		glUniform1i ( secondary.uniforms_["cutaway"].location , 0 );
//
//		glUniformMatrix4fv ( secondary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//		glUniformMatrix4fv ( secondary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//		glBindVertexArray(vertexArray);
//		glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
//		glBindVertexArray(0);
//
//		secondary.deactive ( );

	}

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

	QString shaderDirectory ("/media/d/Workspace/IRESReader/src/IRESReader/GUI/Qt/RCC/Shaders/");

	qDebug () << "Directory " << shadersDir.path ();
	shadersDir.cdUp ();
	qDebug () << "Directory " << shadersDir.path ();

	charles_Shader.create( "Charles Shader" ,(shaderDirectory + "Charles.vert").toStdString(),
						 (shaderDirectory + "Charles.geom").toStdString(),
						 (shaderDirectory + "Charles.frag").toStdString() );

	textureViewer.create("textureViewer",(shaderDirectory + "fboTest.vert").toStdString(),
				             (shaderDirectory + "fboTest.frag").toStdString());

	cutVolume.create("cutVolume",(shaderDirectory + "CutVolume.vert").toStdString(),
				     (shaderDirectory + "CutVolume.geom").toStdString(),
				     (shaderDirectory + "CutVolume.frag").toStdString());

	primary.create("primary",(shaderDirectory + "Primary.vert").toStdString(),
				 (shaderDirectory + "Primary.geom").toStdString(),
				 (shaderDirectory + "Primary.frag").toStdString());

	secondary.create("secondary",(shaderDirectory + "Secondary.vert").toStdString(),
			             (shaderDirectory + "Secondary.geom").toStdString(),
				     (shaderDirectory + "Secondary.frag").toStdString());
	// Burns Approach
	BurnsCutaway430.create("BurnsCutaway430",(shaderDirectory + "BurnsCutaway430.vert").toStdString(),
						 (shaderDirectory + "BurnsCutaway430.frag").toStdString());

	BurnsCutaway430Wireframe.create("BurnsCutaway430Wireframe",(shaderDirectory + "BurnsCutaway430Wireframe.vert").toStdString(),
								   (shaderDirectory + "BurnsCutaway430Wireframe.geom").toStdString(),
								   (shaderDirectory + "BurnsCutaway430Wireframe.frag").toStdString());

	BurnsJFAInitializing430.create("BurnsJFAInitializing430",(shaderDirectory + "BurnsJFAInitializing430.vert").toStdString(),
								 (shaderDirectory + "BurnsJFAInitializing430.frag").toStdString());

	BurnsJFAStep430.create("BurnsJFAStep430",(shaderDirectory + "BurnsJFAStep430.vert").toStdString(),
						 (shaderDirectory + "BurnsJFAStep430.frag").toStdString());
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


	wireframe.create ("SinglePassWireframe",  (shaderDirectory + "SinglePassWireframe.vert").toStdString(),
			 (shaderDirectory + "SinglePassWireframe.geom").toStdString(),
			 (shaderDirectory + "SinglePassWireframe.frag").toStdString());


	orientedBoxApproach.create ( "OrientedBoxApproach", (shaderDirectory + "OrientedBoxApproach.vert").toStdString(),
				   (shaderDirectory + "OrientedBoxApproach.geom").toStdString(),
				   (shaderDirectory + "OrientedBoxApproach.frag").toStdString());


	/// Cube in Geometry Shader
	cube_in_GeometryShader.create ( "Cube_in_Geometry_Shader", (shaderDirectory + "Cube_in_Geometry_Shader.vert").toStdString(),
								   (shaderDirectory + "Cube_in_Geometry_Shader.geom").toStdString(),
								   (shaderDirectory + "Cube_in_Geometry_Shader.frag").toStdString());


//	textureViewer.create("textureViewer",(shadersDir.path ()+"/share/Shaders/fboTest.vert").toStdString(),
//			                             (shadersDir.path ()+"/share/Shaders/fboTest.frag").toStdString());
//
//	cutVolume.create("cutVolume",(shadersDir.path ()+"/share/Shaders/CutVolume.vert").toStdString(),
//			             (shadersDir.path ()+"/share/Shaders/CutVolume.geom").toStdString(),
//			             (shadersDir.path ()+"/share/Shaders/CutVolume.frag").toStdString());
//
//	primary.create("primary",(shadersDir.path ()+"/share/Shaders/Primary.vert").toStdString(),
//			                 (shadersDir.path ()+"/share/Shaders/Primary.geom").toStdString(),
//			                 (shadersDir.path ()+"/share/Shaders/Primary.frag").toStdString());
//
//	secondary.create("secondary",(shadersDir.path ()+"/share/Shaders/Secondary.vert").toStdString(),
//			                     (shadersDir.path ()+"/share/Shaders/Secondary.geom").toStdString(),
//			                     (shadersDir.path ()+"/share/Shaders/Secondary.frag").toStdString());
//	// Burns Approach
//	BurnsCutaway430.create("BurnsCutaway430",(shadersDir.path ()+"/share/Shaders/BurnsCutaway430.vert").toStdString(),
//			                         (shadersDir.path ()+"/share/Shaders/BurnsCutaway430.frag").toStdString());
//
//	BurnsCutaway430Wireframe.create("BurnsCutaway430Wireframe",(shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.vert").toStdString(),
//		                                                   (shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.geom").toStdString(),
//	                                                           (shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.frag").toStdString());
//
//	BurnsJFAInitializing430.create("BurnsJFAInitializing430",(shadersDir.path ()+"/share/Shaders/BurnsJFAInitializing430.vert").toStdString(),
//			                                         (shadersDir.path ()+"/share/Shaders/BurnsJFAInitializing430.frag").toStdString());
//
//	BurnsJFAStep430.create("BurnsJFAStep430",(shadersDir.path ()+"/share/Shaders/BurnsJFAStep430.vert").toStdString(),
//			                         (shadersDir.path ()+"/share/Shaders/BurnsJFAStep430.frag").toStdString());
//	// BoudingBox Approach
//	BoundingBoxInitialization.create ("BoundingBoxApproach",(shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.vert").toStdString(),
//							        (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.geom").toStdString(),
//							        (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.frag").toStdString());
//
//	BoundingBoxDebug.create ("BoundingBoxApproach Debug",(shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.vert").toStdString(),
//							     (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.geom").toStdString(),
//							     (shadersDir.path ()+"/share/Shaders/BoundingBoxApproachDebug.frag").toStdString());
//
//	BoundingBoxCutaway.create ("BoundingBoxCutaway",(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.vert").toStdString(),
//							(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.geom").toStdString(),
//							(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.frag").toStdString());
//
//
//	debugNormal.create ("DebugNormal",  (shadersDir.path ()+"/share/Shaders/DebugNormal.vert").toStdString(),
//			   (shadersDir.path ()+"/share/Shaders/DebugNormal.geom").toStdString(),
//			   (shadersDir.path ()+"/share/Shaders/DebugNormal.frag").toStdString());
//
//
//	wireframe.create ("SinglePassWireframe",  (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.vert").toStdString(),
//			 (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.geom").toStdString(),
//			 (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.frag").toStdString());
//
//
//	orientedBoxApproach.create ( "OrientedBoxApproach", (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.vert").toStdString(),
//                                   (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.geom").toStdString(),
//			           (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.frag").toStdString());
//
//
//	/// Cube in Geometry Shader
//	cube_in_GeometryShader.create ( "Cube_in_Geometry_Shader", (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.vert").toStdString(),
//							           (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.geom").toStdString(),
//							           (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.frag").toStdString());

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

	if ( event->key() == Qt::Key_F5 )
	{
		LoadShaders();
		qDebug() << " Hello ";
	}

	updateGL();
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
		updateGL();

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
	updateGL();

}

void GLWidget::wheelEvent ( QWheelEvent *event )
{

	if ( QApplication::keyboardModifiers ( ) == Qt::ShiftModifier )
	{
		zoom_angle_ += event->delta ( ) / 120.0;

	}
	else
	{
		scrollStep_ += event->delta ( ) / 120.0;
		angle = static_cast<float> ( 1.0 / std::tan ( scrollStep_ * Celer::Math::kDeg2Rad ) );
		qDebug ( ) << scrollStep_;
	}

	updateGL();
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

void GLWidget::setPrimaryVisibility( bool visibility )
{
	if ( reservoir_list_of_vertices.size( ) > 0 )
		draw_primary = visibility;
	updateGL();
}

void GLWidget::setSecondaryVisibility( bool visibility )
{
	if ( reservoir_list_of_vertices.size( ) > 0 )
		draw_secondary = visibility;
	updateGL();
}

