#version 430 core

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;
layout (location = 4 ) in vec4 focus;
layout (location = 5 ) in ivec4 IJK;

out VertexData
{
	vec4 vert;
	vec4 color;
	vec4 normal;
   flat vec4 IJK;
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

	VertexOut.color  = colors;
	VertexOut.vert 	 =  ViewMatrix * vertices;
	VertexOut.normal =  vec4(normalize(normalMatrix * normals.xyz),0.0);
	VertexOut.IJK = IJK;

	if ( focus.x == 0.0 )
	{
		gl_Position = ProjectionMatrix * ViewMatrix * vec4(1.0,0.0,0.0,0.0);
	}
	else
	{
		gl_Position = ProjectionMatrix * ViewMatrix * vertices;

	}


}


