#include "mainWindow.hpp"

vector<MainWindow*> MainWindow::windows;

void MainWindow::initialize() {

	GLenum glewInitResult;
	cameraTrackball = new Trackball;
	lightTrackball = new Trackball;

	initWindow();

	//Glew Initialization:
	glewInitResult = glewInit();

	//Check Glew Initialization:
	if (GLEW_OK != glewInitResult) {
		cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
		exit(EXIT_FAILURE);
	}
	errorCheckFunc(__FILE__, __LINE__);

	//Enable Depth Testing and Culling:
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	errorCheckFunc(__FILE__, __LINE__);
 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	errorCheckFunc(__FILE__, __LINE__);
 
	//Init OpenGL Matrices:
	cameraTrackball->initOpenGLMatrices();
	lightTrackball->initOpenGLMatrices();
	Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );
	cameraTrackball->setProjectionMatrix(projectionMatrix);

	//Creating VBOs:
	mesh->createBuffers();
	cameraTrackball->createBuffers();
	cameraTrackball->bufferData();

	initializeAntTweakBar();

	createShader("phongShader", "shaders/phongShader.vert", "shaders/phongShader.frag", "Phong Shader", "Simple Phong Shading.");
	createShader("normalsShader", "shaders/normalsShader.vert", "shaders/normalsShader.frag", "Normals Shader", "This shader maps normal to color. Good for debugging the normals.");
	createSSAO();
	createToon();
	createCompute();
	setCurrentShader("phongShader");

	//Trackball Shader generation:
	cameraTrackball->loadShader();

	//Load model:
	mesh->loadObjFile("../../models/stay-puff-poisson.obj");
	Eigen::Vector3f translation = -mesh->getObjectCenter();
	cameraTrackball->applyScaleToModelMatrix(mesh->getScale());
	cameraTrackball->translateModelMatrix(translation);

	//Setting final help informations:
	TwDefine("menu/Shaders help='Select the current shader to be used in mesh rendering.'");

	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
}

void MainWindow::createToon() {

	Toon* toon = new Toon("Toon");
	toon->initialize();
	Shader* toonShader = toon->getShader();
	addShader(toonShader);
	addEffect(toon);

	float* quantizationLevel = toon->getQuantizationLevelPointer();

	//AntTweakBar:
	TwAddButton(menu, "toonShader", selectShader, toonShader, " label= 'Toon Shader' key=4 group='Shaders' help='Toonish effect' ");
	TwAddVarRW(menu, "quantizationLevel", TW_TYPE_FLOAT, quantizationLevel, 
				" label='Number of Colors' group='Toon' keyIncr='+' keyDecr='-' step=1 help='Changes the number of colors used.' min=1");
	TwDefine("menu/Toon help='Toon Shader configurations.'");
	TwDefine("menu/Toon visible=false");

}

void MainWindow::createSSAO() {

	//SSAO Shader generation:
	SSAO* ssao = new SSAO("SSAO",currentWidth,currentHeight);
	ssao->initialize();
	Shader* ssaoShader = ssao->getShader();
	addShader(ssaoShader);
	addEffect(ssao);

	//AntTweakBar:
	TwAddButton(menu, "ssaoShader", selectShader, ssaoShader, 
				" label='SSAO Shader' key='3' group='Shaders' help='Screen Space Ambient Occlusion Shader.'");

	//Getting Pointers:
	bool* applyBlur = ssao->getBlurFlagPointer();
	bool* ambientPass = ssao->getAmbientPassFlagPointer();
	int* blurRange = ssao->getBlurRangePointer();
	int* numberOfSamples = ssao->getNumberOfSamplesPointer();
	float* samplingRadius = ssao->getSamplingRadiusPointer();

	//SSAO Shading Menu:
	TwAddVarRW(menu, "applyBlur", TW_TYPE_BOOLCPP, applyBlur , 
				" label='Apply Blur' key='B' group='SSAO' help='If checked, blur the final scene in order to remove high frequency noise caused by SSAO.'");
	TwAddVarRW(menu, "ambientPass", TW_TYPE_BOOLCPP, ambientPass , 
				" label='Display Only SSAO Pass' key='A' group='SSAO' help='If checked, renders the mesh only with the ambient occlusion pass.'");
	TwAddVarRW(menu, "blurRange", TW_TYPE_INT8, blurRange, 
				" label='Blur Intensity' group='SSAO' keyIncr='+' keyDecr='-' step=2 help='Changes the blur intensity.' min=1 max=21");
	TwAddVarRO(menu, "numberOfSamples", TW_TYPE_INT8, numberOfSamples, 
				" label='Number of Samples' group='SSAO' help='Number of sampling points used for SSAO computation' ");
	TwAddVarRO(menu, "samplingRadius", TW_TYPE_FLOAT, samplingRadius, " label='Sampling Radius' group='SSAO'" 
			   " help='Maximum distance between a point and the sample from which occlusion should be taken in account for SSAO computation.'");

	TwDefine("menu/SSAO help='SSAO Shader configurations.'");
	TwDefine("menu/SSAO visible=false");

}

