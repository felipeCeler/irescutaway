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

	// FIXME cutBox
	glGenVertexArrays ( 1, &vertexArray_cutBox );
		glGenBuffers ( 1,&vertexBuffer_cutBox );

	// Cuboid
	glGenVertexArrays ( 1, &vertexArray_cuboid );
		glGenBuffers  ( 1, &vertexBuffer_cuboid ); 	// Geometry
		glGenBuffers  ( 1, &vertexBuffer_cube_color  ); // Property Color
		glGenBuffers  ( 1, &vertexBuffer_cube_IJK  );   // Cube IJK

	// Face Features
	glGenVertexArrays ( 1, &vertexArray_faceFeature );
		glGenBuffers ( 1, &vertexBuffer_faceFeature ); // Geometry
		glGenBuffers ( 1, &vertexBuffer_faceColor );   // Property Color
		glGenBuffers ( 1, &vertexBuffer_faceIJK );     // Face IJK

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

	trackball_ = new Trackball();

	trackball_->initOpenGLMatrices();

	trackball_->initializeBuffers();

	enable_blend_ = 0;

	depthFBO = new Framebuffer( width() , height(), 2 );

	meanFilter = new MeanFilter( "Guassian blur");
	meanFilter->resize(width(), height());

	loadShaders ( );

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

	cutBoxs.clear();

	std::cout << " number of boxes initial " << boxes.size() << std::endl;

	// For while we are not considering cluster, just individual cells.
	// Clusters are just the union cell's bounding boxes.
	while ( boxes.size ( ) != 0 )
	{

		box = boxes.front( );

		boxes.pop_front( );

//		std::list<Celer::BoundingBox3<GLfloat> >::iterator box_iterator = boxes.begin ( );
//
//		int cont = 0;
//
//		while ( ( box_iterator != boxes.end ( ) )  )
//		{
//
//			if ( box.intersect( *box_iterator ) )
//			{
//				box 	     = box + (*box_iterator);
//				box_iterator = boxes.erase ( box_iterator );
//				cont++;
//			}
//			else
//			{
//				++box_iterator;
//			}
//		}

		cutVolumes.push_back( box );
	}


	box_vertices.resize( cutVolumes.size( ) );
	int cont = 0;

	cutBoxs.resize( cutVolumes.size( ) );

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

		cutBoxs[cont].center  =  Eigen::Vector4f(it->center( ).x,it->center( ).y,it->center( ).z,1.0f);
		cutBoxs[cont].axis[0] =  Eigen::Vector4f(1.0f,0.0f,0.0f,1.0f);
		cutBoxs[cont].axis[1] =  Eigen::Vector4f(0.0f,1.0f,0.0f,1.0f);
		cutBoxs[cont].axis[2] =  Eigen::Vector4f(0.0f,0.0f,1.0f,1.0f);
		cutBoxs[cont].extent  =  Eigen::Vector4f(abs(it->box_max().x-it->box_min().x),
                                                               abs(it->box_max().y-it->box_min().y),
		                                               abs(it->box_max().z-it->box_min().z),1.0f);
		cutBoxs[cont].aperture = Eigen::Vector4f(1.0f,1.0f,1.0f,1.0f);

		cont++;
	}

	glBindVertexArray ( vertexArray_cutBox );

                glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cutBox );
                glBufferData ( GL_ARRAY_BUFFER , cutBoxs.size( ) * sizeof(cutBoxs[0]) , &cutBoxs[0] , GL_STATIC_DRAW );

                int size_of_vertice = sizeof(Eigen::Vector4f);
                int size_of_struct  =  sizeof(CutBox);

                //http://www.opengl.org/wiki/Vertex_Specification
                for ( int location = 0 ; location < 6 ; location++)
                {
                        glEnableVertexAttribArray(location);
                        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
                }

        glBindVertexArray(0);


	glBindVertexArray ( vertexArray_box );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_box );
		glBufferData ( GL_ARRAY_BUFFER , box_vertices.size( ) * sizeof ( box_vertices[0] ) , &box_vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 0 , 0 );
	glBindVertexArray ( 0 );

}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{
	boxes.clear ( );

	dynamic_ = true;
//	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

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

			if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
			{
				focus = Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f );
				box.reset();
				box.fromPointCloud ( reservoir_model_.blocks[i].vertices.begin ( ) , reservoir_model_.blocks[i].vertices.end ( ) );
				boxes.push_back( box );

			}
			else
			{
				focus = Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f );
			}

			// Cuboid
			cubeColor[index] = Eigen::Vector4f(color.x,color.y,color.z,color.w);
			cubeFocus[index] = Eigen::Vector4f(focus.x,focus.y,focus.z,focus.w);
			// Face Feature
			facesFeatureColors[index] = Eigen::Vector4f(color.x,color.y,color.z,color.w);

 			index++;
		}
		else
		{
			continue;
		}
	}

	for ( int shell_index = 0; shell_index < reservoir_model_.list_of_block_id.size() ; shell_index++ )
	{
		facesFeatureColors[shell_index] = cubeColor[reservoir_model_.list_of_block_id[shell_index]];
	}

	// Loop over the boxes
	cutVolumeGenerator();

	// Cuboid
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_color);
	glBufferData ( GL_ARRAY_BUFFER , cubeColor.size( ) * sizeof(cubeColor[0]) , &cubeColor[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_IJK);
	glBufferData ( GL_ARRAY_BUFFER , cubeIJK.size( ) * sizeof(cubeIJK[0]) , &cubeIJK[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);

        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_Focus);
        glBufferData ( GL_ARRAY_BUFFER , cubeFocus.size( ) * sizeof(cubeFocus[0]) , &cubeFocus[0] , GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0);

        // Face Features
        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceColor);
        glBufferData ( GL_ARRAY_BUFFER , facesFeatureColors.size( ) * sizeof(facesFeatureColors[0]) , &facesFeatureColors[0] , GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0);

    updateGL();

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

			cubeColor[index] = Eigen::Vector4f( color.x,color.y,color.z,color.w );
			index++;

		}
		else
		{
			continue;
		}
	}

	for ( int shell_index = 0; shell_index < reservoir_model_.list_of_block_id.size() ; shell_index++ )
	{
		facesFeatureColors[shell_index] = cubeColor[reservoir_model_.list_of_block_id[shell_index]];
	}

	// Cuboid
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_color);
	glBufferData ( GL_ARRAY_BUFFER , cubeColor.size( ) * sizeof(cubeColor[0]) , &cubeColor[0] , GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	// FaceFeature
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceColor );
	glBufferData ( GL_ARRAY_BUFFER , facesFeatureColors.size( ) * sizeof(facesFeatureColors[0]) , &facesFeatureColors[0] , GL_STATIC_DRAW );
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

			blockIndex_ = blockIndex;

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

	blockIndex_ = 0;

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

	std::cout << " Block Index " << blockIndex_  << std::endl;

	new_reservoir_file_.setHeaderData("sapphire-208000", ires::Date(ires::Date::Year(2013), ires::Date::Month(6), ires::Date::Day(10)), 0);

	bool result = new_reservoir_file_.writeFile("sapphire-208000.ires2");

	std::cout << "result" << result << std::endl;

	if (result )
	{
		new_reservoir_file_.readFile( "sapphire-208000.ires2.ires" );
	}

	std::size_t i = blockIndex_;

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
			std::cout << " Ires 1.0 " << reservoir_model_.static_porperties[j].name << " : " << reservoir_model_.static_porperties[j].values_[blockIndex_] << std::endl;
			std::cout << " Ires 2.0 " << new_names[j] <<  " : " << new_values[blockIndex_] << std::endl;

		}

	}

}

