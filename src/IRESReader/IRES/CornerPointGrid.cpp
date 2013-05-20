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

				//std::cout << " dynamic names: " << dynamic_properties[i].name << std::endl;

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

			inFile.read ( (char*) &vertices[0] , sizeof(Celer::Vector3<double>) * header_.number_of_Vertices );

			// Copying Indices
			block_indices.clear ( );
			block_indices.resize ( header_.number_of_Blocks_Doubles );

			inFile.read ( (char*) &block_indices[0] , sizeof(int) * header_.number_of_Blocks_Doubles );

			// Copying Dynamic Properties for each Time Step.

			// FIXME Temporary array of values.
			std::vector<float> blocks_values;

			//dynamic_properties_values.resize(header_.number_of_Dynamic_Properties );
			// For each time step ....
			for ( int name_index = 0; name_index < header_.number_of_Dynamic_Properties; name_index++ )
			{

				// For each dynamic properties ...
				for ( int time_index = 0; time_index < header_.number_of_Time_Steps; time_index++ )
				{

					// Read data for each block !
					blocks_values.resize ( header_.number_of_Blocks );

					inFile.read ( (char*) &blocks_values[0] , sizeof(float) * header_.number_of_Blocks );

					dynamic_properties[name_index].values_.push_back ( std::make_pair ( time_index , blocks_values ) );

					//std::cout << " Time Step : " << time_index <<  "Blocks : "<< blocks_values[0] << std::endl;

					blocks_values.clear ( );

				}

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

			box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );

			for ( std::size_t  i = 0; i < vertices.size ( ) ; i++)
			{
				vertices[i] -= box.center();
				vertices[i].x /= (box.diagonal()/10);
				vertices[i].y /= (box.diagonal()/10);
				vertices[i].z /= (box.diagonal()/10);
				//std::cout << " Vector " <<  vertices[i] << std::endl;
			}

			box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );

			std::size_t i = 0;

			// To hold new Modelos Blocks;
			blocks.clear();
			//blocks.resize( header_.number_of_Blocks );

			int property_index = 0;

			while ( i < block_indices.size ( ) )
			{
				if ( block_indices[i] != -1)
				{

					IRES::Block new_block;

					/// From Nicole`s presentation
					Celer::Vector4<int> IJK ( (  (property_index) % header_.number_of_Blocks_in_I_Direction) + 1,
								  ( ((property_index) / header_.number_of_Blocks_in_I_Direction) % header_.number_of_Blocks_in_J_Direction ) +1,
								  (  (property_index) / (header_.number_of_Blocks_in_I_Direction * header_.number_of_Blocks_in_J_Direction )) +1,
								  0 );


					Celer::Vector4<int> IJKs [] =
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
					Celer::Vector3<float> v0( static_cast<float>(vertices[index[0]].x), static_cast<float>(vertices[index[0]].y), static_cast<float>(vertices[index[0]].z));
					Celer::Vector3<float> v1( static_cast<float>(vertices[index[1]].x), static_cast<float>(vertices[index[1]].y), static_cast<float>(vertices[index[1]].z));
					Celer::Vector3<float> v3( static_cast<float>(vertices[index[2]].x), static_cast<float>(vertices[index[2]].y), static_cast<float>(vertices[index[2]].z));
					Celer::Vector3<float> v2( static_cast<float>(vertices[index[3]].x), static_cast<float>(vertices[index[3]].y), static_cast<float>(vertices[index[3]].z));
					// Bottom Face
					Celer::Vector3<float> v4( static_cast<float>(vertices[index[4]].x), static_cast<float>(vertices[index[4]].y), static_cast<float>(vertices[index[4]].z));
					Celer::Vector3<float> v7( static_cast<float>(vertices[index[5]].x), static_cast<float>(vertices[index[5]].y), static_cast<float>(vertices[index[5]].z));
					Celer::Vector3<float> v5( static_cast<float>(vertices[index[6]].x), static_cast<float>(vertices[index[6]].y), static_cast<float>(vertices[index[6]].z));
					Celer::Vector3<float> v6( static_cast<float>(vertices[index[7]].x), static_cast<float>(vertices[index[7]].y), static_cast<float>(vertices[index[7]].z));

					Celer::Vector4<float> vertices [] =
					{
					    // Top Face
					    Celer::Vector4<float> ( v0, 1.0f),
					    Celer::Vector4<float> ( v1, 1.0f),
					    Celer::Vector4<float> ( v3, 1.0f),
					    Celer::Vector4<float> ( v2, 1.0f),
					    // Bottom Face
					    Celer::Vector4<float> ( v4, 1.0f),
					    Celer::Vector4<float> ( v7, 1.0f),
					    Celer::Vector4<float> ( v5, 1.0f),
					    Celer::Vector4<float> ( v6, 1.0f),
					    // Front Face
					    Celer::Vector4<float> ( v0, 1.0f),
					    Celer::Vector4<float> ( v7, 1.0f),
					    Celer::Vector4<float> ( v3, 1.0f),
					    Celer::Vector4<float> ( v4, 1.0f),
					    // Back Face
					    Celer::Vector4<float> ( v1, 1.0f),
					    Celer::Vector4<float> ( v2, 1.0f),
					    Celer::Vector4<float> ( v6, 1.0f),
					    Celer::Vector4<float> ( v5, 1.0f),
					    // Right Face
					    Celer::Vector4<float> ( v2, 1.0f),
					    Celer::Vector4<float> ( v3, 1.0f),
					    Celer::Vector4<float> ( v5, 1.0f),
					    Celer::Vector4<float> ( v4, 1.0f),
					    // Left Face
					    Celer::Vector4<float> ( v0, 1.0f),
					    Celer::Vector4<float> ( v1, 1.0f),
					    Celer::Vector4<float> ( v7, 1.0f),
					    Celer::Vector4<float> ( v6, 1.0f)
					};


					Celer::Vector3<float> topNormal 	= ((v1 - v0) ^ (v3 - v0)).norm();
					//std::cout << topNormal << std::endl;
					Celer::Vector3<float> bottomNormal 	= ((v7 - v4) ^ (v5 - v4)).norm();
					//std::cout << bottomNormal << std::endl;
					Celer::Vector3<float> frontNormal 	= ((v7 - v0) ^ (v3 - v0)).norm();
					//std::cout << frontNormal << std::endl;
					Celer::Vector3<float> backmNormal 	= ((v2 - v1) ^ (v6 - v1)).norm();
					//std::cout << backmNormal << std::endl;
					Celer::Vector3<float> rightNormal 	= ((v3 - v2) ^ (v5 - v2)).norm();
					//std::cout << rightNormal << std::endl;
					Celer::Vector3<float> leftNormal 	= ((v1 - v0) ^ (v7 - v0)).norm();
					//std::cout << leftNormal << std::endl;
	//
	//
					// Care about the type: GL_DOUBLE or GL_FLOAT
					Celer::Vector4<float> normals[] =
					{
						//  Top Face
						Celer::Vector4<float> ( topNormal, 1.0f),
						Celer::Vector4<float> ( topNormal, 1.0f),
						Celer::Vector4<float> ( topNormal, 1.0f),
						Celer::Vector4<float> ( topNormal, 1.0f),
						// Bottom Face
						Celer::Vector4<float> ( bottomNormal, 1.0f),
						Celer::Vector4<float> ( bottomNormal, 1.0f),
						Celer::Vector4<float> ( bottomNormal, 1.0f),
						Celer::Vector4<float> ( bottomNormal, 1.0f),
						// Front Face
						Celer::Vector4<float> ( frontNormal, 1.0f),
						Celer::Vector4<float> ( frontNormal, 1.0f),
						Celer::Vector4<float> ( frontNormal, 1.0f),
						Celer::Vector4<float> ( frontNormal, 1.0f),
						// Back Face
						Celer::Vector4<float> ( backmNormal, 1.0f),
						Celer::Vector4<float> ( backmNormal, 1.0f),
						Celer::Vector4<float> ( backmNormal, 1.0f),
						Celer::Vector4<float> ( backmNormal, 1.0f),
						// Right Face
						Celer::Vector4<float> ( rightNormal, 1.0f),
						Celer::Vector4<float> ( rightNormal, 1.0f),
						Celer::Vector4<float> ( rightNormal, 1.0f),
						Celer::Vector4<float> ( rightNormal, 1.0f),
						// Left Face
						Celer::Vector4<float> ( leftNormal, 1.0f),
						Celer::Vector4<float> ( leftNormal, 1.0f),
						Celer::Vector4<float> ( leftNormal, 1.0f),
						Celer::Vector4<float> ( leftNormal, 1.0f)
					};



					Celer::Vector4<float> focus [] =
					{
					    // Top Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    // Bottom Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    // Front Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    // Back Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    // Right Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    // Left Face
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f),
					    Celer::Vector4<float> ( 1.0,1.0,1.0,1.0f)
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

						for ( std::vector<std::pair<int, std::vector<float> > >::iterator it = dynamic_properties[property].values_.begin() ; it != dynamic_properties[property].values_.end() ; ++it)
						{
							new_block.dynamic_properties[property].values_.push_back( std::make_pair<int,float>( it->first,it->second[i] ) ) ;
						}

					}

					blocks.push_back(new_block);

					i += 8;
					property_index += 1;

				}  // end of looping list of blocks
				else
				{
					i += 1;
					property_index += 1;
				}

			}

			std::cout << " Size of List of Blocks " << blocks.size ( ) << std::endl;

			inFile.close ( );
		}





	}
} /* namespace ires */
