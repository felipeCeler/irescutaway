#ifndef __RENDERIMAGE__
#define __RENDERIMAGE__
#include "shaderlib/effect.hpp"

using namespace std;

/**
 * A simple shader to display a texture
 **/
class RenderImage: public Effect {

public:

  /**
   * Constructor.
   * @param name The effect's name.
   **/
  RenderImage(const char* name)  {
    effectName = name;
    quad = new Mesh();
    quad->createQuad();
  }

  ///Initializes the effect, creating and loading the shader.
  virtual void initialize();

  /**
   * Render the texture
   **/
  virtual void renderTexture(int texUnit = 0);

  virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL) {
      // avoid unused var warning
      (void)mesh;
      (void)cameraTrackball;
      (void)lightTrackball;

  }

  ///Reloads the shader, reading the files again.
  virtual void reloadShaders();

private:

  /// A quad to be rendered forcing one call of the fragment shader per image pixel
  Mesh* quad;

};

#endif
