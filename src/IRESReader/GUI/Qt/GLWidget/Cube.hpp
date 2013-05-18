/*
 * Cube.hpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#ifndef CUBE_HPP_
#define CUBE_HPP_

#include <vector>
#include <algorithm>
/// Celer::Libraries

#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>
/// Celer::OpenGL Wrapper
#include <Celer/OpenGLFramework/OpenGLContext.hpp>

class Cube
{
	public:
		Cube ( );
		virtual ~Cube ( );
		void createBuffers ( );
		bool isCreated();
		void draw ( );

		std::vector<Celer::Vector3<GLfloat> > 	bb_vertices;
		std::vector<GLuint >			bb_indices;

		std::vector<Celer::Vector3<GLfloat> > 	vertices;
		std::vector<Celer::Vector3<GLfloat> > 	normals;
		std::vector<Celer::Vector3<GLfloat> > 	colors;
		std::vector<GLuint > 			indices;

		Celer::BoundingBox3<GLfloat> 		box;

		void create24Vertices ();
		void draw24Vertices();
		void createQuadStrips ();
		void drawQuadStrips();
		void create8VerticesIndices ();
		void draw8Vertices();
		void create8Bufffers();
		void draw8Buffers();

		void createBoundingBox();
		void drawBoundingBox();


	private:

		bool isCreated_;
		/////
		GLuint uniformBuffer_matrices_;
		//////
		GLuint vertexArray_for_the_Cube_;
		////// Vertex Buffers
			GLuint vertexBuffer_cube_24vertices_;
			GLuint vertices_24slot_;
			GLuint vertexBuffer_cube_24normals_;
			GLuint normals_24slot_;
			GLuint vertexBuffer_cube_24colors_;
			GLuint colors_24slot_;
			GLuint vertexBuffer_cube_24verticesIndices_;

			GLuint vertexBuffer_cube_quadStrip_;
			GLuint vertices_quadStripSlot_;
			GLuint vertexBuffer_cube_quadStripNormals_;
			GLuint normals_quadStripSlot_;
			GLuint vertexBuffer_cube_quadStripColors_;
			GLuint colors_quadStripSlot_;

			GLuint vertexBuffer_cube_8vertices_;
			GLuint vertices_8slot_;
			GLuint vertexBuffer_cube_8verticesIndices_;

			GLuint vertexBuffer_boundingBox_vertices_;
			GLuint boundingBox_slot_;

			GLuint vertexBuffer_8Buffers_;
			GLuint vertices_0_8buffer_slot_;
			GLuint vertices_1_8buffer_slot_;
			GLuint vertices_2_8buffer_slot_;
			GLuint vertices_3_8buffer_slot_;
			GLuint vertices_4_8buffer_slot_;
			GLuint vertices_5_8buffer_slot_;
			GLuint vertices_6_8buffer_slot_;
			GLuint vertices_7_8buffer_slot_;



};

#endif /* CUBE_HPP_ */
