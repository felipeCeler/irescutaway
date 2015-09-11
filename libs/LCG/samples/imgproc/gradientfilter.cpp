#include "gradientfilter.hpp"

void GradientFilter::initialize() {

  shader = loadShader("gradHorizontal", 1);
  shader->initialize();
  errorCheckFunc(__FILE__, __LINE__);
}

void GradientFilter::reloadShaders() {
  shader->reloadShaders();
}

void GradientFilter::renderTexture(int texUnit) {

  shader->enable();

  shader->setUniform("imageTexture", texUnit);
  shader->setUniform("viewportSize", viewportSize[0], viewportSize[1]);

  errorCheckFunc(__FILE__, __LINE__);

  quad->render();

  shader->disable();
  
}
