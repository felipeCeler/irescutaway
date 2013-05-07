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

	if ( gl_FragCoord.z < ( cutaway.z ) )
	{
		discard;

	}
	else
	{
		outputColor = I * vec4(0.0, 1.0, 0.0, 1.0) + (1.0 - I) * VertexIn.color;
	}
}
