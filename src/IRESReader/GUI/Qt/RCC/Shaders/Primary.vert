#version 420

// For while, all transformations come from the Celer::Camera.

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 0 ) in vec3 vertices;
layout (location = 1 ) in vec3 normals;
layout (location = 2 ) in vec4 colors;



out vec3 normal;
out vec4 color;

void main()
{


	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = transpose(normalMatrix);
	normalMatrix = inverse(normalMatrix);

	normal = normalize(normalMatrix * normals);

	color = colors;

	gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertices,1.0);

}

