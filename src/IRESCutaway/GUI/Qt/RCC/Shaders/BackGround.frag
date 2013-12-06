#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;

out vec4 out_Color;

void main() {


  int kernelsizex = 3;
  vec4 result = vec4(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec4 pixelColor = texture(imageTexture, texCoord ).rgba;


  float y = gl_FragCoord.y/viewportSize.y;

  out_Color = vec4(vec3(0.605,0.691,0.781) + (1-y)* vec3(0.1,0.1,0.1),1.0);
}
