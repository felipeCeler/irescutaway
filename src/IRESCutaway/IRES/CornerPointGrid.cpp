
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

		static_name[0] = "Modified Block Volume";
		static_name[1] = "Porosity";

		isOpen_ = 0;
	}

	CornerPointGrid::~CornerPointGrid ( )
	{

	}

	void CornerPointGrid::createBuffers ( )
	{
		// TODO Auto-generated constructor stub

		// Cuboid
		glGenVertexArrays ( 1, &vertexArrayCuboids );
			glGenBuffers  ( 1, &vertexBufferCuboidGeometry );  // Geometry
			glGenBuffers  ( 1, &vertexBufferCuboidColor  );      // Property Color
			glGenBuffers  ( 1, &vertexBufferCuboidIJK  );        // Cube IJK
			glGenBuffers  ( 1, &vertexBufferCuboidProperties  ); // Cube Property

		// Face Features
		glGenVertexArrays ( 1, &vertexArrayFaces );
			glGenBuffers ( 1, &vertexBufferFaceGeometry );   // Geometry
			glGenBuffers ( 1, &vertexBufferFaceColor  );      // Property Color
			glGenBuffers ( 1, &vertexBufferFaceIJK );        // Face IJK
			glGenBuffers ( 1, &vertexBufferFaceProperties ); // Face Properties

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

			iresFaces_.clear ( );
			std::vector<float> vertexList;

			reservoir_file.generateFaceList( iresFaces_ );

			vertexList = reservoir_file.getVertexList( );

			faces.clear();
			faceColor.clear();
			faceIJK.clear();
			faceType.clear();
			faceProperty.clear();
			// Geometry
			faces.resize 	     ( iresFaces_.size ( ) * 16 );
			// Attributes
			faceColor.resize     ( iresFaces_.size ( ) * 4 );
			faceIJK.resize       ( iresFaces_.size ( ) * 4 );
			faceType.resize      ( iresFaces_.size ( ) * 4  );
			faceProperty.resize  ( iresFaces_.size ( ) * 4 );

			faceCount = 0;

			std::vector<float> faceCorner;
			std::vector<float> faceFault;

			reservoir_file.getFacePropertyValues(0,faceCorner);

			reservoir_file.getFacePropertyValues(1,faceFault);

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

				faceColor[i*4]   = 1.0f;
				faceColor[i*4+1] = 0.0f;
				faceColor[i*4+2] = 0.0f;
				faceColor[i*4+3] = 1.0f;

				faceIJK[i*4]   = 0.0f;
				faceIJK[i*4+1] = 0.0f;
				faceIJK[i*4+2] = 0.0f;
				faceIJK[i*4+3] = 0.0f;

				int IDtmp = iresFaces_[i].id;
				ires::Face::FACE_BLOCK_POS pos = iresFaces_[i].faceBlockRelPos;
				float V = faceCorner[ 6*IDtmp + pos ];
				/// One way to know which edge are corners given a value V is:
				if ( V >= 8 )
				{
					faceIJK[i * 4] = 1.0;
					V -= 8;
				}
				if ( V >= 4 )
				{
					faceIJK[i * 4 + 1] = 1.0;
					V -= 4;
				}
				if ( V >= 2 )
				{
					faceIJK[i * 4 + 2] = 1.0;
					V -= 2;
				}
				if ( V == 1 )
				{
					faceIJK[i * 4 + 3] = 1.0;
				}

				float F = faceFault[ 6*IDtmp + pos ];

				// F == 0 pass
				// F == 1 fault

				faceType[i*4]   = static_cast<float> (iresFaces_[i].isExtern);
				faceType[i*4+1] = F;
				faceType[i*4+2] = static_cast<float> (iresFaces_[i].isExtern);
				faceType[i*4+3] = static_cast<float> (iresFaces_[i].faceBlockRelPos);

				faceCount++;

			}

			// Reading Cubes
			cuboids.clear ( );
			cuboids.resize( number_of_blocks_ * 32 );

			cubeColor.resize        ( number_of_blocks_ * 4 );
			cuboidFocus.resize      ( number_of_blocks_ * 4 );
			cuboidIJK.resize        ( number_of_blocks_ * 4 );
			cuboidProperties.resize ( number_of_blocks_ * 4 );

			int stride_32 = 0;
			int stride_4  = 0;

			cuboidCount = 0;

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
					// Top Face
					cuboids[stride_32+0]  = v[0];
					cuboids[stride_32+1]  = v[1];
					cuboids[stride_32+2]  = v[2];
					cuboids[stride_32+3]  = 1.0f;

					cuboids[stride_32 +4] = v[3];
					cuboids[stride_32 +5] = v[4];
					cuboids[stride_32 +6] = v[5];
					cuboids[stride_32 +7] = 1.0f;

					cuboids[stride_32+8]  = v[6];
					cuboids[stride_32+9]  = v[7];
					cuboids[stride_32+10] = v[8];
					cuboids[stride_32+11] = 1.0f;

					cuboids[stride_32+12] = v[9];
					cuboids[stride_32+13] = v[10];
					cuboids[stride_32+14] = v[11];
					cuboids[stride_32+15] = 1.0f;

					// Bottom Face
					cuboids[stride_32+16] = v[12];
					cuboids[stride_32+17] = v[13];
					cuboids[stride_32+18] = v[14];
					cuboids[stride_32+19] = 1.0f;

					cuboids[stride_32+20] = v[15];
					cuboids[stride_32+21] = v[16];
					cuboids[stride_32+22] = v[17];
					cuboids[stride_32+23] = 1.0f;

					cuboids[stride_32+24] = v[18];
					cuboids[stride_32+25] = v[19];
					cuboids[stride_32+26] = v[20];
					cuboids[stride_32+27] = 1.0f;

					cuboids[stride_32+28] = v[21];
					cuboids[stride_32+29] = v[22];
					cuboids[stride_32+30] = v[23];
					cuboids[stride_32+31] = 1.0f;

					stride_32 += 32;

					cubeColor[stride_4]     = 1.0f;
					cubeColor[stride_4+1]   = 0.0f;
					cubeColor[stride_4+2]   = 0.0f;
					cubeColor[stride_4+3]   = 1.0f;

					cuboidIJK[stride_4]     = I;
					cuboidIJK[stride_4+1]   = J;
					cuboidIJK[stride_4+2]   = K;
					cuboidIJK[stride_4+3]   = 0.0;

					cuboidFocus[stride_4]   = 1.0f;
					cuboidFocus[stride_4+1] = 0.0f;
					cuboidFocus[stride_4+2] = 0.0f;
					cuboidFocus[stride_4+3] = 1.0f;

					stride_4 += 4;

					cuboidCount++;

				}
				else
				{
					continue;
				}

			}

			cuboids.resize    (stride_32);
			cubeColor.resize  (stride_4 );
			cuboidFocus.resize(stride_4 );
			cuboidIJK.resize  (stride_4 );

			glBindVertexArray ( vertexArrayCuboids );

				glBindBuffer( GL_ARRAY_BUFFER, vertexBufferCuboidGeometry );
				glBufferData ( GL_ARRAY_BUFFER , cuboids.size( ) * sizeof(cuboids[0]) , &cuboids[0] , GL_STATIC_DRAW );

				int size_of_vertice = 4 * sizeof(float);
				int size_of_struct  = 8 * size_of_vertice;

				// @link - http://www.opengl.org/wiki/Vertex_Specification
				// Assign for vertex attribute location 0 - 8 on point of the cuboid
				for ( int location = 0 ; location < 8 ; location++)
				{
					glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
				}

				glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidColor);
				glBufferData ( GL_ARRAY_BUFFER , cubeColor.size( ) * sizeof(cubeColor[0]) , &cubeColor[0] , GL_STATIC_DRAW );

				glEnableVertexAttribArray(8);
				glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 0, 0);

				glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidIJK);
				glBufferData ( GL_ARRAY_BUFFER , cuboidIJK.size( ) * sizeof(cuboidIJK[0]) , &cuboidIJK[0] , GL_STATIC_DRAW );

				glEnableVertexAttribArray(9);
				glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 0, 0);

				glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidFocus);
				glBufferData ( GL_ARRAY_BUFFER , cuboidFocus.size( ) * sizeof(cuboidFocus[0]) , &cuboidFocus[0] , GL_STATIC_DRAW );

				glEnableVertexAttribArray(10);
				glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 0, 0);

				glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidProperties);
				glBufferData ( GL_ARRAY_BUFFER , cuboidProperties.size( ) * sizeof(cuboidProperties[0]) , &cuboidProperties[0] , GL_STATIC_DRAW );

				glEnableVertexAttribArray(11);
				glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 0, 0);


			glBindVertexArray(0);

	                /// FacesFeatures

			glBindVertexArray ( vertexArrayFaces );

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBufferFaceGeometry );
	                        glBufferData ( GL_ARRAY_BUFFER , faces.size( ) * sizeof(faces[0]) , &faces[0] , GL_STATIC_DRAW );

	                        int size_of_vertice_face = 4 * sizeof(float);
	                        int size_of_struct_face  = 4 * size_of_vertice_face;

				// @link - http://www.opengl.org/wiki/Vertex_Specification
				// Assign for vertex attribute location 0 - 4 on point of the face
	                        for ( int location = 0 ; location < 4 ; location++)
	                        {
	                                glEnableVertexAttribArray(location);
	                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct_face , reinterpret_cast<void*>(size_of_vertice_face * location));
	                        }

	                        glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceType);
	                        glBufferData ( GL_ARRAY_BUFFER , faceType.size( ) * sizeof(faceType[0]) , &faceType[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(4);
	                        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceColor);
	                        glBufferData ( GL_ARRAY_BUFFER , faceColor.size( ) * sizeof(faceColor[0]) , &faceColor[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(5);
	                        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceIJK);
	                        glBufferData ( GL_ARRAY_BUFFER , faceIJK.size( ) * sizeof(faceIJK[0]) , &faceIJK[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(6);
	                        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                        glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceProperties);
	                        glBufferData ( GL_ARRAY_BUFFER , faceProperty.size( ) * sizeof(faceProperty[0]) , &faceProperty[0] , GL_STATIC_DRAW );

	                        glEnableVertexAttribArray(7);
	                        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);

	                glBindVertexArray(0);


			loadProperties( );
		}
	}

	void CornerPointGrid::drawFace   ( ) const
	{
		glBindVertexArray ( vertexArrayFaces );
		glDrawArrays 	  ( GL_POINTS , 0 , this->faceCount );
		glBindVertexArray ( 0 );
	}

	void CornerPointGrid::drawCuboid ( ) const
	{
		glBindVertexArray ( this->vertexArrayCuboids );
		glDrawArrays      ( GL_POINTS , 0 , this->cuboidCount );
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

			if ( static_name[0].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Volume -> " << static_porperties[property_index].name << std::endl;
				static_min[0] =  static_porperties[property_index].min_;
				static_max[0] =  static_porperties[property_index].max_;
				static_indices[0] = property_index;
			}
			if ( static_name[1].compare( static_porperties[property_index].name ) == 0 )
			{
				std::cout << "Porosity -> " << static_porperties[property_index].name << std::endl;
				static_min[1] =  static_porperties[property_index].min_;
				static_max[1] =  static_porperties[property_index].max_;
				static_indices[1] = property_index;
			}
		}

		current_property = 0;

		int index = 0;

		for ( std::size_t i = 0; i < number_of_blocks_; i++)
		{
			if ( reservoir_file.isValidBlock(i) )
			{
				cuboidProperties[index+0] = static_porperties[static_indices[0]].values_[i];
				cuboidProperties[index+1] = static_porperties[static_indices[1]].values_[i];
				cuboidProperties[index+2] = static_porperties[static_indices[0]].values_[i];
				cuboidProperties[index+3] = static_porperties[static_indices[0]].values_[i];
				index += 4;
			}
		}

		for ( std::size_t i = 0; i < iresFaces_.size() ; i++ )
		{
			faceProperty[i*4  ]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
			faceProperty[i*4+1]   = static_porperties[static_indices[1]].values_[iresFaces_[i].id];
			faceProperty[i*4+2]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
			faceProperty[i*4+3]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
		}

		cuboidProperties.resize(index);
		// Cuboid
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidProperties);
		glBufferData ( GL_ARRAY_BUFFER , cuboidProperties.size( ) * sizeof(cuboidProperties[0]) , &cuboidProperties[0] , GL_STATIC_DRAW );
		glBindBuffer ( GL_ARRAY_BUFFER, 0);

		// FaceFeature
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceProperties );
		glBufferData ( GL_ARRAY_BUFFER , faceProperty.size( ) * sizeof(faceProperty[0]) , &faceProperty[0] , GL_STATIC_DRAW );
		glBindBuffer ( GL_ARRAY_BUFFER, 0);


	}

} /* namespace ires */
