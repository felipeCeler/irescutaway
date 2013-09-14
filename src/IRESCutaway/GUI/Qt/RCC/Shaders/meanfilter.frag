#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;
uniform int kernelsize;

out vec4 out_Color;

void main() {


  int kernelsizex = 9;
  vec4 result = vec4(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec2 pixelSize = vec2(1.0/float(viewportSize[0]), 1.0/float(viewportSize[1]));
  
  for(int i = -kernelsizex; i <= kernelsizex; i++) {
    for(int j = -kernelsizex; j <= kernelsizex; j++) {
      vec2 offset = vec2(i*pixelSize.x, j*pixelSize.y);
      vec4 pixelColor = texture(imageTexture, texCoord + offset).rgba;
      result += pixelColor.rgba;
    }
  }
  result /= float((2*kernelsizex+1) * (2*kernelsizex+1));

  out_Color = vec4(result);
}
