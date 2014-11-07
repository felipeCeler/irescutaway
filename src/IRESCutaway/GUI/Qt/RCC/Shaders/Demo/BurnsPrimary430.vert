#version 430

// For while, all transformations come from the Celer::Camera.

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 0 ) in vec3 vertices;
layout (location = 1 ) in vec3 normals;
layout (location = 2 ) in vec4 colors;


uniform float move_x;
uniform float move_y;
uniform float move_z;

out vec3 normal;
out vec4 color;

uniform float move;

void main()
{
	mat3 normalMatrix       = mat3(ViewMatrix);
	normalMatrix            = transpose(normalMatrix);
	normalMatrix            = inverse(normalMatrix);

	normal = normalize(normalMatrix * normals);

	color = vec4(1.0);

	float scale = 1.0f;

	vec3 scaled = vec3( scale*vertices.x, scale*vertices.y, scale*vertices.z) + vec3(move_x,move_y,move_z);

//	float m = move*0.1;
//
//	scaled += vec4(m,m,m,1.0);

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(scaled,1.0);
}

