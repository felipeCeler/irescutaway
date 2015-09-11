#include "mainWindow.hpp"

void MainWindow::initialize() {

  initWindow();

  // Glew Initialization:
  GLenum glewInitResult = glewInit();
  if (GLEW_OK != glewInitResult) {
    cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
    exit(EXIT_FAILURE);
  }
  errorCheckFunc(__FILE__, __LINE__);

  // Disable Depth Testing and Culling for image processing
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // initialize the pointers
  meanfilter = 0;
  gradientfilter = 0;
  renderimage = 0;
  fbo = 0;

  // remember to initialize TW first so we can create the callbacks
  initializeAntTweakBar();
 
  // create the mean filter effect
  createMeanFilter();

  // create the gradient filter (Sobel) effect
  createGradientFilter();

  // create the render image effect
  createRenderImage();

  // initialized the framebuffer with the current screen resolution
  initializeFBO();

  // loads a default image
  loadImage("camelo.jpg");
       
  cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
}

void MainWindow::createMeanFilter() {

  meanfilter = new MeanFilter("MeanFilter");
  meanfilter->initialize();
  meanfilter->resize(currentWidth, currentHeight);

  //AntTweakBar:
  // the enable/disable button, we could also query the state instead of directly modify the meanfilter attribute
  bool* active = meanfilter->getActivePtr();
  TwAddVarRW(menu,"meanFilter",TW_TYPE_BOOLCPP, active, 
  	     " label='Applies Mean Filter' key=m help='If checked, applies mean filter to image.'");
  int* kernelsize = ((MeanFilter*)meanfilter)->getKernelSizePtr();
  TwAddVarRW(menu, "kernelsize", TW_TYPE_INT16, kernelsize, 
  	     " label='Kernel size'  keyIncr='+' keyDecr='-' step=1 help='Changes the kernel size.' min=1");
}

void MainWindow::createGradientFilter() {

  gradientfilter = new GradientFilter("GradFilter");
  gradientfilter->initialize();
  gradientfilter->resize(currentWidth, currentHeight);

  //AntTweakBar:
  bool* active = gradientfilter->getActivePtr();
  TwAddVarRW(menu,"gradientFilter",TW_TYPE_BOOLCPP, active, 
  	     " label='Applies Gradient Filter'  key=m help='If checked, applies gradient filter to image.'");

}


void MainWindow::createRenderImage() {
  renderimage = new RenderImage("RenderImage");
  renderimage->initialize();
  renderimage->resize(currentWidth, currentHeight);
}


void MainWindow::initializeFBO() {

  // initialize FBO with current screen resolution and two texture attachments
  fbo = new Framebuffer(currentWidth, currentHeight, 2);
}


void MainWindow::loadImage(string filename) {

  CImg<unsigned char> glimage(filename.c_str());
  // to use the image as an Opengl texture, we must flip an interleave
  // the original CImg format is (r1r2r3 .... g1g2g3 .... b1b2b3) and we need (r1g1b2 r2g2b2 ....)
  glimage.mirror('y');
  CImg<unsigned char> interleaved = glimage.get_permute_axes("cxyz");

  glGenTextures(1, &image_tex);
  glBindTexture(GL_TEXTURE_2D, image_tex);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glimage.width(), glimage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, interleaved.data());

}

void MainWindow::initializeAntTweakBar() {

  //AntTweakBar Initialization:
  TwInit(TW_OPENGL, NULL);
  TwWindowSize(currentWidth, currentHeight);
  menu = TwNewBar("menu");

  //Defining default values for menu position and size:
  ostringstream menuXPositionStream;
  ostringstream menuYPositionStream;
  ostringstream menuXSizeStream;
  ostringstream menuYSizeStream;
  int menuXSize = 260;
  int menuYSize = 200;
  menuXPositionStream << currentWidth - menuXSize;
  menuYPositionStream << 0;
  menuXSizeStream << menuXSize;
  menuYSizeStream << menuYSize;

  //Defining default values for menu:
  std::string parameters;
  parameters = "menu label='Menu' color='51 204 255' contained=true text=dark fontresizable=false valueswidth=60 "
    "size='" + menuXSizeStream.str() + " " + menuYSizeStream.str() + "' "
    "position='" + menuXPositionStream.str() + " " + menuYPositionStream.str() + "'";
  TwDefine(parameters.c_str());
	
  //Standard Menu:
  TwAddButton(menu, "reloadShader", reloadShadersCB, this, 
	      " label='Reload Shaders' key=r help='Reload the current shader from the file.'");
	
}

