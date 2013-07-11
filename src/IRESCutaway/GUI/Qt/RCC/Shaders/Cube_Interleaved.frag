#version 430 core

out vec4 outputColor;

in VertexData
{
	vec4 vertice;
	vec4 normal;
	vec4 color;
} VertexIn;

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

noperspective in vec4 dist;

uniform mat4 ViewMatrix;

void main(void)
{

//	CutPlane cutPlaneIn;
//
//
//	cutPlaneIn.point  = ViewMatrix * vec4(0.0,0.0,0.0,1.0);
//
//	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));
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
