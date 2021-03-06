
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

		showBorderLine = 0;
		showFault      = 0;

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
			glGenBuffers ( 1, &vertexBufferCuboidGeometry );  // Geometry
			glGenBuffers ( 1, &vertexBufferCuboidStatic );    // Cube Property
			glGenBuffers ( 1 ,&indexBufferCuboid);  // Indices

		// Face Features
		glGenVertexArrays ( 1, &vertexArrayFaces );
			glGenBuffers ( 1, &vertexBufferFaceGeometry );   // Geometry
			glGenBuffers ( 1, &vertexBufferFaceStatic ); // Face Properties
			glGenBuffers ( 1, &vertexBufferFaceType ); // Face Properties
			glGenBuffers ( 1 ,&indexBufferFace);  // Indices


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

			// - Extracting Static Property Information
			std::vector<std::string> static_names;

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

			// - Extracting Dynamic Property Information

			std::vector<std::string> 	       dynamic_names;
			std::vector<unsigned int >	       numTimeSteps;
			reservoir_file.getDynamicPropertyNames(dynamic_names);

			dynamic_properties.resize ( dynamic_names.size( ) );

			for ( std::size_t i = 0; i < dynamic_names.size(); i++)
			{
				std::cout << " Dynamic Name " <<  dynamic_names[i] << std::endl;

			}

			reservoir_file.getDynamicPropertyNumTimesteps( numTimeSteps );

			for ( std::size_t i = 0; i < dynamic_names.size(); i++ )
			{
				dynamic_properties[i].name = dynamic_names[i];
				dynamic_properties[i].values_.resize( numTimeSteps[i] );

				dynamic_properties[i].min_.resize( numTimeSteps[i] );
				dynamic_properties[i].max_.resize( numTimeSteps[i] );

				dynamic_properties[i].numTimeSteps = numTimeSteps[i];

				for (unsigned int t = 0; t < numTimeSteps[i]; t++)
				{
					reservoir_file.getDynamicPropertyValues( i, t, dynamic_properties[i].values_[t]);
					dynamic_properties[i].min_[t] = *std::min_element ( dynamic_properties[i].values_[t].begin ( ) , dynamic_properties[i].values_[t].end ( ) );
					dynamic_properties[i].max_[t] = *std::max_element ( dynamic_properties[i].values_[t].begin ( ) , dynamic_properties[i].values_[t].end ( ) );
					//std::cout << "Time step: " << t << " min "<< dynamic_properties[i].min_[t] << " max " <<  dynamic_properties[i].max_[t] << std::endl;

				}
			}

			// - Extracting Face Information

			iresFaces_.clear ( );
			std::vector<float> vertexList;

			reservoir_file.generateFaceList( iresFaces_ );

			vertexList = reservoir_file.getVertexList( );

			faces.clear();
			faceType.clear();
			// Geometry
			faces.resize 	     ( iresFaces_.size ( ) * 2 * 16 );
			// Attributes
			faceType.resize      ( iresFaces_.size ( ) * 2 * 4  );

			faceCount = 0;

			std::vector<float> faceCorner; // Border Lines
			std::vector<float> faceFault;  // Faces that define a fault;

			reservoir_file.getFacePropertyValues(0,faceCorner);

			reservoir_file.getFacePropertyValues(1,faceFault);

			for ( std::size_t i = 0; i < iresFaces_.size( ) ; i++)
			{


                                faces[faceCount*16]   = vertexList[iresFaces_[i].a*3];
                                faces[faceCount*16+1] = vertexList[iresFaces_[i].a*3+1];
                                faces[faceCount*16+2] = vertexList[iresFaces_[i].a*3+2];
                                faces[faceCount*16+3] = 1.0f;

                                faces[faceCount*16+4] = vertexList[iresFaces_[i].b*3];
                                faces[faceCount*16+5] = vertexList[iresFaces_[i].b*3+1];
                                faces[faceCount*16+6] = vertexList[iresFaces_[i].b*3+2];
                                faces[faceCount*16+7] = 1.0f;

                                faces[faceCount*16+8]  = vertexList[iresFaces_[i].c*3];
                                faces[faceCount*16+9]  = vertexList[iresFaces_[i].c*3+1];
                                faces[faceCount*16+10] = vertexList[iresFaces_[i].c*3+2];
                                faces[faceCount*16+11] = 1.0f;

                                faces[faceCount*16+12] = vertexList[iresFaces_[i].d*3];
                                faces[faceCount*16+13] = vertexList[iresFaces_[i].d*3+1];
                                faces[faceCount*16+14] = vertexList[iresFaces_[i].d*3+2];
                                faces[faceCount*16+15] = 1.0f;


                                ires::Face::FACE_BLOCK_POS pos = iresFaces_[i].faceBlockRelPos;
                                float V 		       = faceCorner[ 6*iresFaces_[i].id + pos ];
                                float F 		       = faceFault[ 6*iresFaces_[i].id + pos ];

                                // Face Type[0] = Shell or Internal
                                // Face Type[1] = Fault
                                        // F == 0 no fault
                                        // F == 1 fault
                                // Face Type[2] = Border Lines
                                // Face Type[3] = Face Position
                                        //   NOTDEFINED = -100000000 ,
                                        //           BOTTOM = 0 ,
                                        //           TOP    = 1 ,
                                        //           LEFT   = 2 ,
                                        //           RIGHT  = 3 ,
                                        //           FRONT  = 4 ,
                                        //           BACK   = 5

                                faceType[faceCount*4]   = static_cast<float> (iresFaces_[i].isExtern);
                                faceType[faceCount*4+1] = F;
                                faceType[faceCount*4+2] = V;
                                faceType[faceCount*4+3] = static_cast<float> (iresFaces_[i].faceBlockRelPos);

                                faceCount++;

			}


			// - Extracting Cuboid Information

			float 	 v[24];
			cuboids.clear ( );
			cuboids.resize( number_of_blocks_ * 32 );

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

					stride_4 += 4;

					cuboidCount++;

				}
				else
				{
					continue;
				}

			}

			cuboids.resize    (stride_32);
			// Element Array Buffer
			indexCuboids.resize( cuboidCount );
			std::iota (indexCuboids.begin(),indexCuboids.end(),0);


			faces.resize(faceCount * 16);
			faceType.resize(faceCount * 4 );

                        // Element Array Buffer
                        indexFaces.resize( faceCount );
                        std::iota (indexFaces.begin(),indexFaces.end(),0);


			glBindVertexArray ( vertexArrayCuboids );

				// - Geometry Information  v0 to v7
				glBindBuffer( GL_ARRAY_BUFFER, vertexBufferCuboidGeometry );
				glBufferData ( GL_ARRAY_BUFFER , cuboids.size( ) * sizeof(cuboids[0]) , &cuboids[0] , GL_STATIC_DRAW );

				int size_of_vertice = 4 * sizeof(float);
				int size_of_struct  = 8 * size_of_vertice;

				//! @link - http://www.opengl.org/wiki/Vertex_Specification
				// Assign for vertex attribute location 0 - 8 on point of the cuboid
				for ( int location = 0 ; location < 8 ; location++)
				{
					glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct , reinterpret_cast<void*>(size_of_vertice * location));
				}

				// - Cube Static Property - v8
				glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidStatic);
				glBufferData ( GL_ARRAY_BUFFER , cuboidStatic.size( ) * sizeof(cuboidStatic[0]) , &cuboidStatic[0] , GL_STATIC_DRAW );

				glEnableVertexAttribArray(8);
				glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 0, 0);

				glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, indexBufferCuboid);
				glBufferData ( GL_ELEMENT_ARRAY_BUFFER, indexCuboids.size() * sizeof(indexCuboids[0]), &indexCuboids[0], GL_STATIC_DRAW );


			glBindVertexArray(0);

	                /// FacesFeatures

			glBindVertexArray ( vertexArrayFaces );

	                        glBindBuffer( GL_ARRAY_BUFFER, vertexBufferFaceGeometry );
	                        glBufferData ( GL_ARRAY_BUFFER , faces.size( ) * sizeof(faces[0]) , &faces[0] , GL_STATIC_DRAW );

	                        int size_of_vertice_face = 4 * sizeof(float);
	                        int size_of_struct_face  = 4 * size_of_vertice_face;

				//! @link - http://www.opengl.org/wiki/Vertex_Specification
				// Assign for vertex attribute location 0 - 4 on point of the face
	                        for ( int location = 0 ; location < 4 ; location++)
	                        {
	                                glEnableVertexAttribArray(location);
	                                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, size_of_struct_face , reinterpret_cast<void*>(size_of_vertice_face * location));
	                        }


                                glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceType);
                                glBufferData ( GL_ARRAY_BUFFER , faceType.size( ) * sizeof(faceType[0]) , &faceType[0] , GL_STATIC_DRAW );

                                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
                                glEnableVertexAttribArray(4);

                                glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceStatic);
                                glBufferData ( GL_ARRAY_BUFFER , faceStatic.size( ) * sizeof(faceStatic[0]) , &faceStatic[0] , GL_STATIC_DRAW );

                                glEnableVertexAttribArray(5);
                                glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

                                glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, indexBufferFace);
                                glBufferData ( GL_ELEMENT_ARRAY_BUFFER, indexFaces.size() * sizeof(indexFaces[0]), &indexFaces[0], GL_STATIC_DRAW );

	                glBindVertexArray(0);

	                glFinish();

                        //cuboids.clear();
                        //faces.clear();
                        //faceType.clear();

	                loadStaticProperties  ( );
	                loadDynamicProperties ( );
		}
	}

	void CornerPointGrid::drawFaces   ( ) const
	{
		glBindVertexArray ( vertexArrayFaces );
		glDrawArrays 	  ( GL_POINTS , 0 , this->faceCount );
		glBindVertexArray ( 0 );

	}

        void CornerPointGrid::drawIndexFaces   ( const std::size_t& size ) const
        {
                glBindVertexArray ( vertexArrayFaces );

                glDrawElements( GL_POINTS , size ,GL_UNSIGNED_INT, 0);

                glBindVertexArray ( 0 );
        }

	void CornerPointGrid::drawCuboid ( ) const
	{
		glBindVertexArray ( vertexArrayCuboids );
		glDrawArrays      ( GL_POINTS , 0 , this->cuboidCount );
		glBindVertexArray ( 0 );
	}

        void CornerPointGrid::drawIndexCuboids   ( const std::size_t& size ) const
        {
                glBindVertexArray ( vertexArrayCuboids );

                glDrawElements( GL_POINTS , size ,GL_UNSIGNED_INT, 0);

                glBindVertexArray ( 0 );
        }

	void CornerPointGrid::loadDynamicProperties (  )
	{
	        int index = 0;

                if ( cuboidDynamicIds.size ( ) > 0 )
                {

                        for ( std::size_t p = 0; p < cuboidDynamicIds.size ( ); p++ )
                        {
                                glDeleteBuffers ( cuboidDynamicIds[p].size ( ) , &cuboidDynamicIds[p][0] );
                        }

                }

                cuboidDynamicIds.clear ( );

                cuboidDynamicIds.resize ( dynamic_properties.size ( ) );

                cuboidDynamic.resize ( number_of_blocks_ );


                for ( std::size_t p = 0; p < dynamic_properties.size ( ); p++ )
                {
                        cuboidDynamicIds[p].resize ( dynamic_properties[p].numTimeSteps );

                        glGenBuffers ( dynamic_properties[p].numTimeSteps , &cuboidDynamicIds[p][0] );

                        for ( unsigned int t = 0; t < dynamic_properties[p].numTimeSteps; t++ )
                        {

                                for ( std::size_t b = 0; b < number_of_blocks_; b++ )
                                {
                                        if ( reservoir_file.isValidBlock ( b ) )
                                        {
                                                cuboidDynamic[index] = dynamic_properties[p].values_[t][b];
                                                index++;
                                        }
                                }

                                cuboidDynamic.resize ( index );
                                index = 0;
                                glBindBuffer ( GL_ARRAY_BUFFER , cuboidDynamicIds[p][t] );
                                glBufferData ( GL_ARRAY_BUFFER , cuboidDynamic.size ( ) * sizeof ( cuboidDynamic[0] ) , &cuboidDynamic[0] , GL_STATIC_DRAW );
                                glFinish();
                        }

                }

                //cuboidDynamic.clear ( );

                if ( faceDynamicIds.size ( ) > 0 )
                {

                        for ( std::size_t p = 0; p < faceDynamicIds.size ( ); p++ )
                        {
                                glDeleteBuffers ( faceDynamicIds[p].size ( ) , &faceDynamicIds[p][0] );
                        }

                }


                //faceDynamicIds.clear ( );

                faceDynamicIds.resize ( dynamic_properties.size ( ) );
                faceDynamic.resize ( iresFaces_.size() );

                index = 0;

                for ( std::size_t p = 0; p < dynamic_properties.size ( ); p++ )
                {
                        faceDynamicIds[p].resize ( dynamic_properties[p].numTimeSteps );

                        glGenBuffers ( dynamic_properties[p].numTimeSteps , &faceDynamicIds[p][0] );


                        for ( unsigned int t = 0; t < dynamic_properties[p].numTimeSteps; t++ )
                        {

                                for ( std::size_t b = 0; b < iresFaces_.size ( ); b++ )
                                {

                                        faceDynamic[index] = dynamic_properties[p].values_[t][iresFaces_[b].id];

                                        index++;


                                }

                                faceDynamic.resize(index);
                                index = 0;
                                glBindBuffer ( GL_ARRAY_BUFFER , faceDynamicIds[p][t] );
                                glBufferData ( GL_ARRAY_BUFFER , faceDynamic.size ( ) * sizeof ( faceDynamic[0] ) , &faceDynamic[0] , GL_STATIC_DRAW );
                                glFinish();
                        }

                }
                //faceDynamic.clear ( );

	}

	void CornerPointGrid::loadStaticProperties( )
	{

		// Four property x = Modified Block Volume
		//               y = Porosity
		//	         z = 0
		//               w = 0

		cuboidStatic.clear();
		cuboidStatic.resize( 4 * number_of_blocks_ );

		faceStatic.clear();
		faceStatic.resize  ( iresFaces_.size ( ) * 4 );


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

		current_static = 0;

		std::size_t index_cuboid  = 0;

		for ( std::size_t i = 0; i < number_of_blocks_; i++)
		{
			if ( reservoir_file.isValidBlock(i) )
			{
				cuboidStatic[index_cuboid+0] = static_porperties[static_indices[0]].values_[i];
				cuboidStatic[index_cuboid+1] = static_porperties[static_indices[1]].values_[i];
				cuboidStatic[index_cuboid+2] = static_porperties[static_indices[0]].values_[i];
				cuboidStatic[index_cuboid+3] = static_porperties[static_indices[0]].values_[i];
				index_cuboid += 4;
			}
		}

		cuboidStatic.resize(index_cuboid);

		std::size_t index_external = 0;
		//std::size_t index_internal = 0;

		for ( std::size_t i = 0; i < iresFaces_.size() ; i++ )
		{

                        faceStatic[index_external+0]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
                        faceStatic[index_external+1]   = static_porperties[static_indices[1]].values_[iresFaces_[i].id];
                        faceStatic[index_external+2]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
                        faceStatic[index_external+3]   = static_porperties[static_indices[0]].values_[iresFaces_[i].id];
                        index_external += 4;
		}

		faceStatic.resize( index_external );

		// Cuboid
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferCuboidStatic);
		glBufferData ( GL_ARRAY_BUFFER , cuboidStatic.size( ) * sizeof(cuboidStatic[0]) , &cuboidStatic[0] , GL_STATIC_DRAW );
		glBindBuffer ( GL_ARRAY_BUFFER, 0);

		// FaceFeature
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBufferFaceStatic );
		glBufferData ( GL_ARRAY_BUFFER , faceStatic.size( ) * sizeof(faceStatic[0]) , &faceStatic[0] , GL_STATIC_DRAW );
		glBindBuffer ( GL_ARRAY_BUFFER, 0);

        glFinish();

       //faceStatic.clear();
	}

} /* namespace ires */
