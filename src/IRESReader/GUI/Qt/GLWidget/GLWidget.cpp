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
//	timer_.start ( 0 );


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


	glGenVertexArrays ( 1 , &vertexArray_Cube);
		// Cube in Geometry Shader
		glGenBuffers ( 1, &reservoir_vertices_triangles_adjacency_buffer);

	glGenVertexArrays ( 1 , &vertexArrayScreen );


	reservoir_vertices_location 	= 1;
	reservoir_normal_location 	= 2;
	reservoir_color_location 	= 3;
	reservoir_renderFlag_location 	= 4;
	reservoir_IJK_location 		= 5;

	// Cube in Geomtry Shader
	reservoir_vertices_triangles_adjacency_location = 11;

	draw_secondary = 1;
	draw_primary = 0;

	isBurnsApproach = 0;
	isBoudingBoxApproach = 0;
	cluster = 0;

	glGenVertexArrays (1, &vertexArray_box);
		glGenBuffers( 1, &vertexBuffer_box);


	cube_.createBuffers( Celer::Vector3<float>(0,0,0) );
}

bool GLWidget::isIresWasOpenedSucessufully ( ) const
{
	return ires_has_been_open_sucessefully;
}

void GLWidget::CutVolumeGenerator( )
{

	Celer::BoundingBox3<GLfloat> box;


	while ( boxes.size ( ) != 0 )
	{

		box = boxes.front( );

		boxes.pop_front( );

		std::list<Celer::BoundingBox3<GLfloat> >::iterator box_iterator = boxes.begin ( );

		int cont = 0;

		while ( ( box_iterator != boxes.end ( ) ) && ( cont != 10) )
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

	boxes.clear ( );
	cutVolumes.clear();

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::size_t i = 0;

	std::vector<Celer::Vector4<GLfloat> > colors ( 24 );
	std::vector<Celer::Vector4<GLfloat> > renderFlags ( 24 );

	Celer::Vector4<GLfloat> color;

	while ( i < reservoir_model_.block_indices.size ( ) )
	{
		if ( reservoir_model_.block_indices[i] != -1 )
		{

			float regularValue = ( reservoir_model_.static_porperties[property_index].values_[i / 8] );

			float normalizedColor = ( regularValue - min ) / ( max - min );

			switch ( (int) ( normalizedColor * 10.0f ) )
			{
				case 10:
					color = Celer::Vector4<GLfloat> ( 1.0f , 0.0f , 0.0f , 1.0f );
					break;
				case 9:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.25f , 0.0f , 1.0f );
					break;
				case 8:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.0f , 1.0f );
					break;
				case 7:
					color = Celer::Vector4<GLfloat> ( 0.25f , 0.5f , 0.0f , 1.0f );
					break;
				case 6:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.25f , 0.0f , 1.0f );
					break;
				case 5:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.2f , 1.0f );
					break;
				case 4:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.4f , 1.0f );
					break;
				case 3:
					color = Celer::Vector4<GLfloat> ( 0 , 0.4 , 0.5 , 1.0f );
					break;
				case 2:
					color = Celer::Vector4<GLfloat> ( 0 , 0.2 , 0.5 , 1.0f );
					break;
				case 1:
					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.5 , 1.0f );
					break;
				case 0:
					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.375 , 1.0f );
					break;
					//if value is equal to max
				default:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.5f , 1.0f );
					break;

			}

			colors = std::vector<Celer::Vector4<GLfloat> > ( 24 , color );

			if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
			{
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f ) );



				int index [] =
				{
				    // Top Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+2],/* 0 - 5*/
				    // Bottom Face
				    reservoir_model_.block_indices[i+4],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+5],reservoir_model_.block_indices[i+6],/* 6 - 11 */
				    // Front Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+4],/* 12 - 17*/
				    // Back Face
				    reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+2],reservoir_model_.block_indices[i+6],reservoir_model_.block_indices[i+5],/* 18 - 23*/
				    // Right Face
				    reservoir_model_.block_indices[i+2],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+5],reservoir_model_.block_indices[i+4],/* 24 - 29*/
				    // Left Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+6] /* 30 - 35*/
				};

				// Top Face
				Celer::Vector3<GLfloat> v0( static_cast<GLfloat>(reservoir_model_.vertices[index[0]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].z));
				Celer::Vector3<GLfloat> v1( static_cast<GLfloat>(reservoir_model_.vertices[index[1]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].z));
				Celer::Vector3<GLfloat> v3( static_cast<GLfloat>(reservoir_model_.vertices[index[2]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].z));
				Celer::Vector3<GLfloat> v2( static_cast<GLfloat>(reservoir_model_.vertices[index[3]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].z));
				// Bottom Face
				Celer::Vector3<GLfloat> v4( static_cast<GLfloat>(reservoir_model_.vertices[index[4]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].z));
				Celer::Vector3<GLfloat> v7( static_cast<GLfloat>(reservoir_model_.vertices[index[5]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].z));
				Celer::Vector3<GLfloat> v5( static_cast<GLfloat>(reservoir_model_.vertices[index[6]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].z));
				Celer::Vector3<GLfloat> v6( static_cast<GLfloat>(reservoir_model_.vertices[index[7]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].z));


				std::vector<Celer::Vector4<GLfloat> > vertices;
				vertices.resize( 24 );


				// Top Face
				vertices[0] = Celer::Vector4<GLfloat> ( v0 , 1.0f );
				vertices[1] = Celer::Vector4<GLfloat> ( v1 , 1.0f );
				vertices[2] = Celer::Vector4<GLfloat> ( v3 , 1.0f );
				vertices[3] = Celer::Vector4<GLfloat> ( v2 , 1.0f );
				// Bottom Face
				vertices[4] = Celer::Vector4<GLfloat> ( v4 , 1.0f );
				vertices[5] = Celer::Vector4<GLfloat> ( v7 , 1.0f );
				vertices[6] = Celer::Vector4<GLfloat> ( v5 , 1.0f );
				vertices[7] = Celer::Vector4<GLfloat> ( v6 , 1.0f );
				// Front Face
				vertices[8] = Celer::Vector4<GLfloat> ( v0 , 1.0f );
				vertices[9] = Celer::Vector4<GLfloat> ( v7 , 1.0f );
				vertices[10] = Celer::Vector4<GLfloat> ( v3 , 1.0f );
				vertices[11] = Celer::Vector4<GLfloat> ( v4 , 1.0f );
				// Back Face
				vertices[12] = Celer::Vector4<GLfloat> ( v1 , 1.0f );
				vertices[13] = Celer::Vector4<GLfloat> ( v2 , 1.0f );
				vertices[14] = Celer::Vector4<GLfloat> ( v6 , 1.0f );
				vertices[15] = Celer::Vector4<GLfloat> ( v5 , 1.0f );
				// Right Face
				vertices[16] = Celer::Vector4<GLfloat> ( v2 , 1.0f );
				vertices[17] = Celer::Vector4<GLfloat> ( v3 , 1.0f );
				vertices[18] = Celer::Vector4<GLfloat> ( v5 , 1.0f );
				vertices[19] = Celer::Vector4<GLfloat> ( v4 , 1.0f );
				// Left Face
				vertices[20] = Celer::Vector4<GLfloat> ( v0 , 1.0f );
				vertices[21] = Celer::Vector4<GLfloat> ( v1 , 1.0f );
				vertices[22] = Celer::Vector4<GLfloat> ( v7 , 1.0f );
				vertices[23] = Celer::Vector4<GLfloat> ( v6 , 1.0f );


				Celer::BoundingBox3<GLfloat> box;

				box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );

				boxes.push_back( box );


			}
			else
			{
				renderFlags = std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f ) );
			}

			std::copy ( colors.begin( ) 	, colors.begin( )      + 24 , std::back_inserter ( reservoir_list_of_colors     ) );
			std::copy ( renderFlags.begin( ), renderFlags.begin( ) + 24 , std::back_inserter ( reservoir_list_of_renderFlag ) );
			i += 8;
		}
		else
		{
			i += 1;
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

}

