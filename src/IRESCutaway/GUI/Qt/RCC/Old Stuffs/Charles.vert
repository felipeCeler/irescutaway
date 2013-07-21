#version 430 core

layout(location = 0) in vec4 vertices;


out VertexData
{
    vec4 color;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


void main(void)
{

	VertexOut.color  = vec4(0.0,1.0,0.0,1.0 );
	gl_Position =  ProjectionMatrix * ViewMatrix * vertices;
}
