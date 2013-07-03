/*
 * Cube.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#include "Cube.hpp"

Cube::Cube ( )
{

	isCreated_ = 0;
	/// Vertex Array for the cube
	vertexArray_for_the_Cube_ = 0;

	////// Vertex Buffers
		vertexBuffer_cube_24vertices_ = 0;
		vertices_24slot_ = 0;
		vertexBuffer_cube_24normals_ = 0;
		normals_24slot_ = 1;
		vertexBuffer_cube_24colors_ = 0;
		colors_24slot_ = 2;
		vertexBuffer_cube_24verticesIndices_ = 0;

		vertexBuffer_cube_quadStrip_ = 0;
		vertices_quadStripSlot_ = 3;
		vertexBuffer_cube_quadStripNormals_ = 0 ;
		normals_quadStripSlot_ = 4;
		vertexBuffer_cube_quadStripColors_ = 0;
		colors_quadStripSlot_ = 5;

		vertexBuffer_cube_8vertices_ = 0;
		vertices_8slot_ = 6;
		vertexBuffer_cube_8verticesIndices_ = 0;

		vertexBuffer_boundingBox_vertices_ = 0;
		boundingBox_slot_ = 10;

		vertexBuffer_TrianglesStripAdjacency_ = 0;
		vertices_TrianglesStripAdjacency_slot_ = 11;

}

bool Cube::isCreated ( )
{
	return isCreated_;
}

void Cube::create24Vertices ( const Celer::Vector3<float> translate )
{

	// Cube /////////////////////////////////////////////////////////////////////////////////////
	//    v2----- v1
	//   /|      /|
	//  v3------v0|
	//  | |     | |
	//  | |v6---|-|v7
	//  |/      |/
	//  v5------v4

	// Vertex coords array for glDrawArrays() ====================================================
	// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
	// of 8 vertices and 12 triangle defined by indices. And, each vertex is 3 components
	// (x,y,z) of floats, therefore, the size of vertex array in bytes is 8 * 3 * sizeof(GLfloat).

	vertices.clear();
	normals.clear();
	indices.clear();

	Celer::Vector3<GLfloat> vertex_data[] = {
		//  Top Face
		Celer::Vector3<GLfloat>(1.0f, 1.0f, -1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f, 1.0f) + translate,
		// Bottom Face
		Celer::Vector3<GLfloat>(1.0f, -1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, -1.0f, -1.0f)+ translate,
		// Front Face
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, -1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f, 1.0f)+ translate,
		// Back Face
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, -1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, -1.0f)+ translate,
		// Right Face
		Celer::Vector3<GLfloat>(1.0f, -1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(1.0f, -1.0f, 1.0f)+ translate,
		// Left Face
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, -1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, 1.0f)+ translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f)+ translate
	};


	Celer::Vector3<GLfloat> normal_data[] = {
		// Top Face
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		// Bottom Face
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		// Front Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		// Back Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		// Right Face
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		// Left Face
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f)

	};
	GLuint vertex_indices[] =
	{
		 0,  1,  2,
		 0,  2,  3,
		 4,  5,  6,
		 4,  6,  7,
		 8,  9,  10,
		 8,  10, 11,
		 12, 13, 14,
		 12, 14, 15,
		 16, 17, 18,
		 16, 18, 19,
		 20, 21, 22,
		 20, 22, 23
	};

	std::copy( vertex_data	 , vertex_data + 24	, std::back_inserter ( vertices ) );
	std::copy( normal_data	 , normal_data + 24	, std::back_inserter ( normals ) );
	std::copy( vertex_indices, vertex_indices + 36	, std::back_inserter ( indices ) );



	/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_cube_24vertices_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_24vertices_ );
		glBufferData ( GL_ARRAY_BUFFER , vertices.size( ) * sizeof(vertices[0]) , &vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(vertices_24slot_);
		glVertexAttribPointer(vertices_24slot_, 3, GL_FLOAT, GL_FALSE, 0, 0);
		/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_cube_24normals_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_24normals_ );
		glBufferData ( GL_ARRAY_BUFFER , normals.size( ) * sizeof(normals[0]) , &normals[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(normals_24slot_);
		glVertexAttribPointer(normals_24slot_, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers ( 1 , &vertexBuffer_cube_24colors_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_24colors_ );
		glBufferData ( GL_ARRAY_BUFFER , normals.size( ) * sizeof(normals[0]) , &normals[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(colors_24slot_);
		glVertexAttribPointer(colors_24slot_, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/// Requesting Indices
	glGenBuffers ( 1 , & vertexBuffer_cube_24verticesIndices_);
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_24verticesIndices_ );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER , indices.size( ) * sizeof(indices[0]) , &indices[0] , GL_STATIC_DRAW );



}

void Cube::createQuadStrips ( const Celer::Vector3<float> translate )
{


	// Cube /////////////////////////////////////////////////////////////////////////////////////
	//    v2----- v1
	//   /|      /|
	//  v3------v0|
	//  | |     | |
	//  | |v6---|-|v7
	//  |/      |/
	//  v5------v4

	// Vertex coords array for glDrawArrays() ====================================================
	// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
	// of 8 vertices and 12 triangle defined by indices. And, each vertex is 3 components
	// (x,y,z) of floats, therefore, the size of vertex array in bytes is 8 * 3 * sizeof(GLfloat).

	vertices.clear();
	normals.clear();
	colors.clear();
	indices.clear();

	Celer::Vector3<GLfloat> vertex_data[] = {
		//  Top Face
		Celer::Vector3<GLfloat>( 1.0f, 1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>( 1.0f, 1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f) + translate,
		// Bottom Face
		Celer::Vector3<GLfloat>( 1.0f, -1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, -1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>( 1.0f, -1.0f,-1.0f) + translate,
		// Front Face
		Celer::Vector3<GLfloat>( 1.0f, 1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f,-1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>( 1.0f,-1.0f, 1.0f) + translate,
		// Back Face
		Celer::Vector3<GLfloat>( 1.0f, 1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>( 1.0f,-1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f,-1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f,-1.0f) + translate,
		// Right Face
		Celer::Vector3<GLfloat>(1.0f, 1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(1.0f,-1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(1.0f,-1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(1.0f, 1.0f,-1.0f) + translate,
		// Left Face
		Celer::Vector3<GLfloat>(-1.0f, 1.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f,-0.0f,-1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f,-1.0f, 1.0f) + translate,
		Celer::Vector3<GLfloat>(-1.0f, 1.0f, 1.0f) + translate
	};

	Celer::Vector3<GLfloat> normal_data[] = {
		// Top Face
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		// Bottom Face
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, -1.0f, 0.0f),
		// Front Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		// Back Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, -1.0f),
		// Right Face
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		// Left Face
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(-1.0f, 0.0f, 0.0f)

	};


	Celer::Vector3<GLfloat> color_data[] = {
		// Top Face
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		// Bottom Face
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		Celer::Vector3<GLfloat>(0.0f, 1.0f, 0.0f),
		// Front Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		// Back Face
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		Celer::Vector3<GLfloat>(0.0f, 0.0f, 1.0f),
		// Right Face
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		// Left Face
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f),
		Celer::Vector3<GLfloat>(1.0f, 0.0f, 0.0f)

	};



	Celer::Vector3<GLfloat> vertex_data_strip[] =
	{
		// Top Face
		vertex_data[0],vertex_data[1],vertex_data[3],vertex_data[2],/* 0 - 5*/
		// Bottom Face
		vertex_data[4],vertex_data[5],vertex_data[7],vertex_data[6],/* 6 - 11 */
		// Front Face
		vertex_data[0],vertex_data[3],vertex_data[4],vertex_data[5],/* 12 - 17*/
		// Back Face
		vertex_data[1],vertex_data[7],vertex_data[2],vertex_data[6],/* 18 - 23*/
		// Right Face
		vertex_data[0],vertex_data[4],vertex_data[1],vertex_data[7],/* 24 - 29*/
		// Left Face
		vertex_data[2],vertex_data[6],vertex_data[3],vertex_data[5] /* 30 - 35*/
	};

	Celer::Vector3<GLfloat> normal_data_strip[] =
	{
		// Top Face
		normal_data[0],normal_data[1],normal_data[3],normal_data[2],/* 0 - 5*/
		// Bottom Face
		normal_data[4],normal_data[5],normal_data[7],normal_data[6],/* 6 - 11 */
		// Front Face
		normal_data[0],normal_data[3],normal_data[4],normal_data[5],/* 12 - 17*/
		// Back Face
		normal_data[1],normal_data[7],normal_data[2],normal_data[6],/* 18 - 23*/
		// Right Face
		normal_data[0],normal_data[4],normal_data[1],normal_data[7],/* 24 - 29*/
		// Left Face
		normal_data[2],normal_data[6],normal_data[3],normal_data[5] /* 30 - 35*/
	};

	std::copy( vertex_data_strip	 , vertex_data_strip + 24	, std::back_inserter ( vertices ) );
	std::copy( normal_data_strip	 , normal_data_strip + 24	, std::back_inserter ( normals ) );
	std::copy( color_data		 , color_data + 24		, std::back_inserter ( colors ) );



	/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_cube_quadStrip_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_quadStrip_ );
		glBufferData ( GL_ARRAY_BUFFER , vertices.size ( ) * sizeof ( vertices[0] ) , &vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(vertices_quadStripSlot_);
		glVertexAttribPointer(vertices_quadStripSlot_, 3, GL_FLOAT, GL_FALSE, 0, 0);
		/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_cube_quadStripNormals_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_quadStripNormals_ );
		glBufferData ( GL_ARRAY_BUFFER , normals.size ( ) * sizeof ( normals[0] ) , &normals[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(normals_quadStripSlot_);
		glVertexAttribPointer(normals_quadStripSlot_, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers ( 1 , &vertexBuffer_cube_quadStripColors_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_quadStripColors_ );
		glBufferData ( GL_ARRAY_BUFFER , colors.size ( ) * sizeof ( colors[0] ) , &colors[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(colors_quadStripSlot_);
		glVertexAttribPointer(colors_quadStripSlot_, 3, GL_FLOAT, GL_FALSE, 0, 0);


		box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );

		std::cout << "center " 		<< box.center ( ) 	<< std::endl;
		std::cout << "min " 		<< box.box_min ( ) 		<< std::endl;
		std::cout << "max " 		<< box.box_max ( ) 		<< std::endl;
		std::cout << "diagonal " 	<< box.diagonal ( ) 	<< std::endl;
}

void Cube::createBoundingBox()
{

	bb_vertices.clear();
	bb_indices.clear();

//	box.fromPointCloud ( vertices.begin ( ) , vertices.end ( ) );
//
//	std::cout << "center " << box.center() << std::endl;
//	std::cout << "min " << box.min() << std::endl;
//	std::cout << "max " << box.max() << std::endl;
//	std::cout << "diagonal " << box.diagonal() << std::endl;


	Celer::Vector3<GLfloat> v0 ( 3.0f*box.box_max ( ).x , 3.0f*box.box_max ( ).y , 3.0f*box.box_max ( ).z );
	Celer::Vector3<GLfloat> v1 ( box.box_max ( ).x , box.box_max ( ).y , box.box_min ( ).z );
	Celer::Vector3<GLfloat> v2 ( box.box_min ( ).x , box.box_max ( ).y , box.box_min ( ).z );
	Celer::Vector3<GLfloat> v3 ( 3.0f*box.box_min ( ).x , 3.0f*box.box_max ( ).y , 3.0f*box.box_max ( ).z );

	Celer::Vector3<GLfloat> v4 ( 3.0f*box.box_max ( ).x , 3.0f*box.box_min ( ).y , 3.0f*box.box_max ( ).z );
	Celer::Vector3<GLfloat> v5 ( 3.0f*box.box_min ( ).x , 3.0f*box.box_min ( ).y , 3.0f*box.box_max ( ).z );
	Celer::Vector3<GLfloat> v6 ( box.box_min ( ).x , box.box_min ( ).y , box.box_min ( ).z );
	Celer::Vector3<GLfloat> v7 ( box.box_max ( ).x , box.box_min ( ).y , box.box_min ( ).z );


	// Care about the type: GL_DOUBLE or GL_FLOAT
	Celer::Vector3<GLfloat> vertex_bb[] =
	{
	  // X Y Z
		//  Top Face
		v0,v1,v3,v2,
		// Bottom Face
		v4,v5,v7,v6,
		// Front Face
		/*v0,v3,v4,v5,*/
		// Back Face
		v2,v1,v6,v7,
		// Right Face
		v0,v4,v1,v7,
		// Left Face
		v2,v6,v3,v5
	};

	// Care about the type: GL_INT
	GLuint vertex_bb_indices [] =
	{
	   // Top Face 		// Bottom
	   0, 1, 2, 2, 3, 0,    4, 5, 6, 6, 7, 4,

	   // Front 		// Back
	   0, 3, 5, 5, 4 , 0,   2, 1, 7, 7, 6, 2,

	   // Right   	 	// Left
	   0, 4, 7, 7, 1, 0,    2, 6, 5, 5, 3, 2
	};// 2 Triangle per face.

	std::copy( vertex_bb	    , vertex_bb + 24	     , std::back_inserter ( bb_vertices ) );
	std::copy( vertex_bb_indices, vertex_bb_indices + 36 , std::back_inserter ( bb_indices ) );


	glGenBuffers ( 1 , &vertexBuffer_boundingBox_vertices_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_boundingBox_vertices_ );
		glBufferData ( GL_ARRAY_BUFFER , bb_vertices.size ( ) * sizeof ( bb_vertices[0] ) , &bb_vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(boundingBox_slot_);
		glVertexAttribPointer(boundingBox_slot_, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

void Cube::create8VerticesIndices ()
{

	vertices.clear();
	indices.clear();

	// Care about the type: GL_DOUBLE or GL_FLOAT
	Celer::Vector3<GLfloat> vertex_data_8[8] =
	{
	 // X Y Z
	      Celer::Vector3<GLfloat> ( 1.0, 1.0, 1.0), // vertex 0
	      Celer::Vector3<GLfloat> ( 1.0, 1.0,-1.0), // vertex 1
	      Celer::Vector3<GLfloat> (-1.0, 1.0,-1.0), // vertex 2
	      Celer::Vector3<GLfloat> (-1.0, 1.0, 1.0), // vertex 3

	      Celer::Vector3<GLfloat> ( 1.0,-1.0, 1.0), // vertex 4
	      Celer::Vector3<GLfloat> (-1.0,-1.0, 1.0), // vertex 5
	      Celer::Vector3<GLfloat> (-1.0,-1.0,-1.0), // vertex 6
	      Celer::Vector3<GLfloat> ( 1.0,-1.0,-1.0)  // vertex 7
	}; // 8 vertices with 3 components ( Real ) each.

	// Care about the type: GL_INT
	GLuint vertex_indices_8[] =
	{
	   // Top Face 		// Bottom
	   0, 1, 2, 2, 3, 0,    4, 5, 6, 6, 7, 4,

	   // Front 		// Back
	   0, 3, 5, 5, 4, 0,   2, 1, 7, 7, 6, 2,

	   // Right   	 	// Left
	   0, 4, 7, 7, 1, 0,    2, 6, 5, 5, 3, 2
	};// 2 Triangle per face.


	std::copy( vertex_data_8   , vertex_data_8    + 8  , std::back_inserter ( vertices ) );
	std::copy( vertex_indices_8, vertex_indices_8 + 36 , std::back_inserter ( indices ) );

	/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_cube_8vertices_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_cube_8vertices_ );
		glBufferData ( GL_ARRAY_BUFFER , vertices.size( ) * sizeof(vertices[0]) , &vertices[0] , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(vertices_8slot_);
		glVertexAttribPointer(vertices_8slot_, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/// Requesting Indices
	glGenBuffers ( 1 , &vertexBuffer_cube_8verticesIndices_ );
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_8verticesIndices_ );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER , indices.size( ) * sizeof(indices[0]) , &indices[0] , GL_STATIC_DRAW );

}

void Cube::createTriangleStripAdjacencyBufffers()
{

	// Care about the type: GL_DOUBLE or GL_FLOAT
	Celer::Vector4<GLfloat> vertex_triangle_ajacency[6] =
	{
	 // X Y Z
	      Celer::Vector4<GLfloat> ( 1.0, 1.0, 1.0,-1.0), // vertex 0
	      Celer::Vector4<GLfloat> ( 1.0, 1.0,-1.0,-1.0), // vertex 1
	      Celer::Vector4<GLfloat> (-1.0, 1.0,-1.0,-1.0), // vertex 2
	      Celer::Vector4<GLfloat> (-1.0, 1.0, 1.0, 1.0), // vertex 3

	      Celer::Vector4<GLfloat> ( 1.0,-1.0, 1.0,-1.0), // vertex 4
	      Celer::Vector4<GLfloat> (-1.0,-1.0, 1.0,-1.0) // vertex 5

	}; // 6 vertices with 4 components ( Real ) each.



	/// Requesting Vertex Buffers to the GPU
	glGenBuffers ( 1 , &vertexBuffer_TrianglesStripAdjacency_ );
		glBindBuffer ( GL_ARRAY_BUFFER , vertexBuffer_TrianglesStripAdjacency_ );
		glBufferData ( GL_ARRAY_BUFFER , 6 * sizeof(vertex_triangle_ajacency[0]) , vertex_triangle_ajacency , GL_STATIC_DRAW );
		// Set up generic attributes pointers
		glEnableVertexAttribArray(vertices_TrianglesStripAdjacency_slot_);
		glVertexAttribPointer(vertices_TrianglesStripAdjacency_slot_, 4, GL_FLOAT, GL_FALSE, 0, 0);

}

void Cube::drawTriangleStripAdjacencyBuffers()
{

	// 1rst attribute buffer : vertices
	glBindVertexArray(vertexArray_for_the_Cube_);
//	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES_ADJACENCY , 0 , 6);

	glBindVertexArray(0);

}

void Cube::createBuffers( const Celer::Vector3<float> translate )
{

	// Is there an OpenGL Context ?
	Celer::OpenGL::OpenGLContext::instance ( )->glewInitialize ( "File Cube.cpp line 37" );

	glGenVertexArrays ( 1 , &vertexArray_for_the_Cube_);
	glBindVertexArray(vertexArray_for_the_Cube_);

	create24Vertices( translate );
	createQuadStrips( translate );
	create8VerticesIndices();
	createTriangleStripAdjacencyBufffers();
	createBoundingBox();

	glBindVertexArray(0);


	isCreated_ = 1;


}

void Cube::draw24Vertices()
{

	// 1rst attribute buffer : vertices

	glBindVertexArray(vertexArray_for_the_Cube_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_24verticesIndices_);
//	// Draw the triangle !
	glDrawElements(GL_TRIANGLES, 36 , GL_UNSIGNED_INT, 0);

	//glDrawArrays(GL_LINES_ADJACENCY , 0 , vertices.size());

	glBindVertexArray(0);
}

void Cube::drawQuadStrips()
{

	// 1rst attribute buffer : vertices
	glBindVertexArray(vertexArray_for_the_Cube_);
//	// Draw the triangle !
	glDrawArrays(GL_LINES_ADJACENCY , 0 , 24);

	glBindVertexArray(0);
}

void Cube::draw8Vertices()
{

	glBindVertexArray(vertexArray_for_the_Cube_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer_cube_8verticesIndices_);
//	// Draw the triangle !
	glDrawElements(GL_TRIANGLES, 36 , GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Cube::drawBoundingBox()
{

	// 1rst attribute buffer : vertices
	glBindVertexArray(vertexArray_for_the_Cube_);
//	// Draw the triangle !
	glDrawArrays(GL_LINES_ADJACENCY , 0 , 24);

	glBindVertexArray(0);
}

Cube::~Cube ( )
{

	// TODO Auto-generated destructor stub

	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER , 0 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , 0 );

	glDeleteBuffers ( 1 , &vertexBuffer_cube_24vertices_ );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_24normals_ );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_24colors_ );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_24verticesIndices_ );

	glDeleteBuffers ( 1 , &vertexBuffer_cube_8vertices_ );
	glDeleteBuffers ( 1 , &vertexBuffer_cube_8verticesIndices_ );

	glDeleteVertexArrays(1, &vertexArray_for_the_Cube_);

	isCreated_ = 0;

}


//glEnableVertexAttribArray(verticesSlot);
//glVertexAttribPointer(verticesSlot, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Celer::Vector3<GLfloat>), (char*)0 + 0*sizeof(GLfloat));
//
//glEnableVertexAttribArray(colorSlot);
//glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Celer::Vector3<GLfloat>), (char*)0 + 3*sizeof(GLfloat));
