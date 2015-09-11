#ifndef __EFFECT__
#define __EFFECT__

#include "framebuffer.hpp"
#include "mesh.hpp"
#include "trackball.hpp"
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <limits>
#include <GL/glew.h>


/**
 * Effects class. This class must be extended by the user in order to hold most of the application core. It handles all kind of CPU pre-computations and rendering methods.
 **/
class Effect {

public:

	/**
	 * Effects Constructor. Initializes the shaders directory and the fbo and shader identification integer. The shaders directory can be passed as a parameter, but is set by default as "shaders/".
	 * @param shadersDir The directory containing the shader files.
	**/
  Effect (string shadersDir = "shaders/") : fbo(0), shader(0), shaders_dir(shadersDir) {
    enable();
  }

  /**
	* Initializes all effect's funcionality. This method must be implemented by the programmer in the extended class. It will typically hold the shader loading and framebuffer initialization, however,
	* if the application uses other parameters, their initialization should also be done in this method.
  **/
  virtual void initialize() = 0;

  /**
   * Loads a shader by name, initializes it, and inserts in shaders list.
   * @param shader_name String with name of the shader, will also be used to locate shader files.
   * @param debug_level Used for debugging the shaders loading. If 0, nothing is printed in the console. If 1, the application will print the name of the shader being loaded. If 2, the application 
	 * will print the name of the shader, as well as it's code.
   **/
  virtual Shader* loadShader(string shader_name, int debug_level = 0) {
    Shader* shader_ptr = new Shader(shaders_dir, shader_name, debug_level);
    shader_ptr->initialize();
    shaders_list.push_back(shader_ptr);
    return shader_ptr;
  }

  /**
   * @return The effect's name.
   **/
  const char* getEffectName() {
    return effectName;
  }

  /**
   * @return The pointer to the effect's main shader object. 
   **/
  Shader* getShader() {
    return shader;
  }

  /**
   * @return The pointer to the effect's fbo.
   **/
  Framebuffer* getFbo() {
    return fbo;
  }

	/**
	 * Sets the shaders files directory.
	**/
	void setShadersDir(string dir) {
		shaders_dir = dir;
	}

  /**
   * Main rendering routine, that must be implemented by the programmer in the extended class. It typically receives a mesh and the camera and light Trackballs from the application, 
	 * enabling the effect's shader for their rendering. However, any of these parameters might be ommited in specific applications, as the user desires.
   * @param mesh A pointer to the mesh object that will be rendered.
   * @param cameraTrackball A pointer to the Trackball object containing the camera transformations.
   * @param lightTrackball A pointer to the Trackball object containing the light transformations.
   **/
  virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL) = 0;

  /**
   * Renders an image given as an input texture.
   * Useful for multi-pass renderings, where an effect receives the output of another, or for
   * image processing applications.
   * @param texUnit The texture unit holding the texture to be rendered (can be from an fbo or not)
   **/
  virtual void renderTexture(int texUnit = 0) {
        (void)texUnit; //avoid unuesed var warning
    }


  /**
	 * Reloads all shaders needed for effect usage. This method allows the user to change the shaders codes and see the results in real time, sparing him the need to restart the application.
	**/
  virtual void reloadShaders() {
    if (shader) {
      shader->reloadShaders();
    }
      
    for (unsigned int i = 0; i < shaders_list.size(); ++i) {
      shaders_list[i]->reloadShaders();
    }
  }

  /**
   * Resizes the viewport and the framebuffer.
   * @param w New width
   * @param h New height
   **/
  virtual void resize(int w, int h) {

		if(fbo) {
			delete fbo;
			fbo = new Framebuffer(w,h);
		}

		viewportSize << w, h;

	}

  /**
	 * Default Destructor. Deletes the framebuffer and all the shaders.
	**/
  ~Effect() {

    if(fbo) {
      delete fbo;
    }

    if(shader) {
      delete shader;
    }

    for (unsigned int i = 0; i < shaders_list.size(); ++i){
      if(shaders_list[i]) {
            delete shaders_list[i];
      }
    }
    shaders_list.clear();

  }

  /**
	 * An effect is defined as active if it's currently being used in rendering. 
	 * @return If the effect is active or not
	**/
  virtual bool isEnabled(void) {
    return active;
  }

  /// Activates the effect
  virtual void enable (void) {
    active = true;
  }

  /// Deactivates the effect
  virtual void disable (void) {
    active = false;
  }

  /// Toggles the effect state between active/not active
  virtual void toggle(void) {
    active = !active;
  }

  /** 
	 * Useful for interface callbacks.
	 * @return The pointer to the parameter that indicates wether the effect is active or not.
	**/
  virtual bool* getActivePtr(void) {
    return &active;
  }

protected:

  ///Since framebuffers are often used in GPU applications, every effect contains by default one framebuffer object.
  Framebuffer* fbo;

  ///Every effect has an identification string, used for convenient management in the application.
  const char* effectName;

  /** 
   * Every effect must have at least one shader program, responsible for the main rendering. However, if the programmer thinks it's needed, more shaders can be created in order to do multiple pass 
	 * rendering.
	**/
  Shader* shader;

  /// Vector of pointers to shaders used in this effect, in case the user needs multiple pass rendering.
  std::vector<Shader*> shaders_list;

  /// Directory in which the shader files are stored.
  string shaders_dir;

  /// Viewport Size.
  Eigen::Vector2i viewportSize;

  /// Flag indicating wether the effect is active (currently being used in rendering) or not.
  bool active;

};

#endif
