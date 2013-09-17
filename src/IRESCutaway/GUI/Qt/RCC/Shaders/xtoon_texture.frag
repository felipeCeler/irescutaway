#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;

out vec4 out_Color;

void main() {


  int kernelsizex = 3;
  vec4 result = vec4(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec4 pixelColor = texture(imageTexture, texCoord ).rgba;

  out_Color = vec4(pixelColor);
}
