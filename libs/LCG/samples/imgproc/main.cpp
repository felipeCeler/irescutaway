#include "mainWindow.hpp"


MainWindow *window;

void GLFWCALL reshapeFunction(int width, int height) {
  window->resize(width, height);
}

void GLFWCALL keyboardFunction(int key, int action) {
  if(!TwEventKeyGLFW(key, action)) {}
}

void GLFWCALL mouseButtonFunction(int button, int action) {
  if(!TwEventMouseButtonGLFW(button,action)) {
  }
}

void GLFWCALL mouseWheelFunction(int pos) {
  if(!TwEventMouseWheelGLFW(pos)) {}
}

void GLFWCALL mousePosFunction(int x, int y) {
  if(!TwEventMousePosGLFW(x, y)) {}
 }



int main(int argc, char * argv[]) {		

  window = new MainWindow(500,335);

  window->initialize();

  //Setting Callback functions:
  glfwSetWindowSizeCallback(reshapeFunction);
  glfwSetKeyCallback(keyboardFunction);
  glfwSetMousePosCallback(mousePosFunction);
  glfwSetMouseButtonCallback(mouseButtonFunction);
  glfwSetMouseWheelCallback(mouseWheelFunction);


  window->mainLoop();
  window->shutDown();

  delete window;
}

