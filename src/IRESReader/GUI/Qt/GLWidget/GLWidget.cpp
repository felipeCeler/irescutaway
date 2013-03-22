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
		glGenBuffers ( 1, &primary_vertices_buffer );
		glGenBuffers ( 1, &primary_normal_buffer );
		glGenBuffers ( 1, &primary_color_buffer );
		glGenBuffers ( 1, &primary_renderFlag_buffer);
		glGenBuffers ( 1, &primary_indices_buffer);

		glGenBuffers ( 1, &secondary_vertices_buffer );
		glGenBuffers ( 1, &secondary_normal_buffer );
		glGenBuffers ( 1, &secondary_color_buffer );
		glGenBuffers ( 1, &secondary_renderFlag_buffer);
		glGenBuffers ( 1, &secondary_indices_buffer);

		glGenBuffers ( 1, &screen_buffer);
		glGenBuffers ( 1, &texture_buffer);
	glBindVertexArray(vertexArray);

	primary_vertices_location = 0;
	primary_normal_location = 1;
	primary_color_location = 2;
	primary_renderFlag_location = 8;

	secondary_vertices_location = 3;
	secondary_normal_location = 4;
	secondary_color_location = 5;
	secondary_renderFlag_location = 9;


	draw_secondary = 1;
	draw_primary = 0;



}

bool GLWidget::isIresWasOpenedSucessufully ( ) const
{
	return ires_has_been_open_sucessefully;
}

