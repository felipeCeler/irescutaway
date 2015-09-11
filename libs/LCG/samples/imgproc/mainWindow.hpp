#ifndef __MAINWINDOW__
#define __MAINWINDOW__

#define WINDOW_TITLE_PREFIX "Shaders Library Visualization"

#include "shaderlib/trackball.hpp"
#include "shaderlib/shader.hpp"
#include "shaderlib/mesh.hpp"
#include "meanfilter.hpp"
#include "gradientfilter.hpp"
#include "renderimage.hpp"
#include "AntTweakBar.h"
#include "CImg.h"
#include <string>

using namespace cimg_library;

/**
 * Main Window class. This class defines the program, storing the GLFW windows, AntTweakBar interface, the mesh being rendered, the shader objects etc. 
 **/
class MainWindow {

public:

  ///Default Constructor. Sets default window size to 800x800, initial window position to (100, 100).
  MainWindow():  currentWidth(800), currentHeight(800), currentPosX(100), currentPosY(100) {    
  }

  /**Constructor that takes the window width and height as parameters. Initial window position is (100,100).
   * @param width The width to be set as viewport Width.
   * @param height The height to be set as viewport Height.
   **/
  MainWindow(int width, int height): currentWidth(width), currentHeight(height), currentPosX(100), currentPosY(100) {
  }

  ///Initializes GLEW, calls window initialization function, enables depth testing and culling, initializes OpenGL Matrices and loads the mesh file.
  void initialize();

  /**
   * Returns the current window title string.
   * @return Return the window title string.
   **/
  string getWindowTitle() {
    return windowTitle;
  }

  /**
   * Returns the current viewport width.
   * @return Returns currentWidth.
   **/
  int getCurrentWidth() {
    return currentWidth;
  }

  /**
   * Sets the current Viewport Width.
   * @param width The value to be set as current viewport width.
   **/
  void setCurrentWidth(int width) {
    currentWidth = width;
  }

  /**
   * Returns the current viewport height.
   * @return Returns currentHeight.
   **/
  int getCurrentHeight() {
    return currentHeight;
  }

  /**
   * Sets the current Viewport Height.
   * @param height The value to be set as current viewport height.
   **/
  void setCurrentHeight(int height) {
    currentHeight = height;
  }

  /**
   * Returns the current X coordinate of viewport position in pixels.
   * @return Returns the current x window position in pixels.
   **/
  int getCurrentPosX() {
    return currentPosX;
  }

  /**
   * Returns the current Y coordinate of viewport position in pixels.
   * @return Returns the current y window position in pixels.
   **/
  int getCurrentPosY() {
    return currentPosY;
  }

  /**
   * Searches the windows vector, returning a pointer to the currently active GLFW window.
   * @return Returns the currently active GLFW window.
   **/
  static MainWindow* getCurrentWindow();


  ///GLFW Render Function.
  void mainLoop();

  ///Terminates GLFW Window.
  void shutDown() {
    glfwTerminate();
    exit(EXIT_SUCCESS);
  }

  ///Default Destructor. Detaches current shaders from the shader program, deletes the shaders, mesh buffers, trackballs, windows, effects, terminates antTweakBar and closes GLFW window.
  ~MainWindow() {
	
    cout << TwTerminate() << endl << endl;

    glfwCloseWindow();

  }

  ///Resizes the current window.
  void resize(int width, int height);

  static void TW_CALL reloadShadersCB(void *filter);
  static void TW_CALL selectShaderCB(void *filter);

private:

  /**
   * Reloads all effects and shaders
   **/
  void reloadShaders(void);

  /**
   * Apply a filter to a texture attachment.
   * The source is read from one FBO attachment and written to another.
   * The source and target ids are switched to follow a ping-pong strategy.
   * @param filter Pointer to the filter (effect) instance
   **/
  void applyFilter (Effect* filter);

  ///Initializes all menus and buttons from antTweakBar.
  void initializeAntTweakBar();

  /**
   * Initialize FBO with current screen resolution and default image
   **/
  void initializeFBO();

  /**
   * Loads an image to the input texture
   * @param filename String containing path and filename
   **/
  void loadImage(string filename);

  /**
   * Handles all shader creation, loading it from an external file, adding to the shaders vector and creating the button in the interface.
   * @param shaderName The shader name to be set on the Shader variable.
   * @param vertPath The path to the vertices shader file.
   * @param fragPath The path to the fragment shader file.
   * @param displayName The string that will be displayed on the graphical interface besides the button to activate the shader.
   * @param helpMessage The help message to be set in graphical interface to describe the shader.
   * @param computePaths Vector containing the paths to the compute shader files. 
   **/
  void createShader(const char* shaderName, const char* vertPath, const char* fragPath, const char* displayName, 
		    const char* helpMessage = NULL);

  ///Creates the mean filter for image processing
  void createMeanFilter();

  ///Mean filter, a non-average weighted mean filter
  Effect* meanfilter;

  ///Creates the gradient two-pass filter (horizontal and vertical) for image processing
  void createGradientFilter();

  ///Gradient filter, done in two passes (horizontal and vertical)
  Effect* gradientfilter;

  ///Creates the simple render image effect
  void createRenderImage();

  /// This is an auxiliary effect just to render the texture
  Effect* renderimage;

  ///Framebuffer to apply multiple filters in a sequence
  Framebuffer* fbo;

  ///Texture with the original image
  GLuint image_tex;

  /// identifies which is the source and target attachments during a ping-pong scheme
  bool source;
  bool target;

  ///Current Viewport Width measured in pixels.
  int currentWidth;
		
  ///Current Viewport Height measured in pixels.
  int currentHeight;

  ///Current Viewport Position in X axis in pixels.
  int currentPosX;

  ///Current Viewport Position in Y axis in pixels.
  int currentPosY;

  ///GLFW Window identification handle.
  int windowHandle;

  ///Window Title:
  string windowTitle;

  ///Initializes GLFW window and sets all the callback functions.
  void initWindow();

  ///Handles uniform setting for the mesh, as well as mesh and trackball's rendering.		
  void draw(void);
	
  ///AntTweakBar menu:
  TwBar* menu;

};

#endif

