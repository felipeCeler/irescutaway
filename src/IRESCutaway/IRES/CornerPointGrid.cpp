/*
 * CornerPointGrid.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 */

#include "CornerPointGrid.hpp"

namespace IRES
{

	CornerPointGrid::CornerPointGrid ( )
	{
		// TODO Auto-generated constructor stub
	}

	CornerPointGrid::~CornerPointGrid ( )
	{
		// TODO Auto-generated destructor stub
	}


	void CornerPointGrid::openIRES_Version_2 ( const std::string& filename )
	{

		ires::Ires reservoir_file(true);

		time_t start,end;

		reservoir_file.readFile( filename );

		header_v2_ = reservoir_file.getHeader();

		time (&start);

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

		header_ = TheHeader();

		header_.number_of_Blocks_in_I_Direction = header_v2_.numI;
		header_.number_of_Blocks_in_J_Direction = header_v2_.numJ;
		header_.number_of_Blocks_in_K_Direction = header_v2_.numK;
		header_.number_of_Blocks = header_v2_.numI * header_v2_.numJ * header_v2_.numK;

		std::vector<std::string> 	static_names;

		reservoir_file.getStaticPropertyNames(static_names);

		static_porperties.clear();
		static_porperties.resize( static_names.size( ) );

		for ( int i = 0; i < static_names.size(); i++)
		{

			static_names[i].erase ( std::remove_if ( static_names[i].begin ( ) , static_names[i].end ( ) , ::iscntrl ) , static_names[i].end ( ) );
			static_porperties[i].name = static_names[i];

			reservoir_file.getStaticPropertyValues( i, static_porperties[i].values_ );

			static_porperties[i].min_ = *std::min_element ( static_porperties[i].values_.begin ( ) , static_porperties[i].values_.end ( ) );
			static_porperties[i].max_ = *std::max_element ( static_porperties[i].values_.begin ( ) , static_porperties[i].values_.end ( ) );

 		}

		float 	  	v[24];
		Eigen::Vector3f vecs[8];

		this->blocks.clear( );
		this->blocks.reserve( header_.number_of_Blocks );

		this->box_v2.reset();

                IRES::Block new_block;
                new_block.vertices.resize(24);
                new_block.normals.resize(24);
                new_block.IJK.resize(24);
                new_block.focus.resize(24);

        	list_of_vertex_indice.clear( );
        	list_of_vertex_geometry_a.clear( );
        	list_of_vertex_geometry_b.clear( );
        	list_of_vertex_geometry_c.clear( );
        	list_of_vertex_geometry_d.clear( );
        	list_of_vertex_color.clear( );

        	list_of_block_id.clear( );
        	list_of_block_flag.clear( );


		reservoir_file.generateFaceList( list_of_vertex_indice ,
		                                 list_of_vertex_geometry_a,
		                                 list_of_vertex_geometry_b,
		                                 list_of_vertex_geometry_c,
		                                 list_of_vertex_geometry_d,
		                                 list_of_vertex_color,
		                                 list_of_block_id ,
		                                 list_of_block_flag );

		reservoir_file.generateTriangleList(list_of_vertex_geometry_charles,0,false,false,true );

		for ( int i = 0; i < header_.number_of_Blocks; i++)
		{
			new_block.setIdentification ( i );

			unsigned int I;
			unsigned int J;
			unsigned int K;
			reservoir_file.getIJKfromIndex( i , I, J, K );

			Eigen::Vector4f IJK ( I, J, K, 0 );

			std::fill (new_block.IJK.begin(),new_block.IJK.end(),IJK);

			if ( reservoir_file.getBlockVertices ( i , v ) )
			{
				new_block.valid = true;

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


                                new_block.vertices[0] = (Eigen::Vector4f ( vecs[4][0],vecs[4][1],vecs[4][2], 1.0f));
                                new_block.vertices[1] = (Eigen::Vector4f ( vecs[5][0],vecs[5][1],vecs[5][2], 1.0f));
                                new_block.vertices[2] = (Eigen::Vector4f ( vecs[7][0],vecs[7][1],vecs[7][2], 1.0f));
                                new_block.vertices[3] = (Eigen::Vector4f ( vecs[6][0],vecs[6][1],vecs[6][2], 1.0f));
                                // Bottom Face
                                new_block.vertices[4] = (Eigen::Vector4f ( vecs[0][0],vecs[0][1],vecs[0][2], 1.0f));
                                new_block.vertices[5] = (Eigen::Vector4f ( vecs[3][0],vecs[3][1],vecs[3][2], 1.0f));
                                new_block.vertices[6] = (Eigen::Vector4f ( vecs[1][0],vecs[1][1],vecs[1][2], 1.0f));
                                new_block.vertices[7] = (Eigen::Vector4f ( vecs[2][0],vecs[2][1],vecs[2][2], 1.0f));
                                // Front Face
                                new_block.vertices[8] = (Eigen::Vector4f ( vecs[4][0],vecs[4][1],vecs[4][2], 1.0f));
                                new_block.vertices[9] = (Eigen::Vector4f ( vecs[0][0],vecs[0][1],vecs[0][2], 1.0f));
                                new_block.vertices[10] = (Eigen::Vector4f ( vecs[5][0],vecs[5][1],vecs[5][2], 1.0f));
                                new_block.vertices[11] = (Eigen::Vector4f ( vecs[1][0],vecs[1][1],vecs[1][2], 1.0f));
                                // Back Face
                                new_block.vertices[12] = (Eigen::Vector4f ( vecs[2][0],vecs[2][1],vecs[2][2], 1.0f));
                                new_block.vertices[13] = (Eigen::Vector4f ( vecs[3][0],vecs[3][1],vecs[3][2], 1.0f));
                                new_block.vertices[14] = (Eigen::Vector4f ( vecs[6][0],vecs[6][1],vecs[6][2], 1.0f));
                                new_block.vertices[15] = (Eigen::Vector4f ( vecs[7][0],vecs[7][1],vecs[7][2], 1.0f));
                                // Right Face
                                new_block.vertices[16] = (Eigen::Vector4f ( vecs[1][0],vecs[1][1],vecs[1][2], 1.0f));
                                new_block.vertices[17] = (Eigen::Vector4f ( vecs[2][0],vecs[2][1],vecs[2][2], 1.0f));
                                new_block.vertices[18] = (Eigen::Vector4f ( vecs[5][0],vecs[5][1],vecs[5][2], 1.0f));
                                new_block.vertices[19] = (Eigen::Vector4f ( vecs[6][0],vecs[6][1],vecs[6][2], 1.0f));
                                // Left Face
                                new_block.vertices[20] = (Eigen::Vector4f ( vecs[0][0],vecs[0][1],vecs[0][2], 1.0f));
                                new_block.vertices[21] = (Eigen::Vector4f ( vecs[3][0],vecs[3][1],vecs[3][2], 1.0f));
                                new_block.vertices[22] = (Eigen::Vector4f ( vecs[4][0],vecs[4][1],vecs[4][2], 1.0f));
                                new_block.vertices[23] = (Eigen::Vector4f ( vecs[7][0],vecs[7][1],vecs[7][2], 1.0f));


				Eigen::Vector3f topNormal    = (vecs[5] - vecs[4]).cross(vecs[7] - vecs[4]);
				//std::cout << topNormal << std::endl;
				Eigen::Vector3f bottomNormal = (vecs[3] - vecs[0]).cross(vecs[1] - vecs[0]);
				//std::cout << bottomNormal << std::endl;
				Eigen::Vector3f frontNormal  = (vecs[1] - vecs[0]).cross(vecs[4] - vecs[0]);
				//std::cout << frontNormal << std::endl;
				Eigen::Vector3f backmNormal  = (vecs[3] - vecs[2]).cross(vecs[6] - vecs[2]);
				//std::cout << backmNormal << std::endl;
				Eigen::Vector3f rightNormal  = (vecs[2] - vecs[1]).cross(vecs[5] - vecs[1]);
				//std::cout << rightNormal << std::endl;
				Eigen::Vector3f leftNormal   = (vecs[4] - vecs[0]).cross(vecs[3] - vecs[0]);
				//std::cout << leftNormal << std::endl;

				// Care about the type: GL_DOUBLE or GL_FLOAT
				//  Top Face
				new_block.normals[0] = (Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f));
				new_block.normals[1] = (Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f));
				new_block.normals[2] = (Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f));
				new_block.normals[3] = (Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f));
					// Bottom Face
				new_block.normals[4] = (Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f));
				new_block.normals[5] = (Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f));
				new_block.normals[6] = (Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f));
				new_block.normals[7] = (Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f));
					// Front Face
				new_block.normals[8] = (Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f));
				new_block.normals[9] = (Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f));
				new_block.normals[10] = (Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f));
				new_block.normals[11] = (Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f));
					// Back Face
				new_block.normals[12] = (Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f));
				new_block.normals[13] = (Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f));
				new_block.normals[14] = (Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f));
				new_block.normals[15] = (Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f));
					// Right Face
				new_block.normals[16] = (Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f));
				new_block.normals[17] = (Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f));
				new_block.normals[18] = (Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f));
				new_block.normals[19] = (Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f));
					// Left Face
				new_block.normals[20] = (Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f));
				new_block.normals[21] = (Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f));
				new_block.normals[22] = (Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f));
				new_block.normals[23] = (Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f));

				    // Top Face
				new_block.focus[0] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[1] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[2] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[3] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				    // Bottom Face
				new_block.focus[4] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[5] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[6] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[7] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				    // Front Face
				new_block.focus[8] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[9] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[10] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[11] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				    // Back Face
				new_block.focus[12] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[13] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[14] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[15] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				    // Right Face
				new_block.focus[16] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[17] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[18] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[19] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				    // Left Face
				new_block.focus[20] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[21] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[22] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));
				new_block.focus[23] = (Eigen::Vector4f ( 1.0,1.0,1.0,1.0f));

				new_block.AABB.fromPointCloud( new_block.vertices.begin() , new_block.vertices.end() );

				this->box_v2 = this->box_v2 + new_block.AABB;

			}else
			{
				new_block.valid = false;
			}

			new_block.static_porperties.resize(static_porperties.size());

			for ( std::size_t property = 0;  property < static_porperties.size();  ++property )
			{
				new_block.static_porperties[property].name  	     = static_porperties[property].name;
				new_block.static_porperties[property].unit  	     = static_porperties[property].unit;
				new_block.static_porperties[property].variable_name  = static_porperties[property].variable_name;
				new_block.static_porperties[property].component      = static_porperties[property].component;
				new_block.static_porperties[property].value_	     = static_porperties[property].values_[i];

			}

			new_block.dynamic_properties.resize(dynamic_properties.size());

			for ( std::size_t property = 0;  property < dynamic_properties.size();  ++property )
			{

				new_block.dynamic_properties[property].name  	     = dynamic_properties[property].name;
				new_block.dynamic_properties[property].unit  	     = dynamic_properties[property].unit;
				new_block.dynamic_properties[property].variable_name = dynamic_properties[property].variable_name;
				new_block.dynamic_properties[property].component     = dynamic_properties[property].component;

				for ( size_t  it = 0 ; it < dynamic_properties[property].values_.size() ; ++it)
				{
					new_block.dynamic_properties[property].values_.push_back( std::pair<int,float>( dynamic_properties[property].values_[it].first, dynamic_properties[property].values_[it].second[property] ) ) ;
				}

			}

			this->blocks.push_back(new_block);

		}
	}



} /* namespace ires */