void GLWidget::changePropertyRange ( const double& minRange, const double& maxRange, int property_index )
{
//
//	primary_list_of_vertices.clear ( );
//	primary_list_of_normals.clear ( );
//	primary_list_of_colors.clear ( );
//	primary_list_of_indices.clear ( );
//
//	secondary_list_of_renderFlag.clear();
//
//
//	std::cout << "Changing the property to : " << ires_cornerPoint_test_.static_porperties[property_index].name << std::endl;
//
//	float min = *std::min_element ( ires_cornerPoint_test_.static_porperties[property_index].values_.begin ( ) , ires_cornerPoint_test_.static_porperties[property_index].values_.end ( ) );
//	float max = *std::max_element ( ires_cornerPoint_test_.static_porperties[property_index].values_.begin ( ) , ires_cornerPoint_test_.static_porperties[property_index].values_.end ( ) );
//
//
//
//	for ( int i = 0; i < ires_cornerPoint_test_.blocks.size ( ); i+=8 )
//	{
//
//		float regularValue  = ( ires_cornerPoint_test_.static_porperties[property_index].values_[i/8] );
//
//		float normalizedColor = ( regularValue - min ) / ( max - min );
//
//		Celer::Vector4<GLfloat> color;
//
//
//
//		if ( ( regularValue >= minRange ) && ( regularValue <= maxRange ) )
//		{
//			switch ( (int) ( normalizedColor * 10.0f ) )
//			{
//				case 10:
//					color = Celer::Vector4<GLfloat> ( 1.0f , 0.0f , 0.0f , 1.0f );
//					break;
//				case 9:
//					color = Celer::Vector4<GLfloat> ( 0.5f , 0.25f , 0.0f , 1.0f );
//					break;
//				case 8:
//					color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.0f , 1.0f );
//					break;
//				case 7:
//					color = Celer::Vector4<GLfloat> ( 0.25f , 0.5f , 0.0f , 1.0f );
//					break;
//				case 6:
//					color = Celer::Vector4<GLfloat> ( 0.0f , 0.25f , 0.0f , 1.0f );
//					break;
//				case 5:
//					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.2f , 1.0f );
//					break;
//				case 4:
//					color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.4f , 1.0f );
//					break;
//				case 3:
//					color = Celer::Vector4<GLfloat> ( 0 , 0.4 , 0.5 , 1.0f );
//					break;
//				case 2:
//					color = Celer::Vector4<GLfloat> ( 0 , 0.2 , 0.5 , 1.0f );
//					break;
//				case 1:
//					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.5 , 1.0f );
//					break;
//				case 0:
//					color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.375 , 1.0f );
//					break;
//					//if value is equal to max
//				default:
//					color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.5f , 1.0f );
//					break;
//			}
//
//
//			int index [] =
//			{
//			    // Top Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+2],/* 0 - 5*/
//			    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+0],
//			    // Bottom Face
//			    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+6],/* 6 - 11 */
//			    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+4],
//			    // Front Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+7],/* 12 - 17*/
//			    ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],
//			    // Back Face
//			    ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+5],/* 18 - 23*/
//			    ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+1],
//			    // Right Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+6],/*24 - 29*/
//			    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+0],
//			    // Left Face
//			    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],/*30 - 35*/
//			    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+2],
//			};
//
//			Celer::Vector3<double> vertices [] =
//			{
//			    // Top Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[0]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[1]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[2]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[3]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[4]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[5]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[6]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[7]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[8]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[9]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[10]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[11]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[12]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[13]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[14]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[15]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[16]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[17]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[18]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[19]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[20]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[21]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[22]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[23]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[24]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[25]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[26]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[27]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[28]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[29]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[30]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[31]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[32]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[33]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[34]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[35]])
//
//		       };
//
//			Celer::Vector3<double> top_face_normal 	  = ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[1]] ) ^ ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[2]] );
//			top_face_normal.normalize( );
//			Celer::Vector3<double> bottom_face_normal = ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[7]] ) ^ ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[8]] );
//			bottom_face_normal.normalize();
//			Celer::Vector3<double> front_face_normal  = ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[13]] ) ^ ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[14]] );
//			front_face_normal.normalize();
//			Celer::Vector3<double> back_face_normal   = ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[19]] ) ^ ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[20]] );
//			back_face_normal.normalize( );
//			Celer::Vector3<double> right_face_normal  = ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[25]] ) ^ ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[26]] );
//			right_face_normal.normalize( );
//			Celer::Vector3<double> left_face_normal   = ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[31]] ) ^ ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[32]] );
//			left_face_normal.normalize( );
//
//			Celer::Vector3<double> normals [] =
//			{
//			    // Top Face
//		            top_face_normal,
//		            top_face_normal,
//		            top_face_normal,
//		            top_face_normal,
//		            top_face_normal,
//		            top_face_normal,
//			    // Bottom Face
//		            bottom_face_normal,
//		            bottom_face_normal,
//		            bottom_face_normal,
//		            bottom_face_normal,
//		            bottom_face_normal,
//		            bottom_face_normal,
//			    // Front Face
//		            front_face_normal,
//		            front_face_normal,
//		            front_face_normal,
//		            front_face_normal,
//		            front_face_normal,
//		            front_face_normal,
//			    // Back Face
//		            back_face_normal,
//		            back_face_normal,
//		            back_face_normal,
//		            back_face_normal,
//		            back_face_normal,
//		            back_face_normal,
//			    // Right Face
//		            right_face_normal,
//		            right_face_normal,
//		            right_face_normal,
//		            right_face_normal,
//		            right_face_normal,
//		            right_face_normal,
//			    // Left Face
//		            left_face_normal,
//		            left_face_normal,
//		            left_face_normal,
//		            left_face_normal,
//		            left_face_normal,
//		            left_face_normal
//
//		       };
//
//
//
//			Celer::Vector4<GLfloat> colors [] =
//			{
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//
//			    color,
//			    color,
//			    color,
//			    color,
//			    color,
//			    color
//
//			 };
//
//
//			GLfloat renderFlags [] =
//			{
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f,
//			    0.0f
//
//			 };
//
//			std::copy ( index , index + 36 , std::back_inserter ( primary_list_of_indices ) );
//			std::copy ( vertices , vertices + 36 , std::back_inserter ( primary_list_of_vertices ) );
//			std::copy ( normals , normals + 36 , std::back_inserter ( primary_list_of_normals ) );
//			std::copy ( colors , colors + 36 , std::back_inserter ( primary_list_of_colors ) );
//			std::copy ( renderFlags , renderFlags + 36 , std::back_inserter ( secondary_list_of_renderFlag ) );
//
//		}
//		else
//		{
//			color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.5f , 1.0f );
//
//			GLfloat renderFlags [] =
//			{
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f,
//			    1.0f
//
//			 };
//
//			std::copy ( renderFlags , renderFlags + 36 , std::back_inserter ( secondary_list_of_renderFlag ) );
//		}
//
//
//	}
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_vertices_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , primary_list_of_vertices.size ( ) * sizeof ( primary_list_of_vertices[0] ) , &primary_list_of_vertices[0] , GL_STATIC_DRAW );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_normal_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , primary_list_of_normals.size ( ) * sizeof ( primary_list_of_normals[0] ) , &primary_list_of_normals[0] , GL_STATIC_DRAW );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_color_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , primary_list_of_colors.size ( ) * sizeof ( primary_list_of_colors[0] ) , &primary_list_of_colors[0] , GL_STATIC_DRAW );
//
//	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , primary_indices_buffer );
//	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , primary_list_of_indices.size ( ) * sizeof ( primary_list_of_indices[0] ) , &primary_list_of_indices[0] , GL_STATIC_DRAW );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_renderFlag_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_renderFlag.size ( ) * sizeof ( secondary_list_of_renderFlag[0] ) , &secondary_list_of_renderFlag[0] , GL_STATIC_DRAW );
//
//	draw_primary = 1;

}

