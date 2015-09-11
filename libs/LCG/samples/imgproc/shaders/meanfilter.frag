#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;
uniform int kernelsize;

out vec4 out_Color;

void main() {

  vec3 result = vec3(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec2 pixelSize = vec2(1.0/float(viewportSize[0]), 1.0/float(viewportSize[1]));
  
  for(int i = -kernelsize; i <= kernelsize; i++) {
    for(int j = -kernelsize; j <= kernelsize; j++) {
      vec2 offset = vec2(i*pixelSize.x, j*pixelSize.y);
      vec3 pixelColor = texture(imageTexture, texCoord + offset).rgb;
      result += pixelColor.rgb;
    }
  }

  result /= float((2*kernelsize+1) * (2*kernelsize+1));

  out_Color = vec4(result, 1.0);
}
