#version 430 core

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;
layout (location = 4 ) in vec4 focus;
layout (location = 5 ) in ivec4 IJK;

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

	VertexOut.color = colors;
	VertexOut.normal = normals;

	if ( focus.x == 0.0 )
	{
		gl_Position = ProjectionMatrix * ViewMatrix * vec4(1.0,0.0,0.0,0.0);
	}
	else
	{
		gl_Position = ProjectionMatrix * ViewMatrix * vertices;

	}


}


