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

					int numTimeSteps;

					// FIXME std::pair< timeStep, values for each block >
					std::vector< std::vector<float>  > values_;
                                        // FIXME
                                        // Taking advantage of glVertex(X)Pointer to switch between GL_ARRAY_BUFFER Object
                                        // https://www.opengl.org/wiki/Vertex_Specification

                                        // Vertex Buffer to each time step
                                        std::vector< GLuint > ids;


			};


			std::vector<CornerPointGrid::Dynamic_Property>  dynamic_properties;

			std::vector<CornerPointGrid::Static_Property >  static_porperties;

			ires::IresHeader header_v2_;

		public:

			CornerPointGrid 		( );

			virtual ~CornerPointGrid 	( );

			void 	createBuffers 	 	( );
			void 	drawFace  	 	( ) const;
			void 	drawCuboid 	 	( ) const;

			void 	loadStaticProperties 	( );
			void 	loadDynamicProperties  	( );

			// Properties values Interface for static
                                std::string static_name   [3];
                                std::size_t static_indices[3];

                                float static_min[3];
                                float static_max[3];

                                int current_static;

			// Properties values Interface for dynamic

                                int current_dynamic;
                                int current_dynamic_time_step;

                        // Property Interface

                        int setupType;             // Static or Dynamic
                        int currentProperty;	   // Static or Dynamic
                        int currentTimeStep;	   // Only for Dynamic

                        int  showFault;		   // Fault visibility
                        bool showBorderLine;       // Border Line Visibility

			void setupStatic ( );
			void setupDynamic ( );

			bool isOpen_;

			bool isOpen ( ) const;

			// My crazy implementation
			void openIRES ( const std::string& filename );

		public:


			bool isInitialized;

			std::size_t number_of_blocks_;

			ires::Ires reservoir_file;   // IRES Version 2.1

			/// About IRES Geometry Information

			// Cube in Interleaved VertexBuffer
			// Use same layout location as vertexArray

			GLuint vertexArrayCuboids;   	 	// 1 Vertex Array

				std::vector<float> cuboids;
				GLuint vertexBufferCuboidGeometry; // 1 Vertex Buffer with 8 vertex which define a Cuboid

				std::vector<float> cuboidStatic;   /// property x = Modified Block Volume
				GLuint vertexBufferCuboidStatic;   ///          y = Porosity

				std::vector<std::vector<GLuint> > cuboidDynamicIds;
				std::vector<float> cuboidDynamic;

			std::size_t 	   cuboidCount;

			/// -- Shell / Fracture Geometry

			// Face in Interleaved VertexBuffer
			// Use same layout location as vertexArray

			GLuint vertexArrayFaces;		/// 1 Vertex Array

				std::vector<float> faces;
				GLuint 		   vertexBufferFaceGeometry;


				std::vector<float> faceType; 	   /// vec4 ( isShell, isFault, isBorder , 0.0 );
				GLuint 		   vertexBufferFaceType;

				GLuint 		   vertexBufferFaceStatic; /// property x = Modified Block Volume
				std::vector<float> faceStatic;             ///          y = Porosity

				std::vector<std::vector<GLuint> > faceDynamicIds;
				std::vector<float> faceDynamic;

			std::vector<ires::Face>   iresFaces_;
			std::size_t 		  faceCount;

	};

} /* namespace IRES */
#endif /* CORNERPOINTGRID_HPP_ */
