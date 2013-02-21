#version 420
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

in vec2 texCoord;

out vec4 fragmentColor;

void main()
{
	vec4 rgba = texture2DRect ( primary, vec2(gl_FragCoord.s    	   ,gl_FragCoord.t));

	fragmentColor = vec4(rgba);
}
