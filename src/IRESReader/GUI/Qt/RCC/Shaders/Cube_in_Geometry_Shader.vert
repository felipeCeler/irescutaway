#version 430 core

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;
layout (location = 4 ) in vec4 focus;
layout (location = 5 ) in ivec4 IJK;


// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out VertexData
{
    vec4 vertice;
    vec3 normal;
    vec4 color;
    vec4 focus;
flat    ivec4 IJK;
} cell_properties;


layout (std140) uniform ColorBlock {
    vec4 diffuse;
    vec4 ambient;
};

void main(void)
{

	cell_properties.vertice  = vertices;
	cell_properties.normal 	 = normals.xyz;
	cell_properties.color 	 = colors;
	cell_properties.focus    = focus;
	cell_properties.IJK 	 = IJK;

	gl_Position =  vertices + diffuse;
}
