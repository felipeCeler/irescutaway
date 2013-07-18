#version 430

out vec4 outputColor;

in VertexData
{
		vec4 vertices[2];
		vec4 vertice;
		vec4 normal;
		vec4 color;
flat 	bool proxy;
} VertexIn;

noperspective in vec4 dist;

uniform mat4 ViewMatrix;

void main(void)
{

	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert = VertexIn.vertice.xyz;


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


	if ( VertexIn.proxy)
	{
		outputColor =  vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );	
	}else
	{
		outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	}


	//outputColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	//outputColor = VertexIn.color;


}
