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


			std::vector<CornerPointGrid::Dynamic_Property> dynamic_properties;
			std::vector<CornerPointGrid::Static_Property> static_porperties;

			std::vector<ires::U32> list_of_vertex_indice;
			std::vector<ires::F32> list_of_vertex_geometry_a;
			std::vector<ires::F32> list_of_vertex_geometry_b;
			std::vector<ires::F32> list_of_vertex_geometry_c;
			std::vector<ires::F32> list_of_vertex_geometry_d;
			std::vector<ires::F32> list_of_vertex_color;
			std::vector<ires::U32> list_of_block_id;
			std::vector<ires::U32> list_of_block_flag;

			ires::IresHeader header_v2_;

			std::vector<ires::F32> list_of_vertex_geometry_charles;

			/// About IRES Geometry Information

			// Cube in Interleaved VertexBuffer
			// Use same layout location as vertexArray

			// 1 Vertex Array
			GLuint vertexArray_cuboids;
			// 1 Vertex Buffer with 8 vertex which define a Cuboid
			GLuint vertexBuffer_cuboid_geometry;

			std::vector<float> cubeColor;
			/// vec4 (R, G, B , 0 );
			GLuint vertexBuffer_cube_color;
			std::vector<float> cubeIJK;
			/// vec4 (I, J, K , 0 );
			GLuint vertexBuffer_cube_IJK;
			std::vector<float> cubeFocus;
			/// vec4 (Primary/Secondary , Active  ,0.0 , 0.0);
			GLuint vertexBuffer_cube_Focus;

			std::vector<float> cubeProperties;
			// Four property x = Bubble Point Pressure
			//               y = Pressure
			//	         z = Porosity
			//               w = Modified Block Volume
			GLuint vertexBuffer_cube_properties;

			std::vector<float> cuboids;
			GLint cuboids_size;

			/// -- Shell / Fracture Geometry

			// Face in Interleaved VertexBuffer
			// Use same layout location as vertexArray

			// 1 Vertex Array
			GLuint vertexArray_faces;
			// 1 Vertex Buffer
			GLuint vertexBuffer_face_geometry;

			std::vector<float> facesColor;
			/// vec4 (R, G, B , 0 );
			GLuint vertexBuffer_face_color;
			std::vector<float> facesIJK;
			/// vec4 (R, G, B , 0 );
			GLuint vertexBuffer_face_IJK;
			std::vector<float> facesType;
			/// vec4 ( isShell, isFault, 0.0 , 0.0 );
			GLuint vertexBuffer_face_type;

			std::vector<float> facesProperty;
			// Four property x = Bubble Point Pressure
			//               y = Pressure
			//	         z = Porosity
			//               w = Modified Block Volume
			GLuint vertexBuffer_face_properties;

			std::vector<float> faces;
			GLint faces_size;

			bool isInitialized;

			std::size_t number_of_blocks_;

			ires::Ires reservoir_file;

		public:

			CornerPointGrid ( );

			virtual ~CornerPointGrid ( );

			void createBuffers ( );

			void drawFace ( ) const;
			void drawCuboid ( ) const;

			void loadProperties ( );

			std::string properties_name[4];
			std::size_t indices[4];
			float min_value[4];
			float max_value[4];
			int current_property;

			bool isOpen_;

			bool isOpen ( ) const;

			// My crazy implementation
			void openIRES ( const std::string& filename );

	};

} /* namespace IRES */
#endif /* CORNERPOINTGRID_HPP_ */
