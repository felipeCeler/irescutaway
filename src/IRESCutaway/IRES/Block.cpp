/*
 * Block.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: felipe
 */

#include "Block.hpp"

namespace IRES
{

	Block::Block ( )
	{
		// TODO Auto-generated constructor stub


		id_ 	= -1;
		valid 	= false;

	}

	Block::Block ( const Block& new_block )
	{
	 	// FIXME Always remember to do a copy constructor if you are
		//      planning to use the class in a container !!!


		id_ = id();

		vertices = new_block.vertices;
		indices  = new_block.indices;
		IJK      = new_block.IJK;

		
		valid 	 = new_block.valid;

		static_porperties = new_block.static_porperties;
		dynamic_properties = new_block.dynamic_properties;

	}

	Block::~Block ( )
	{
		// TODO Auto-generated destructor stub
	}

} /* namespace IRES */
