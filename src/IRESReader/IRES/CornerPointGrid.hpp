/*
 * CornerPointGrid.hpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 */

#ifndef CORNERPOINTGRID_HPP_
#define CORNERPOINTGRID_HPP_


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

// Celer Framework
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>

// IRES Classes
#include <IRES/Block.hpp>

/// Ires Library
#include <ires/ires.h>

namespace IRES
{

	//! Corner Point Geometry and Properties from - � Computer Modelling Group | CMG.
	/*!
	 * What is Corner Point Grid ...
	 *
	 * What the meaning of this class ...
	 *
	 * WTF I'm just read ...
	 */

	// LORE - Its data come from *.ires
	//
	class CornerPointGrid
	{


		public:


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


			struct Static_Property
			{

					std::string name;
					std::string unit;
					std::string variable_name;
					std::string component;

					// FIXME Just values of each property for each block.
					std::vector<float> values_;
			};

			// Size it's the number_of_Blocks * number_of_Time_Steps * number_of_Dynamic_Properties
			struct Dynamic_Property
			{

					std::string name;
					std::string unit;
					std::string variable_name;
					std::string component;
					// From Ires.dynamic_ name/units/variable_name/components

					// FIXME std::pair< timeStep, values for each block >
					std::vector<std::pair<int, std::vector<float> > > values_;

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


			TheHeader 				header_;

			std::vector<int>	 		number_of_fault_lines;

			std::vector<CornerPointGrid::Dynamic_Property> 	dynamic_properties;
			std::vector<CornerPointGrid::Static_Property> 	static_porperties;

			// Well Data
			std::vector<std::string> 		well_names;
			std::vector<std::string> 		well_status;
			std::vector<std::string> 		well_types;

			std::vector<std::string> 		well_properties_names;
			std::vector<std::string> 		well_properties_units;
			std::vector<std::string> 		well_properties_components;

			std::vector<Well_Data> 			wells;

			std::vector<Celer::Vector3<double> > 	vertices;

			std::vector<int> 			block_indices;

			std::vector<IRES::Block>                blocks;

			Celer::BoundingBox3<double> box;

			ires::IresHeader header_v2_;

		public:

			CornerPointGrid ( );

			virtual ~CornerPointGrid ( );

			void openIRES_Version_2 ( const std::string& filename );

			void openIRES ( const std::string& filename );


	};



} /* namespace IRES */
#endif /* CORNERPOINTGRID_HPP_ */