void GLWidget::changeProperty ( int property_index )
{


	reservoir_list_of_colors.clear ( );

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	std::size_t i = 0;

	while ( i < reservoir_model_.block_indices.size( ) )
	{
		if ( reservoir_model_.block_indices[i] != -1 )
		{

			float normalized_color = ( reservoir_model_.static_porperties[property_index].values_[i/8] - min ) / ( max - min );

			Celer::Vector4<GLfloat> color(1.0,1.0,1.0,1.0);

			switch ( (int) ( normalized_color * 10.0f ) )
			{
				case 10:
					color = Celer::Vector4<GLfloat> ( 1.0f , 0.0f , 0.0f , 1.0f );
					break;
				case 9:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.25f , 0.0f , 1.0f );
					break;
				case 8:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.0f , 1.0f );
					break;
				case 7:
					color = Celer::Vector4<GLfloat> ( 0.25f , 0.5f , 0.0f , 1.0f );
					break;
				case 6:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.25f , 0.0f , 1.0f );
					break;
				case 5:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.2f , 1.0f );
					break;
				case 4:
					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.4f , 1.0f );
					break;
				case 3:
					color = Celer::Vector4<GLfloat> ( 0 , 0.4 , 0.5 , 1.0f );
					break;
				case 2:
					color = Celer::Vector4<GLfloat> ( 0 , 0.2 , 0.5 , 1.0f );
					break;
				case 1:
					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.5 , 1.0f );
					break;
				case 0:
					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.375 , 1.0f );
					break;
					//if value is equal to max
				default:
					color = Celer::Vector4<GLfloat> ( 0.5f , 0.0f , 0.0f , 1.0f );
					break;
			}

			Celer::Vector4<GLfloat> colors[] =
			{
				color, color, color, color, color, color,

				color, color, color, color, color, color,

				color, color, color, color, color, color,

				color, color, color, color, color, color,
			};

			std::copy ( colors , colors + 24 , std::back_inserter ( reservoir_list_of_colors ) );

			i += 8;

		}
		else
		{
			i += 1;
		}
	}

	glBindVertexArray(vertexArray);

	glBindBuffer ( GL_ARRAY_BUFFER , reservoir_color_buffer );
	glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_colors.size ( ) * sizeof ( reservoir_list_of_colors[0] ) , &reservoir_list_of_colors[0] , GL_STREAM_DRAW );

	glBindVertexArray(0);

	// Vertex Array : Set up generic attributes pointers

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

