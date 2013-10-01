
/* CornerPointGrid.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 */

#include "CornerPointGrid.hpp"

#include <Eigen/Eigen>

namespace IRES
{

	CornerPointGrid::CornerPointGrid ( ) : reservoir_file(true)
	{
		isInitialized = 0;

		properties_name[0] = "Bubble Point Pressure";
		properties_name[1] = "Pressure";
		properties_name[2] = "Porosity";
		properties_name[3] = "Modified Block Volume";

		isOpen_ = 0;
	}

	CornerPointGrid::~CornerPointGrid ( )
	{
		// TODO Auto-generated destructor stub
	}

	void CornerPointGrid::createBuffers ( )
	{
		// TODO Auto-generated constructor stub

		// Cuboid
		glGenVertexArrays ( 1, &vertexArray_cuboids );
			glGenBuffers  ( 1, &vertexBuffer_cuboid_geometry );  // Geometry
			glGenBuffers  ( 1, &vertexBuffer_cube_color  );      // Property Color
			glGenBuffers  ( 1, &vertexBuffer_cube_IJK  );        // Cube IJK
			glGenBuffers  ( 1, &vertexBuffer_cube_properties  ); // Cube Property

		// Face Features
		glGenVertexArrays ( 1, &vertexArray_faces );
			glGenBuffers ( 1, &vertexBuffer_face_geometry );   // Geometry
			glGenBuffers ( 1, &vertexBuffer_face_color  );      // Property Color
			glGenBuffers ( 1, &vertexBuffer_face_IJK );        // Face IJK
			glGenBuffers ( 1, &vertexBuffer_face_properties ); // Face Properties

		isInitialized = 1;

	}

	bool CornerPointGrid::isOpen( ) const
	{
		return isOpen_;
	}