void GLWidget::openIRESCharles( const std::string& filename )
{
	//reservoir_model_.openIRES( filename );

	reservoir_model_.openIRES_Version_2( filename );

	if ( reservoir_model_.blocks.size( ) > 0 )
	{

		cuboids.clear();
		cubeColor.clear( );
		cubeIJK.clear( );
		cubeFocus.clear( );

		facesFeature.clear();

		facesFeature.resize ( reservoir_model_.list_of_block_id.size ( ) );
		facesFeatureColors.resize ( reservoir_model_.list_of_block_id.size ( ) );

		facesFeatureIJK.resize ( reservoir_model_.list_of_block_id.size ( ) );
		facesFeatureType.resize ( reservoir_model_.list_of_block_id.size ( ) );


		for ( std::size_t i = 0; i < reservoir_model_.list_of_block_id.size( ) ; i++)
		{
			facesFeature[i].vertices[0] = Eigen::Vector4f( reservoir_model_.list_of_vertex_geometry_a[i*3],
								       reservoir_model_.list_of_vertex_geometry_a[i*3+1],
								       reservoir_model_.list_of_vertex_geometry_a[i*3+2],1.0);

			facesFeature[i].vertices[1] = Eigen::Vector4f( reservoir_model_.list_of_vertex_geometry_b[i*3],
								       reservoir_model_.list_of_vertex_geometry_b[i*3+1],
								       reservoir_model_.list_of_vertex_geometry_b[i*3+2],1.0);

			facesFeature[i].vertices[2] = Eigen::Vector4f( reservoir_model_.list_of_vertex_geometry_c[i*3],
								       reservoir_model_.list_of_vertex_geometry_c[i*3+1],
								       reservoir_model_.list_of_vertex_geometry_c[i*3+2],1.0);

			facesFeature[i].vertices[3] = Eigen::Vector4f( reservoir_model_.list_of_vertex_geometry_d[i*3],
								       reservoir_model_.list_of_vertex_geometry_d[i*3+1],
								       reservoir_model_.list_of_vertex_geometry_d[i*3+2],1.0);

			facesFeatureColors[i] = Eigen::Vector4f (1.0,0.0,0.0,1.0);

			facesFeatureIJK[i] = Eigen::Vector4f (1.0,0.0,0.0,1.0);
			facesFeatureType[i] = Eigen::Vector4f ( reservoir_model_.list_of_block_flag[i],
							              reservoir_model_.list_of_block_flag[i],
							              reservoir_model_.list_of_block_flag[i], 1.0f);

		}

		std::cout << " Face Interleaved = " << reservoir_model_.list_of_block_id.size() << std::endl;

		cuboids.resize(reservoir_model_.blocks.size( ));
		cubeColor.resize( reservoir_model_.blocks.size( ) );
		cubeIJK.resize( reservoir_model_.blocks.size( ));
		cubeFocus.resize( reservoir_model_.blocks.size( ));

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

				cuboids[index].vertices[4] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[0].x,reservoir_model_.blocks[i].vertices[0].y,reservoir_model_.blocks[i].vertices[0].z,1.0f);
				cuboids[index].vertices[5] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[1].x,reservoir_model_.blocks[i].vertices[1].y,reservoir_model_.blocks[i].vertices[1].z,1.0f);
				cuboids[index].vertices[7] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[2].x,reservoir_model_.blocks[i].vertices[2].y,reservoir_model_.blocks[i].vertices[2].z,1.0f);
				cuboids[index].vertices[6] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[3].x,reservoir_model_.blocks[i].vertices[3].y,reservoir_model_.blocks[i].vertices[3].z,1.0f);

				cuboids[index].vertices[0] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[4].x,reservoir_model_.blocks[i].vertices[4].y,reservoir_model_.blocks[i].vertices[4].z,1.0f);
				cuboids[index].vertices[3] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[5].x,reservoir_model_.blocks[i].vertices[5].y,reservoir_model_.blocks[i].vertices[5].z,1.0f);
				cuboids[index].vertices[1] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[6].x,reservoir_model_.blocks[i].vertices[6].y,reservoir_model_.blocks[i].vertices[6].z,1.0f);
				cuboids[index].vertices[2] = Eigen::Vector4f(reservoir_model_.blocks[i].vertices[7].x,reservoir_model_.blocks[i].vertices[7].y,reservoir_model_.blocks[i].vertices[7].z,1.0f);

				cubeColor[index]  = Eigen::Vector4f(1.0,0.0,0.0,1.0);

				cubeIJK[index] = Eigen::Vector4f ( reservoir_model_.blocks[i].IJK[0].x , reservoir_model_.blocks[i].IJK[0].y , reservoir_model_.blocks[i].IJK[0].z , 1.0f );

				cubeFocus[index] = Eigen::Vector4f (1.0 ,0.0, 0.0 ,1.0 );

				index++;

			}
			else
			{

			}
		}

		cuboids.resize(  index  );

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

                        int size_of_vertice = sizeof(Eigen::Vector4f);
                        int size_of_struct  =  sizeof(Cuboid);

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


                glBindVertexArray(0);

                /// FacesFeatures

		glBindVertexArray ( vertexArray_faceFeature );

                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_faceFeature );
                        glBufferData ( GL_ARRAY_BUFFER , facesFeature.size( ) * sizeof(facesFeature[0]) , &facesFeature[0] , GL_STATIC_DRAW );

                        int size_of_vertice_face = sizeof(Eigen::Vector4f);
                        int size_of_struct_face  =  sizeof(FaceFeature);

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

        trackball_->useOrthographicMatrix(-1.0f, 1.0 , -1.0f, 1.0, 0.0, 500.0);

	if (depthFBO)
		delete depthFBO;

	std::cout << " FBO Resize " << width << " : "<<  height;

	depthFBO = new Framebuffer( width , height, 2 );

	meanFilter->resize(width, height);


}
/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.
/// Real Looping

