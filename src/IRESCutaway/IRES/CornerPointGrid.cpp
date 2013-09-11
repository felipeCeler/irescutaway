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
		std::cout << std::setw(50) << std::left << "Number of Time Steps : " << header_v2_.numTimesteps << std::endl;
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

 		}

		float 			v[24];
		Eigen::Vector3f vecs[8];

		this->blocks.clear( );
		this->blocks.reserve( header_.number_of_Blocks );
		        std::vector<int>                    IJKS(24);
		        std::vector<Eigen::Vector4f > vertices(24);
		        std::vector<Eigen::Vector4f > normals(24);
		        std::vector<Eigen::Vector4f > focus(24);

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


//		for ( int i = 0 ; i < 1000 ; i++)
//		{
//			std::cout << list_of_vertex_indice[i] <<  " - ";;
//		}
//
//		std::cout << "Size of Blocks Ids " << *std::max_element(list_of_block_id.begin(),list_of_block_id.end()) << std::endl;
//
//		std::cout << "Size of Blocks Ids IRES " << header_.number_of_Blocks << std::endl;

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


				Eigen::Vector3f topNormal  =	(vecs[5] - vecs[4]).cross(vecs[7] - vecs[4]);
				//std::cout << topNormal << std::endl;
				Eigen::Vector3f bottomNormal 	= (vecs[3] - vecs[0]).cross(vecs[1] - vecs[0]);
				//std::cout << bottomNormal << std::endl;
				Eigen::Vector3f frontNormal 	= (vecs[1] - vecs[0]).cross(vecs[4] - vecs[0]);
				//std::cout << frontNormal << std::endl;
				Eigen::Vector3f backmNormal 	= (vecs[3] - vecs[2]).cross(vecs[6] - vecs[2]);
				//std::cout << backmNormal << std::endl;
				Eigen::Vector3f rightNormal 	= (vecs[2] - vecs[1]).cross(vecs[5] - vecs[1]);
				//std::cout << rightNormal << std::endl;
				Eigen::Vector3f leftNormal 	= (vecs[4] - vecs[0]).cross(vecs[3] - vecs[0]);
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

//				new_block.AABB.fromPointCloud( new_block.vertices.begin() , new_block.vertices.end() );
//
//				this->box_v2 = this->box_v2 + new_block.AABB;

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

		}// for