void GLWidget::changeProperty ( int property_index )
{


	std::cout << "Changing the property to : " << reservoir_model_.static_porperties[property_index].name << std::endl;

	float min = *std::min_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );
	float max = *std::max_element ( reservoir_model_.static_porperties[property_index].values_.begin ( ) , reservoir_model_.static_porperties[property_index].values_.end ( ) );

	std::cout << "Property Max : " << min << std::endl;
	std::cout << "Property Min : " << max << std::endl;

	std::size_t i = 0;

	while ( i < reservoir_model_.blocks.size())
	{
		if ( reservoir_model_.blocks[i] != -1)
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


			secondary_list_of_colors[reservoir_model_.blocks[i+0]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+1]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+2]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+3]] = color;

			secondary_list_of_colors[reservoir_model_.blocks[i+4]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+5]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+6]] = color;
			secondary_list_of_colors[reservoir_model_.blocks[i+7]] = color;

			i += 8;

		}
		else
		{
			i += 1;
		}
	}

	glBindBuffer ( GL_ARRAY_BUFFER , secondary_color_buffer );
	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_colors.size ( ) * sizeof ( secondary_list_of_colors[0] ) , &secondary_list_of_colors[0] , GL_STREAM_DRAW );
	// Vertex Array : Set up generic attributes pointers



//	secondary_list_of_colors.clear();
//
//	std::cout << "Changing the property to : " << ires_cornerPoint_test_.static_porperties[property_index].name << std::endl;
//
//	float min = *std::min_element ( ires_cornerPoint_test_.static_porperties[property_index].values_.begin ( ) , ires_cornerPoint_test_.static_porperties[property_index].values_.end ( ) );
//	float max = *std::max_element ( ires_cornerPoint_test_.static_porperties[property_index].values_.begin ( ) , ires_cornerPoint_test_.static_porperties[property_index].values_.end ( ) );
//
//	for ( int i = 0; i < ires_cornerPoint_test_.blocks.size ( ); i+=8 )
//	{
//
//		float normalizedColor = ( ires_cornerPoint_test_.static_porperties[property_index].values_[i/8] - min ) / ( max - min );
//
//		Celer::Vector4<GLfloat> color;
//
//		switch ( (int) ( normalizedColor * 10.0f ) )
//		{
//			case 10:
//				color = Celer::Vector4<GLfloat> ( 1.0f , 0.0f , 0.0f , 1.0f );
//				break;
//			case 9:
//				color = Celer::Vector4<GLfloat> ( 0.5f , 0.25f , 0.0f , 1.0f );
//				break;
//			case 8:
//				color = Celer::Vector4<GLfloat> ( 0.5f , 0.5f , 0.0f , 1.0f );
//				break;
//			case 7:
//				color = Celer::Vector4<GLfloat> ( 0.25f , 0.5f , 0.0f , 1.0f );
//				break;
//			case 6:
//				color = Celer::Vector4<GLfloat> ( 0.0f , 0.25f , 0.0f , 1.0f );
//				break;
//			case 5:
//				color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.2f , 1.0f );
//				break;
//			case 4:
//				color = Celer::Vector4<GLfloat> ( 0.0f , 0.5f , 0.4f , 1.0f );
//				break;
//			case 3:
//				color = Celer::Vector4<GLfloat> ( 0 , 0.4 , 0.5 , 1.0f );
//				break;
//			case 2:
//				color = Celer::Vector4<GLfloat> ( 0 , 0.2 , 0.5 , 1.0f );
//				break;
//			case 1:
//				color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.5 , 1.0f );
//				break;
//			case 0:
//				color = Celer::Vector4<GLfloat> ( 0 , 0 , 0.375 , 1.0f );
//				break;
//				//if value is equal to max
//			default:
//				color = Celer::Vector4<GLfloat> ( 0.5f , 0.0f , 0.0f , 1.0f );
//				break;
//		}
//
//
//
//		Celer::Vector4<GLfloat> colors [] =
//		{
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//
//		    color,
//		    color,
//		    color,
//		    color,
//		    color,
//		    color
//
//		 };
//
//		std::copy ( colors , colors + 36 , std::back_inserter ( secondary_list_of_colors ) );
//	}
//
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_color_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_colors.size ( ) * sizeof ( secondary_list_of_colors[0] ) , &secondary_list_of_colors[0] , GL_STATIC_DRAW );

}

