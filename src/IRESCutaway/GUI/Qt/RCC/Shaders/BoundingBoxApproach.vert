#version 430 core

layout(location = 0) in vec3 center_point;

out CubeData
{
		vec4 v[8];
		vec3 n[6];
		vec4 color;
} cube;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];

void main(void)
{
	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));

	ext_x = new_x*0.3;
	ext_y = new_y*0.3;
	ext_z = new_z*0.1;

	v[0] = vec4(center_point + ext_x + ext_y + 100*ext_z + 5*ext_x + 5*ext_y,1.0);
	v[1] = vec4(center_point + ext_x + ext_y - ext_z,1.0);
	v[2] = vec4(center_point - ext_x + ext_y - ext_z,1.0);
	v[3] = vec4(center_point - ext_x + ext_y + 100*ext_z - 5*ext_x + 5*ext_y,1.0);

	v[4] = vec4(center_point + ext_x - ext_y + 100*ext_z + 5*ext_x - 5*ext_y,1.0);
	v[5] = vec4(center_point - ext_x - ext_y + 100*ext_z - 5*ext_x - 5*ext_y,1.0);
	v[6] = vec4(center_point - ext_x - ext_y - ext_z,1.0);
	v[7] = vec4(center_point + ext_x - ext_y - ext_z,1.0);

	// Top Face
	vec3 normal_top = normalize (cross ( (v[3] - v[0]).xyz, (v[1] - v[0]).xyz ) );
	normal_top =  normalize(normalMatrix * normal_top);

	// Bottom
	vec3 normal_bottom = normalize (cross ( (v[7] - v[4]).xyz, (v[5] - v[4]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Front
	vec3 normal_front = normalize (cross ( (v[4] - v[3]).xyz, (v[0] - v[3]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Back
	vec3 normal_back = normalize (cross ( (v[2] - v[1]).xyz, (v[7] - v[1]).xyz ) );
	normal_back =  normalize(normalMatrix * normal_back);

	// Right
	vec3 normal_right = normalize (cross ( (v[1] - v[0]).xyz, (v[4] - v[0]).xyz ) );
	normal_right = normalize(normalMatrix * normal_right);

	// Left
	vec3 normal_left = normalize (cross ( (v[3] - v[2]).xyz, (v[6] - v[2]).xyz ) );
	normal_left = normalize(normalMatrix * normal_left);


	cube.v[0] = v[0];
	cube.v[1] = v[1];
	cube.v[2] = v[2];
	cube.v[3] = v[3];

	cube.v[4] = v[4];
	cube.v[5] = v[5];
	cube.v[6] = v[6];
	cube.v[7] = v[7];

	cube.n[0] = normal_top;
	cube.n[1] = normal_bottom;
	cube.n[2] = normal_front;
	cube.n[3] = normal_back;
	cube.n[4] = normal_right;
	cube.n[5] = normal_left;

	gl_Position = vec4(center_point,1.0);
}