//                time (&end);
//                double diff = difftime (end,start);
//
//                std::cout << " IRESv2 Reading Done " << diff << " seconds. "  << std::endl;
//
//		time_t bstart, bend;
//                time (&bstart);
//
//		Celer::BoundingBox3<float> AABB;
//
//		if ( this->box_v2.diagonal() > 10.0f)
//		{
//		        std::cout << " Scaling the Model to make diagonal its bounding box about 10.0f " << std::endl;
//			// Scaling
//
//			for ( std::size_t  i = 0; i < this->blocks.size ( ) ; i++)
//			{
//				//std::cout << " Vector " <<  i << std::endl;
//				if ( this->blocks[i].valid )
//				{
//					for ( std::size_t b = 0; b < this->blocks[i].vertices.size ( ); b++ )
//					{
//						this->blocks[i].vertices[b] -=  Eigen::Vector4f (this->box_v2.center ( ),0.0);
//						this->blocks[i].vertices[b].x /= ( this->box_v2.diagonal ( ) * 0.1f );
//						this->blocks[i].vertices[b].y /= ( this->box_v2.diagonal ( ) * 0.1f );
//						this->blocks[i].vertices[b].z /= ( this->box_v2.diagonal ( ) * 0.1f );
//						//std::cout << " Vector " <<  this->blocks[i].vertices.size ( ) << std::endl;
//					}
//
//					AABB.fromPointCloud( this->blocks[i].vertices.begin() , this->blocks[i].vertices.end() );       //std::cout << " Vector " <<  vertices[i] << std::endl;
//					this->blocks[i].AABB = AABB;
//				}
//			}
//
//			std::cout << " Vector " <<  this->box_v2.diagonal() << std::endl;
//			this->box_v2.reset( );
//			this->box_v2 = AABB;
//			std::cout << " Vector " <<  this->box_v2.diagonal() << std::endl;
//			std::cout << " Vector " <<  this->box_v2.center() << std::endl;
//		}
//
//		time (&bend);
//		double bdiff = difftime (bend,bstart);
//		std::cout << " New BB " << bdiff << " seconds. "  << std::endl;
	}

	void CornerPointGrid::openIRES ( const std::string& filename )
	{
		std::ifstream inFile;

		inFile.open ( filename.c_str ( ) , std::ios_base::binary );

		if ( inFile.fail ( ) )
		{
			std::cout << " Error! maybe the file doesn't exist " << std::endl;
		}
		else if ( !inFile )
		{
			std::cout << " While opening a file an error is encountered " << std::endl;
		}
		else  // Let's ready this piece of "cake".
		{

			header_ = TheHeader();

			std::cout << " File is successfully opened " << std::endl;

			inFile.read ( (char*) &header_ , sizeof ( header_ ) );

			std::cout << std::setfill ( '-' ) << std::setw ( 55 ) << "-" <<  std::endl;

			std::cout << std::setw(50) << std::left << "Version : " << std::setiosflags(std::ios::right) << header_.ires_version << std::endl;
			std::cout << std::setw(50) << std::left << "First Title : " << header_.first_title_ << std::endl;
			std::cout << std::setw(50) << std::left << "Second Title : " << header_.second_title_ << std::endl;
			std::cout << std::setw(50) << std::left << "Third Title :  " << header_.thirt_title_ << std::endl;
			std::cout << std::setw(50) << std::left << "Case ID : " << header_.caseID << std::endl;
			std::cout << std::setw(50) << std::left << "Simulation Type :" << header_.simulation_type << std::endl;
			std::cout << std::setw(50) << std::left << "Run date : " << header_.run_date << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Time Steps : " << header_.number_of_Time_Steps << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks : " << header_.number_of_Blocks << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Dynamic : " << header_.number_of_Dynamic_Properties << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Static : " << header_.number_of_Static_Properties << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Wells : " << header_.number_of_Wells << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Vertices : " << header_.number_of_Vertices << std::endl;
			//
			std::cout << std::setw(50) << std::left << "Number of Double Blocks :" << header_.number_of_Blocks_Doubles << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in I direction :" << header_.number_of_Blocks_in_I_Direction << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in J direction :" << header_.number_of_Blocks_in_J_Direction << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Blocks in K direction :" << header_.number_of_Blocks_in_K_Direction << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Fault Surfaces :" << header_.number_of_Fault_Surfaces << std::endl;
			std::cout << std::setw(50) << std::left << "Number of Well Properties :" << header_.number_of_Well_Properties << std::endl;
			std::cout << std::setw(50) << std::left << "well Property Size ? :" << header_.well_Properties_Size << std::endl;

			std::cout << std::setfill ( '-' ) << std::setw ( 55 ) << "-" << std::endl;

			// -- Copying Fault Lines
			number_of_fault_lines.clear ( );
			number_of_fault_lines.resize ( header_.number_of_Blocks_in_K_Direction );

			inFile.read ( (char*) &number_of_fault_lines[0] , ( sizeof(int) * number_of_fault_lines.size ( ) ) );

			// --- Copying Dynamic Properties
			std::string data;
			data.resize ( 64 );
			dynamic_properties.clear ( );
			dynamic_properties.resize ( header_.number_of_Dynamic_Properties );

			// --- Copying Dynamic Names

			for ( int i = 0; i < header_.number_of_Dynamic_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				dynamic_properties[i].name = std::string ( data.begin ( ) , data.end ( ) );

				std::cout << " dynamic names: " << dynamic_properties[i].name << std::endl;

				data.clear ( );
				data.resize ( 64 );

			}

			// --- Copying Dynamic Units

			for ( int i = 0; i < header_.number_of_Dynamic_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				dynamic_properties[i].unit = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " dynamic units: " << dynamic_properties[i].unit << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}
			// --- Copying Dynamic Variables

			for ( int i = 0; i < header_.number_of_Dynamic_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				dynamic_properties[i].variable_name = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " dynamic variables: " << dynamic_properties[i].variable_name << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}
			// --- Copying Dynamic Variables

			for ( int i = 0; i < header_.number_of_Dynamic_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				dynamic_properties[i].component = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " dynamic components: " << dynamic_properties[i].component << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Static Properties

			static_porperties.clear ( );
			static_porperties.resize ( header_.number_of_Static_Properties );

			// --- Copying Static Names - ||||||||||||||||||||||||||||||||||||||||||||||||||

			for ( int i = 0; i < header_.number_of_Static_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				data.erase ( std::remove_if ( data.begin ( ) , data.end ( ) , ::iscntrl ) , data.end ( ) );

				static_porperties[i].name = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " static names: " << static_porperties[i].name << std::endl;

				data.clear ( );
				data.resize ( 64 );

			}

			// --- Copying Static Units

			for ( int i = 0; i < header_.number_of_Static_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				static_porperties[i].unit = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " static units: " << static_porperties[i].unit << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}
			// --- Copying Static Variables

			for ( int i = 0; i < header_.number_of_Static_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				static_porperties[i].variable_name = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " static variables: " << static_porperties[i].variable_name << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Static Component

			for ( int i = 0; i < header_.number_of_Static_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				static_porperties[i].component = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " static components: " << static_porperties[i].component << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			//

			// --- Copying Well Names and Properties - ||||||||||||||||||||||||||||||||||||||||||||||||||
			well_names.clear ( );
			well_names.resize ( header_.number_of_Wells );

			for ( int i = 0; i < header_.number_of_Wells; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_names[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well names: " << well_names[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );

			}

			// --- Well Status
			well_status.clear ( );
			well_status.resize ( header_.number_of_Wells );

			for ( int i = 0; i < header_.number_of_Wells; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_status[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well status: " << well_status[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Well Types
			well_types.clear ( );
			well_types.resize ( header_.number_of_Wells );

			for ( int i = 0; i < header_.number_of_Wells; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_types[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well types: " << well_types[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Well Properties Names
			well_properties_names.clear ( );
			well_properties_names.resize ( header_.number_of_Well_Properties );

			for ( int i = 0; i < header_.number_of_Well_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_properties_names[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well properties names: " << well_properties_names[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Well Properties Units
			well_properties_units.clear ( );
			well_properties_units.resize ( header_.number_of_Well_Properties );

			for ( int i = 0; i < header_.number_of_Well_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_properties_units[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well properties units: " << well_properties_units[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// --- Copying Well Properties Components
			well_properties_components.clear ( );
			well_properties_components.resize ( header_.number_of_Well_Properties );

			for ( int i = 0; i < header_.number_of_Well_Properties; i++ )
			{

				inFile.read ( &data[0] , sizeof(char[64]) );

				well_properties_components[i] = std::string ( data.begin ( ) , data.end ( ) );

				//std::cout << " well properties components: " << well_properties_components[i] << std::endl;

				data.clear ( );
				data.resize ( 64 );
			}

			// Copying Vertices data
			vertices.clear ( );
			vertices.resize ( header_.number_of_Vertices );

			inFile.read ( (char*) &vertices[0] , sizeof(Eigen::Vector3d) * header_.number_of_Vertices );

			// Copying Indices
			block_indices.clear ( );
			block_indices.resize ( header_.number_of_Blocks_Doubles );

			inFile.read ( (char*) &block_indices[0] , sizeof(int) * header_.number_of_Blocks_Doubles );

			// Copying Dynamic Properties for each Time Step.

			// FIXME Temporary array of values.
			std::vector<float> blocks_values;

			//dynamic_properties_values.resize(header_.number_of_Dynamic_Properties );
			// For each time step //....
			for ( int name_index = 0; name_index < header_.number_of_Dynamic_Properties; name_index++ )
			{

				// For each dynamic properties ...
				for ( int time_index = 0; time_index < header_.number_of_Time_Steps; time_index++ )
				{

					// Read data for each block !
					blocks_values.resize ( header_.number_of_Blocks );

					inFile.read ( (char*) &blocks_values[0] , sizeof(float) * header_.number_of_Blocks );

					dynamic_properties[name_index].values_.push_back ( std::make_pair ( time_index , blocks_values ) );

					blocks_values.clear ( );

				}

			}

			for ( int time_index = 0; time_index < header_.number_of_Time_Steps; time_index++ )
				{std::cout << " Time Step : " << time_index <<  "Blocks : "<<
				dynamic_properties[0].name << " min : " <<
				*std::min_element ( dynamic_properties[0].values_[time_index].second.begin ( ) , dynamic_properties[0].values_[time_index].second.end ( ) ) << std::endl;
				std::cout << " Time Step : " << 14 <<  "Blocks : "<<
				dynamic_properties[0].name << " min : " <<
				*std::max_element ( dynamic_properties[0].values_[time_index].second.begin ( ) , dynamic_properties[0].values_[time_index].second.end ( ) ) << std::endl;
				std::cout << " Time Step : " << std::endl;
			}

			//std::cout << " Position Static: " << inFile.tellg ( ) << std::endl;

			// Copying Static Properties

			// For each static property
			for ( int name_index = 0; name_index < header_.number_of_Static_Properties; name_index++ )
			{
				blocks_values.resize ( header_.number_of_Blocks );

				inFile.read ( (char*) &blocks_values[0] , sizeof(float) * header_.number_of_Blocks );

				//std::cout << " Property: " << static_porperties[name_index].name << " max : " << *std::max_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
				//std::cout << " Property: " << static_porperties[name_index].name << " min : " << *std::min_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
				static_porperties[name_index].values_.resize ( header_.number_of_Blocks );
				std::copy ( blocks_values.begin ( ) , blocks_values.end ( ) , static_porperties[name_index].values_.begin ( ) );

				blocks_values.clear ( );
			}


		}


		/// Filling the list with real Blocks ( block indice different of -1 )


		if ( block_indices.size ( ) > 0 )
		{

//			box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );
//
//			for ( std::size_t  i = 0; i < vertices.size ( ) ; i++)
//			{
//				vertices[i] -= box.center();
//				vertices[i].x /= (box.diagonal()/10);
//				vertices[i].y /= (box.diagonal()/10);
//				vertices[i].z /= (box.diagonal()/10);
//				//std::cout << " Vector " <<  vertices[i] << std::endl;
//			}
//
//			box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );

			std::size_t i = 0;

			// To hold new Modelos Blocks;
			blocks.clear();
			//blocks.resize( header_.number_of_Blocks );

			int property_index = 0;

			while ( i < block_indices.size ( ) )
			{
				IRES::Block new_block;

				if ( block_indices[i] != -1)
				{

					/// From Nicole`s presentation
					Eigen::Vector4f IJK ( (  (property_index) % header_.number_of_Blocks_in_I_Direction) + 1,
								  ( ((property_index) / header_.number_of_Blocks_in_I_Direction) % header_.number_of_Blocks_in_J_Direction ) +1,
								  (  (property_index) / (header_.number_of_Blocks_in_I_Direction * header_.number_of_Blocks_in_J_Direction )) +1,
								  0 );


					Eigen::Vector4f IJKs [] =
					{

					     IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK, IJK,IJK,IJK,IJK ,IJK,IJK,IJK,IJK

					};


					int index [] =
					{
					    // Top Face
					    block_indices[i+0],block_indices[i+1],block_indices[i+3],block_indices[i+2],/* 0 - 5*/
					    // Bottom Face
					    block_indices[i+4],block_indices[i+7],block_indices[i+5],block_indices[i+6],/* 6 - 11 */
					    // Front Face
					    block_indices[i+0],block_indices[i+7],block_indices[i+3],block_indices[i+4],/* 12 - 17*/
					    // Back Face
					    block_indices[i+1],block_indices[i+2],block_indices[i+6],block_indices[i+5],/* 18 - 23*/
					    // Right Face
					    block_indices[i+2],block_indices[i+3],block_indices[i+5],block_indices[i+4],/* 24 - 29*/
					    // Left Face
					    block_indices[i+0],block_indices[i+1],block_indices[i+7],block_indices[i+6] /* 30 - 35*/
					};

					// Top Face
					Eigen::Vector3f v0( static_cast<float>(vertices[index[0]][0]), static_cast<float>(vertices[index[0]][1]), static_cast<float>(vertices[index[0]][2]));
					Eigen::Vector3f v1( static_cast<float>(vertices[index[1]][0]), static_cast<float>(vertices[index[1]][1]), static_cast<float>(vertices[index[1]][2]));
					Eigen::Vector3f v3( static_cast<float>(vertices[index[2]][0]), static_cast<float>(vertices[index[2]][1]), static_cast<float>(vertices[index[2]][2]));
					Eigen::Vector3f v2( static_cast<float>(vertices[index[3]][0]), static_cast<float>(vertices[index[3]][1]), static_cast<float>(vertices[index[3]][2]));
					// Bottom Face
					Eigen::Vector3f v4( static_cast<float>(vertices[index[4]][0]), static_cast<float>(vertices[index[4]][1]), static_cast<float>(vertices[index[4]][2]));
					Eigen::Vector3f v7( static_cast<float>(vertices[index[5]][0]), static_cast<float>(vertices[index[5]][1]), static_cast<float>(vertices[index[5]][2]));
					Eigen::Vector3f v5( static_cast<float>(vertices[index[6]][0]), static_cast<float>(vertices[index[6]][1]), static_cast<float>(vertices[index[6]][2]));
					Eigen::Vector3f v6( static_cast<float>(vertices[index[7]][0]), static_cast<float>(vertices[index[7]][1]), static_cast<float>(vertices[index[7]][2]));

					Eigen::Vector4f vertices [] =
					{
					    // Top Face
					    Eigen::Vector4f ( v0[0],v0[1],v0[2], 1.0f),
					    Eigen::Vector4f ( v1[0],v1[1],v1[2], 1.0f),
					    Eigen::Vector4f ( v3[0],v3[1],v3[2], 1.0f),
					    Eigen::Vector4f ( v2[0],v2[1],v2[2], 1.0f),
					    // Bottom Face
					    Eigen::Vector4f ( v4[0],v4[1],v4[2], 1.0f),
					    Eigen::Vector4f ( v7[0],v7[1],v7[2], 1.0f),
					    Eigen::Vector4f ( v5[0],v5[1],v5[2], 1.0f),
					    Eigen::Vector4f ( v6[0],v6[1],v6[2], 1.0f),
					    // Front Face
					    Eigen::Vector4f ( v0[0],v0[1],v0[2], 1.0f),
					    Eigen::Vector4f ( v7[0],v7[1],v7[2], 1.0f),
					    Eigen::Vector4f ( v3[0],v3[1],v3[2], 1.0f),
					    Eigen::Vector4f ( v4[0],v4[1],v4[2], 1.0f),
					    // Back Face
					    Eigen::Vector4f ( v1[0],v1[1],v1[2], 1.0f),
					    Eigen::Vector4f ( v2[0],v2[1],v2[2], 1.0f),
					    Eigen::Vector4f ( v6[0],v6[1],v6[2], 1.0f),
					    Eigen::Vector4f ( v5[0],v5[1],v5[2], 1.0f),
					    // Right Face
					    Eigen::Vector4f ( v2[0],v2[1],v2[2], 1.0f),
					    Eigen::Vector4f ( v3[0],v3[1],v3[2], 1.0f),
					    Eigen::Vector4f ( v5[0],v5[1],v5[2], 1.0f),
					    Eigen::Vector4f ( v4[0],v4[1],v4[2], 1.0f),
					    // Left Face
					    Eigen::Vector4f ( v0[0],v0[1],v0[2], 1.0f),
					    Eigen::Vector4f ( v1[0],v1[1],v1[2], 1.0f),
					    Eigen::Vector4f ( v7[0],v7[1],v7[2], 1.0f),
					    Eigen::Vector4f ( v6[0],v6[1],v6[2], 1.0f)
					};


					Eigen::Vector3f topNormal 	= (v1 - v0).cross(v3 - v0);
					//std::cout << topNormal << std::endl;
					Eigen::Vector3f bottomNormal 	= (v7 - v4).cross(v5 - v4);
					//std::cout << bottomNormal << std::endl;
					Eigen::Vector3f frontNormal 	= (v7 - v0).cross(v3 - v0);
					//std::cout << frontNormal << std::endl;
					Eigen::Vector3f backmNormal 	= (v2 - v1).cross(v6 - v1);
					//std::cout << backmNormal << std::endl;
					Eigen::Vector3f rightNormal 	= (v3 - v2).cross(v5 - v2);
					//std::cout << rightNormal << std::endl;
					Eigen::Vector3f leftNormal 	= (v1 - v0).cross(v7 - v0);
					//std::cout << leftNormal << std::endl;
	//
	//
					// Care about the type: GL_DOUBLE or GL_FLOAT
					Eigen::Vector4f normals[] =
					{
						//  Top Face
						Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f),
						Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f),
						Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f),
						Eigen::Vector4f ( topNormal[0],topNormal[1],topNormal[2], 1.0f),
						// Bottom Face
						Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f),
						Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f),
						Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f),
						Eigen::Vector4f ( bottomNormal[0],bottomNormal[1],bottomNormal[2], 1.0f),
						// Front Face
						Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f),
						Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f),
						Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f),
						Eigen::Vector4f ( frontNormal[0],frontNormal[1],frontNormal[2], 1.0f),
						// Back Face
						Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f),
						Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f),
						Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f),
						Eigen::Vector4f ( backmNormal[0],backmNormal[1],backmNormal[2], 1.0f),
						// Right Face
						Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f),
						Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f),
						Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f),
						Eigen::Vector4f ( rightNormal[0],rightNormal[1],rightNormal[2], 1.0f),
						// Left Face
						Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2],1.0f),
						Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f),
						Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f),
						Eigen::Vector4f ( leftNormal[0],leftNormal[1],leftNormal[2], 1.0f)
					};



					Eigen::Vector4f focus [] =
					{
					    // Top Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    // Bottom Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    // Front Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    // Back Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    // Right Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    // Left Face
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f),
					    Eigen::Vector4f ( 1.0,1.0,1.0,1.0f)
					};


					std::copy ( vertices 	, vertices + 24 , std::back_inserter ( new_block.vertices  ) );
					std::copy ( normals 	, normals  + 24 , std::back_inserter ( new_block.normals) );
					std::copy ( index 	, index    + 24 , std::back_inserter ( new_block.indices   ) );
					std::copy ( focus 	, focus    + 24 , std::back_inserter ( new_block.focus) );
					std::copy ( IJKs 	, IJKs     + 24 , std::back_inserter ( new_block.IJK  ) );

					new_block.setIdentification ( property_index );

					new_block.static_porperties.resize(static_porperties.size());

					for ( std::size_t property = 0;  property < static_porperties.size();  ++property )
					{

						new_block.static_porperties[property].name  	   = static_porperties[property].name;
						new_block.static_porperties[property].unit  	   = static_porperties[property].unit;
						new_block.static_porperties[property].variable_name  = static_porperties[property].variable_name;
						new_block.static_porperties[property].component      = static_porperties[property].component;
						new_block.static_porperties[property].value_	   = static_porperties[property].values_[property_index];

					}

					new_block.dynamic_properties.resize(dynamic_properties.size());

					for ( std::size_t property = 0;  property < dynamic_properties.size();  ++property )
					{

//						std::cout << "new_block.dynamic_properties.name : " << dynamic_properties[property].name << std::endl;
//						std::cout << "new_block.dynamic_properties.unit : " << dynamic_properties[property].unit << std::endl;
//						std::cout << "new_block.dynamic_properties.variable_name : " << dynamic_properties[property].variable_name << std::endl;
//						std::cout << "new_block.dynamic_properties.component : " << dynamic_properties[property].component << std::endl;

						new_block.dynamic_properties[property].name  	     = dynamic_properties[property].name;
						new_block.dynamic_properties[property].unit  	     = dynamic_properties[property].unit;
						new_block.dynamic_properties[property].variable_name = dynamic_properties[property].variable_name;
						new_block.dynamic_properties[property].component     = dynamic_properties[property].component;

						for ( size_t  it = 0 ; it < dynamic_properties[property].values_.size() ; ++it)
						{
							new_block.dynamic_properties[property].values_.push_back( std::pair<int,float>( dynamic_properties[property].values_[it].first, dynamic_properties[property].values_[it].second[property] ) ) ;
						}

					}

					new_block.valid = true;


					i += 8;
					property_index += 1;

				}  // end of looping list of blocks
				else
				{

					new_block.setIdentification ( property_index );

					new_block.static_porperties.resize(static_porperties.size());

					for ( std::size_t property = 0;  property < static_porperties.size();  ++property )
					{

						new_block.static_porperties[property].name  	     = static_porperties[property].name;
						new_block.static_porperties[property].unit  	     = static_porperties[property].unit;
						new_block.static_porperties[property].variable_name  = static_porperties[property].variable_name;
						new_block.static_porperties[property].component      = static_porperties[property].component;
						new_block.static_porperties[property].value_	     = static_porperties[property].values_[property_index];

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


					i += 1;
					property_index += 1;

				}

				blocks.push_back(new_block);


			}

			std::cout << " Size of List of Blocks " << blocks.size ( ) << std::endl;

			inFile.close ( );
		}





	}
} /* namespace ires */
