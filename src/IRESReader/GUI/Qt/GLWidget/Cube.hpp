/*
 * Cube.hpp
 *
 *  Created on: Jan 7, 2013
 *      Author: felipe
 */

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include <vector>
/// Celer::Libraries

#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Physics/BoundingBox3.hpp>
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

		std::vector<Celer::Vector3<GLdouble> > 	points;
		std::vector<GLuint> 			indices;
		Celer::BoundingBox3<GLdouble> 		box;

	private:

		bool isCreated_;

		//////
		GLuint vertexArray_for_the_Cube;
		////// Vertex Buffers
			GLuint vertexBuffer_cube_vertices;
				GLuint verticesSlot;
				GLuint colorSlot;
			GLuint vertexBuffer_cube_indices;

};

#endif /* GEOMETRY_HPP_ */
