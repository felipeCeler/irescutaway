/*
 * Cube.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#include "Cube.hpp"


Cube::Cube ( )
{
	verticesSlot	= 0;

	colorSlot	= 1;

	isCreated_ = 0;

}

bool Cube::isCreated ( )
{
	return isCreated_;
}

void Cube::creatBuffers( )
{

	// Is there an OpenGL Context ?
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File Cube.cpp line 29" );
	// Cube ///////////////////////////////////////////////////////////////////////
		//    v6----- v5
		//   /|      /|
		//  v1------v0|
		//  | |     | |
		//  | |v7---|-|v4
		//  |/      |/
		//  v2------v3

		// Vertex coords array for glDrawArrays() =====================================
		// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
		// of 36 vertices (6 sides * 2 tris * 3 vertices = 36 vertices). And, each
		// vertex is 3 components (x,y,z) of floats, therefore, the size of vertex
		// array is 108 floats (36 * 3 = 108).
		GLfloat g_vertex_buffer_data[] = {
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f
		};

		// One color for each vertex. They were generated randomly.
		GLfloat g_color_buffer_data[] = {
			0.583f,  0.771f,  0.014f,
			0.609f,  0.115f,  0.436f,
			0.327f,  0.483f,  0.844f,
			0.822f,  0.569f,  0.201f,
			0.435f,  0.602f,  0.223f,
			0.310f,  0.747f,  0.185f,
			0.597f,  0.770f,  0.761f,
			0.559f,  0.436f,  0.730f,
			0.359f,  0.583f,  0.152f,
			0.483f,  0.596f,  0.789f,
			0.559f,  0.861f,  0.639f,
			0.195f,  0.548f,  0.859f,
			0.014f,  0.184f,  0.576f,
			0.771f,  0.328f,  0.970f,
			0.406f,  0.615f,  0.116f,
			0.676f,  0.977f,  0.133f,
			0.971f,  0.572f,  0.833f,
			0.140f,  0.616f,  0.489f,
			0.997f,  0.513f,  0.064f,
			0.945f,  0.719f,  0.592f,
			0.543f,  0.021f,  0.978f,
			0.279f,  0.317f,  0.505f,
			0.167f,  0.620f,  0.077f,
			0.347f,  0.857f,  0.137f,
			0.055f,  0.953f,  0.042f,
			0.714f,  0.505f,  0.345f,
			0.783f,  0.290f,  0.734f,
			0.722f,  0.645f,  0.174f,
			0.302f,  0.455f,  0.848f,
			0.225f,  0.587f,  0.040f,
			0.517f,  0.713f,  0.338f,
			0.053f,  0.959f,  0.120f,
			0.393f,  0.621f,  0.362f,
			0.673f,  0.211f,  0.457f,
			0.820f,  0.883f,  0.371f,
			0.982f,  0.099f,  0.879f
		};

		verticesSlot	= 0;
		colorSlot	= 1;

		/// Requesting Vertex Buffers to the GPU
		glGenBuffers ( 1 , &vertexBuffer_cube_vertices );
		glGenBuffers ( 1 , &vertexBuffer_cube_colors );


		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_vertices );
		glBufferData ( GL_ARRAY_BUFFER , 108 * sizeof(GLfloat) , g_vertex_buffer_data , GL_STATIC_DRAW );

		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_colors );
		glBufferData ( GL_ARRAY_BUFFER , 108 * sizeof(GLfloat) , g_color_buffer_data , GL_STATIC_DRAW );

		glBindBuffer ( GL_ARRAY_BUFFER , 0 );

		/// Requesting Vertex Array to the GPU
		glGenVertexArrays ( 1 , &vertexArray_for_the_Cube);

		glBindVertexArray(vertexArray_for_the_Cube);

			glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_vertices );
			glEnableVertexAttribArray(verticesSlot);
			glVertexAttribPointer(verticesSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_colors );
			glEnableVertexAttribArray(colorSlot);
			glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		isCreated_ = 1;


}

void Cube::draw()
{

	// 1rst attribute buffer : vertices

	glBindVertexArray(vertexArray_for_the_Cube);
		
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 36); // 12*3 indices starting at 0 -> 12 triangles

	glBindVertexArray(0);

}

Cube::~Cube ( )
{

	// TODO Auto-generated destructor stub

	glDeleteBuffers ( 1 , &vertexBuffer_cube_vertices );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_colors );

	glDeleteVertexArrays(1, &vertexArray_for_the_Cube);

	isCreated_ = 0;

}