void GLWidget::openIRES ( const std::string& filename )
{

	reservoir_model_.openIRES(filename);

	if ( reservoir_model_.blocks.size ( ) > 0 )
	{
		ires_has_been_open_sucessefully = 1;

		secondary_list_of_indices.clear ( );
		secondary_list_of_vertices.clear ( );
		secondary_list_of_normals.clear ( );
		secondary_list_of_colors.clear ( );
		secondary_list_of_renderFlag.clear( );

		secondary_list_of_vertices.resize( reservoir_model_.vertices.size() );
		secondary_list_of_normals.resize( reservoir_model_.vertices.size() );
		secondary_list_of_colors.resize( reservoir_model_.vertices.size() );

		box = Celer::BoundingBox3<double> ( );

		box.fromPointCloud ( reservoir_model_.vertices.begin ( ) , reservoir_model_.vertices.begin ( ) + 8 );

		for ( int i = 0; i < 8 ; i++)
		{
			reservoir_model_.vertices[i] -= box.center();
			reservoir_model_.vertices[i].x /= box.diagonal();
			reservoir_model_.vertices[i].y /= box.diagonal();
			reservoir_model_.vertices[i].z /= box.diagonal();

//			secondary_list_of_vertices[i] = Celer::Vector4<GLfloat> ( static_cast<GLfloat>(reservoir_model_.vertices[i].x),
//									          static_cast<GLfloat>(reservoir_model_.vertices[i].y),
//									          static_cast<GLfloat>(reservoir_model_.vertices[i].z),
//									           1.0f);
			secondary_list_of_colors[i] = Celer::Vector4<GLfloat> ( 1.0f, 1.0f, 0.0f, 1.0f);
		}

		box.fromPointCloud ( reservoir_model_.vertices.begin ( ) , reservoir_model_.vertices.begin ( ) + 8 );

		std::size_t i = 0;

		while ( i < 8)
		{
			if ( reservoir_model_.blocks[i] != -1)
			{
				int index []
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


				Celer::Vector4<GLfloat> vertices []
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

				std::copy ( index 	, index + 24 , std::back_inserter ( secondary_list_of_indices ) );
				std::copy ( vertices 	, vertices + 24 , std::back_inserter ( secondary_list_of_vertices) );
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

		glBindBuffer ( GL_ARRAY_BUFFER , secondary_vertices_buffer );
		glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_vertices.size ( ) * sizeof ( secondary_list_of_vertices[0] ) , &secondary_list_of_vertices[0] , GL_STATIC_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( secondary_vertices_location );
		glVertexAttribPointer ( secondary_vertices_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ARRAY_BUFFER , secondary_color_buffer );
		glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_colors.size ( ) * sizeof ( secondary_list_of_colors[0] ) , &secondary_list_of_colors[0] , GL_STREAM_DRAW );
		// Vertex Array : Set up generic attributes pointers
		glEnableVertexAttribArray ( secondary_color_location );
		glVertexAttribPointer ( secondary_color_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , secondary_indices_buffer );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER , secondary_list_of_indices.size ( ) * sizeof ( secondary_list_of_indices[0] ) , &secondary_list_of_indices[0] , GL_STATIC_DRAW );

		//changeProperty(0);


	}
	else
	{
		ires_has_been_open_sucessefully = 0;
	}


}