void MainWindow::initWindow() {

  //GLFW Initialization:
  if (!glfwInit()) {
    cerr << "Error: Could not initialize GLFW Window" << endl;
    exit(EXIT_FAILURE);
  }

  //Configurating window:

  //Forward Compatibility enabled. Do I need it?
  //glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //Setting minimum version to OpenGL 4.0
  //glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  //glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  //Setting the new profile, since we're using new opengl:
  //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //Initializing Window:
  if (!glfwOpenWindow(currentWidth,currentHeight,0,0,0,8,8,8,GLFW_WINDOW)) {
    cerr << "Error: Could not open GLFW Window" << endl;
    exit(EXIT_FAILURE);
  }

  //Setting window title and initial position:
  glfwSetWindowTitle(WINDOW_TITLE_PREFIX);
  glfwSetWindowPos(currentPosX,currentPosY);

  //Enable Key Repeating when a key is held down:
  glfwEnable(GLFW_KEY_REPEAT);
}


void MainWindow::applyFilter(Effect* filter) {
  glDrawBuffer(GL_COLOR_ATTACHMENT0+target);
  filter->renderTexture(fbo->bindAttachment(source));
  source = !source;
  target = !target;
  fbo->unbindAttachments();
}

void MainWindow::draw(void)
{

  glViewport(0, 0, currentWidth, currentHeight);

  // turn on buffer for multiple filter in sequence, enables the texture format (i.e GL_TEXTURE_2D)
  fbo->bind();
  glEnable(fbo->textureType());

  // ids for the ping-pong strategy, switches read and write buffers after each pass
  source = 0;
  target = 1;  

  // first pass, draw image to fbo's first attachment
  // another option is to save the original image to a third attachment, and reserve the other two
  // for the ping-pong scheme, this would avoid this first pass completely
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image_tex);
  renderimage->renderTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // apply as many filters as you like
  if(meanfilter->isEnabled()) {
    applyFilter(meanfilter);
  }
  if(gradientfilter->isEnabled()) {
    applyFilter(gradientfilter);
  } 

  // last pass, draw resulting image to screen
  fbo->unbind();
  glDrawBuffer(GL_BACK);
  renderimage->renderTexture(fbo->bindAttachment(source));
  glBindTexture(GL_TEXTURE_2D, 0);
  fbo->unbindAttachments();

  errorCheckFunc(__FILE__, __LINE__);

  //AntTweakBar:
  TwDraw();
  
  glDisable(fbo->textureType());
  errorCheckFunc(__FILE__, __LINE__);
}


void MainWindow::mainLoop(void) {
  double oldTime = glfwGetTime();
  while(glfwGetWindowParam(GLFW_OPENED)) {
    if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {
      break;
    }
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    glfwSwapBuffers();
  }
}

void MainWindow::resize(int width, int height) {

  setCurrentWidth(width);
  setCurrentHeight(height);
  glViewport(0, 0, width, height);

  meanfilter->resize(width, height);
  gradientfilter->resize(width, height);
  renderimage->resize(width, height);

  if (fbo) {
    delete fbo;
  }
  initializeFBO();
  
}

void MainWindow::reloadShaders(void) {
  meanfilter->reloadShaders();
  gradientfilter->reloadShaders();
  renderimage->reloadShaders();
}

void TW_CALL MainWindow::reloadShadersCB(void *window) {
  ((MainWindow*)window)->reloadShaders();
}

void TW_CALL MainWindow::selectShaderCB(void *filter) {
  ((Effect*)filter)->toggle();
}
