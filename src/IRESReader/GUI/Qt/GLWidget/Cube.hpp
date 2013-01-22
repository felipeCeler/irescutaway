/*
 * Cube.hpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

/// Celer::Libraries
	/// OpenGL
#include <Celer/OpenGLFramework/OpenGLContext.hpp>

class Cube
{
	public:
		Cube ( );
		virtual ~Cube ( );
		void creatBuffers ( );
		bool isCreated();
		void draw ( );

	private:

		bool isCreated_;
		//////
		GLuint vertexArray_for_the_Cube;
		////// Vertex Buffers
			GLuint vertexBuffer_cube_vertices;
			GLuint verticesSlot;
			GLuint vertexBuffer_cube_colors;
			GLuint colorSlot;

};

#endif /* GEOMETRY_HPP_ */
