#version 420

// For while, all transformations come from the Celer::Camera.

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;
layout (location = 4 ) in vec4 focus;


out VertexData
{
    vec4 propertyColor;
    vec4 normal;

} vertexShader_VertexData;

//
//out vec3 vert;
//out vec3 normal;
//out vec4 color;


void main()
{
//	vert = (ViewMatrix * vec4(vertices,1.0)).xyz;
//
//	mat3 normalMatrix = mat3(ViewMatrix);
//	normalMatrix = transpose(normalMatrix);
//	normalMatrix = inverse(normalMatrix);
//
//	normal = normalize(normalMatrix * normals);
//
//	color = colors;

	if ( focus.x == 0.0 )
	{
		vertexShader_VertexData.propertyColor = colors;
		gl_Position = ProjectionMatrix * ViewMatrix * vertices;
	}
	else
	{
		vertexShader_VertexData.propertyColor = colors;
		gl_Position = ProjectionMatrix * ViewMatrix * vec4(1.0,0.0,0.0,0.0);

	}



}
