#include "renderimage.hpp"

void RenderImage::initialize() {
    shader = loadShader("renderimage", 1);
    errorCheckFunc(__FILE__, __LINE__);
}

void RenderImage::reloadShaders() {
    shader->reloadShaders();
}

void RenderImage::renderTexture(int texUnit) {

    glViewport(0, 0, viewportSize[0], viewportSize[1]);

    shader->enable();

    shader->setUniform("imageTexture", texUnit);
    shader->setUniform("viewportSize", viewportSize[0], viewportSize[1]);

    quad->render();

    shader->disable();
  
}