void GLWidget::drawCutawaySurface ( )
{
	if ( cutVolumes.size ( ) > 0 )
	{

		glDepthFunc ( GL_GREATER );
		glClearDepth ( 0.0 );

		depthFBO->bind( );

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		BoundingBoxInitializationLCG->enable( );

		BoundingBoxInitializationLCG->setUniform( "x" , volume_width );
		BoundingBoxInitializationLCG->setUniform( "y" , volume_height );
		BoundingBoxInitializationLCG->setUniform("ModelMatrix",trackball_->getModelMatrix().data(), 4, GL_FALSE, 1);
		BoundingBoxInitializationLCG->setUniform("ViewMatrix",trackball_->getViewMatrix().data(), 4, GL_FALSE, 1);
		BoundingBoxInitializationLCG->setUniform("ProjectionMatrix", trackball_->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);

		BoundingBoxInitializationLCG->setUniform ("freeze", freezeView_ );
		BoundingBoxInitializationLCG->setUniform ("FreezeViewMatrix",freeze_viewmatrix_.data ( ),4, GL_FALSE, 1 );

		glBindVertexArray ( vertexArray_cutBox );
		glDrawArrays ( GL_POINTS , 0 , cutBoxs.size ( ) );
		glBindVertexArray ( 0 );

		BoundingBoxInitializationLCG->disable( );


		glDrawBuffer(GL_COLOR_ATTACHMENT0+1);

		meanFilter->renderTexture( depthFBO->bindAttachment(0));

		depthFBO->unbindAll();


		glDrawBuffer(GL_BACK);

	}

}