	void CornerPointGrid::openIRES ( const std::string& filename )
	{

		if ( !isInitialized )
			return;

		isOpen_ = reservoir_file.readFile( filename );


		if ( isOpen_ )
		{
			header_v2_ = reservoir_file.getHeader();

			number_of_blocks_ = header_v2_.numI * header_v2_.numJ * header_v2_.numK;

			std::cout << std::setfill ( '-' ) << std::setw ( 55 ) << "-" <<  std::endl;

			std::cout << std::setw(50) << std::left << "Version : " << std::setiosflags(std::ios::right) << header_v2_.version << std::endl;
			std::cout << std::setw(50) << std::left << "First Title : " << header_v2_.title << std::endl;
			std::cout << std::setw(50) << std::left << "Run date : " << header_v2_.runDate << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in I direction :" << header_v2_.numI << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in J direction :" << header_v2_.numJ << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in K direction :" << header_v2_.numK << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Dynamic : " << header_v2_.numDynamicProps << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Static : " << header_v2_.numStaticProps << std::endl;

			std::cout << std::setfill ( '-' ) << std::setw ( 55 ) << "-" << std::endl;

			std::vector<std::string> 	static_names;

			reservoir_file.getStaticPropertyNames(static_names);

			static_porperties.clear();
			static_porperties.resize( static_names.size( ) );

			for ( std::size_t i = 0; i < static_names.size(); i++)
			{

				static_names[i].erase ( std::remove_if ( static_names[i].begin ( ) , static_names[i].end ( ) , ::iscntrl ) , static_names[i].end ( ) );
				static_porperties[i].name = static_names[i];

				reservoir_file.getStaticPropertyValues( i, static_porperties[i].values_ );

				static_porperties[i].min_ = *std::min_element ( static_porperties[i].values_.begin ( ) , static_porperties[i].values_.end ( ) );
				static_porperties[i].max_ = *std::max_element ( static_porperties[i].values_.begin ( ) , static_porperties[i].values_.end ( ) );

			}

			std::vector<std::string> 	dynamic_names;
			reservoir_file.getDynamicPropertyNames(dynamic_names);

			for ( std::size_t i = 0; i < dynamic_names.size(); i++)
			{
				std::cout << " Dynamic Name " <<  dynamic_names[i] << std::endl;
			}


			float 	  	v[24];
			Eigen::Vector3f vecs[8];

			iresFaces_.clear ( );
			std::vector<float> vertexList;

			reservoir_file.generateFaceList( iresFaces_ );

			vertexList = reservoir_file.getVertexList( );

			faces.clear();

			// Geometry
			faces.resize ( iresFaces_.size ( ) * 16 );
			// Attributes
			facesColor.resize    ( iresFaces_.size ( ) * 4 );
			facesIJK.resize      ( iresFaces_.size ( ) * 4 );
			facesType.resize     ( iresFaces_.size ( ) * 4  );
			facesProperty.resize ( iresFaces_.size ( ) * 4 );

			faceIDs.resize       ( iresFaces_.size ( ) );

			faces_size = 0;

			for ( std::size_t i = 0; i < iresFaces_.size( ) ; i++)
			{
				faces[i*16]   = vertexList[iresFaces_[i].a*3];
				faces[i*16+1] = vertexList[iresFaces_[i].a*3+1];
				faces[i*16+2] = vertexList[iresFaces_[i].a*3+2];
				faces[i*16+3] = 1.0f;

				faces[i*16+4] = vertexList[iresFaces_[i].b*3];
				faces[i*16+5] = vertexList[iresFaces_[i].b*3+1];
				faces[i*16+6] = vertexList[iresFaces_[i].b*3+2];
				faces[i*16+7] = 1.0f;

				faces[i*16+8]  = vertexList[iresFaces_[i].c*3];
				faces[i*16+9]  = vertexList[iresFaces_[i].c*3+1];
				faces[i*16+10] = vertexList[iresFaces_[i].c*3+2];
				faces[i*16+11] = 1.0f;

				faces[i*16+12] = vertexList[iresFaces_[i].d*3];
				faces[i*16+13] = vertexList[iresFaces_[i].d*3+1];
				faces[i*16+14] = vertexList[iresFaces_[i].d*3+2];
				faces[i*16+15] = 1.0f;

				facesColor[i*4]   = 1.0f;
				facesColor[i*4+1] = 0.0f;
				facesColor[i*4+2] = 0.0f;
				facesColor[i*4+3] = 1.0f;

				facesIJK[i*4]   = 1.0f;
				facesIJK[i*4+1] = 0.0f;
				facesIJK[i*4+2] = 0.0f;
				facesIJK[i*4+3] = 1.0f;

				facesType[i*4]   = static_cast<float> (iresFaces_[i].isExtern);
				facesType[i*4+1] = static_cast<float> (iresFaces_[i].isExtern);
				facesType[i*4+2] = static_cast<float> (iresFaces_[i].isExtern);
				facesType[i*4+3] = 1.0f;

				faceIDs[i] = iresFaces_[i].id;

				faces_size++;

			}

			// Reading Cubes
			cuboids.clear ( );
			cuboids.resize( number_of_blocks_ * 32 );

			cubeColor.resize     ( number_of_blocks_ * 4 );
			cubeFocus.resize     ( number_of_blocks_ * 4 );
			cubeIJK.resize       ( number_of_blocks_ * 4 );
			cubeProperties.resize( number_of_blocks_ * 4 );

			int stride_32 = 0;
			int stride_4  = 0;

			cuboids_size = 0;

			for ( std::size_t i = 0; i < number_of_blocks_; i++)
			{
				unsigned int I;
				unsigned int J;
				unsigned int K;

				reservoir_file.getIJKfromIndex( i , I, J, K );

				if ( reservoir_file.getBlockVertices ( i , v ) )
				{
					// FIXME getBlockVertices returns a list of 8 vertices.
					// The first 4 of them belongs to the top face of the cube and the remaining
					// to the bottom.
					//

					vecs[0][0] = v[0];
					vecs[0][1] = v[1];
					vecs[0][2] = v[2];

					vecs[1][0] = v[3];
					vecs[1][1] = v[4];
					vecs[1][2] = v[5];

					vecs[2][0] = v[6];
					vecs[2][1] = v[7];
					vecs[2][2] = v[8];

					vecs[3][0] = v[9];
					vecs[3][1] = v[10];
					vecs[3][2] = v[11];


					vecs[4][0] = v[12];
					vecs[4][1] = v[13];
					vecs[4][2] = v[14];

					vecs[5][0] = v[15];
					vecs[5][1] = v[16];
					vecs[5][2] = v[17];

					vecs[6][0] = v[18];
					vecs[6][1] = v[19];
					vecs[6][2] = v[20];

					vecs[7][0] = v[21];
					vecs[7][1] = v[22];
					vecs[7][2] = v[23];

					// Top Face
					cuboids[stride_32+0]   = vecs[0][0];
					cuboids[stride_32+1]   = vecs[0][1];
					cuboids[stride_32+2]   = vecs[0][2];
					cuboids[stride_32+3]   = 1.0f;

					cuboids[stride_32 +4]   = vecs[1][0];
					cuboids[stride_32 +5]   = vecs[1][1];
					cuboids[stride_32 +6]   = vecs[1][2];
					cuboids[stride_32 +7]   = 1.0f;

					cuboids[stride_32+8]   = vecs[2][0];
					cuboids[stride_32+9]   = vecs[2][1];
					cuboids[stride_32+10]  = vecs[2][2];
					cuboids[stride_32+11]  = 1.0f;

					cuboids[stride_32+12]  = vecs[3][0];
					cuboids[stride_32+13]  = vecs[3][1];
					cuboids[stride_32+14]  = vecs[3][2];
					cuboids[stride_32+15]  = 1.0f;

					// Bottom Face
					cuboids[stride_32+16]  = vecs[4][0];
					cuboids[stride_32+17]  = vecs[4][1];
					cuboids[stride_32+18]  = vecs[4][2];
					cuboids[stride_32+19]  = 1.0f;

					cuboids[stride_32+20] = vecs[5][0];
					cuboids[stride_32+21] = vecs[5][1];
					cuboids[stride_32+22] = vecs[5][2];
					cuboids[stride_32+23] = 1.0f;

					cuboids[stride_32+24] = vecs[6][0];
					cuboids[stride_32+25] = vecs[6][1];
					cuboids[stride_32+26] = vecs[6][2];
					cuboids[stride_32+27] = 1.0f;

					cuboids[stride_32+28] = vecs[7][0];
					cuboids[stride_32+29] = vecs[7][1];
					cuboids[stride_32+30] = vecs[7][2];
					cuboids[stride_32+31] = 1.0f;

					stride_32 += 32;

					cubeColor[stride_4]   = 1.0f;
					cubeColor[stride_4+1] = 0.0f;
					cubeColor[stride_4+2] = 0.0f;
					cubeColor[stride_4+3] = 1.0f;

					cubeIJK[stride_4]   = I;
					cubeIJK[stride_4+1] = J;
					cubeIJK[stride_4+2] = K;
					cubeIJK[stride_4+3] = 0.0;

					cubeFocus[stride_4]   = 1.0f;
					cubeFocus[stride_4+1] = 0.0f;
					cubeFocus[stride_4+2] = 0.0f;
					cubeFocus[stride_4+3] = 1.0f;

					stride_4 += 4;

					cuboids_size++;

				}
				else
				{
					continue;
				}

			}

			cuboids.resize  (stride_32);
			cubeColor.resize(stride_4 );
			cubeFocus.resize(stride_4 );
			cubeIJK.resize  (stride_4 );

			glBindVertexArray ( vertexArray_cuboids );

				glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cuboid_geometry );
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

			glBindVertexArray ( vertexArray_faces );

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_geometry );
	                        glBufferData ( GL_ARRAY_BUFFER , faces.size( ) * sizeof(faces[0]) , &faces[0] , GL_STATIC_DRAW );

