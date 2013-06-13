/*
 * Block.hpp
 *
 *  Created on: Apr 23, 2013
 *      Author: felipe
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include <vector>
#include <string>

// Celer Framework
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>

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
			std::vector< Celer::Vector4<float> > vertices;
			// 24 normals for each vertex
			std::vector< Celer::Vector4<float> > normals;
			// 24 colors per vertex
			std::vector< Celer::Vector4<float> > colors;
			// Primary and Secondary approach
			std::vector< Celer::Vector4<float> > focus;
			// To draw using glDrawElements*
			std::vector<int>	 	     indices;
			// FIXME Crazy structure on Corner Point
			std::vector< Celer::Vector4<int> >   IJK;

			bool 				     valid;

			Celer::BoundingBox3<float> 	     AABB;

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
