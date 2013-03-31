#include <QtGui/QtGui>

#include <limits>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iterator>

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

	JumpFloodingStep = 0;
	VertexShaderStep = 0;
	FragmentShaderStep = 0;
	
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
	fboInitialization = new  QGLFramebufferObject ( width() , height() , format );

	glGenVertexArrays ( 1 , &vertexArray);
		glGenBuffers ( 1, &reservoir_vertices_buffer );
		glGenBuffers ( 1, &reservoir_normal_buffer );
		glGenBuffers ( 1, &reservoir_color_buffer );
		glGenBuffers ( 1, &reservoir_renderFlag_buffer );
		glGenBuffers ( 1, &reservoir_indices_buffer );

		glGenBuffers ( 1, &screen_buffer);
		glGenBuffers ( 1, &texture_buffer);
	glBindVertexArray(vertexArray);

	glGenVertexArrays ( 1 , &vertexArrayScreen );


	reservoir_vertices_location 	= 1;
	reservoir_normal_location 	= 2;
	reservoir_color_location 	= 3;
	reservoir_renderFlag_location 	= 4;


	draw_secondary = 1;
	draw_primary = 0;



}

bool GLWidget::isIresWasOpenedSucessufully ( ) const
{
	return ires_has_been_open_sucessefully;
}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{


	reservoir_list_of_colors.clear ( );
	reservoir_list_of_renderFlag.clear ( );
	reservoir_list_of_renderFlag.clear ( );

	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::size_t i = 0;

	std::vector<Celer::Vector4<GLfloat> > colors ( 24 );
	std::vector<Celer::Vector4<GLfloat> > renderFlags ( 24 );

	Celer::Vector4<GLfloat> color;

	while ( i < reservoir_model_.blocks.size ( ) )
	{
		if ( reservoir_model_.blocks[i] != -1 )
		{

			float regularValue = ( reservoir_model_.static_porperties[property_index].values_[i / 8] );

			float normalizedColor = ( regularValue - min ) / ( max - min );

			if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
			{
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

				colors 		= std::vector<Celer::Vector4<GLfloat> > ( 24 , color );
				renderFlags 	= std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 0.0f , 1.0f , 0.0f , 1.0f ));
			}else
			{

				colors 		= std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 1.0f , 0.0f , 0.0f , 1.0f ) );
				renderFlags 	= std::vector<Celer::Vector4<GLfloat> > ( 24 , Celer::Vector4<GLfloat> ( 1.0f , 1.0f , 0.0f , 1.0f ) );
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

	while ( i < reservoir_model_.blocks.size( ) )
	{
		if ( reservoir_model_.blocks[i] != -1 )
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

void GLWidget::openIRES ( const std::string& filename )
{

	reservoir_model_.openIRES(filename);

	if ( reservoir_model_.blocks.size ( ) > 0 )
	{
		ires_has_been_open_sucessefully = 1;

		reservoir_list_of_indices.clear ( );
		reservoir_list_of_vertices.clear ( );
		reservoir_list_of_normals.clear ( );
		reservoir_list_of_colors.clear ( );
		reservoir_list_of_renderFlag.clear( );


		box = Celer::BoundingBox3<double> ( );

		box.fromPointCloud ( reservoir_model_.vertices.begin ( ) , reservoir_model_.vertices.end ( ) );

		for ( int i = 0; i < reservoir_model_.vertices.size ( ) ; i++)
		{
			reservoir_model_.vertices[i] -= box.center();
			reservoir_model_.vertices[i].x /= box.diagonal();
			reservoir_model_.vertices[i].y /= box.diagonal();
			reservoir_model_.vertices[i].z /= box.diagonal();
			//std::cout << " Vector " <<  reservoir_model_.vertices[i] << std::endl;
		}

		box.fromPointCloud ( reservoir_model_.vertices.begin ( ) , reservoir_model_.vertices.end ( ) );

		std::size_t i = 0;

		while ( i <  reservoir_model_.blocks.size ( ))
		{
			if ( reservoir_model_.blocks[i] != -1)
			{
				int index [] =
				{
				    // Top Face
				    reservoir_model_.blocks[i+0],reservoir_model_.blocks[i+1],reservoir_model_.blocks[i+3],reservoir_model_.blocks[i+2],/* 0 - 5*/
				    // Bottom Face
				    reservoir_model_.blocks[i+4],reservoir_model_.blocks[i+7],reservoir_model_.blocks[i+5],reservoir_model_.blocks[i+6],/* 6 - 11 */
				    // Front Face
				    reservoir_model_.blocks[i+0],reservoir_model_.blocks[i+7],reservoir_model_.blocks[i+3],reservoir_model_.blocks[i+4],/* 12 - 17*/
				    // Back Face
				    reservoir_model_.blocks[i+1],reservoir_model_.blocks[i+2],reservoir_model_.blocks[i+6],reservoir_model_.blocks[i+5],/* 18 - 23*/
				    // Right Face
				    reservoir_model_.blocks[i+2],reservoir_model_.blocks[i+3],reservoir_model_.blocks[i+5],reservoir_model_.blocks[i+4],/* 24 - 29*/
				    // Left Face
				    reservoir_model_.blocks[i+0],reservoir_model_.blocks[i+1],reservoir_model_.blocks[i+7],reservoir_model_.blocks[i+6] /* 30 - 35*/
				};


				Celer::Vector4<GLfloat> vertices [] =
				{
				    // Top Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[0]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[0]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[1]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[1]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[2]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[2]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[3]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[3]].z), 1.0f),
				    // Bottom Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[4]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[4]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[5]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[5]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[6]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[6]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[7]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[7]].z), 1.0f),
				    // Front Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[8]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[8]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[8]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[9]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[9]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[9]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[10]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[10]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[10]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[11]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[11]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[11]].z), 1.0f),
				    // Back Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[12]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[12]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[12]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[13]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[13]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[13]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[14]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[14]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[14]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[15]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[15]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[15]].z), 1.0f),
				    // Right Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[16]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[16]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[16]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[17]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[17]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[17]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[18]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[18]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[18]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[19]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[19]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[19]].z), 1.0f),
				    // Left Face
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[20]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[20]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[20]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[21]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[21]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[21]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[22]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[22]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[22]].z), 1.0f),
				    Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[index[23]].x), static_cast<GLfloat>(reservoir_model_.vertices[index[23]].y), static_cast<GLfloat>(reservoir_model_.vertices[index[23]].z), 1.0f)
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


				std::copy ( index 	, index    + 24 , std::back_inserter ( reservoir_list_of_indices   ) );
				std::copy ( vertices 	, vertices + 24 , std::back_inserter ( reservoir_list_of_vertices  ) );
				std::copy ( focus 	, focus    + 24 , std::back_inserter ( reservoir_list_of_renderFlag) );

				i += 8;

			}  // end of looping list of blocks
			else
			{
				i += 1;
			}
		}


		camera_.setPosition ( box.center ( ) );
		camera_.setTarget ( box.center ( ) );
		std::cout << box.diagonal ( );
		camera_.setOffset ( 3.0 * box.diagonal ( ) );

		camera_.setPerspectiveProjectionMatrix ( 60 , camera_.aspectRatio ( ) , 1.0 , 1000.0 * box.diagonal ( ) );

		std::cout << camera_.position ( );

		camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );

		cameraStep_ = 10.0f;


		glBindVertexArray(vertexArray);


		glBindBuffer ( GL_ARRAY_BUFFER , reservoir_vertices_buffer );
		glBufferData ( GL_ARRAY_BUFFER , reservoir_list_of_vertices.size ( ) * sizeof ( reservoir_list_of_vertices[0] ) , &reservoir_list_of_vertices[0] , GL_STATIC_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( reservoir_vertices_location );
		glVertexAttribPointer ( reservoir_vertices_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

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

		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , reservoir_indices_buffer );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER , reservoir_list_of_indices.size ( ) * sizeof ( reservoir_list_of_indices[0] ) , &reservoir_list_of_indices[0] , GL_STATIC_DRAW );

		glBindVertexArray(0);

		glBindVertexArray(vertexArrayScreen);


		GLfloat openGLScreenCoordinates[] =
		{
	                      0.0f, (float) height() ,
	          (float) width() , (float) height() ,
	          (float) width() , 0.0f,

		  (float) width() , 0.0f,
		               0.0f, 0.0f,
		               0.0f, (float) height()  };

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

