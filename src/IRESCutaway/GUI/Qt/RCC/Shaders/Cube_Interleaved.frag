#version 430

out vec4 outputColor;

in VertexData
{
		vec4 vertice;
		vec4 normal;
		vec4 color;
flat 	bool proxy;
} VertexIn;

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

noperspective in vec4 dist;

uniform mat4 ViewMatrix;

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

struct Face
{
	int 	vertices[6];
	vec3 	normal;
};

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

/// Culling Procedure
vec4 ve[8];

CutPlane cutPlaneIn[6];
Face cutVolume[6];

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

void main(void)
{

	ext_x = new_x*0.2;
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

//	CutPlane cutPlaneIn;
//
//
//	cutPlaneIn.point  = ViewMatrix * vec4(0.0,0.0,0.0,1.0);
//
	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));
//
//	cutPlaneIn.normal = vec4(0.0,0.0,1.0,1.0);
//
//	cutPlaneIn.normal = vec4(normalMatrix * cutPlaneIn.normal.xyz,1.0);

	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert = VertexIn.vertice.xyz;

//	if ( dot ( cutPlaneIn.normal , ( cutPlaneIn.point - VertexIn.vertice ) ) > 0.01 )
//	{
//		discard;
//	}
//
//	if ( !gl_FrontFacing )
//	{
//
//	}else
//	{
//
//	}


		ve[0] = vec4(center_points[0].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[1] = vec4(center_points[0].xyz + ext_x + ext_y - ext_z,1.0);
		ve[2] = vec4(center_points[0].xyz - ext_x + ext_y - ext_z,1.0);
		ve[3] = vec4(center_points[0].xyz - ext_x + ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);

		ve[4] = vec4(center_points[0].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[5] = vec4(center_points[0].xyz - ext_x - ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
		ve[6] = vec4(center_points[0].xyz - ext_x - ext_y - ext_z,1.0);
		ve[7] = vec4(center_points[0].xyz + ext_x - ext_y - ext_z,1.0);

		for ( int i = 0; i < 6; i++)
		{
				vec3 normal = normalize (cross ( (ve[cutVolume[i].vertices[3]].xyz - ve[cutVolume[i].vertices[0]].xyz),
							 (ve[cutVolume[i].vertices[1]].xyz - ve[cutVolume[i].vertices[0]].xyz) ) );

				cutPlaneIn[i].point  = ViewMatrix * ve[cutVolume[i].vertices[3]];
				cutPlaneIn[i].normal = vec4(-normal,1.0);
				cutPlaneIn[i].normal = vec4(normalMatrix * cutPlaneIn[i].normal.xyz,1.0);

		}

		if ( ( dot ( cutPlaneIn[0].normal , ( VertexIn.vertice - cutPlaneIn[0].point ) ) < 0.01 ) &&
			 ( dot ( cutPlaneIn[1].normal , ( VertexIn.vertice - cutPlaneIn[1].point ) ) < 0.01 ) &&
			 ( dot ( cutPlaneIn[2].normal , ( VertexIn.vertice - cutPlaneIn[2].point ) ) < 0.01 ) &&
			 ( dot ( cutPlaneIn[3].normal , ( VertexIn.vertice - cutPlaneIn[3].point ) ) < 0.01 ) &&
			 ( dot ( cutPlaneIn[4].normal , ( VertexIn.vertice - cutPlaneIn[4].point ) ) < 0.01 ) &&
			 ( dot ( cutPlaneIn[5].normal , ( VertexIn.vertice - cutPlaneIn[5].point ) ) < 0.01 ) )
			 
		{
			if (!VertexIn.proxy )
				discard;
		}



	newNormal = normalize ( newNormal );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = VertexIn.color;

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );


	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);


	outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );



	//outputColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	//outputColor = VertexIn.color;


}
