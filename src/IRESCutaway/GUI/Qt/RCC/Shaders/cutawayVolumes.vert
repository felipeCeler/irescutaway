#version 430
#pragma optionNV(unroll all)

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


layout(std140) uniform GlobalMatrices
{
    mat4 ModelMatrix;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
} globalMatrices;

layout(std140) uniform CutVolumes
{
        vec4   center_points[63];
        ivec4  size;
}cutVolumes;

/// FIXME - Do research and understand the best away to alignment data on Shader.
out CubeData
{
	vec4 v[8];
	vec4 n[6];
	vec4 color;
flat	bool culled[8];

} cube;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

struct CutPlane
{
        vec4 point;
        vec4 normal;
};

struct Face
{
        int vertices[6];
};

/// Culling Procedure
vec4 v[8];

CutPlane cutPlaneIn;
Face cutVolume[6];

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

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

	cube.color  = color;

        /// Culling Procedure
        ext_x = new_x*0.3;
        ext_y = new_y*0.3;
        ext_z = new_z*0.1;

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


        cube.culled[0] = false;
        cube.culled[1] = false;
        cube.culled[2] = false;
        cube.culled[3] = false;
        cube.culled[4] = false;
        cube.culled[5] = false;
        cube.culled[6] = false;
        cube.culled[7] = false;

        // For each cut volume
        // For each vertex in the cube

        int hits = 0;

        for ( int vertex_index = 0; vertex_index < 8; vertex_index++)
        {
		for ( int j = 0; j < cutVolumes.size.x ; j++ )
		{
			vec3 center_of_mass = cutVolumes.center_points[j].xyz;

			v[0] = vec4(center_of_mass + ext_x + ext_y + 100*ext_z + 5*ext_x + 5*ext_y,1.0);
			v[1] = vec4(center_of_mass + ext_x + ext_y - ext_z,1.0);
			v[2] = vec4(center_of_mass - ext_x + ext_y - ext_z,1.0);
			v[3] = vec4(center_of_mass - ext_x + ext_y + 100*ext_z - 5*ext_x + 5*ext_y,1.0);

			v[4] = vec4(center_of_mass + ext_x - ext_y + 100*ext_z + 5*ext_x - 5*ext_y,1.0);
			v[5] = vec4(center_of_mass - ext_x - ext_y + 100*ext_z - 5*ext_x - 5*ext_y,1.0);
			v[6] = vec4(center_of_mass - ext_x - ext_y - ext_z,1.0);
			v[7] = vec4(center_of_mass + ext_x - ext_y - ext_z,1.0);

                        // For each side of the cube volume
			hits = 0;
                        for ( int i = 0; i < 6; i++)
                        {
                                // normals point inside the volume
                                vec3 normal = normalize (cross ( (v[cutVolume[i].vertices[3]].xyz - v[cutVolume[i].vertices[0]].xyz),
                                                                 (v[cutVolume[i].vertices[1]].xyz - v[cutVolume[i].vertices[0]].xyz) ) );

                                cutPlaneIn.point  = v[cutVolume[i].vertices[3]];
                                cutPlaneIn.normal = vec4(-normal,0.0);
                                // Vertex lies in the same side
                                if ( dot ( cutPlaneIn.normal , ( cube.v[vertex_index] - cutPlaneIn.point  ) ) < 0.0 )
                                {
                                	hits++;
                                }
                        }

                        if (hits == 6)
                        {
                        	 cube.culled[vertex_index] = true;
                        }

                }
        }



	mat3 normalMatrix = inverse(transpose(mat3(globalMatrices.ViewMatrix)));


	cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
	cube.n[1] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);

	cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[0]).xyz , (cube.v[4]-cube.v[0]).xyz )),0.0);
	cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);

	cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
	cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);

	gl_Position =  center;
}
//Canonical Plane
//cutPlaneIn.point  = vec4(0.0,0.0,0.0,1.0);
//cutPlaneIn.normal = vec4(0.0,0.0,1.0,1.0);
//
//// For each vertex in the cube
//for ( int vertex_index = 0; vertex_index < 8; vertex_index++)
//{
//        if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - cube.v[vertex_index] ) ) > 0.0 )
//                cube.culled[vertex_index] = true;
//        else
//                cube.culled[vertex_index] = false;
//}
///// Culling Procedure
