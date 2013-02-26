#version 420

// For while, all transformations come from the Celer::Camera.

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 3 ) in vec3 vertices;
layout (location = 4 ) in vec3 normals;
layout (location = 5 ) in vec4 colors;


out vec3 vert;
out vec3 normal;
out vec4 propertyColor;


void main()
{
	vert = (ViewMatrix * vec4(vertices,1.0)).xyz;

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = transpose(normalMatrix);
	normalMatrix = inverse(normalMatrix);

	normal = normalize(normalMatrix * normals);

	propertyColor = colors;

	gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertices,1.0);

}
