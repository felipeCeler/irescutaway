#version 430 core

layout ( location = 0 ) in vec4 center_box;
layout ( location = 1 ) in vec4 axis_x;
layout ( location = 2 ) in vec4 axis_y;
layout ( location = 3 ) in vec4 axis_z;
layout ( location = 4 ) in vec4 extends;
layout ( location = 5 ) in vec4 aperture;


out CubeData
{
		vec4 v[8];
		vec3 n[6];
		vec4 color;
} cube;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 FreezeViewMatrix;
uniform mat4 ProjectionMatrix;

// freeze camera matrix
uniform int freeze;

//uniform vec3 camera_center;

uniform float x;
uniform float y;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];

void main(void)
{
        //vec3 center_of_mass = cutVolumes.center_points[gl_VertexID].xyz;
	vec3 center_of_mass = center_box.xyz;

        mat4 invView;
        if (freeze == 0)
            invView = inverse(ViewMatrix);
        else
            invView = inverse(FreezeViewMatrix);

        vec3 camera_center = (invView * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

        // coordinate system for the cutaway frustum
        vec3 camera_up = normalize((invView * vec4(0.0, 1.0, 0.0, 0.0)).xyz);
        vec3 camera_dir = normalize( camera_center - center_of_mass );
        vec3 camera_right = normalize( cross(camera_dir.xyz, camera_up.xyz) );

//        ext_x = camera_right*0.1;
//        ext_y = camera_up*0.1;
//        ext_z = camera_dir*0.1;

//        float zfactor = 50.0;
//        v[0] = vec4(center_of_mass + ext_x + ext_y + zfactor*ext_z + x*ext_x + y*ext_y,1.0);
//	v[1] = vec4(center_of_mass + ext_x + ext_y - ext_z,1.0);
//	v[2] = vec4(center_of_mass - ext_x + ext_y - ext_z,1.0);
//        v[3] = vec4(center_of_mass - ext_x + ext_y + zfactor*ext_z - x*ext_x + y*ext_y,1.0);
//
//        v[4] = vec4(center_of_mass + ext_x - ext_y + zfactor*ext_z + x*ext_x - y*ext_y,1.0);
//        v[5] = vec4(center_of_mass - ext_x - ext_y + zfactor*ext_z - x*ext_x - y*ext_y,1.0);
//	v[6] = vec4(center_of_mass - ext_x - ext_y - ext_z,1.0);
//	v[7] = vec4(center_of_mass + ext_x - ext_y - ext_z,1.0);

        float ex = max (max (abs(dot(camera_right, axis_x.xyz*extends.x)), abs(dot(camera_right, axis_y.xyz*extends.y)) ), abs(dot(camera_right, axis_z.xyz*extends.z)) );
        float ey = max (max (abs(dot(camera_up, axis_x.xyz*extends.x)), abs(dot(camera_up, axis_y.xyz*extends.y)) ), abs(dot(camera_up, axis_z.xyz*extends.z)) );
        float ez = max (max (abs(dot(camera_dir, axis_x.xyz*extends.x)), abs(dot(camera_dir, axis_y.xyz*extends.y)) ), abs(dot(camera_dir, axis_z.xyz*extends.z)) );


        ext_x = (camera_right) * ex;
        ext_y = (camera_up   ) * ey;
        ext_z = (camera_dir  ) * ez;

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

	gl_Position = vec4(v[0]);
}
