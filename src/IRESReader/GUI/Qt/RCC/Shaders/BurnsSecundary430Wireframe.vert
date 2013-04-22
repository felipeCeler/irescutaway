#version 420

// For while, all transformations come from the Celer::Camera.

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 1 ) in vec4 vertices;
layout (location = 2 ) in vec4 normals;
layout (location = 3 ) in vec4 colors;
layout (location = 4 ) in vec4 focus;
layout (location = 5 ) in ivec4 IJK;


out VertexData
{
	vec4 vert;
	vec4 normal;
	vec4 propertyColor;

} vertexShader_VertexData;

//
//out vec3 vert;
//out vec3 normal;
//out vec4 color;


void main()
{
	vertexShader_VertexData.vert = ViewMatrix * vertices;

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = transpose(normalMatrix);
	normalMatrix = inverse(normalMatrix);

	vertexShader_VertexData.normal = vec4(normalize(normalMatrix * normals.xyz),1.0);
//
//	color = colors;


	if ( focus.x == 0.0 )
	{
		vertexShader_VertexData.propertyColor = colors; //vec4(1.0,0.0,0.0,1.0);
		gl_Position = ProjectionMatrix * ViewMatrix * vec4(0.0,0.0,0.0,0.0);//vertices;
	}
	else
	{
		vertexShader_VertexData.propertyColor = colors;
		gl_Position = ProjectionMatrix * ViewMatrix * vertices;
	}


}
