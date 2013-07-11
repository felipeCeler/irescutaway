#version 430 core

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 v3;
layout(location = 4) in vec4 v4;
layout(location = 5) in vec4 v5;
layout(location = 6) in vec4 v6;
layout(location = 7) in vec4 v7;

layout(location = 8)  in vec4 color;
layout(location = 9)  in vec4 IJK;
layout(location = 10) in vec4 focus;
layout(location = 11) in vec4 center;


/// FIXME - Do research and understand the best away to alignment data on Shader.
out CubeData
{

	vec4 v[8];
	vec4 n[6];
	vec4 color;
flat	vec4 IJK;
flat	vec4 focus;
	vec4 center;
flat	bool culled[8];

} cube;

struct Face
{
	int 	vertices[4];
	vec3 	normal;
};

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

void main(void)
{

	cube.v[4] = v4;
	cube.v[5] = v5;
	cube.v[7] = v7;
	cube.v[6] = v6;

	cube.v[0] = v0;
	cube.v[3] = v3;
	cube.v[1] = v1;
	cube.v[2] = v2;

	cube.color    = color;
	cube.IJK      = IJK;
	cube.focus    = focus;
	cube.center = center;

	/// Culling Procedure
	vec4 ve[8];

	CutPlane cutPlaneIn;
	Face faces[6];

	vec3 ext_x = new_x*0.2;
	vec3 ext_y = new_y*0.3;
	vec3 ext_z = new_z*0.1;

	// top
	faces[0].vertices[0] = 0;
	faces[0].vertices[1] = 1;
	faces[0].vertices[2] = 2;
	faces[0].vertices[3] = 3;
	// bottom
	faces[1].vertices[0] = 4;
	faces[1].vertices[1] = 5;
	faces[1].vertices[2] = 6;
	faces[1].vertices[3] = 7;
	// front
	faces[2].vertices[0] = 0;
	faces[2].vertices[1] = 3;
	faces[2].vertices[2] = 5;
	faces[2].vertices[3] = 4;
	// back
	faces[3].vertices[0] = 1;
	faces[3].vertices[1] = 7;
	faces[3].vertices[2] = 6;
	faces[3].vertices[3] = 2;
	// right
	faces[4].vertices[0] = 0;
	faces[4].vertices[1] = 4;
	faces[4].vertices[2] = 7;
	faces[4].vertices[3] = 1;
	// left
	faces[5].vertices[0] = 2;
	faces[5].vertices[1] = 6;
	faces[5].vertices[2] = 5;
	faces[5].vertices[3] = 3;

	vec4 center_v = (v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7) / 8;

	bool isclipped_globally[8];
	isclipped_globally[0] = false;
	isclipped_globally[1] = false;
	isclipped_globally[2] = false;
	isclipped_globally[3] = false;
	isclipped_globally[4] = false;
	isclipped_globally[5] = false;
	isclipped_globally[6] = false;
	isclipped_globally[7] = false;
	bool isclipped_locally  = true;

//	for (int j = 0 ; j < 1 ; j++)
//	{
//		ve[0] = vec4(center_points[j].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
//		ve[1] = vec4(center_points[j].xyz + ext_x + ext_y - ext_z,1.0);
//		ve[2] = vec4(center_points[j].xyz - ext_x + ext_y - ext_z,1.0);
//		ve[3] = vec4(center_points[j].xyz - ext_x + ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
//
//		ve[4] = vec4(center_points[j].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
//		ve[5] = vec4(center_points[j].xyz - ext_x - ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
//		ve[6] = vec4(center_points[j].xyz - ext_x - ext_y - ext_z,1.0);
//		ve[7] = vec4(center_points[j].xyz + ext_x - ext_y - ext_z,1.0);
//
//		isclipped_locally = true;
//
//		for ( int vertex_index = 0; vertex_index < 8; vertex_index++)
//		{
//
//			for ( int i = 0; i < 6; i++)
//			{
//				vec3 normal = normalize (cross ( (ve[faces[i].vertices[3]].xyz - ve[faces[i].vertices[0]].xyz),
//							 (ve[faces[i].vertices[1]].xyz - ve[faces[i].vertices[0]].xyz) ) );
//
//				cutPlaneIn.point  = ve[faces[i].vertices[3]];
//				cutPlaneIn.normal = vec4(-normal,1.0);
//
//				if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - cube.v[vertex_index] ) ) < 0.01 )
//				{
//					isclipped_locally = false;
//				}
//			}
//
//
//			isclipped_globally[vertex_index] = isclipped_globally[vertex_index] || isclipped_locally;
//		}
//	}


		cutPlaneIn.point  = vec4(0.0,0.0,0.0,0.0);
		cutPlaneIn.normal = vec4(0.0,0.0,1.0,0.0);

		if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - center_v ) ) > 0.01 )
			isclipped_globally[0] = false;

	cube.culled[0] = isclipped_globally[0];
	cube.culled[1] = isclipped_globally[1];
	cube.culled[2] = isclipped_globally[2];
	cube.culled[3] = isclipped_globally[3];
	cube.culled[4] = isclipped_globally[4];
	cube.culled[5] = isclipped_globally[5];
	cube.culled[6] = isclipped_globally[6];
	cube.culled[7] = isclipped_globally[7];


	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));


	cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
	cube.n[1] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);

	cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[0]).xyz , (cube.v[4]-cube.v[0]).xyz )),0.0);
	cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);

	cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
	cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);

	gl_Position =  center;
}

// Canonical Plane
//	cutPlaneIn.point  = vec4(0.0,0.0,0.0,1.0);
//	cutPlaneIn.normal = vec4(0.0,0.0,1.0,1.0);
//
//	if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - center_v ) ) > 0.01 )
//		isclipped_globally = true;
