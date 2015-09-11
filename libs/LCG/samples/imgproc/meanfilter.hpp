#ifndef __MEAN__
#define __MEAN__
#include "shaderlib/effect.hpp"

using namespace std;

/**
 * A simple mean filter for image processing.
 * No weights are taken in consideration, averages all neighbors equally.
 **/
class MeanFilter: public Effect {

public:

  /**
   * Constructor. Initialize the kernel size.
   * @param name The effect's name.
   **/
  MeanFilter(const char* name): kernelsize(3)  {
    effectName = name;
    quad = new Mesh();
    quad->createQuad();

  }

  ///Initializes the effect, creating and loading the shader.
  virtual void initialize();

  /**
   * Applys the mean filter to an image.
   **/
  virtual void renderTexture(int texUnit = 0, int texUnit2 = 0, int kernelSize = 0);

  virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL) {}

  ///Reloads the shader, reading the files again.
  virtual void reloadShaders();

  int* getKernelSizePtr(void) {
    return &kernelsize;
  }

private:

  /// The size of the mean filter kernel (window size to apply convolution)
  int kernelsize;  

  /// A quad to be rendered forcing one call of the fragment shader per image pixel
  Mesh* quad;

};

#endif
