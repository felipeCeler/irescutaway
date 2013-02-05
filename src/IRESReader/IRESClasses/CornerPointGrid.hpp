/*
 * CornerPointGrid.hpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 */

#ifndef CORNERPOINTGRID_HPP_
#define CORNERPOINTGRID_HPP_

#include <string>

namespace ires
{

	//! Corner Point Geometry and Properties from - © Computer Modelling Group | CMG.
	/*!
	 * What is Corner Point Grid ...
	 *
	 * What the meaning of this class ...
	 */

	// LORE - Its data come from *.ires
	//
	class CornerPointGrid
	{

		private:

			struct IRESHeader
			{

			};

		public:
			CornerPointGrid ( const std::string& filename );
			virtual ~CornerPointGrid ( );
	};



} /* namespace ires */
#endif /* CORNERPOINTGRID_HPP_ */