void GLWidget::resizeGL ( int width , int height )
{
	glViewport ( 0 , 0 , width , height );
	camera_.setWindowSize ( width , height );


	camera_.setAspectRatio ( width  , height  );
	camera_.setPerspectiveProjectionMatrix ( 0 , camera_.aspectRatio ( ) , 1.0 , 1000.0*box.diagonal() );
	camera_.setOrthographicProjectionMatrix( 0.0f, (float)width , 0.0f, (float)height, -1.0, 1.0 );


	centerX_ = static_cast<float> ( width * 0.5 );
	centerY_ = static_cast<float> ( height * 0.5 );



	if ( fboStep[0] )
		delete fboStep[0];

	if ( fboStep[1] )
		delete fboStep[1];

	if ( fboInitialization )
		delete fboInitialization;

	QGLFramebufferObjectFormat format;
	//format.setAttachment(QGLFramebufferObject::Depth);
	format.setTextureTarget ( GL_TEXTURE_RECTANGLE );
	format.setInternalTextureFormat ( GL_RGBA32F );

	fboStep[0] = new QGLFramebufferObject ( width , height , format );
	fboStep[1] = new QGLFramebufferObject ( width , height , format );
	fboInitialization = new QGLFramebufferObject ( width , height , format );


	glBindVertexArray(vertexArrayScreen);


	GLfloat openGLScreenCoordinates[] =
	{
                       0.0f, (float) height ,
          (float) width , (float) height ,
          (float) width , 0.0f,

	  (float) width , 0.0f,
	               0.0f, 0.0f,
	               0.0f, (float) height  };

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

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
	//TridimensionalSetUp ( );
	cutawaySetup ( );

}

