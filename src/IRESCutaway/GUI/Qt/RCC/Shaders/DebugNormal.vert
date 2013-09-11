#version 430 core

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;

out VertexData
{
    vec4 color;
    vec4 normal;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = inverse( normalMatrix );
	normalMatrix = transpose( normalMatrix);

	VertexOut.normal =  normals;

	VertexOut.color  = normals;

	gl_Position = vec4(vertices);

}
