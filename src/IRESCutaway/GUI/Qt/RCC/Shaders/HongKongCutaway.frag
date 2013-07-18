#version 430

out vec4 outputColor;

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

in VertexData
{
	vec4 vertices[2];
	vec4 normals[6];
flat	bool proxy;
} VertexOut;



struct Face
{
	int 	vertices[4];
};

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

/// Culling Procedure
Face     cutSurface[6];
CutPlane cutPlanes[6];

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];


noperspective in vec4 dist;

uniform mat4 ViewMatrix;

void main(void)
{

	vec3 newNormal = VertexOut.normals[0].xyz;
	vec3 newVert = VertexOut.vertices[0].xyz;


	newNormal = normalize ( newNormal );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = vec4(1.0,0.0,0.0,1.0);

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );


	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);


	if (VertexOut.proxy)
	{


		if( ( dot ( VertexOut.normals[0] , ( VertexOut.vertices[0] - VertexOut.vertice ) ) > 0.000001 ) &&
		    ( dot ( VertexOut.normals[1] , ( VertexOut.vertices[0] - VertexOut.vertice ) ) > 0.000001 ) &&
		    ( dot ( VertexOut.normals[2] , ( VertexOut.vertices[0] - VertexOut.vertice ) ) > 0.000001 ) &&
		    ( dot ( VertexOut.normals[3] , ( VertexOut.vertices[1] - VertexOut.vertice ) ) > 0.000001 ) &&
	            ( dot ( VertexOut.normals[4] , ( VertexOut.vertices[1] - VertexOut.vertice ) ) > 0.000001 ) &&
	            ( dot ( VertexOut.normals[5] , ( VertexOut.vertices[1] - VertexOut.vertice ) ) > 0.000001 ) )
		{
			color_t = vec4(0.0,1.0,0.0,1.0);
		}else
		{
			discard;
		}
	}

	if ( VertexOut.proxy)
	{
		outputColor =  vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );	
	}else
	{
		outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	}


	//outputColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	//outputColor = VertexIn.color;


}
