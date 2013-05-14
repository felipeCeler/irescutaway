#version 430 core
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

in VertexData
{
	vec4 color;
	vec4 normal;
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

		float s = abs(dot(cutaway.xyz,vec3(0.0,0.0,1.0)) * 1.0 );

		if ( abs(gl_FragCoord.z - cutaway.w ) < 0.0005 )
		{
			outputColor = vec4(0.0,0.0,0.0,1.0);

		}else
		{

		outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * (VertexIn.color + vec4(s,s,s,1.0));
		}


	}
}
