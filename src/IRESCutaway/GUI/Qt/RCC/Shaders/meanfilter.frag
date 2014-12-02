#version 430


uniform sampler2D imageTexture;
uniform sampler2D imageTexture2;
uniform ivec2 viewportSize;
uniform int kernelsize;

out vec4 out_Normal;
out vec4 out_Vertex;

void main() {


  int kernelsizex = kernelsize;
  vec4 resultNormal = vec4(0.0);
  vec4 resultVertex = vec4(0.0);
  vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

  vec2 pixelSize = vec2(1.0/float(viewportSize[0]), 1.0/float(viewportSize[1]));
  
  for(int i = -kernelsizex; i <= kernelsizex; i++) {
    for(int j = -kernelsizex; j <= kernelsizex; j++) {
      vec2 offset = vec2(i*pixelSize.x, j*pixelSize.y);
      vec4 pixelNormal = texture(imageTexture, texCoord + offset).rgba;
      vec4 pixelVertex = texture(imageTexture2, texCoord + offset).rgba;
      resultNormal += pixelNormal.rgba;
      resultVertex += pixelVertex.rgba;
    }
  }
  resultNormal /= float((2*kernelsizex+1) * (2*kernelsizex+1));
  resultVertex /= float((2*kernelsizex+1) * (2*kernelsizex+1));

  out_Normal = vec4(resultNormal);
  out_Vertex = vec4(resultVertex);
}


