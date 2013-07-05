#version 430 core

out vec4 outputColor;

in VertexData
{
    vec4 vertice;
    vec4 normal;
    vec4 color;
    vec4 focus;
flat ivec4 IJK;

} VertexIn;

noperspective in vec4 dist;

struct CutPlane
{
   vec4 point[6];
   vec4 normal[6];
};


struct Face
{
	int 	vertices[4];
	vec3 	normal;
};


uniform mat4 ViewMatrix;


uniform vec3 center_point;
uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;


void main(void)
{


	CutPlane cutPlaneIn;
	Face faces[6];

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);

	vec4 ve[8];

	vec3 ext_x = new_x*0.5;
	vec3 ext_y = new_y*0.5;
	vec3 ext_z = new_z*0.5;

	vec3 centerp = center_point;

//	vec3 ext_x = vec3(1,0,0);
//	vec3 ext_y = vec3(0,1,0);
//	vec3 ext_z = vec3(0,0,1);
//
//	vec3 centerp = vec3(0,0,0);

	ve[0] = vec4(centerp + ext_x + ext_y + ext_z + 10*ext_z+ ext_x,1.0);
	ve[1] = vec4(centerp + ext_x + ext_y - ext_z,1.0);
	ve[2] = vec4(centerp - ext_x + ext_y - ext_z,1.0);
	ve[3] = vec4(centerp - ext_x + ext_y + ext_z + 10*ext_z- ext_x,1.0);

	ve[4] = vec4(centerp + ext_x - ext_y + ext_z + 10*ext_z+ ext_x,1.0);
	ve[5] = vec4(centerp - ext_x - ext_y + ext_z + 10*ext_z- ext_x,1.0);
	ve[6] = vec4(centerp - ext_x - ext_y - ext_z,1.0);
	ve[7] = vec4(centerp + ext_x - ext_y - ext_z,1.0);


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
	//back
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


	for ( int i = 0; i < 6; i++)
	{
		vec3 normal = normalize (cross ( (ve[faces[i].vertices[3]].xyz - ve[faces[i].vertices[0]].xyz),
						 (ve[faces[i].vertices[1]].xyz - ve[faces[i].vertices[0]].xyz) ) );

		cutPlaneIn.point[i]  = ve[faces[i].vertices[3]];
		cutPlaneIn.normal[i] = vec4(-normal,1.0);
	}

//	cutPlaneIn.point[0]  = vec4(0.0,0.0,0.0,1.0);
//	cutPlaneIn.normal[0] = vec4(0.0,0.0,-1.0,1.0);

//	if ( (dot(cutPlaneIn.normal[0],(cutPlaneIn.point[0] - VertexIn.vertice) )  > 0.01 ) )
//		discard;



//	if ( (dot(cutPlaneIn.normal[0],(cutPlaneIn.point[0] - VertexIn.vertice) )  > 0.01 ) &&
//	     (dot(cutPlaneIn.normal[1],(cutPlaneIn.point[1] - VertexIn.vertice) )  > 0.01 ) &&
//	     (dot(cutPlaneIn.normal[2],(cutPlaneIn.point[2] - VertexIn.vertice) )  > 0.01 ) &&
//	     (dot(cutPlaneIn.normal[3],(cutPlaneIn.point[3] - VertexIn.vertice) )  > 0.01 ) &&
//	     (dot(cutPlaneIn.normal[4],(cutPlaneIn.point[4] - VertexIn.vertice) )  > 0.01 ) &&
//	     (dot(cutPlaneIn.normal[5],(cutPlaneIn.point[5] - VertexIn.vertice) )  > 0.01 )
//	   )
//		discard;



	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert =  (ViewMatrix * VertexIn.vertice).xyz;

	if ( !gl_FrontFacing )
	{
		newNormal = -cutPlaneIn.normal[0].xyz;

	}


	newNormal = normalize ( newNormal );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = VertexIn.color;

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

	//outputColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);

	outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );




}
