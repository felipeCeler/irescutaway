#version 420


layout (location = 3 ) in vec3 vertices;
layout (location = 4 ) in vec3 normals;
layout (location = 5 ) in vec4 colors;
layout (location = 9 ) in float renderFlags;

out VertexData
{
    vec4 propertyColor;
    vec4 normal;

} vertexShaderOut_VertexData;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	//vert = (ViewMatrix * vec4(vertices,1.0)).xyz;

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = transpose(normalMatrix);
	normalMatrix = inverse(normalMatrix);

	vertexShaderOut_VertexData.normal = vec4(normalize(normalMatrix * normals),1.0);

	vertexShaderOut_VertexData.propertyColor = colors;

	gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertices,renderFlags);

}

