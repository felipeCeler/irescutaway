#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;

out vec4 out_Color;

const mat3 mask = mat3 
  (-1.0, 0.0, 1.0, 
   -2.0, 0.0, 2.0, 
   -1.0, 0.0, 1.0);
 
void main() {

  vec3 result = vec3(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec2 pixelSize = vec2(1.0/viewportSize[0], 1.0/viewportSize[1]);

  for(int i = -1 ; i <= 1 ; i++) {
    for(int j = -1 ; j <= 1 ; j++) {
      vec2 offset = vec2(i*pixelSize.x, j*pixelSize.y);
      vec3 pixelColor = texture(imageTexture, texCoord + offset).rgb;
      result += pixelColor.rgb * mask[i+1][j+1];
    }
  }

  out_Color = vec4(result, 1.0);
}
