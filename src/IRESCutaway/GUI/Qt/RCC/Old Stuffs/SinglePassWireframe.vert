#version 430 core

layout(location = 3) in vec3 vertices;
layout(location = 4) in vec3 normals;
layout(location = 5) in vec3 colors;

out VertexData
{
    vec4 color;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 cubeDisplacemant;

void main(void)
{
	//VertexOut.color = vec4(vec3(1.0),1.0);
	vec4 viewPos = ViewMatrix * vec4(vertices,1.0);

	VertexOut.color  = vec4(0.0,1.0,0.0,1.0 );

	gl_Position = ProjectionMatrix * ViewMatrix *vec4((vertices+cubeDisplacemant),1.0);



}


