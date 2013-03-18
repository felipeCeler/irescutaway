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
			std::cout << "While opening a file an error is encountered" << std::endl;
		}
		else  // Let's ready this piece of "cake".
		{
			std::cout << "File is successfully opened" << std::endl;

			inFile.read ( (char*) &header_ , sizeof ( header_ ) );

			std::cout << std::setfill ( '-' ) << std::setw ( 55 ) << "-" << std::endl;

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

				std::cout << " static names: " << static_porperties[i].name << std::endl;

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
			blocks.clear ( );
			blocks.resize ( header_.number_of_Blocks_Doubles );

			inFile.read ( (char*) &blocks[0] , sizeof(int) * header_.number_of_Blocks_Doubles );

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

			std::cout << " Position Static: " << inFile.tellg ( ) << std::endl;

			// Copying Static Properties

			// For each static property
			for ( int name_index = 0; name_index < header_.number_of_Static_Properties; name_index++ )
			{
				blocks_values.resize ( header_.number_of_Blocks );

				inFile.read ( (char*) &blocks_values[0] , sizeof(float) * header_.number_of_Blocks );

				std::cout << " Property: " << static_porperties[name_index].name << " max : " << *std::max_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
				std::cout << " Property: " << static_porperties[name_index].name << " min : " << *std::min_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
				static_porperties[name_index].values_.resize ( header_.number_of_Blocks );
				std::copy ( blocks_values.begin ( ) , blocks_values.end ( ) , static_porperties[name_index].values_.begin ( ) );

				blocks_values.clear ( );
			}


		}
		inFile.close ( );
	}





} /* namespace ires */
