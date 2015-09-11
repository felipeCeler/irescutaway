#include "meanfilter.hpp"

void MeanFilter::initialize() {

  shader = loadShader("meanfilter", 1);
  shader->initialize();

}

void MeanFilter::reloadShaders() {
  shader->reloadShaders();
}

void MeanFilter::renderTexture(int texUnit, int texUnit2, int kernelSize) {

  shader->enable();

  shader->setUniform("imageTexture", texUnit);
  shader->setUniform("imageTexture2", texUnit2);
  shader->setUniform("kernelsize", kernelSize);
  shader->setUniform("viewportSize", viewportSize[0], viewportSize[1]);
  quad->render();

  shader->disable();  
  
}
