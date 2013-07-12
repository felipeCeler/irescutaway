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


struct Edge
{
	int source;
	int target;
};

struct Face
{
	int 	vertices[4];
	Edge 	edges[4];
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


	Face cell[6];
	// Cube Orientation as IRESv2
	// top
	cell[0].vertices[0] = 4;
	cell[0].vertices[1] = 5;
	cell[0].vertices[2] = 7;
	cell[0].vertices[3] = 6;
	// bottom
	cell[1].vertices[0] = 0;
	cell[1].vertices[1] = 3;
	cell[1].vertices[2] = 1;
	cell[1].vertices[3] = 2;
	// front
	cell[2].vertices[0] = 4;
	cell[2].vertices[1] = 0;
	cell[2].vertices[2] = 5;
	cell[2].vertices[3] = 1;
	// back
	cell[3].vertices[0] = 2;
	cell[3].vertices[1] = 3;
	cell[3].vertices[2] = 6;
	cell[3].vertices[3] = 7;
	// right
	cell[4].vertices[0] = 1;
	cell[4].vertices[1] = 2;
	cell[4].vertices[2] = 5;
	cell[4].vertices[3] = 6;
	// left
	cell[5].vertices[0] = 0;
	cell[5].vertices[1] = 3;
	cell[5].vertices[2] = 4;
	cell[5].vertices[3] = 7;



	/// Culling Procedure
	vec4 ve[8];

	CutPlane cutPlaneIn;
	Face cutVolume[6];

	vec3 ext_x = new_x*0.2;
	vec3 ext_y = new_y*0.3;
	vec3 ext_z = new_z*0.1;

	// top
	cutVolume[0].vertices[0] = 0;
	cutVolume[0].vertices[1] = 1;
	cutVolume[0].vertices[2] = 2;
	cutVolume[0].vertices[3] = 3;
	// bottom
	cutVolume[1].vertices[0] = 4;
	cutVolume[1].vertices[1] = 5;
	cutVolume[1].vertices[2] = 6;
	cutVolume[1].vertices[3] = 7;
	// front
	cutVolume[2].vertices[0] = 0;
	cutVolume[2].vertices[1] = 3;
	cutVolume[2].vertices[2] = 5;
	cutVolume[2].vertices[3] = 4;
	// back
	cutVolume[3].vertices[0] = 1;
	cutVolume[3].vertices[1] = 7;
	cutVolume[3].vertices[2] = 6;
	cutVolume[3].vertices[3] = 2;
	// right
	cutVolume[4].vertices[0] = 0;
	cutVolume[4].vertices[1] = 4;
	cutVolume[4].vertices[2] = 7;
	cutVolume[4].vertices[3] = 1;
	// left
	cutVolume[5].vertices[0] = 2;
	cutVolume[5].vertices[1] = 6;
	cutVolume[5].vertices[2] = 5;
	cutVolume[5].vertices[3] = 3;

	vec4 center_v = (v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7) / 8;

	bool isclipped_locally  = false;

	for (int j = 0 ; j < 1 ; j++)
	{
		ve[0] = vec4(center_points[j].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[1] = vec4(center_points[j].xyz + ext_x + ext_y - ext_z,1.0);
		ve[2] = vec4(center_points[j].xyz - ext_x + ext_y - ext_z,1.0);
		ve[3] = vec4(center_points[j].xyz - ext_x + ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);

		ve[4] = vec4(center_points[j].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[5] = vec4(center_points[j].xyz - ext_x - ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
		ve[6] = vec4(center_points[j].xyz - ext_x - ext_y - ext_z,1.0);
		ve[7] = vec4(center_points[j].xyz + ext_x - ext_y - ext_z,1.0);


		for ( int vertex_index = 0; vertex_index < 8; vertex_index++)
		{

			cube.culled[vertex_index] = true;

			for ( int i = 0; i < 6; i++)
			{
				vec3 normal = normalize (cross ( (ve[cutVolume[i].vertices[3]].xyz - ve[cutVolume[i].vertices[0]].xyz),
							 (ve[cutVolume[i].vertices[1]].xyz - ve[cutVolume[i].vertices[0]].xyz) ) );

				cutPlaneIn.point  = ve[cutVolume[i].vertices[3]];
				cutPlaneIn.normal = vec4(-normal,1.0);

				// Vertex lies in the same side
				if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - cube.v[vertex_index] ) ) > 0.01 )
				{
					isclipped_locally = true;
				}else
				{
					isclipped_locally = false;
				}

				cube.culled[vertex_index] = cube.culled[vertex_index] && isclipped_locally;
			}


		}

	}





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
