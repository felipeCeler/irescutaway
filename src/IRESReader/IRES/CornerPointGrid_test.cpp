/*
 * CornerPointGrid_test.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 *
 * g++ CornerPointGrid_test.cpp -I /media/d/Workspace/IRESReader/libs/Celer/src
 */


#ifndef CORNERPOINTGRID_TEST_CPP_
#define CORNERPOINTGRID_TEST_CPP_

#include "CornerPointGrid.hpp"

// Learning ifstream
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cstring>
#include <cctype>

// Tuple from C++
//#include <tuple> suport but the -std=gnuxx0x

// Celer Framework
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>


class Ires
{
	public:
		Ires ( )
		{

		};

		// Size it`s number_of_Blocks * number_of_Time_Steps * number_of_Dynamic_Properties
		struct Dynamic_Property
		{
//
//				Dynamic_Property( )
//				{
//
//				}
//
//				Dynamic_Property( const Dynamic_Property& new_dynamic_property )
//				{
//					name 		= new_dynamic_property.name;
//					unit 		= new_dynamic_property.unit;
//					variable_name 	= new_dynamic_property.variable_name;
//					component 	= new_dynamic_property.component;
//
//					values_.resize( new_dynamic_property.values_.size( ));
//					std::copy ( new_dynamic_property.values_.begin(),new_dynamic_property.values_.end(),values_.begin());
//				}

				std::string 	name;
				std::string 	unit;
				std::string 	variable_name;
				std::string 	component;
				// From Ires.dynamic_ name/units/variable_name/components

				// FIXME std::pair< timeStep, values for each block >
				std::vector<std::pair<int,std::vector<float> > > values_;

		};

		struct Static_Property
		{
				// From Ires.static_ name/units/variable_name/components

//				Static_Property ( )
//				{
//
//				}
//
//				Static_Property ( const Static_Property& new_static_property )
//				{
//					name = new_static_property.name;
//					unit = new_static_property.unit;
//					variable_name = new_static_property.variable_name;
//					component = new_static_property.component;
//
//					values_.resize( new_static_property.values_.size());
//					std::copy( new_static_property.values_.begin(),new_static_property.values_.end(),values_.begin());
//				}

				std::string name;
				std::string unit;
				std::string variable_name;
				std::string component;

				// FIXME Just values of each property for each block.
				std::vector<float> values_;
		};

		// TODO Really confused !! Now It's Totally confused.
		// Building the structure of the using Nicole's Presentation
		struct Well_Data
		{

				struct Perforated_Block
				{
						std::string name_;
						// Can be open, closed, etc ...
						std::string status_;
						// Index of the Blocks intersected by the Well line.
						int perforated_Block_Index_;

						bool has_entry_exit_;

						double entry_x_;
						double entry_y_;
						double entry_z_;

						double exit_x_;
						double exit_y_;
						double exit_z_;

						int I_;
						int J_;
						int K_;

						double lenght_;

						float fraction_;

				};


				std::string 	name_;
				std::string 	type_;
				int		well_number_;
				// Can be Producer or Injector


				// Number of Block which is intersected by the Well line.
				// FIXME Number of Entries ?
				int number_of_Perforated_Blocks_;

				// Control points of the Well. ( index of the block and 3D point )
				std::vector<std::pair<int,Celer::Vector3<double> >  > tractory_;

				// For each well, property and on a specific time, get the data.
				// FIXME That data comes from a perforated block.
				std::vector<std::pair<int, std::pair<int, float> > > values_;

				// FIXME How to ready that data
				// As Charles explain me. But still get hard !!

		};

		struct TheHeader
		{

				float ires_version;

				char first_title_[64];
				char second_title_[64];
				char thirt_title_[64];
				char caseID[64];
				char simulation_type[64];
				char run_date[64];

