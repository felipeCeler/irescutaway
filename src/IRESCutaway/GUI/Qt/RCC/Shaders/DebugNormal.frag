#version 420 core
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

in VertexData
{
   vec4 color;
} VertexOut;

out vec4 outputColor;

in vec2 texcoord;


void main(void)
{

	vec4 color = texture2DRect ( primary , gl_FragCoord.st ).rgba;

	float cutaway = color.w;

	if ( gl_FragCoord.z < ( cutaway ) )
	{
		discard;
	}else
	{
		outputColor = vec4(1.0,1.0,1.0,1.0);
	}
}
