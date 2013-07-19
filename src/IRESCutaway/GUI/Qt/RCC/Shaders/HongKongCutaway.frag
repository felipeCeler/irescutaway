#version 430
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect normals;
uniform sampler2DRect vertices;

out vec4 outputColor;

in VertexData
{
		vec4 vertice;
		vec4 normal;
		vec4 color;
flat 	bool proxy;
} VertexIn;


noperspective in vec4 dist;


void main(void)
{

	vec4 cutaway  = texture2DRect( normals  , gl_FragCoord.xy).rgba;
	vec4 vertices = texture2DRect( vertices , gl_FragCoord.xy).rgba;

	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert = VertexIn.vertice.xyz;
	vec4 color_t = VertexIn.color;

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);

	if (gl_FragCoord.z < (cutaway.w)) //&&  length( gl_FragCoord.st - color.xy) < 20.0 )
	{
		discard;

	}
	{
		gl_FragDepth = gl_FragCoord.z;
//	if ( abs(gl_FragCoord.z - cutaway.w ) < 0.00001 )
//	{
//		//discard;
//		color_t = vec4(1.0, 0.0, 0.0, 0.0);
//
//	}

	newNormal = normalize ( newNormal );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );


	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );



	outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );

	}

	//outputColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );
	//outputColor = VertexIn.color;


}
