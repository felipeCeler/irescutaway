/*
 * PlyLoader.hpp;
 *
 *  Created on: Apr 15, 2011
 *      Author: felipe
 *
 *   Reference: http://code.google.com/p/scanned-3d-models/
 */

#ifndef PLYLOADER_HPP_
#define PLYLOADER_HPP_


#ifdef _WIN32 // _WIN32 is defined by many compilers available for the Windows operating system, but not by others.
#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include <vector>

#include <Celer/OpenGL/OpenGLContext.hpp>
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>


class Model_PLY
{
	public:
		int Load ( const char *filename );
		void Draw ( ) const;
		void DrawLines ( );
		void calculateNormal( float *coord1, float *coord2, float *coord3, float  norm[3] );
		Model_PLY ( );
		void init ();

		float* Faces_Triangles;
		float* Faces_Quads;
		float* Vertex_Buffer;
		float* Color;
		float* Colors;
		float* Normal;
		float* Normals;

		int TotalConnectedTriangles;
		int TotalConnectedQuads;
		int TotalConnectedPoints;
		int TotalFaces;
		bool open;

		std::vector<Celer::Vector3<float> > vertices;
		std::vector<Celer::Vector3<float> > normals;

		Celer::BoundingBox3<float> box;

		GLuint vertexArray;
		GLuint vertex_buffer;
		GLuint normal_buffer;


};


#endif /* PLYLOADER_HPP_ */
