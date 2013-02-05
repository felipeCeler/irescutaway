/*
 * Cube.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#include "Cube.hpp"

#include <Celer/Core/Geometry/Math/Vector3.hpp>

Cube::Cube ( )
{

	isCreated_ = 0;
	/// Vertex Array for the cube
	vertexArray_for_the_Cube = 0;
		/// Buffer Object which hold the data.
		vertexBuffer_cube_vertices = 0;
		vertexBuffer_cube_indices  = 0;
		/// Vertex location
		verticesSlot	= 0;
		/// Color location
		colorSlot	= 1;

}

bool Cube::isCreated ( )
{
	return isCreated_;
}

void Cube::creatBuffers( )
{

	// Is there an OpenGL Context ?
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File Cube.cpp line 39" );
	// Cube ///////////////////////////////////////////////////////////////////////
	//    v2----- v1
	//   /|      /|
	//  v3------v0|
	//  | |     | |
	//  | |v6---|-|v7
	//  |/      |/
	//  v5------v4

	// Vertex coords array for glDrawArrays() =====================================
	// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
	// of 8 vertices and 12 triangle difined by indices. And, each vertex is 3 components
	// (x,y,z) of floats, therefore, the size of vertex array is 8 * 3 * sizeof(float).


//		GLfloat g_vertex_buffer_data[] =
//		{
//		    // X Y Z R G B
//		       3.0f, 3.0f, 3.0f, 1.0, 0.0, 0.0, // vertex 0
//		       3.0f, 3.0f,-3.0f, 1.0, 0.0, 0.0, // vertex 1
//		      -3.0f, 3.0f,-3.0f, 0.0, 0.0, 1.0, // vertex 2
//		      -3.0f, 3.0f, 3.0f, 0.0, 1.0, 0.0, // vertex 3
//
//		       3.0f,-3.0f, 3.0f, 1.0, 0.0, 0.0, // vertex 4
//		      -3.0f,-3.0f, 3.0f, 0.0, 1.0, 0.0, // vertex 5
//		      -3.0f,-3.0f,-3.0f, 0.0, 0.0, 1.0, // vertex 6
//		       3.0f,-3.0f,-3.0f, 1.0, 0.0, 0.0, // vertex 7
//		}; // 4 vertices with 6 components (floats) each\

	float x = 5878424.7906;

	points.push_back ( Celer::Vector3<GLdouble> ( 2440863.6397 , 5878424.7906 , 1015.6089 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440938.7195 , 5878414.9469 , 969.9014 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440950.3564 , 5878486.1683 , 972.0026 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440875.6062 , 5878495.3373 , 1023.9014 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440863.6397 , 5878424.7906 , 1065.6089 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440938.7195 , 5878414.9469 , 1019.9014 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440950.3564 , 5878486.1683 , 1022.0026 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2440875.6062 , 5878495.3373 , 1073.9014 ) );
	points.push_back ( Celer::Vector3<GLdouble> ( 2441013.7993 , 5878405.1033 , 914.1412 ) );

//	points.push_back ( Celer::Vector3<GLdouble> ( x * 1.0f , x * 1.0f , x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> ( x * 1.0f , x * 1.0f ,-x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> (-x * 1.0f , x * 1.0f ,-x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> (-x * 1.0f , x * 1.0f , x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> ( x * 1.0f ,-x * 1.0f , x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> (-x * 1.0f ,-x * 1.0f , x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> (-x * 1.0f ,-x * 1.0f ,-x * 1.0f ) );
//	points.push_back ( Celer::Vector3<GLdouble> ( x * 1.0f ,-x * 1.0f ,-x * 1.0f ) );

	box.fromPointCloud ( points.begin ( ) , points.end ( ) );

	points.clear ( );

	points.push_back ( box.max ( ) );
	points.push_back ( Celer::Vector3<GLdouble> ( box.max ( ).x , box.max ( ).y , box.min ( ).z ) );
	points.push_back ( Celer::Vector3<GLdouble> ( box.min ( ).x , box.max ( ).y , box.min ( ).z ) );
	points.push_back ( Celer::Vector3<GLdouble> ( box.min ( ).x , box.max ( ).y , box.max ( ).z ) );

	points.push_back ( Celer::Vector3<GLdouble> ( box.max ( ).x , box.min ( ).y , box.max ( ).z ) );
	points.push_back ( Celer::Vector3<GLdouble> ( box.min ( ).x , box.min ( ).y , box.max ( ).z ) );
	points.push_back ( box.min ( ) );
	points.push_back ( Celer::Vector3<GLdouble> ( box.max ( ).x , box.min ( ).y , box.min ( ).z ) );

//	// 2 Triangle per face.
	GLuint g_vertex_indices[] =
	{
           // Top Face 		// Bottom
	   0, 1, 2, 2, 3, 0,    4, 5, 6, 6, 7, 4,

	   // Front 		// Back
	   0, 3, 5, 5, 4 , 0,   2, 1, 7, 7, 6, 2,

	   // Right   	 	// Left
	   0, 4, 7, 7, 1, 0,    2, 6, 5, 5, 3, 2
	};

	indices = std::vector<GLuint>(g_vertex_indices,g_vertex_indices+36);

	verticesSlot	= 0;

	glGenVertexArrays ( 1 , &vertexArray_for_the_Cube);
	glBindVertexArray(vertexArray_for_the_Cube);

		/// Requesting Vertex Buffers to the GPU
		glGenBuffers ( 1 , &vertexBuffer_cube_vertices );
			glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_vertices );
			glBufferData ( GL_ARRAY_BUFFER , points.size( ) * sizeof(points[0]) , &points[0] , GL_STATIC_DRAW );

		/// Requesting Indices
		glGenBuffers ( 1 , &vertexBuffer_cube_indices);
			glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_indices );
			glBufferData ( GL_ELEMENT_ARRAY_BUFFER , indices.size( ) * sizeof(indices[0]) , &indices[0] , GL_STATIC_DRAW );

	   // Set up generic attributes pointers
	glEnableVertexAttribArray(verticesSlot);
	GlVertexAttribPointer(verticesSlot, 3, GL_DOUBLE, GL_FALSE, 0, 0);


	glBindVertexArray(0);

	isCreated_ = 1;


}

void Cube::draw()
{

	// 1rst attribute buffer : vertices

	glBindVertexArray(vertexArray_for_the_Cube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_indices);
	// Draw the triangle !
	glDrawElements(GL_TRIANGLES, 36 , GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}

Cube::~Cube ( )
{

	// TODO Auto-generated destructor stub

	glDeleteBuffers ( 1 , &vertexBuffer_cube_vertices );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_indices );

	glDeleteVertexArrays(1, &vertexArray_for_the_Cube);

	isCreated_ = 0;

}


//glEnableVertexAttribArray(verticesSlot);
//glVertexAttribPointer(verticesSlot, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Celer::Vector3<GLfloat>), (char*)0 + 0*sizeof(GLfloat));
//
//glEnableVertexAttribArray(colorSlot);
//glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Celer::Vector3<GLfloat>), (char*)0 + 3*sizeof(GLfloat));