void GLWidget::drawSecundary ( )
{
	if ( enable_blend_ )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	}

	GLfloat light_elements[lights.size ( ) * 3];
	for ( int i = 0; i < lights.size ( ); ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			light_elements[i * 3 + j] = lights[i][j];
		}
	}

	shellLCG->enable( );

	shellLCG->setUniform ( "normals" , depthFBO->bindAttachment(1) );

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

	GLfloat light_elements[lights.size ( ) * 3];
	for ( int i = 0; i < lights.size ( ); ++i )
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

        	GLfloat light_elements[lights.size ( ) * 3];
        	for ( int i = 0; i < lights.size ( ); ++i )
        	{
        		for ( int j = 0; j < 3; ++j )
        		{
        			light_elements[i * 3 + j] = lights[i][j];
        		}
        	}


        	shellLCG->enable( );

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

        }

        if ( draw_primary && (cuboids.size() != 0)  )
        {
//                primary.active ( );
//
//                glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//                glUniform2f ( primary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//
//                //glUniformMatrix4fv ( primary.uniforms_["ModelMatrix"].location , 1 , GL_FALSE , trackball_->getModelMatrix().data() );
//                glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_FALSE , trackball_->getViewMatrix().data() );
//                glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_FALSE , trackball_->getProjectionMatrix().data() );
//
//                //glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//                //glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//                glBindVertexArray ( vertexArray_cube_interleaved );
//                glDrawArrays ( GL_POINTS , 0 , cube_interleaved.size() );
//                glBindVertexArray ( 0 );
//
//                primary.deactive ( );

                // shell

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

		GLfloat light_elements[lights.size ( ) * 3];
		for ( int i = 0; i < lights.size ( ); ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				light_elements[i * 3 + j] = lights[i][j];
			}
		}

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

	// Celer


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