void GLWidget::openIRES ( const std::string& filename )
{

	reservoir_model_.openIRES(filename);

	if ( reservoir_model_.block_indices.size ( ) > 0 )
	{
		ires_has_been_open_sucessefully = 1;

		reservoir_list_of_vertices.clear ( );
		reservoir_list_of_normals.clear ( );
		reservoir_list_of_colors.clear ( );
		reservoir_list_of_IJKs.clear ( );
		reservoir_list_of_renderFlag.clear( );
		reservoir_list_of_indices.clear ( );

		// Cube in Geometry Shader
		reservoir_list_of_triangle_adjacency.clear();

		std::size_t i = 0;

		while ( i < reservoir_model_.block_indices.size ( ) )
		{
			if ( reservoir_model_.block_indices[i] != -1)
			{


				/// From Nicole`s presentation
				Celer::Vector4<int> IJK ( (  (i/8) % reservoir_model_.header_.number_of_Blocks_in_I_Direction) + 1,
						          ( ((i/8) / reservoir_model_.header_.number_of_Blocks_in_I_Direction) % reservoir_model_.header_.number_of_Blocks_in_J_Direction ) +1,
						          (  (i/8) / (reservoir_model_.header_.number_of_Blocks_in_I_Direction * reservoir_model_.header_.number_of_Blocks_in_J_Direction )) +1,
						          0 );

				Celer::Vector4<int> IJKs [] =
				{

				     IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK ,IJK,IJK,IJK,IJK

				};


				int index [] =
				{
				    // Top Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+2],/* 0 - 5*/
				    // Bottom Face
				    reservoir_model_.block_indices[i+4],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+5],reservoir_model_.block_indices[i+6],/* 6 - 11 */
				    // Front Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+4],/* 12 - 17*/
				    // Back Face
				    reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+2],reservoir_model_.block_indices[i+6],reservoir_model_.block_indices[i+5],/* 18 - 23*/
				    // Right Face
				    reservoir_model_.block_indices[i+2],reservoir_model_.block_indices[i+3],reservoir_model_.block_indices[i+5],reservoir_model_.block_indices[i+4],/* 24 - 29*/
				    // Left Face
				    reservoir_model_.block_indices[i+0],reservoir_model_.block_indices[i+1],reservoir_model_.block_indices[i+7],reservoir_model_.block_indices[i+6] /* 30 - 35*/
				};

				// Top Face
				Celer::Vector3<GLfloat> v0( static_cast<GLfloat>(reservoir_model_.vertices[index[0]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].z));
				Celer::Vector3<GLfloat> v1( static_cast<GLfloat>(reservoir_model_.vertices[index[1]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].z));
				Celer::Vector3<GLfloat> v3( static_cast<GLfloat>(reservoir_model_.vertices[index[2]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].z));
				Celer::Vector3<GLfloat> v2( static_cast<GLfloat>(reservoir_model_.vertices[index[3]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].z));
				// Bottom Face
				Celer::Vector3<GLfloat> v4( static_cast<GLfloat>(reservoir_model_.vertices[index[4]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].z));
				Celer::Vector3<GLfloat> v7( static_cast<GLfloat>(reservoir_model_.vertices[index[5]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].z));
				Celer::Vector3<GLfloat> v5( static_cast<GLfloat>(reservoir_model_.vertices[index[6]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].z));
				Celer::Vector3<GLfloat> v6( static_cast<GLfloat>(reservoir_model_.vertices[index[7]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].z));

				Celer::Vector4<GLfloat> vertices [] =
				{
				    // Top Face
				    Celer::Vector4<GLfloat> ( v0, 1.0f),
				    Celer::Vector4<GLfloat> ( v3, 1.0f),
				    Celer::Vector4<GLfloat> ( v1, 1.0f),
				    Celer::Vector4<GLfloat> ( v2, 1.0f),
				    // Bottom Face
				    Celer::Vector4<GLfloat> ( v4, 1.0f),
				    Celer::Vector4<GLfloat> ( v7, 1.0f),
				    Celer::Vector4<GLfloat> ( v5, 1.0f),
				    Celer::Vector4<GLfloat> ( v6, 1.0f),
				    // Front Face
				    Celer::Vector4<GLfloat> ( v0, 1.0f),
				    Celer::Vector4<GLfloat> ( v7, 1.0f),
				    Celer::Vector4<GLfloat> ( v3, 1.0f),
				    Celer::Vector4<GLfloat> ( v4, 1.0f),
				    // Back Face
				    Celer::Vector4<GLfloat> ( v1, 1.0f),
				    Celer::Vector4<GLfloat> ( v2, 1.0f),
				    Celer::Vector4<GLfloat> ( v6, 1.0f),
				    Celer::Vector4<GLfloat> ( v5, 1.0f),
				    // Right Face
				    Celer::Vector4<GLfloat> ( v2, 1.0f),
				    Celer::Vector4<GLfloat> ( v3, 1.0f),
				    Celer::Vector4<GLfloat> ( v5, 1.0f),
				    Celer::Vector4<GLfloat> ( v4, 1.0f),
				    // Left Face
				    Celer::Vector4<GLfloat> ( v0, 1.0f),
				    Celer::Vector4<GLfloat> ( v1, 1.0f),
				    Celer::Vector4<GLfloat> ( v7, 1.0f),
				    Celer::Vector4<GLfloat> ( v6, 1.0f)
				};

				Celer::Vector4<GLfloat> vertices_triangle_adjacency [] =
				{
   				     // Top Face
				     Celer::Vector4<GLfloat> ( v0, v5.x ),
				     Celer::Vector4<GLfloat> ( v3, v5.y ),
				     Celer::Vector4<GLfloat> ( v1, v5.z ),
				     Celer::Vector4<GLfloat> ( v2, v6.x ),
				     // Bottom Face
				     Celer::Vector4<GLfloat> ( v4, v6.y ),
				     Celer::Vector4<GLfloat> ( v7, v6.z )
				};

				Celer::Vector3<GLfloat> topNormal 	= ((v3 - v0) ^ (v1 - v0)).norm();
				//std::cout << topNormal << std::endl;
				Celer::Vector3<GLfloat> bottomNormal 	= ((v7 - v4) ^ (v5 - v4)).norm();
				//std::cout << bottomNormal << std::endl;
				Celer::Vector3<GLfloat> frontNormal 	= ((v7 - v0) ^ (v3 - v0)).norm();
				//std::cout << frontNormal << std::endl;
				Celer::Vector3<GLfloat> backmNormal 	= ((v2 - v1) ^ (v6 - v1)).norm();
				//std::cout << backmNormal << std::endl;
				Celer::Vector3<GLfloat> rightNormal 	= ((v3 - v2) ^ (v5 - v2)).norm();
				//std::cout << rightNormal << std::endl;
				Celer::Vector3<GLfloat> leftNormal 	= ((v1 - v0) ^ (v7 - v0)).norm();
				//std::cout << leftNormal << std::endl;
//
//
				// Care about the type: GL_DOUBLE or GL_FLOAT
				Celer::Vector4<GLfloat> normals[] =
				{
				  // X Y Z
					//  Top Face
					Celer::Vector4<GLfloat> ( topNormal, 1.0f),
					Celer::Vector4<GLfloat> ( topNormal, 1.0f),
					Celer::Vector4<GLfloat> ( topNormal, 1.0f),
					Celer::Vector4<GLfloat> ( topNormal, 1.0f),
					// Bottom Face
					Celer::Vector4<GLfloat> ( bottomNormal, 1.0f),
					Celer::Vector4<GLfloat> ( bottomNormal, 1.0f),
					Celer::Vector4<GLfloat> ( bottomNormal, 1.0f),
					Celer::Vector4<GLfloat> ( bottomNormal, 1.0f),
					// Front Face
					Celer::Vector4<GLfloat> ( frontNormal, 1.0f),
					Celer::Vector4<GLfloat> ( frontNormal, 1.0f),
					Celer::Vector4<GLfloat> ( frontNormal, 1.0f),
					Celer::Vector4<GLfloat> ( frontNormal, 1.0f),
					// Back Face
					Celer::Vector4<GLfloat> ( backmNormal, 1.0f),
					Celer::Vector4<GLfloat> ( backmNormal, 1.0f),
					Celer::Vector4<GLfloat> ( backmNormal, 1.0f),
					Celer::Vector4<GLfloat> ( backmNormal, 1.0f),
					// Right Face
					Celer::Vector4<GLfloat> ( rightNormal, 1.0f),
					Celer::Vector4<GLfloat> ( rightNormal, 1.0f),
					Celer::Vector4<GLfloat> ( rightNormal, 1.0f),
					Celer::Vector4<GLfloat> ( rightNormal, 1.0f),
					// Left Face
					Celer::Vector4<GLfloat> ( leftNormal, 1.0f),
					Celer::Vector4<GLfloat> ( leftNormal, 1.0f),
					Celer::Vector4<GLfloat> ( leftNormal, 1.0f),
					Celer::Vector4<GLfloat> ( leftNormal, 1.0f)
				};



				Celer::Vector4<GLfloat> focus [] =
				{
				    // Top Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    // Bottom Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    // Front Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    // Back Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    // Right Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    // Left Face
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
				    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f)
				};


				std::copy ( vertices 	, vertices + 24 , std::back_inserter ( reservoir_list_of_vertices  ) );
				std::copy ( normals 	, normals  + 24 , std::back_inserter ( reservoir_list_of_normals) );
				std::copy ( index 	, index    + 24 , std::back_inserter ( reservoir_list_of_indices   ) );
				std::copy ( focus 	, focus    + 24 , std::back_inserter ( reservoir_list_of_renderFlag) );
				std::copy ( IJKs 	, IJKs     + 24 , std::back_inserter ( reservoir_list_of_IJKs  ) );

				//Cube in Geometry Shader
				std::copy ( vertices_triangle_adjacency,  vertices_triangle_adjacency + 6 , std::back_inserter (reservoir_list_of_triangle_adjacency) );


				i += 8;



			}  // end of looping list of blocks
			else
			{
				i += 1;
			}
		}

		max_I_ = 0;
		min_I_ = 0;
		max_J_ = 0;
		min_J_ = 0;
		max_K_ = 0;
		min_K_ = 0;


		camera_.setPosition ( reservoir_model_.box.center ( ) );
		camera_.setTarget ( reservoir_model_.box.center ( ) );
		std::cout << reservoir_model_.box.diagonal ( );
		camera_.setOffset ( 3.0 * reservoir_model_.box.diagonal ( ) );


		std::cout << camera_.position ( );

		camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );

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

		// FIXME DONT PUT VERTEX BUFFER ID at glEnableVertexAttribArray mad BOY ouvindo RAULZITO!!
		glBindVertexArray(vertexArray_Cube);

			// Vertex Buffer for Cube in Geometry Shader
			glBindBuffer ( GL_ARRAY_BUFFER , reservoir_vertices_triangles_adjacency_buffer );
			glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_triangle_adjacency.size ( ) * sizeof ( reservoir_list_of_triangle_adjacency[0] ) , &reservoir_list_of_triangle_adjacency[0] , GL_STREAM_DRAW );
			// Vertex Array : Set up generic attributes pointers
			glEnableVertexAttribArray ( reservoir_vertices_triangles_adjacency_location );
			glVertexAttribPointer ( reservoir_vertices_triangles_adjacency_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindVertexArray(vertexArrayScreen);


		GLfloat openGLScreenCoordinates[] = {
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

		changeProperty(0);

	}
	else
	{
		ires_has_been_open_sucessefully = 0;
	}


}