	                        int size_of_vertice_face = 4 * sizeof(float);
	                        int size_of_struct_face  = 4 * size_of_vertice_face;

	                        //http://www.opengl.org/wiki/Vertex_Specification
	                        for ( int location = 0 ; location < 4 ; location++)
	                        {
	                                glEnableVertexAttribArray(location);
	                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct_face , reinterpret_cast<void*>(size_of_vertice_face * location));
	                        }

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_type);
	                        glBufferData ( GL_ARRAY_BUFFER , facesType.size( ) * sizeof(facesType[0]) , &facesType[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(4);
	                        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_color);
	                        glBufferData ( GL_ARRAY_BUFFER , facesColor.size( ) * sizeof(facesColor[0]) , &facesColor[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(5);
	                        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_IJK);
	                        glBufferData ( GL_ARRAY_BUFFER , facesIJK.size( ) * sizeof(facesIJK[0]) , &facesIJK[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(6);
	                        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_properties);
	                        glBufferData ( GL_ARRAY_BUFFER , facesProperty.size( ) * sizeof(facesProperty[0]) , &facesProperty[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(7);
	                        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                glBindVertexArray(0);


			loadProperties( );
		}
	}

	void CornerPointGrid::drawFace   ( ) const
	{
		glBindVertexArray ( vertexArray_faces );
		glDrawArrays ( GL_POINTS , 0 , faces_size );
		glBindVertexArray ( 0 );
	}

	void CornerPointGrid::drawCuboid ( ) const
	{
		glBindVertexArray ( this->vertexArray_cuboids );
		glDrawArrays ( GL_POINTS , 0 , this->cuboids_size );
		glBindVertexArray ( 0 );
	}

	void CornerPointGrid::loadProperties( )
	{

		// Four property x = Bubble Point Pressure
		//               y = Pressure
		//	         z = Porosity
		//               w = Modified Block Volume

		for ( std::size_t property_index = 0; property_index < static_porperties.size( ); property_index++ )
		{
			if ( properties_name[0].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Bubble -> " << static_porperties[property_index].name << std::endl;
				min_value[0] =  static_porperties[property_index].min_;
				max_value[0] =  static_porperties[property_index].max_;
				indices[0] = property_index;
			}
			if ( properties_name[1].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Pressure -> " << static_porperties[property_index].name << std::endl;
				min_value[1] =  static_porperties[property_index].min_;
				max_value[1] =  static_porperties[property_index].max_;
				indices[1] = property_index;
			}
			if ( properties_name[2].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Porosity -> " << static_porperties[property_index].name << std::endl;
				min_value[2] =  static_porperties[property_index].min_;
				max_value[2] =  static_porperties[property_index].max_;
				indices[2] = property_index;
			}
			if ( properties_name[3].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Volume -> " << static_porperties[property_index].name << std::endl;
				min_value[3] =  static_porperties[property_index].min_;
				max_value[3] =  static_porperties[property_index].max_;
				indices[3] = property_index;
			}
		}

		current_property = 0;

		int index = 0;

		for ( std::size_t i = 0; i < number_of_blocks_; i++)
		{
			if ( reservoir_file.isValidBlock(i) )
			{
				cubeProperties[index]   = static_porperties[indices[0]].values_[i];
				cubeProperties[index+1] = static_porperties[indices[1]].values_[i];
				cubeProperties[index+2] = static_porperties[indices[2]].values_[i];
				cubeProperties[index+3] = static_porperties[indices[3]].values_[i];
				index += 4;
			}
			else
			{
				continue;
			}
		}

		for ( std::size_t shell_index = 0; shell_index < faceIDs.size() ; shell_index++ )
		{
			facesProperty[shell_index]   = cubeProperties[faceIDs[shell_index]];
			facesProperty[shell_index+1] = cubeProperties[faceIDs[shell_index]];
			facesProperty[shell_index+2] = cubeProperties[faceIDs[shell_index]];
			facesProperty[shell_index+3] = cubeProperties[faceIDs[shell_index]];
		}


		cubeProperties.resize(index);
		// Cuboid
		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_cube_properties);
		glBufferData ( GL_ARRAY_BUFFER , cubeProperties.size( ) * sizeof(cubeProperties[0]) , &cubeProperties[0] , GL_STATIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		// FaceFeature
		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer_face_properties );
		glBufferData ( GL_ARRAY_BUFFER , facesProperty.size( ) * sizeof(facesProperty[0]) , &facesProperty[0] , GL_STATIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0);


	}

} /* namespace ires */
