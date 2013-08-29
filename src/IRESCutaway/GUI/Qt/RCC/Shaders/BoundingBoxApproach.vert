#version 430 core


layout(std140) uniform CutVolumes
{
        vec4   center_points[63];
        ivec4  size;
}cutVolumes;

out CubeData
{
		vec4 v[8];
		vec3 n[6];
		vec4 color;
} cube;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 camera_center;

uniform float x;
uniform float y;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];

void main(void)
{
        vec3 center_of_mass = cutVolumes.center_points[gl_VertexID].xyz;

        mat4 invView = inverse(ViewMatrix);


        vec3 camera_up = normalize((invView * vec4(0.0, 1.0, 0.0, 0.0)).xyz);
        vec3 camera_dir = normalize( camera_center - center_of_mass );
        vec3 camera_right = normalize( cross(camera_dir.xyz, camera_up.xyz) );

        ext_x = camera_right*0.1;
        ext_y = camera_up*0.1;
        ext_z = camera_dir*0.1;

        float zfactor = 50.0;
        v[0] = vec4(center_of_mass + ext_x + ext_y + zfactor*ext_z + x*ext_x + y*ext_y,1.0);
	v[1] = vec4(center_of_mass + ext_x + ext_y - ext_z,1.0);
	v[2] = vec4(center_of_mass - ext_x + ext_y - ext_z,1.0);
        v[3] = vec4(center_of_mass - ext_x + ext_y + zfactor*ext_z - x*ext_x + y*ext_y,1.0);

        v[4] = vec4(center_of_mass + ext_x - ext_y + zfactor*ext_z + x*ext_x - y*ext_y,1.0);
        v[5] = vec4(center_of_mass - ext_x - ext_y + zfactor*ext_z - x*ext_x - y*ext_y,1.0);
	v[6] = vec4(center_of_mass - ext_x - ext_y - ext_z,1.0);
	v[7] = vec4(center_of_mass + ext_x - ext_y - ext_z,1.0);


        mat3 normalMatrix = (inverse(transpose(mat3(ViewMatrix))));

	// Top Face
	vec3 normal_top = normalize (cross ( (v[3] - v[0]).xyz, (v[1] - v[0]).xyz ) );
	normal_top =  normalize(normalMatrix * normal_top);

	// Bottom
	vec3 normal_bottom = normalize (cross ( (v[7] - v[4]).xyz, (v[5] - v[4]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Front
	vec3 normal_front = normalize (cross ( (v[4] - v[0]).xyz, (v[3] - v[0]).xyz ) );
	normal_front = normalize(normalMatrix * normal_front);

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
	cube.n[2] = -normal_front;
	cube.n[3] = normal_back;
	cube.n[4] = normal_right;
	cube.n[5] = normal_left;

	gl_Position = vec4(cutVolumes.center_points[gl_VertexID].xyz,1.0);
}
