#version 420


uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

layout (location = 0 ) in vec3 vertices;
layout (location = 1 ) in vec3 colors;


out vec3 color;


void main()
{

	gl_Position = projectionMatrix * modelViewMatrix * vec4(vertices,1.0);
	color = colors;
}

