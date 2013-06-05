#version 430 core

layout(location = 11) in vec4 vertices;


// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{

	gl_Position =  vertices;
}
