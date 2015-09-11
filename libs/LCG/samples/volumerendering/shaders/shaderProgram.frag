#version 430

out vec4 out_Color;

uniform sampler2D finalTexture;
uniform ivec2 viewportSize;
	 
void main(void)
{

	//out_Color = vec4(1.0,0.0,0.0,1.0);
	out_Color = texture(finalTexture, gl_FragCoord.xy/viewportSize);

}
