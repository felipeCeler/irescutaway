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

					std::vector<float> min_;
					std::vector<float> max_;

					// FIXME std::pair< timeStep, values for each block >
					std::vector< std::vector<float>  > values_;

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


			// Properties values Interface for static
                                std::string static_name   [2];
                                std::size_t static_indices[2];

                                float static_min[2];
                                float static_max[2];

                                int current_static;

			// Properties values Interface for dynamic
                                std::string dynamic_name   [7];
                                std::size_t dynamic_indices[7];

                                float dynamic_min[7];
                                float dynamic_max[7];

                                int current_dynamic;
                                int current_dynamic_time_step;

                        // Property Interface

                        int setupType;             // Static or Dynamic
                        int currentProperty;	   // Static or Dynamic
                        int currentTimeStep;	   // Only for Dynamic

                        bool showFault;		   // Fault visibility
                        bool showBorderLine;        // Border Line Visibility

			void setupStatic ( );
			void setupDynamic ( );

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


			std::vector<float> cuboidProperties;    /// Four property x = Bubble Point Pressure
			GLuint vertexBufferCuboidProperties;	///               y = Pressure
								///	          z = Porosity
								///               w = Modified Block Volume
			std::vector<float> cuboidDynamic;
			GLuint vertexBufferCuboidDynamic;
			void loadDynamic ( int property_index );

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
