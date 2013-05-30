#version 430 core
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

in VertexData
{
	vec4 vert;
	vec4 color;
	vec4 normal;
   flat vec4 IJK;
} VertexIn;

noperspective in vec4 dist;

out vec4 outputColor;

void main(void)
{

	vec4 cutaway = texture2DRect( primary , gl_FragCoord.st).rgba;

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2.0 * d * d);

	if ( gl_FragCoord.z < ( cutaway.w ) )
	{
		discard;

	}
	else
	{

		float s = dot(VertexIn.normal.xyz,vec3(0.0,0.0,1.0));

		if ( abs(gl_FragCoord.z - cutaway.w ) < 0.00005 )
		{
			outputColor = vec4(0.0,0.0,0.0,1.0);

		}

		else if ( s > 0 )
		{


			vec3 newNormal = VertexIn.normal.xyz;
			vec3 newVert = cutaway.xyz;

			newNormal = normalize ( newNormal );

			vec3 toLight = normalize ( -newVert.xyz );

			vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
			vec3 eye_dir = normalize ( -newVert.xyz );

			vec4 color_t = VertexIn.color;//vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );//geometryShader_VertexData.propertyColor;

			vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
			vec4 la = vec4 ( 0.2 );
			vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
			vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );


			if (  ( VertexIn.IJK.z > 1 ) && ( VertexIn.IJK.z <  6) )
			{
				outputColor =  vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) ;
				//outputColor =  ( VertexIn.color );
			}
			else
			{
				//outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * ( vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) );
				outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * ( VertexIn.color );
			}
		}else
		{
			discard;
		}


	}
}
