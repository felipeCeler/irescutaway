#version 430 core

uniform vec3 lightDirection;

in VertexData
{
   vec4 color;
   vec3 vert;
   vec3 normal;
} VertexOut;

out vec4 outputColor;

in vec2 texcoord;

void main(void)
{

	vec3 newNormal = VertexOut.normal.xyz;
	vec3 newVert = VertexOut.vert.xyz;

	newNormal = normalize ( newNormal );

	vec3 toLight = normalize ( -lightDirection.xyz );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

//	if (VertexOut.color.g == 1.0)
//	{
//		outputColor = vec4(abs(VertexOut.normal),1.0) ;
//	}else
//	{
		outputColor =    vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) ;
//	}

	//outputColor = vec4(abs(VertexOut.normal),1.0) ;
}