//void GLWidget::openIRES ( const std::string& filename )
//{
//
//	ires_cornerPoint_test_.openIRES( filename );
//
//
//	if ( ires_cornerPoint_test_.blocks.size ( ) > 0 )
//	{
//		ires_has_been_open_sucessefully = 1;
//	}
//	else
//	{
//		ires_has_been_open_sucessefully = 0;
//	}
//
//	std::cout << "Vertices       :" << ires_cornerPoint_test_.vertices.size ( ) << std::endl;
//	std::cout << "Blocks Indices :" << ires_cornerPoint_test_.blocks.size ( ) << std::endl;
//
//	secondary_list_of_indices.clear ( );
//	secondary_list_of_vertices.clear ( );
//	secondary_list_of_normals.clear ( );
//	secondary_list_of_colors.clear ( );
//	secondary_list_of_renderFlag.clear( );
//
//	for ( int i = 0; i < ires_cornerPoint_test_.blocks.size( ); i+=8 )
//	{
//		if ( ires_cornerPoint_test_.blocks[i] != -1)
//		{
//			int index [] =
//			{
//			    // Top Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+2],/* 0 - 5*/
//			    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+0],
//			    // Bottom Face
//			    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+6],/* 6 - 11 */
//			    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+4],
//			    // Front Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+7],/* 12 - 17*/
//			    ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],
//			    // Back Face
//			    ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+5],/* 18 - 23*/
//			    ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+1],
//			    // Right Face
//			    ires_cornerPoint_test_.blocks[i+0],ires_cornerPoint_test_.blocks[i+1],ires_cornerPoint_test_.blocks[i+6],/*24 - 29*/
//			    ires_cornerPoint_test_.blocks[i+6],ires_cornerPoint_test_.blocks[i+7],ires_cornerPoint_test_.blocks[i+0],
//			    // Left Face
//			    ires_cornerPoint_test_.blocks[i+2],ires_cornerPoint_test_.blocks[i+3],ires_cornerPoint_test_.blocks[i+4],/*30 - 35*/
//			    ires_cornerPoint_test_.blocks[i+4],ires_cornerPoint_test_.blocks[i+5],ires_cornerPoint_test_.blocks[i+2],
//			};
//
//			Celer::Vector3<double> vertices [] =
//			{
//			    // Top Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[0]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[1]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[2]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[3]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[4]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[5]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[6]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[7]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[8]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[9]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[10]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[11]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[12]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[13]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[14]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[15]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[16]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[17]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[18]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[19]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[20]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[21]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[22]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[23]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[24]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[25]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[26]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[27]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[28]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[29]]),
//			    // Bottom Face
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[30]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[31]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[32]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[33]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[34]]),
//			    Celer::Vector3<double> ( ires_cornerPoint_test_.vertices[index[35]])
//
//		       };
//
//			Celer::Vector3<double> top_face_normal = ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[1]] ) ^ ( ires_cornerPoint_test_.vertices[index[0]] - ires_cornerPoint_test_.vertices[index[2]] );
//			top_face_normal.normalize ( );
//
//			Celer::Vector3<double> bottom_face_normal = ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[7]] )  ^ ( ires_cornerPoint_test_.vertices[index[6]] - ires_cornerPoint_test_.vertices[index[8]] );
//			bottom_face_normal.normalize ( );
//
//			Celer::Vector3<double> front_face_normal = ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[13]] )  ^ ( ires_cornerPoint_test_.vertices[index[12]] - ires_cornerPoint_test_.vertices[index[14]] );
//			front_face_normal.normalize ( );
//
//			Celer::Vector3<double> back_face_normal = ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[19]] ) ^ ( ires_cornerPoint_test_.vertices[index[18]] - ires_cornerPoint_test_.vertices[index[20]] );
//			back_face_normal.normalize ( );
//
//			Celer::Vector3<double> right_face_normal = ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[25]] ) ^ ( ires_cornerPoint_test_.vertices[index[24]] - ires_cornerPoint_test_.vertices[index[26]] );
//			right_face_normal.normalize ( );
//
//			Celer::Vector3<double> left_face_normal = ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[31]] ) ^ ( ires_cornerPoint_test_.vertices[index[30]] - ires_cornerPoint_test_.vertices[index[32]] );
//			left_face_normal.normalize ( );
//
//			Celer::Vector3<double> normals [] =
//			{
//			    // Top Face
//			    top_face_normal,
//			    top_face_normal,
//			    top_face_normal,
//			    top_face_normal,
//			    top_face_normal,
//			    top_face_normal,
//			    // Bottom Face
//			    bottom_face_normal,
//			    bottom_face_normal,
//			    bottom_face_normal,
//			    bottom_face_normal,
//			    bottom_face_normal,
//			    bottom_face_normal,
//			    // Front Face
//			    front_face_normal,
//			    front_face_normal,
//			    front_face_normal,
//			    front_face_normal,
//			    front_face_normal,
//			    front_face_normal,
//			    // Back Face
//			    back_face_normal,
//			    back_face_normal,
//			    back_face_normal,
//			    back_face_normal,
//			    back_face_normal,
//			    back_face_normal,
//			    // Right Face
//			    right_face_normal,
//			    right_face_normal,
//			    right_face_normal,
//			    right_face_normal,
//			    right_face_normal,
//			    right_face_normal,
//			    // Left Face
//			    left_face_normal,
//			    left_face_normal,
//			    left_face_normal,
//			    left_face_normal,
//			    left_face_normal,
//			    left_face_normal
//
//		       };
//
//
//			std::copy ( index , index + 36 , std::back_inserter ( secondary_list_of_indices ) );
//			std::copy ( vertices , vertices + 36 , std::back_inserter ( secondary_list_of_vertices ) );
//			std::copy ( normals , normals + 36 , std::back_inserter ( secondary_list_of_normals ) );
//
//		}  // end of looping list of blocks
//	}
//
//
//
////		  GLfloat openGLScreenCoordinates[] = {
////		       -1.0f,  1.0f,
////		        1.0f,  1.0f,
////		        1.0f, -1.0f,
////
////		        1.0f, -1.0f,
////		       -1.0f, -1.0f  ,
////		       -1.0f,  1.0f,
////		  };
//
//	GLfloat openGLScreenCoordinates[] =
//	{
//                       0.0f, (float) height ( ),
//          (float) width ( ), (float) height ( ),
//          (float) width ( ), 0.0f,
//
//	  (float) width ( ), 0.0f,
//	               0.0f, 0.0f,
//	               0.0f, (float) height ( ) };
//
//	GLfloat textureCoordinates[] =
//	{
//		0.0f,  0.0f,
//		0.0f,  1.0f,
//		1.0f,  0.0f,
//
//		0.0f,  1.0f,
//		1.0f,  1.0f,
//		1.0f,  0.0f,
//	};
//
//
//	secondary_list_of_renderFlag =  std::vector<GLfloat>( secondary_list_of_vertices.size( ) , 1.0f );
//
//	std::cout <<  "secondary_list_of_renderFlag.size()"  << secondary_list_of_renderFlag.size() << " -:- "<< secondary_list_of_renderFlag[200] <<std::endl;
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_vertices_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_vertices.size( ) * sizeof(secondary_list_of_vertices[0]) , 0 , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray ( primary_vertices_location );
//	glVertexAttribPointer ( primary_vertices_location , 3 , GL_DOUBLE , GL_FALSE , 0 , 0 );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_normal_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_normals.size( ) * sizeof(secondary_list_of_normals[0]) , 0 , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray ( primary_normal_location );
//	glVertexAttribPointer ( primary_normal_location , 3 , GL_DOUBLE , GL_FALSE , 0 , 0 );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_color_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_colors.size( ) * sizeof(secondary_list_of_colors[0]) ,0 , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray ( primary_color_location );
//	glVertexAttribPointer ( primary_color_location , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );
//
//	glBindBuffer ( GL_ARRAY_BUFFER , primary_renderFlag_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , primary_list_of_renderFlag.size( ) * sizeof(primary_list_of_renderFlag[0]) ,0 , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray ( primary_renderFlag_location );
//	glVertexAttribPointer ( primary_renderFlag_location , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );
//
//	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , primary_indices_buffer );
//	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , secondary_list_of_indices.size ( ) * sizeof ( secondary_list_of_indices[0] ) , &secondary_list_of_indices[0] , GL_STATIC_DRAW );
//
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_vertices_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_vertices.size( ) * sizeof(secondary_list_of_vertices[0]) , &secondary_list_of_vertices[0] , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray(secondary_vertices_location);
//	glVertexAttribPointer(secondary_vertices_location, 3, GL_DOUBLE, GL_FALSE, 0, 0);
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_normal_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_normals.size( ) * sizeof(secondary_list_of_normals[0]) , &secondary_list_of_normals[0] , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray(secondary_normal_location);
//	glVertexAttribPointer(secondary_normal_location, 3, GL_DOUBLE, GL_FALSE, 0, 0);
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_color_buffer );
//	glBufferData ( GL_ARRAY_BUFFER, secondary_list_of_colors.size( ) * sizeof( secondary_list_of_colors[0] ), &secondary_list_of_colors[0], GL_STATIC_DRAW);
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray(secondary_color_location);
//	glVertexAttribPointer(secondary_color_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
//
//	glBindBuffer ( GL_ARRAY_BUFFER , secondary_renderFlag_buffer );
//	glBufferData ( GL_ARRAY_BUFFER , secondary_list_of_renderFlag.size( ) * sizeof(secondary_list_of_renderFlag[0]) , &secondary_list_of_renderFlag[0] , GL_STATIC_DRAW );
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray ( secondary_renderFlag_location );
//	glVertexAttribPointer ( secondary_renderFlag_location , 1 , GL_FLOAT , GL_FALSE , 0 , 0 );
//
//	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, secondary_indices_buffer );
//	glBufferData ( GL_ELEMENT_ARRAY_BUFFER , secondary_list_of_indices.size() * sizeof(secondary_list_of_indices[0]) , &secondary_list_of_indices[0] , GL_STATIC_DRAW );
//
//
//
//	glBindBuffer ( GL_ARRAY_BUFFER , screen_buffer );
//	glBufferData ( GL_ARRAY_BUFFER, 12 *  sizeof( openGLScreenCoordinates[0] ), openGLScreenCoordinates, GL_STATIC_DRAW);
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray(6);
//	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//	glBindBuffer ( GL_ARRAY_BUFFER , texture_buffer );
//	glBufferData ( GL_ARRAY_BUFFER, 12 * sizeof( textureCoordinates[0] ), textureCoordinates , GL_STATIC_DRAW);
//	// Vertex Array : Set up generic attributes pointers
//	glEnableVertexAttribArray(7);
//	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//
//	changeProperty( 0 );
//
//	std::cout << "Size: " << secondary_list_of_vertices.size ( ) << std::endl;
//
//	box = Celer::BoundingBox3<double> ( );
//
//	box.fromPointCloud ( secondary_list_of_vertices.begin ( ) , secondary_list_of_vertices.end ( ) );
//
//	camera_.setPosition ( box.center ( ) );
//	camera_.setTarget ( box.center ( ) );
//	std::cout << box.diagonal ( );
//	camera_.setOffset ( 3.0 * box.diagonal ( ) );
//
//	camera_.setPerspectiveProjectionMatrix ( 60 , camera_.aspectRatio ( ) , 1.0 , 1000.0 * box.diagonal ( ) );
//
//	std::cout << camera_.position ( );
//
//	camera_.setBehavior ( Celer::Camera<float>::REVOLVE_AROUND_MODE );
//
// 	cameraStep_ = 10.0f;
//
//
//}

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


}