void MainWindow::createCompute() {

	ComputeShaderTest* compute = new ComputeShaderTest("Compute", currentWidth, currentHeight);
	compute->initialize();
	Shader* computeShader = computeShader = compute->getShader();
	addShader(computeShader);
	addEffect(compute);

	TwAddButton(menu, "computeShader", selectShader, computeShader, " label='Compute Shader' key = '4' group='Shaders' help='Playing with compute shaders' ");

}

void MainWindow::createShader(const char* shaderName, const char* vertPath, const char* fragPath, const char* displayName, const char* helpMessage) {

	Shader* shader = new Shader(shaderName, vertPath, fragPath);
	shader->initialize();
	addShader(shader);

	ostringstream key;
	key << shaders.size();

	std::string parameters;
	parameters = " label='" + std::string(displayName) + "' key='" + key.str() + "' group='Shaders' ";
	if(helpMessage) parameters += "help='" + std::string(helpMessage) + "' ";

	TwAddButton(menu, shaderName, selectShader, shader, parameters.c_str());

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
	int menuYSize = currentHeight*0.3;
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

	//Getting Pointers to link to buttons:
	bool* drawTrackball = cameraTrackball->getDrawTrackballFlag();

	//Getting light Direction:
	float* lightDirection = lightTrackball->getQuaternionVector();
	
	//Standard Menu:
	TwAddVarRO(menu, "quaternion", TW_TYPE_QUAT4F, lightDirection, 
				" label='Light Direction' help='Light Direction Representation' opened=true arrow='0 0 -1'");
	TwAddVarRW(menu,"drawTrackball",TW_TYPE_BOOLCPP, drawTrackball, 
			   " label='Draw Position Trackball' key=t help='If checked, draw trackball representation on screen.'");
	TwAddButton(menu, "reloadShader", reloadShaders, NULL, 
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

	//Setting Callback functions:
	glfwSetWindowSizeCallback(reshapeFunction);
	glfwSetKeyCallback(keyboardFunction);
	glfwSetMousePosCallback(mousePosFunction);
	glfwSetMouseButtonCallback(mouseButtonFunction);
	glfwSetMouseWheelCallback(mouseWheelFunction);

}

void MainWindow::enableEffectMenu() {
	if(activeEffect != NULL) {
		if(activeEffect->getEffectName() == "SSAO") {
			TwDefine("menu/SSAO visible=true");
		}
		if(activeEffect->getEffectName() == "Toon") {
			TwDefine("menu/Toon visible=true");
		}
	}
}

void MainWindow::disableEffectMenu() {

	if(activeEffect != NULL) {		
		if(activeEffect->getEffectName() == "SSAO") {
			TwDefine("menu/SSAO visible=false");	
		}	
		if(activeEffect->getEffectName() == "Toon") {
			TwDefine("menu/Toon visible=false");
		}
	}

}

void MainWindow::draw(void)
{
	//When using "phongShader", the matrices multiplication order is: Projection * View * Model * Vertex.

	if(activeEffect != NULL) {
		activeEffect->render(mesh, cameraTrackball, lightTrackball);
	}

	else {
		currentShader->enable();

		//Define standard color for mesh rendering:
		Eigen::Vector4f colorVector;
		colorVector << 0.686, 0.933, 0.933, 1.0;
		//colorVector << 1.0,0.0,0.0,1.0;
		//colorVector << 0.0,0.0,1.0,1.0;

		//Setting Uniforms:
		currentShader->setUniform("in_Color",colorVector.data(),4,1);
		currentShader->setUniform("projectionMatrix", cameraTrackball->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
		currentShader->setUniform("modelMatrix",cameraTrackball->getModelMatrix().data(), 4, GL_FALSE, 1);
		currentShader->setUniform("viewMatrix",cameraTrackball->getViewMatrix().data(), 4, GL_FALSE, 1);
		Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
		currentShader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
		errorCheckFunc(__FILE__, __LINE__);
	
		//Mesh Rendering:
		mesh->render();

		currentShader->disable();
	}
	errorCheckFunc(__FILE__, __LINE__);

	//Drawing the Trackball:
	cameraTrackball->render();

	//AntTweakBar:
	TwDraw();

	errorCheckFunc(__FILE__, __LINE__);
}

MainWindow* MainWindow::getCurrentWindow() {

	if(windows.size() == 1) {
		return windows[0];
	}	
	
	/*int curWindow = glutGetWindow();
	
	for(vector<MainWindow*>::iterator it = windows.begin(); it != windows.end(); it++) {
		if ((*it)->getWindowHandle() == curWindow) {
			return *it;
		}
	}*/

	return NULL;

}

Eigen::Vector2f MainWindow::convertToNormalizedDeviceCoordinates(Eigen::Vector2i position) {
	float x = (float)position[0]/(float)(currentWidth/2.0);
	x = x-1;

	float y = (float)position[1]/(float)(currentHeight/2.0);
	y = 1-y;

	Eigen::Vector2f ret(x,y);
	return ret;
}

void MainWindow::mainLoop(void) {
	double oldTime = glfwGetTime();
	while(glfwGetWindowParam(GLFW_OPENED)) {
		if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {
			break;
		}
		incrementFrameCount();
		glClearColor(1.0,1.0,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		glfwSwapBuffers();
		double currentTime = glfwGetTime();
		int fps = 1/(currentTime-oldTime);
	}
}

void MainWindow::resize(int width, int height) {

	setCurrentWidth(width);
	setCurrentHeight(height);
	glViewport(0, 0, width, height);

	for (int i = 0; i<effects.size(); i++) {
		effects[i]->resize(width, height);
	}

}

void MainWindow::reshapeFunction(int width, int height) {

	MainWindow* currentWindow = MainWindow::getCurrentWindow();  
	Trackball* cameraTrackball = currentWindow->getCameraTrackball();
	Shader* currentShader = currentWindow->getCurrentShader();

	currentWindow->resize(width, height);

	//Projection Matrix definition:
	Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)width/(float)height, 1.0f , 100.0f );
	cameraTrackball->setProjectionMatrix(projectionMatrix);

	Eigen::Matrix4f trackballProjectionMatrix = cameraTrackball->createOrthographicMatrix(-1.0,1.0,-1.0,1.0,1.0f,100.0f);
	cameraTrackball->setTrackballProjectionMatrix(trackballProjectionMatrix);
 
}

/*
void MainWindow::timerFunction(int value)
{
	//Handles FPS Count

	MainWindow* currentWindow = MainWindow::getCurrentWindow();	

    if (0 != value) {
        char* tempString = (char*)
            malloc(512 + strlen(WINDOW_TITLE_PREFIX));
 		
        sprintf(
            tempString,
            "%s: %d FPS @ %d x %d",
            WINDOW_TITLE_PREFIX,
            currentWindow->getFrameCount() * 4,
            currentWindow->getCurrentWidth(),
            currentWindow->getCurrentHeight()
        );
 		
        glutSetWindowTitle(tempString);
        free(tempString);
    }
 
    currentWindow->setFrameCount(0);
    glutTimerFunc(250, timerFunction, 1);
}
*/

void MainWindow::mouseButtonFunction(int button, int action) {
	if(!TwEventMouseButtonGLFW(button,action)) {
		MainWindow* currentWindow = MainWindow::getCurrentWindow();  
		Trackball* cameraTrackball = currentWindow->getCameraTrackball();
		Trackball* lightTrackball = currentWindow->getLightTrackball();

		int x, y;
		glfwGetMousePos(&x, &y);
	
		//Position vector in [0,viewportWidth] domain:
		Eigen::Vector2i pos(x,y);

		//Convert the mouse coordinates vector to normalized device coordinates system
		Eigen::Vector2f positionInTrackballSystem;
		positionInTrackballSystem = currentWindow->convertToNormalizedDeviceCoordinates(pos);

		//Camera Translating:
		if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS) && (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS)) {
			cameraTrackball->setInitialTranslationPosition(positionInTrackballSystem);
			cameraTrackball->beginTranslation();
		}

		//Camera Rotating:
		else if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
			cameraTrackball->setInitialRotationPosition(positionInTrackballSystem);
			cameraTrackball->beginRotation();
		}
		if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE) ) {
			if(cameraTrackball->isRotating()) {
				cameraTrackball->endRotation();
			}
			else if(cameraTrackball->isTranslating()) {
				cameraTrackball->endTranslation();
			}
		}

		//Light Rotating:
		if( (button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS) ) {
			lightTrackball->setInitialRotationPosition(positionInTrackballSystem);
			lightTrackball->beginRotation();

		}
		if( (button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE) ) {
			lightTrackball->endRotation();
		}
	}
}