				int number_of_Time_Steps;
				int number_of_Blocks;
				int number_of_Dynamic_Properties;
				int number_of_Static_Properties;
				int number_of_Wells;
				int number_of_Vertices;
				int number_of_Blocks_Doubles; // What's the meaning of this.
				int number_of_Blocks_in_I_Direction;
				int number_of_Blocks_in_J_Direction;
				int number_of_Blocks_in_K_Direction;
				int number_of_Fault_Surfaces;
				int number_of_Well_Properties;
				int well_Properties_Size; // Wired.

		};


		TheHeader header_;

		std::vector<int> number_of_fault_lines;

		std::vector<Dynamic_Property>   dynamic_properties;

		std::vector<Static_Property>    static_porperties;

		std::vector<std::string> well_names;
		std::vector<std::string> well_status;
		std::vector<std::string> well_types;

		std::vector<std::string> well_properties_names;
		std::vector<std::string> well_properties_units;
		std::vector<std::string> well_properties_components;

		std::vector<Well_Data> 	 wells;

		std::vector<Celer::Vector3<double> > vertices;
		std::vector<int> blocks;

		void openIRES ( const std::string& filename )
		{

			std::ifstream inFile;

			inFile.open ( filename.c_str( ) , std::ios_base::binary );

			if ( inFile.fail ( ) )
			{
				std::cout << " error! maybe the file doesn't exist " << std::endl;
			}
			else if ( !inFile )
			{
				std::cout << "While opening a file an error is encountered" << std::endl;
			}
			else
			{
				std::cout << "File is successfully opened" << std::endl;


				inFile.read ( (char*) &header_ , sizeof ( header_ ) );

				std::cout << "Version :" << header_.ires_version << std::endl;
				std::cout << "Title 1 :" << header_.first_title_ << std::endl;
				std::cout << "Title 2 :" << header_.second_title_ << std::endl;
				std::cout << "Title 3 :" << header_.thirt_title_ << std::endl;
				std::cout << "CaseID :" << header_.caseID << std::endl;
				std::cout << "Simulation Type: " << header_.simulation_type << std::endl;
				std::cout << "Run date: " << header_.run_date << std::endl;
				std::cout << "numTimesteps " << header_.number_of_Time_Steps << std::endl;
				std::cout << "numBlocks " << header_.number_of_Blocks << std::endl;
				std::cout << "numDynamic " << header_.number_of_Dynamic_Properties << std::endl;
				std::cout << "numStatic " << header_.number_of_Static_Properties << std::endl;
				std::cout << "numWells " << header_.number_of_Wells << std::endl;
				std::cout << "numVertices " << header_.number_of_Vertices << std::endl;
//
				std::cout << "blockListLength " << header_.number_of_Blocks_Doubles << std::endl;
				std::cout << "numI " << header_.number_of_Blocks_in_I_Direction << std::endl;
				std::cout << "numJ " << header_.number_of_Blocks_in_J_Direction << std::endl;
				std::cout << "numK " << header_.number_of_Blocks_in_K_Direction << std::endl;
				std::cout << "numFaultSurfaces " << header_.number_of_Fault_Surfaces << std::endl;
				std::cout << "numWellProperties " << header_.number_of_Well_Properties << std::endl;
				std::cout << "wellPropSize " << header_.well_Properties_Size << std::endl;

				//		// -- Copying Fault Lines
				number_of_fault_lines.clear();
				number_of_fault_lines.resize ( header_.number_of_Blocks_in_K_Direction );

				inFile.read ( (char*) &number_of_fault_lines[0] , ( sizeof(int) * number_of_fault_lines.size ( ) ) );

//				for ( int i = 0; i < header_.number_of_Blocks_in_K_Direction; ++i )
//				{
//					std::cout << " fault lines: " << i << number_of_fault_lines[i] << std::endl;
//				}

				// --- Copying Dynamic Properties

				std::string data;
				data.resize ( 64 );
				dynamic_properties.clear();
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

				static_porperties.clear();
				static_porperties.resize ( header_.number_of_Static_Properties );

				// --- Copying Static Names - ||||||||||||||||||||||||||||||||||||||||||||||||||

				for ( int i = 0; i < header_.number_of_Static_Properties; i++ )
				{

					inFile.read ( &data[0] , sizeof(char[64]) );

					data.erase(std::remove_if(data.begin(), data.end(), ::iscntrl), data.end());

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
				well_names.clear();
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
				well_status.clear();
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
				well_types.clear();
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
				well_properties_names.clear();
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
				well_properties_units.clear();
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
				well_properties_components.clear();
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
				vertices.clear();
				vertices.resize ( header_.number_of_Vertices );

				inFile.read ( (char*) &vertices[0] , sizeof(Celer::Vector3<double>) * header_.number_of_Vertices );

//				for ( int i = 0; i < 10; ++i )
//				{
//					std::cout << vertices[i] << std::endl;
//				}

//				for ( int i = header_.number_of_Vertices - 10; i < header_.number_of_Vertices; ++i )
//				{
//					std::cout << vertices[i] << std::endl;
//				}

				//		double x;
				//
				//		for ( int i = 0; i < 24; i++ )
				//		{
				//			inFile.read ( (char *) &x , 8 );
				//
				//			std::cout << "----" << std::endl;
				//
				//			printf ( "\n%.4f\n" , (float)x );
				//
				//		}

				// Copying Block List


				std::vector<int> indices;
				indices.clear ( );
				blocks.clear ( );
				indices.resize( header_.number_of_Blocks_Doubles );
				blocks.resize( header_.number_of_Blocks_Doubles );

				inFile.read ( (char*) &blocks[0] , sizeof(int) * header_.number_of_Blocks_Doubles );

//				for ( int block_index = 0; block_index < header_.number_of_Blocks_Doubles; ++block_index )
//				{
//					if ( indices[block_index] == -1 )
//						continue;
//					blocks.push_back( indices[block_index] );
//				}
				//
				std::cout << " Size of indices : " << indices.size( ) << " | "<<blocks.size( ) << std::endl;
				//

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

					std::cout << " Prop: " << static_porperties[name_index].name << " max : " << *std::max_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
					std::cout << " Prop: " << static_porperties[name_index].name << " min : " << *std::min_element ( blocks_values.begin ( ) , blocks_values.end ( ) ) << std::endl;
					static_porperties[name_index].values_.resize ( header_.number_of_Blocks );
					std::copy( blocks_values.begin(), blocks_values.end( ),static_porperties[name_index].values_.begin());

					blocks_values.clear ( );
				}

				std::cout << " Position Well Property :" << inFile.tellg ( ) << std::endl;

				// The Hard part: Well Data.
				// First get the limits of well values: Number of points in the line, number of blocks intersected by the well the points data.
				wells.clear();
				wells.resize ( header_.number_of_Wells );

				for ( int prop_index = 0; prop_index < header_.number_of_Well_Properties; ++prop_index )
				{

					int entries = 0;

					inFile.read ( (char*) &entries , sizeof(int) );

					std::vector<float> values;

					values.resize ( header_.number_of_Wells * entries );

					inFile.read ( (char*) &values[0] , sizeof(float) * header_.number_of_Wells * entries );

					for ( int entry_index = 0; entry_index < entries; ++entry_index )
					{
						for ( int well_index = 0; well_index < header_.number_of_Wells; ++well_index )
						{
							wells[well_index].values_.push_back (
								std::make_pair ( prop_index, std::make_pair (entry_index ,values[ ( entry_index * header_.number_of_Wells ) + ( well_index )] ) ) );
						}
					}

				}

				std::cout << " Position Well Time Start:" << inFile.tellg ( ) << std::endl;

			}
			inFile.close ( );
		}



};
#endif /* CORNERPOINTGRID_TEST_CPP_ */

//int main ( int argc , char *argv[] )
//{
//
//	Ires model;
//
//	model.openIRES( "/media/d/Workspace/IRESReader/Files/Ires/emerald.ires" );
//
//	return 0;
//}

 /* namespace ires */