/// For DropArea's implementation, we clear invoke clear() and then accept the proposed event.
/// The clear() function sets the text in DropArea to "<drop content>" and sets the backgroundRole to
/// QPalette::Dark. Lastly, it emits the changed() signal.

/// Real Looping
void GLWidget::paintGL ( )
{

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor ( 0.0 , 0.0 , 0.0 , 1.0 );
	TridimensionalSetUp ( );
	//cutawaySetup ( );

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

		glDrawArrays( GL_TRIANGLES , 0, primary_list_of_vertices.size());


		fboStep[1]->release( );

		jumpFloodInitialization.deactive ( );


		// Do Jumping Flooding Algorithm
		int stepSize = ( width ( ) > height ( ) ? width ( ) : height ( ) ) * 0.5;
		bool ExitLoop = 0;



//		JumpFloodingStep->bind ( );
//
//		JumpFloodingStep->setUniformValue ( "viewport" , (float)width ( ) , (float)height ( ) );
//
//		pm_sz = ( camera_.nearPlane ( ) + camera_.farPlane ( ) ) / ( camera_.nearPlane ( ) - camera_.farPlane ( ) );
//
//		glMatrixMode ( GL_PROJECTION );
//		glPushMatrix ( );
//		glLoadIdentity ( );
//
//		camera_.setOrthographicProjectionMatrix ( 0.0 , GLfloat ( width ( ) ) , 0.0 , GLfloat ( height ( ) ) , -100.0 , 100.0 );
//
//		glMultMatrixf ( ( ~camera_.orthographicProjectionMatrix ( ) ) );
//
//		while ( !ExitLoop )
//		{
//			i = ( i + 1 ) % 2;
//			fboStep[i]->bind ( );
//			glActiveTexture ( GL_TEXTURE0 );
//			glEnable ( GL_TEXTURE_RECTANGLE );
//			glBindTexture ( GL_TEXTURE_RECTANGLE , fboStep[ ( i + 1 ) % 2]->texture ( ) );
//
//			JumpFloodingStep->setUniformValue ( "pm_sz" , pm_sz );
//			JumpFloodingStep->setUniformValue ( "theta" , angle );
//			JumpFloodingStep->setUniformValue ( "stepSize" , stepSize );
//
//			glClearColor ( 0.0 , 1.0 , 0.0 , 1.0 );
//			glClear ( GL_COLOR_BUFFER_BIT );
//
//			glBegin ( GL_QUADS );
//			glTexCoord2f ( 0.0 , 0.0 );
//			glVertex2f ( 0.0 , 0.0 );
//			glTexCoord2f ( width ( ) , 0.0 );
//			glVertex2f ( width ( ) , 0.0 );
//			glTexCoord2f ( width ( ) , height ( ) );
//			glVertex2f ( width ( ) , height ( ) );
//			glTexCoord2f ( 0.0 , height ( ) );
//			glVertex2f ( 0.0 , height ( ) );
//			glEnd ( );
//			glFlush ( );
////			QImage im = fboStep[i]->toImage ( );
////			im.save ( QString ( "segundoPasso" ) + QString::number ( stepSize ) + QString ( ".png" ) );
//			stepSize *= 0.5;
//			if ( stepSize < 1 )
//				ExitLoop = true;
//			glDisable ( GL_TEXTURE_RECTANGLE );
//			fboStep[i]->release ( );
//		}
//
//
//		JumpFloodingStep->release ( );



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

			glDrawArrays(GL_TRIANGLES, 0, 6);

			stepSize *= 0.5;
			if ( stepSize < 1 )
				ExitLoop = true;
			glDisable ( GL_TEXTURE_RECTANGLE );
			fboStep[i]->release ( );
		}

		jumpFloodingStep.deactive( ) ;