void GLWidget::cutawaySetup ( )
{

	int i = 1;


	camera_.setViewByMouse ( );

	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}

	camera_.computerViewMatrix ( );

	camera_.setPerspectiveProjectionMatrix ( zoom_angle_ , camera_.aspectRatio ( ) , 1.0 , 1000.0 * box.diagonal ( ) );


	if ( ires_has_been_open_sucessefully  )
	{

 		jumpFloodInitialization.active ( );

 		fboStep[1]->bind();

		glClearColor ( 0.0 , 0.0 , 0.0 , 0.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//glUniform3fv( jumpFloodInitialization.uniforms_["lightDirection"].location,0,camera_.position());

		glUniformMatrix4fv ( jumpFloodInitialization.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
		glUniformMatrix4fv ( jumpFloodInitialization.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix()  );

		//VAO
		glBindVertexArray(vertexArray);
		glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
		glBindVertexArray(0);

		fboStep[1]->release( );

		jumpFloodInitialization.deactive ( );


		// Do Jumping Flooding Algorithm
		int stepSize = ( width ( ) > height ( ) ? width ( ) : height ( ) ) * 0.5;
		bool ExitLoop = 0;


		jumpFloodingStep.active( );


		glUniform2f ( jumpFloodingStep.uniforms_["viewport"].location, (float)width ( ) , (float)height ( ) );

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


			glUniform1f ( jumpFloodingStep.uniforms_["pm_sz"].location,pm_sz );
			glUniform1f ( jumpFloodingStep.uniforms_["theta"].location,angle );
			glUniform1i ( jumpFloodingStep.uniforms_["stepSize"].location,stepSize );
			glUniformMatrix4fv ( jumpFloodingStep.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix() );

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

		jumpFloodingStep.deactive( ) ;


		if ( draw_secondary && (reservoir_list_of_vertices.size ( ) != 0) )
		{
			cutawayWireframe.active ( );

			glClearColor ( 1.0 , 1.0 , 1.0 , 1.0 );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glActiveTexture ( GL_TEXTURE0 );
			glEnable ( GL_TEXTURE_RECTANGLE );
			glBindTexture ( GL_TEXTURE_RECTANGLE , fboStep[i]->texture ( ) );

			glUniform3fv ( cutawayWireframe.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( cutawayWireframe.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
			glUniform1i ( cutawayWireframe.uniforms_["cutaway"].location , 1 );
			glUniformMatrix4fv ( cutawayWireframe.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( cutawayWireframe.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			//VAO
			glBindVertexArray(vertexArray);
			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			glBindVertexArray(0);
			glDisable ( GL_TEXTURE_RECTANGLE );

			cutawayWireframe.deactive ( );
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

		cutawayWireframe.active ( );

		glUniform3fv ( cutawayWireframe.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

		glUniform2f ( cutawayWireframe.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
		glUniform1i ( cutawayWireframe.uniforms_["cutaway"].location , 0 );

		glUniformMatrix4fv ( cutawayWireframe.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
		glUniformMatrix4fv ( cutawayWireframe.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

		glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());

		cutawayWireframe.deactive ( );

	}

}


void GLWidget::TridimensionalSetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	camera_.setViewByMouse ( );

	if ( buttonRelease_ )
	{
		processMultiKeys ( );
	}

	camera_.computerViewMatrix( );

	camera_.setPerspectiveProjectionMatrix ( zoom_angle_ , camera_.aspectRatio ( ) , 1.0 , 1000.0 * box.diagonal ( ) );


 	if ( ires_has_been_open_sucessefully )
	{

//		if ( draw_secondary )
//		{
//
 			secondary.active ( );

//			glUniform3fv ( cutawayWireframe.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//			glUniform2f ( cutawayWireframe.uniforms_["WIN_SCALE"].location , (float)width(), (float)height() );

			glUniform3fv ( secondary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );

			glUniform2f ( secondary.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//
			glUniformMatrix4fv ( secondary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( secondary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reservoir_indices_buffer);
//			glDrawElements( GL_LINES_ADJACENCY , reservoir_list_of_indices.size(), GL_UNSIGNED_INT,0 );

			glDrawArrays ( GL_LINES_ADJACENCY , 0 , reservoir_list_of_vertices.size());
			//glDrawArrays( GL_TRIANGLES , 0, primary_list_of_vertices.size());



			secondary.deactive ( );
//
//		}
//		if ( draw_primary )
//		{
//
//			primary.active ( );
//
//			glUniform3fv ( primary.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//			glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//			glDrawArrays ( GL_TRIANGLES , 0 , primary_list_of_vertices.size ( ) );
//
//			primary.deactive ( );
//
//		}
		
	}

}

void GLWidget::TwodimensionalSetUp ( )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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

	textureViewer.create((shadersDir.path ()+"/share/Shaders/fboTest.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/fboTest.frag").toStdString());

	primary.create((shadersDir.path ()+"/share/Shaders/Primary.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/Primary.geom").toStdString(),(shadersDir.path ()+"/share/Shaders/Primary.frag").toStdString());

	secondary.create((shadersDir.path ()+"/share/Shaders/Secondary.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/Secondary.geom").toStdString(),(shadersDir.path ()+"/share/Shaders/Secondary.frag").toStdString());

	cutaway.create((shadersDir.path ()+"/share/Shaders/Cutaway.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/Cutaway.frag").toStdString());

	cutawayWireframe.create((shadersDir.path ()+"/share/Shaders/CutawayWireframe.vert").toStdString(), (shadersDir.path ()+"/share/Shaders/CutawayWireframe.geom").toStdString(),(shadersDir.path ()+"/share/Shaders/CutawayWireframe.frag").toStdString());

	jumpFloodInitialization.create( (shadersDir.path ()+"/share/Shaders/JFAInitializing.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/JFAInitializing.frag").toStdString());

	jumpFloodingStep.create( (shadersDir.path ()+"/share/Shaders/JFAStep430.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/JFAStep430.frag").toStdString());

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

	if ( ( QApplication::keyboardModifiers ( ) == Qt::ShiftModifier ) && ( event->key ( ) == Qt::Key_A ) )
	{
		drawPrimary();
	}

	if ( ( QApplication::keyboardModifiers ( ) == Qt::ShiftModifier ) && ( event->key ( ) == Qt::Key_S ) )
	{
		drawSecondary();
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

		/*! Três coisas :
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

void GLWidget::drawPrimary(  )
{
	if ( reservoir_list_of_vertices.size( ) > 0 )
		draw_primary = !draw_primary;
}

void GLWidget::drawSecondary( )
{
	if ( reservoir_list_of_vertices.size( ) > 0 )
		draw_secondary = !draw_secondary;
}

