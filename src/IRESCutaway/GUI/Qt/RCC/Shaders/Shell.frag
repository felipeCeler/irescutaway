#version 430 core

in VertexData
{
   vec4 v;
   vec3 n;
   vec4 color;
} VertexIn;

noperspective in vec4 dist;

out vec4 outputColor;

void main(void)
{
	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2.0 * d * d);

	//I = 0;

	vec3 newNormal = VertexIn.n.xyz;
	vec3 newVert = VertexIn.v.xyz;

	newNormal = normalize ( newNormal );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );


	vec4 color =    vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) ;


	outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * color;
	//outputColor =  vec4(newNormal,1.0);
}