//		glClearColor ( 0.0 , 1.0 , 0.0 , 1.0 );
//		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//		glActiveTexture ( GL_TEXTURE0 );
//		glEnable ( GL_TEXTURE_RECTANGLE );
//		glBindTexture( GL_TEXTURE_RECTANGLE , fboStep[i]->texture());
//
//		textureViewer.active();
//
//		camera_.setOrthographicProjectionMatrix( 0.0f, (float)width() , 0.0f, (float)height(), -1.0, 1.0 );
//		glUniformMatrix4fv ( textureViewer.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.orthographicProjectionMatrix() );
//
//
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//
//
//		textureViewer.deactive();
//		glDisable ( GL_TEXTURE_RECTANGLE );

		if ( draw_secondary && (secondary_list_of_vertices.size ( ) != 0) )
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

			glDrawArrays ( GL_TRIANGLES , 0 , secondary_list_of_vertices.size ( ) );

			glDisable ( GL_TEXTURE_RECTANGLE );

			cutawayWireframe.deactive ( );
		}

		if ( draw_primary && ( primary_list_of_vertices.size ( ) != 0 ) )
		{
			primary.active ( );

			glUniformMatrix4fv ( primary.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
			glUniformMatrix4fv ( primary.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );

			glDrawArrays ( GL_TRIANGLES , 0 , primary_list_of_vertices.size ( ) );

			primary.deactive ( );
		}

	}
