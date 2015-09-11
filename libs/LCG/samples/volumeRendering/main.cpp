#include "mainWindow.hpp"

int main(int argc, char * argv[]) {
	MainWindow window;	
	window.initialize();
	window.mainLoop();
	window.shutDown();
}

