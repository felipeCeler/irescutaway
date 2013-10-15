/*
 * CornerPointGrid.hpp
 *
 *  Created on: Jan 30, 2013
 *      Author: felipe
 */

#ifndef CORNERPOINTGRID_HPP_
#define CORNERPOINTGRID_HPP_

/// OpenGL
#include <GL/glew.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

#include <cctype>
#include <ctime>

// IRES Classes
#include <IRES/Block.hpp>

/// Ires Library
#include <ires/ires.h>

namespace IRES
{

	// LORE - Its data come from *.ires

	class CornerPointGrid
	{

		public:

			struct Static_Property
			{

					std::string name;
					std::string unit;
					std::string variable_name;
					std::string component;

					float min_;
					float max_;

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


			std::vector<CornerPointGrid::Dynamic_Property>  dynamic_properties;

			std::vector<CornerPointGrid::Static_Property >  static_porperties;

			ires::IresHeader header_v2_;

		public:

			CornerPointGrid ( );

			virtual ~CornerPointGrid ( );

			void 	createBuffers 	 ( );
			void 	drawFace  	 ( ) const;
			void 	drawCuboid 	 ( ) const;
			void 	loadProperties 	 ( );


			// Properties Values Interface
			std::string properties_name[4];

			std::size_t property_indices[4];

			float min_value[4];
			float max_value[4];

			int current_property;

			bool isOpen_;

			bool isOpen ( ) const;

			// My crazy implementation
			void openIRES ( const std::string& filename );

		public:

			/// About IRES Geometry Information

			// Cube in Interleaved VertexBuffer
			// Use same layout location as vertexArray


			GLuint vertexArrayCuboids;   	 	// 1 Vertex Array
								// 1 Vertex Buffer with 8 vertex which define a Cuboid
			GLuint vertexBufferCuboidGeometry;

			std::vector<float> cubeColor;           /// vec4 (R, G, B , 0 );
			GLuint vertexBufferCuboidColor;

			std::vector<float> cuboidIJK;           /// vec4 (I, J, K , 0 );
			GLuint vertexBufferCuboidIJK;

			std::vector<float> cuboidFocus;         /// vec4 (Primary/Secondary , Active  ,0.0 , 0.0);
			GLuint vertexBufferCuboidFocus;

			std::vector<float> cuboidProperties;    /// Four property x = Bubble Point Pressure
			GLuint vertexBufferCuboidProperties;	///               y = Pressure
								///	          z = Porosity
								///               w = Modified Block Volume


			std::vector<float> cuboids;
			std::size_t 	   cuboidCount;

			/// -- Shell / Fracture Geometry

			// Face in Interleaved VertexBuffer
			// Use same layout location as vertexArray

			GLuint vertexArrayFaces;		/// 1 Vertex Array


			GLuint vertexBufferFaceGeometry;

			std::vector<float> faceColor;		/// vec4 (R, G, B , 0 );
			GLuint vertexBufferFaceColor;

			std::vector<float> faceIJK;		/// vec4 (R, G, B , 0 );
			GLuint vertexBufferFaceIJK;

			std::vector<float> faceType; 		/// vec4 ( isShell, isFault, 0.0 , 0.0 );
			GLuint vertexBufferFaceType;


			std::vector<float> faceProperty;	/// Four property x = Bubble Point Pressure
								///               y = Pressure
								///	          z = Porosity
								///               w = Modified Block Volume
			GLuint vertexBufferFaceProperties;

			std::vector<float> 	  faces;

			std::vector<ires::Face>   iresFaces_;
			std::size_t 		  faceCount;


			bool isInitialized;

			std::size_t number_of_blocks_;

			ires::Ires reservoir_file;

	};

} /* namespace IRES */
#endif /* CORNERPOINTGRID_HPP_ */