//	else if ( ires_has_been_open_sucessefully )
//	{
//
//		cutawayWireframe.active ( );
//
//		glUniform3fv ( cutawayWireframe.uniforms_["lightDirection"].location , 0 , camera_.position ( ) );
//
//		glUniform2f ( cutawayWireframe.uniforms_["WIN_SCALE"].location , (float) width ( ) , (float) height ( ) );
//		glUniform1i ( cutawayWireframe.uniforms_["cutaway"].location , 0 );
//
//		glUniformMatrix4fv ( cutawayWireframe.uniforms_["ViewMatrix"].location , 1 , GL_TRUE , camera_.viewMatrix ( ) );
//		glUniformMatrix4fv ( cutawayWireframe.uniforms_["ProjectionMatrix"].location , 1 , GL_TRUE , camera_.perspectiveProjectionMatrix ( ) );
//
//		glDrawArrays ( GL_TRIANGLES , 0 , secondary_list_of_vertices.size ( ) );
//
//		cutawayWireframe.deactive ( );
//
//	}

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

//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, secondary_indices_buffer);
//			glDrawElements( GL_LINES_ADJACENCY , secondary_list_of_indices.size(), GL_UNSIGNED_INT,0 );

			glDrawArrays ( GL_LINES_ADJACENCY , 0 , secondary_list_of_vertices.size());


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

	primary.create((shadersDir.path ()+"/share/Shaders/Primary.vert").toStdString(),(shadersDir.path ()+"/share/Shaders/Primary.frag").toStdString());

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
	if ( primary_list_of_vertices.size ( ) > 0)
		draw_primary = !draw_primary;
}

void GLWidget::drawSecondary( )
{
	if ( secondary_list_of_vertices.size( ) > 0 )
		draw_secondary = !draw_secondary;
}

