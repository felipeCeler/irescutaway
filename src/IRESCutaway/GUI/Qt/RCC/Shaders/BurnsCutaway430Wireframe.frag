#version 420
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;


noperspective in vec4 dist;


in VertexData
{
	vec4 vertice;
    vec4 color;
    vec4 normal;

} VertexIn;


uniform vec3 lightDirection;

out vec4 fragmentColor;

uniform int cutaway;

void main ( void )
{

	float m = 0.5;

	vec4 color = texture2DRect ( primary , gl_FragCoord.st ).rgba;

	vec2 vector = gl_FragCoord.xy - ( color.xy );

	float cutaway = color.w;

	if ( gl_FragCoord.z < ( cutaway ) )
	{
		discard;
	}
	else
	{

		vec3 newNormal = VertexIn.normal.xyz;
		vec3 newVert = VertexIn.vertice.xyz;

		if ( !gl_FrontFacing )
		{
			newNormal = vec3 ( -vector , length ( vector ) * m );
			newVert.z = cutaway;

		}

		newNormal = normalize ( newNormal );

		vec3 toLight = normalize ( -newVert.xyz );

		vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
		vec3 eye_dir = normalize ( -newVert.xyz );

		vec4 color_t = vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );

		vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
		vec4 la = vec4 ( 0.2 );
		vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
		vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

		//fragmentColor = vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 );

		float d = min ( dist[0] , min ( dist[1] , min ( dist[2] , dist[3] ) ) );
		float I = exp2 ( -2 * d * d );

		fragmentColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * (  vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) );


	}


}