void GLWidget::openIRES2 ( const std::string& filename )
{
	reservoir_model_.openIRES(filename);

	if ( reservoir_model_.blocks.size( ) )
	{

		reservoir_list_of_vertices.clear ( );
		reservoir_list_of_normals.clear ( );
		reservoir_list_of_colors.clear ( );
		reservoir_list_of_IJKs.clear ( );
		reservoir_list_of_renderFlag.clear( );
		reservoir_list_of_indices.clear ( );


		for ( int i = 0; i < reservoir_model_.blocks.size( ) ; i++)
		{

			Celer::Vector4<GLfloat> focus [] =
			{
			    // Top Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    // Bottom Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    // Front Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    // Back Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    // Right Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    // Left Face
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f),
			    Celer::Vector4<GLfloat> ( 1.0,1.0,1.0,1.0f)
			};

			std::copy(reservoir_model_.blocks[i].vertices.begin(), reservoir_model_.blocks[i].vertices.end(),
					std::back_inserter(reservoir_list_of_vertices));
			std::copy(reservoir_model_.blocks[i].normals.begin(), reservoir_model_.blocks[i].normals.end(),
					std::back_inserter(reservoir_list_of_normals));
			std::copy(reservoir_model_.blocks[i].indices.begin(), reservoir_model_.blocks[i].indices.end(),
					std::back_inserter(reservoir_list_of_indices));
			std::copy(focus, focus + 24,
					std::back_inserter(reservoir_list_of_renderFlag));
			std::copy(reservoir_model_.blocks[i].IJK.begin(), reservoir_model_.blocks[i].IJK.end(),
					std::back_inserter(reservoir_list_of_IJKs));

		}


		max_I_ = 0;
		min_I_ = 0;
		max_J_ = 0;
		min_J_ = 0;
		max_K_ = 0;
		min_K_ = 0;


		camera_.setPosition ( reservoir_model_.box.center ( ) );
		camera_.setTarget ( reservoir_model_.box.center ( ) );
		std::cout << reservoir_model_.box.diagonal ( );
		camera_.setOffset ( 3.0 * reservoir_model_.box.diagonal ( ) );

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

		glBindVertexArray(vertexArrayScreen);


		GLfloat openGLScreenCoordinates[] = {
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

		changeProperty(0);

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
	camera_.setPerspectiveProjectionMatrix ( 0 , camera_.aspectRatio ( ) , 1.0 , 500 );
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

	glBindBuffer ( GL_ARRAY_BUFFER , texture_buffer );
	glBufferData ( GL_ARRAY_BUFFER, 12 * sizeof( textureCoordinates[0] ), textureCoordinates , GL_STATIC_DRAW);
	// Vertex Array : Set up generic attributes pointers
	glBindVertexArray(0);


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

	camera_.setPerspectiveProjectionMatrix ( zoom_angle_ , camera_.aspectRatio ( ) , 1.0 , 500 );

	if 	( isBurnsApproach )
	{
		camera_.setTarget( reservoir_model_.box.center( ) );
		BurnsCutawaySetup ( );
	}
	else if ( isBoudingBoxApproach )
	{
		camera_.setTarget( cutVolumes[cluster].center() );
		BoundingVolumeCutawaySetup ( cluster );
	}
	else
	{
		if ( cutVolumes.size() > 0)
			camera_.setTarget( cutVolumes[cluster].center() );
		NoCutawaySetUp ( );
	}

}

void GLWidget::BoundingVolumeCutawaySetup( int cluster )
{


		//Celer::Matrix4x4<float> lookatCamera ( Celer::Vector3<float>::UNIT_X, Celer::Vector3<float>::UNIT_Y , Celer::Vector3<float>::UNIT_Z  );


 	if ( ires_has_been_open_sucessefully )
	{

// 		glEnable(GL_BLEND);
// 		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

 		if ( cutVolumes.size( ) > 0)
 		{
 			Celer::Vector3<float> new_z =  camera_.position() - reservoir_model_.box.center();

 			new_z.normalize();

 			Celer::Vector3<float> new_x = new_z ^ camera_.UpVector();

 			new_x.normalize();

 			Celer::Vector3<float> new_y = new_z ^ new_x;

 			new_y.normalize();

 			Celer::Matrix4x4<float> lookatCamera ( new_x, new_y , new_z  );


			BoundingBoxInitialization.active ( );

			fboStep[1]->bind ( );

			glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glUniform4fv ( BoundingBoxInitialization.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].min ( ) , 1.0f ) );
			glUniform4fv ( BoundingBoxInitialization.uniforms_["max_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[cluster].max ( ) , 1.0f ) );
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

//			if ( cutVolumes.size ( ) > 0 )
//			{
//				BoundingBoxDebug.active ( );
//
//				glUniform4fv ( BoundingBoxDebug.uniforms_["min_point"].location , 1 , Celer::Vector4<float> ( cutVolumes[0].min ( ) , 1.0f ) );
//				glUniform4fv ( BoundingBoxDebug.uniforms_["max_pont"].location , 1 , Celer::Vector4<float> ( cutVolumes[0].max ( ) , 1.0f ) );
//				glUniform3fv ( BoundingBoxDebug.uniforms_["lightDirection"].location , 1 , camera_.position ( ) );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ModelMatrix"].location , 1 , GL_TRUE , lookatCamera );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//				glUniformMatrix4fv ( BoundingBoxDebug.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//				//VAO
//				glBindVertexArray ( vertexArray );
//				glDrawArrays ( GL_POINTS , 0 , 1 );
//				glBindVertexArray ( 0 );
//
//				BoundingBoxDebug.deactive ( );
//			}

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

void GLWidget::BurnsCutawaySetup ( )
{

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


		if ( draw_secondary && (reservoir_list_of_vertices.size ( ) != 0) )
		{
			BurnsCutaway430Wireframe.active ( );

			glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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

			//VAO
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

void GLWidget::NoCutawaySetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


 	if ( ires_has_been_open_sucessefully )
	{

		if ( draw_secondary )
		{

			cube_in_GeometryShader.active();

			glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glBindVertexArray ( vertexArray_Cube );
			glDrawArrays ( GL_TRIANGLES_ADJACENCY , 0 , reservoir_list_of_triangle_adjacency.size());
			glBindVertexArray(0);

			cube_in_GeometryShader.deactive();


//			camera_.setPosition ( cube_.box.center ( ) );
//			camera_.setTarget ( cube_.box.center ( ) );
//			camera_.setOffset ( 3.0 * cube_.box.diagonal ( ) );
//
//
//			cube_in_GeometryShader.active();
//			glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//			glUniformMatrix4fv ( cube_in_GeometryShader.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//			cube_.drawTriangleStripAdjacencyBuffers();
//			cube_in_GeometryShader.deactive();

// 			secondary.active ( );
//
// 			glUniform3fv ( secondary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
// 			glUniform3i ( secondary.uniforms_["min_IJK"].location , min_I_, min_J_, min_K_ );
// 			glUniform3i ( secondary.uniforms_["max_IJK"].location , max_I_, max_J_, max_K_ );
//
//			glUniform2f ( secondary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//
//			glUniformMatrix4fv ( secondary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//			glUniformMatrix4fv ( secondary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//			glBindVertexArray(vertexArray);
//			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
//			glBindVertexArray(0);
//
//			secondary.deactive ( );



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

//			glEnable ( GL_BLEND );
//			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

//			cutVolume.active ( );
//
//			glUniform3fv ( cutVolume.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//			glUniform2f ( cutVolume.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//
//			glUniformMatrix4fv ( cutVolume.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//			glUniformMatrix4fv ( cutVolume.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//			glBindVertexArray ( vertexArray_box );
//			glDrawArrays ( GL_LINES_ADJACENCY , 0 , box_vertices.size() );
//			glBindVertexArray ( 0 );
//
//			cutVolume.deactive ( );
			//glDisable ( GL_BLEND );


		}
		
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

	qDebug () << "Directory " << shadersDir.path ();
	shadersDir.cdUp ();
	qDebug () << "Directory " << shadersDir.path ();

	textureViewer.create("textureViewer",(shadersDir.path ()+"/share/Shaders/fboTest.vert").toStdString(),
			                             (shadersDir.path ()+"/share/Shaders/fboTest.frag").toStdString());

	cutVolume.create("cutVolume",(shadersDir.path ()+"/share/Shaders/CutVolume.vert").toStdString(),
			             (shadersDir.path ()+"/share/Shaders/CutVolume.geom").toStdString(),
			             (shadersDir.path ()+"/share/Shaders/CutVolume.frag").toStdString());

	primary.create("primary",(shadersDir.path ()+"/share/Shaders/Primary.vert").toStdString(),
			                 (shadersDir.path ()+"/share/Shaders/Primary.geom").toStdString(),
			                 (shadersDir.path ()+"/share/Shaders/Primary.frag").toStdString());

	secondary.create("secondary",(shadersDir.path ()+"/share/Shaders/Secondary.vert").toStdString(),
			                     (shadersDir.path ()+"/share/Shaders/Secondary.geom").toStdString(),
			                     (shadersDir.path ()+"/share/Shaders/Secondary.frag").toStdString());
	// Burns Approach
	BurnsCutaway430.create("BurnsCutaway430",(shadersDir.path ()+"/share/Shaders/BurnsCutaway430.vert").toStdString(),
			                         (shadersDir.path ()+"/share/Shaders/BurnsCutaway430.frag").toStdString());

	BurnsCutaway430Wireframe.create("BurnsCutaway430Wireframe",(shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.vert").toStdString(),
		                                                   (shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.geom").toStdString(),
	                                                           (shadersDir.path ()+"/share/Shaders/BurnsCutaway430Wireframe.frag").toStdString());

	BurnsJFAInitializing430.create("BurnsJFAInitializing430",(shadersDir.path ()+"/share/Shaders/BurnsJFAInitializing430.vert").toStdString(),
			                                         (shadersDir.path ()+"/share/Shaders/BurnsJFAInitializing430.frag").toStdString());

	BurnsJFAStep430.create("BurnsJFAStep430",(shadersDir.path ()+"/share/Shaders/BurnsJFAStep430.vert").toStdString(),
			                         (shadersDir.path ()+"/share/Shaders/BurnsJFAStep430.frag").toStdString());
	// BoudingBox Approach
	BoundingBoxInitialization.create ("BoundingBoxApproach",(shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.vert").toStdString(),
							        (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.geom").toStdString(),
							        (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.frag").toStdString());

	BoundingBoxDebug.create ("BoundingBoxApproach Debug",(shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.vert").toStdString(),
							     (shadersDir.path ()+"/share/Shaders/BoundingBoxApproach.geom").toStdString(),
							     (shadersDir.path ()+"/share/Shaders/BoundingBoxApproachDebug.frag").toStdString());

	BoundingBoxCutaway.create ("BoundingBoxCutaway",(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.vert").toStdString(),
							(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.geom").toStdString(),
							(shadersDir.path ()+"/share/Shaders/BoundingBoxCutaway.frag").toStdString());


	debugNormal.create ("DebugNormal",  (shadersDir.path ()+"/share/Shaders/DebugNormal.vert").toStdString(),
			   (shadersDir.path ()+"/share/Shaders/DebugNormal.geom").toStdString(),
			   (shadersDir.path ()+"/share/Shaders/DebugNormal.frag").toStdString());


	wireframe.create ("SinglePassWireframe",  (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.vert").toStdString(),
			 (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.geom").toStdString(),
			 (shadersDir.path ()+"/share/Shaders/SinglePassWireframe.frag").toStdString());


	orientedBoxApproach.create ( "OrientedBoxApproach", (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.vert").toStdString(),
                                   (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.geom").toStdString(),
			           (shadersDir.path ()+"/share/Shaders/OrientedBoxApproach.frag").toStdString());


	/// Cube in Geometry Shader
	cube_in_GeometryShader.create ( "Cube_in_Geometry_Shader", (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.vert").toStdString(),
							           (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.geom").toStdString(),
							           (shadersDir.path ()+"/share/Shaders/Cube_in_Geometry_Shader.frag").toStdString());

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

