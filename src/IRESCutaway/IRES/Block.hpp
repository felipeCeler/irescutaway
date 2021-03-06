/*
 * Block.hpp
 *
 *  Created on: Apr 23, 2013
 *      Author: felipe
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

// Standard Library
#include <string>
#include <vector>

namespace IRES
{

	class Block
	{
		public:


			struct Static_Property
			{

					std::string name;
					std::string unit;
					std::string variable_name;
					std::string component;

					// FIXME Just values of each property for each block.
					float value_;
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
					std::vector<std::pair<int, float > >  values_;

			};


			Block ( );
			Block ( const Block& new_block );

			// 24 vertices of the cuboid. Set to use One Pass Wireframe Rendering (@Jonatas Shaders)
			std::vector<float> vertices;

			// To draw using glDrawElements*
			std::vector<int>	 	     indices;
			// FIXME Crazy structure on Corner Point
			std::vector<float>   IJK;

			bool 				     valid;

			std::vector<Block::Dynamic_Property> 	     dynamic_properties;
			std::vector<Block::Static_Property> 	     static_porperties;

			int id ( ) const { return id_; }
			void setIdentification ( int id ) { id_ = id; };

			virtual ~Block ( );
		private:

			int id_;


	};

} /* namespace IRES */
#endif /* BLOCK_HPP_ */