void MainWindow::mouseWheelFunction(int pos) {

	if(!TwEventMouseWheelGLFW(pos)) {
		MainWindow* currentWindow = MainWindow::getCurrentWindow();  
		Trackball* cameraTrackball = currentWindow->getCameraTrackball();

		//Wheel Up:
		//Precise Zooming:
		if( (pos > 0) && (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) ) {
			cameraTrackball->increaseZoom(1.01);
		}
		//Fast Zooming:
		else if(pos > 0) {
			cameraTrackball->increaseZoom(1.05);
		}

		//Wheel Down:
		//Precise Zooming:	
		if( (pos < 0) && (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS)) {
			cameraTrackball->decreaseZoom(1.01);
		}
		//Fast Zooming:
		else if(pos < 0) {
			cameraTrackball->decreaseZoom(1.05);
		}

		glfwSetMouseWheel(0);
	}
}

void MainWindow::mousePosFunction(int x, int y) {
	if(!TwEventMousePosGLFW(x, y)) {
		//cout << "x: " << x << " y: " << y << endl;

		MainWindow* currentWindow = MainWindow::getCurrentWindow();  
		Trackball* cameraTrackball = currentWindow->getCameraTrackball();
		Trackball* lightTrackball = currentWindow->getLightTrackball();

		//Position vector in [0,viewportWidth] domain:
		Eigen::Vector2i pos(x,y);

		//Convert to Normalized Device Coordinates System:
		Eigen::Vector2f positionInTrackballSystem;
		positionInTrackballSystem = currentWindow->convertToNormalizedDeviceCoordinates(pos);

		//It's important to remember that it only makes sense to compute a rotation if initial and final positions vector are different.

		//Camera Rotating:
		if(cameraTrackball->isRotating()) {
			Eigen::Vector3f initialPosition = cameraTrackball->getInitialRotationPosition();
			if(positionInTrackballSystem != initialPosition.head(2)) {
				cameraTrackball->setFinalRotationPosition(positionInTrackballSystem);
				cameraTrackball->rotateCamera();
				cameraTrackball->setInitialRotationPosition(positionInTrackballSystem);

			}
		}

		//Light Rotating:
		else if(lightTrackball->isRotating()) {
			Eigen::Vector3f initialPosition = lightTrackball->getInitialRotationPosition();
			if(positionInTrackballSystem != initialPosition.head(2)) {
				lightTrackball->setFinalRotationPosition(positionInTrackballSystem);
				lightTrackball->rotateCamera();
				lightTrackball->setInitialRotationPosition(positionInTrackballSystem);
			}
		}

		//Camera Translating:
		else if(cameraTrackball->isTranslating()) {
			Eigen::Vector2f initialPosition = cameraTrackball->getInitialTranslationPosition();
			if(positionInTrackballSystem != initialPosition.head(2)) {
				cameraTrackball->setFinalTranslationPosition(positionInTrackballSystem);
				cameraTrackball->translateCamera();
				cameraTrackball->setInitialTranslationPosition(positionInTrackballSystem);
			}
		}
	}
}

