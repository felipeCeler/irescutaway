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


		id_ = -1;

	}

	Block::Block ( const Block& new_block )
	{
	 	// TODO Auto-generated constructor stub

		id_ = id();

		vertices = new_block.vertices;
		normals  = new_block.normals;
		colors   = new_block.colors;
		focus 	 = new_block.focus;
		indices  = new_block.indices;
		IJK      = new_block.IJK;

		AABB      = new_block.AABB;

		static_porperties  = new_block.static_porperties;
		dynamic_properties = new_block.dynamic_properties;



	}

	Block::~Block ( )
	{
		// TODO Auto-generated destructor stub
	}

} /* namespace IRES */
