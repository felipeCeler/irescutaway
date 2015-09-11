#ifndef __GRADIENT__
#define __GRADIENT__
#include "shaderlib/effect.hpp"

using namespace std;

/**
 * Gradient Filter for image processing.
 * A two pass filter, first apply the horizontal kernel and then the vertical kernel.
 **/
class GradientFilter: public Effect {

public:

  /**
   * Constructor.
   * @param name The effect's name.
   **/
  GradientFilter(const char* name) {
    effectName = name;
    quad = new Mesh();
    quad->createQuad();

  }

  ///Initializes the effect, creating and loading the shader.
  virtual void initialize();

  /**
   * Applys the mean filter to an image.
   **/
  virtual void renderTexture(int texUnit = 0);

  virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL) {}


  ///Reloads the shader, reading the files again.
  virtual void reloadShaders();

private:

  Mesh* quad;

};

#endif
