#version 430

uniform sampler2D sampleTexture;
uniform ivec2 viewportSize;

out vec4 out_Color;

void main(void) {

vec2 texCoord = vec2(gl_FragCoord.x/float(viewportSize[0]), gl_FragCoord.y/float(viewportSize[1]));
vec3 result = vec3(texture(sampleTexture, texCoord).rgb);

out_Color = vec4(result,1.0);


}

