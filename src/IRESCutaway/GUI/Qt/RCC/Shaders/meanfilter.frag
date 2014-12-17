#version 430

#define numberOfSamples 256

uniform sampler2D imageTexture;
uniform sampler2D imageTexture2;
uniform ivec2 viewportSize;
uniform int kernelsize;

uniform vec2 kernel[numberOfSamples];

out vec4 out_Normal;
out vec4 out_Vertex;


void meanFilter(in out vec4 resultNormal, in out vec4 resultVertex)
{
        ivec2 texCoord = ivec2(gl_FragCoord.xy);

        vec2 noisecoords = gl_FragCoord.xy;// / vec2(textureSize(noiseTexture, 0).xy);
//        float noise = texelFetch(noiseTexture, ivec2(noisecoords).xy, 0).x;
//
//        noise *= 3.14;
//        mat2 rotation = mat2(cos(noise), -sin(noise), sin(noise), cos(noise));

        float normalw = 0.0;
        float vertexw = 0.0;

        float depth = abs( texelFetch(imageTexture2, ivec2(gl_FragCoord.xy),0).z);

        // smaller radius for points farther away
        int rad = max(1, min(int(kernelsize), int(kernelsize/depth)));
        for (int i = 0; i <= numberOfSamples; ++i)
        {
            vec2 rotated = vec2(1.0,1.0);//rotation * kernel[i].xy;
            ivec2 randCoord = texCoord + ivec2(kernel[i] * rad);
            //ivec2 randCoord = texCoord + ivec2(i,j);

            vec4 pixelNormal = texelFetch(imageTexture, randCoord,0).rgba;
            vec4 pixelVertex = texelFetch(imageTexture2, randCoord,0).rgba;

            float d = exp(-length(kernel[i]));
            normalw+=d;

            resultNormal += pixelNormal.rgba*d;
            resultVertex += pixelVertex.rgba*d;

        }

        resultNormal /= float(normalw);
        resultNormal.xyz = normalize(resultNormal.xyz);
        resultVertex /= float(normalw);
}

void main() {



  float depth = abs( texelFetch(imageTexture2, ivec2(gl_FragCoord.xy),0).z);
        // smaller radius for points farther away
  int rad = max(0, min(int(kernelsize), int(kernelsize/depth)));

  int kernelsizex = rad;
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

  //meanFilter(resultNormal,resultVertex);

  out_Normal = vec4(resultNormal);
  out_Vertex = vec4(resultVertex);
}