void TW_CALL MainWindow::reloadShaders(void *clientData) {

	MainWindow* currentWindow = MainWindow::getCurrentWindow();
	Shader* currentShader = currentWindow->getCurrentShader();
	Effect* activeEffect = currentWindow->getActiveEffect();

	if(activeEffect != NULL) {
		activeEffect->reloadShaders();
	}

	else {
		currentShader->reloadShaders();
	}

}

void TW_CALL MainWindow::selectShader(void *clientData) {

	MainWindow* currentWindow = MainWindow::getCurrentWindow();

	Shader* shader = (Shader *) clientData;

	currentWindow->setCurrentShader(shader->getShaderName());
	currentWindow->enableEffect();

}

void MainWindow::enableEffect() {
	
	//Reset effect:
	disableEffectMenu();
	activeEffect = NULL;
	
	//Search effects vector to see if there's any effect related to the current shader:
	for(int i = 0; i<effects.size(); i++) {
		string effectShaderName = effects[i]->getShader()->getShaderName();
		if(effectShaderName == currentShader->getShaderName()) {
			activeEffect = effects[i];
			enableEffectMenu();
		}
	}

}

void MainWindow::keyboardFunction(int key, int action) {
	if(!TwEventKeyGLFW(key, action)) {
		MainWindow* currentWindow = MainWindow::getCurrentWindow();  
		Trackball* cameraTrackball = currentWindow->getCameraTrackball();
		Shader* currentShader = currentWindow->getCurrentShader();
	
		/*if ((key == 'R') && (action == GLFW_PRESS)) {
			if(currentShader->getShaderName() == "ssaoShader") {
				ssao->reloadShaders();
			}
			else {
				currentShader->reloadShaders();
			}
		}
	
		if((key == 'B') && (action == GLFW_PRESS)) {
			if(currentShader->getShaderName() == "ssaoShader") {
				ssao->changeBlurFlag();
			}
		}

		if((key == 'A') && (action == GLFW_PRESS)) {
			if(currentShader->getShaderName() == "ssaoShader") {
				ssao->changeAmbientPassFlag();
			}
		}

		if((key == '+') && (action == GLFW_PRESS)) {
			if(currentShader->getShaderName() == "ssaoShader") {
				ssao->incrementBlurRange();
			}
		}

		if((key == '-') && (action == GLFW_PRESS)) {
			if(currentShader->getShaderName() == "ssaoShader") {
				ssao->decrementBlurRange();
			}
		}

		if((key == '1') && (action == GLFW_PRESS)) {
			currentWindow->setCurrentShader("phongShader");
		}

		if((key == '2') && (action == GLFW_PRESS)) {
			currentWindow->setCurrentShader("normalsShader");
		}

		if((key == '3') && (action == GLFW_PRESS)) {
			currentWindow->setCurrentShader("ssaoShader");
		}*/
	}
}
/*
int MainWindow::closeFunction(void) {

	MainWindow* currentWindow = MainWindow::getCurrentWindow(); 

	GLuint shaderProgram = currentWindow->getCurrentShader()->getShaderProgram();
	GLuint vertexShader = currentWindow->getCurrentShader()->getVertexShader();
	GLuint fragmentShader = currentWindow->getCurrentShader()->getFragmentShader();	

    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteProgram(shaderProgram);
	errorCheckFunc(__FILE__, __LINE__);
 
	currentWindow->getMesh()->deleteBuffers();

	currentWindow->deleteTrackball();

	cout << TwTerminate() << endl << endl;

	errorCheckFunc(__FILE__, __LINE__);
}


*/


