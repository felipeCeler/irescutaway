#ifndef __MAINWINDOW__
#define __MAINWINDOW__

#define WINDOW_TITLE_PREFIX "Shaders Library Visualization"

#include "rayCasting.hpp"
#include "AntTweakBar.h"
#include <string>

/**
 * Main Window class. This class defines the program, storing the GLFW windows, AntTweakBar interface, the mesh being rendered, the shader objects etc. 
**/
class MainWindow {

	public:

		///Default Constructor. Sets default window size to 800x800, initial window position to (100, 100).
		MainWindow(): activeEffect(NULL), currentWidth(800), currentHeight(800), currentPosX(100), currentPosY(100), windowHandle(0), frameCount(0) {
			windows.push_back(this);

		}

		/**Constructor that takes the window width and height as parameters. Initial window position is (100,100).
		 * @param width The width to be set as viewport Width.
		 * @param height The height to be set as viewport Height.
		**/
		MainWindow(int width, int height): currentWidth(width), currentHeight(height), currentPosX(100), currentPosY(100), windowHandle(0), frameCount(0) {

			windows.push_back(this);

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

		/**
		 * Returns a pointer to the Trackball object that represents the Camera Trackball.
		 * @return Returns the pointer to the camera trackball.
		**/
		Trackball* getCameraTrackball() {
			return cameraTrackball;
		}

		/**
		 * Returns a pointer to the Trackball object that represents the Light Trackball.
		 * @return Return the pointer to the light trackball.
		**/
		Trackball* getLightTrackball() {
			return lightTrackball;
		}

		/**
		 * Adds a reference to a shader object at the end of the shaders vector.
		 * @param shader The pointer to the shader that will be added to the shaders vector.
		**/
		void addShader(Shader* shader) {
			shaders.push_back(shader);
		}

		/**
		 * Adds a reference to an effect object at the end of the effects vector.
		 * @param effect The pointer to the effect that will be added to the effects vector.
		**/
		void addEffect(Effect* effect) {
			effects.push_back(effect);
		}

		/**
		 * Deletes a shader object and removes its reference from the shaders vector.
		 * @param shader Pointer to the shader that will be deleted and removed from the shaders vector.
		**/
		void deleteShader(Shader* shader) {
			for(int i = 0; i < shaders.size(); i++) {
				if (shaders[i] == shader) {
					delete shaders[i];
					shaders.erase(shaders.begin()+i);
				}
			}
		}

		///Deletes all shader objects and remove their references from the shaders vector.		
		void deleteAllShaders() {
			for(int i = 0; i< shaders.size(); i++) {
				delete shaders[i];
				shaders.erase(shaders.begin()+i);
			}
		}

		///Deletes all effect objects and remove their references from the effects vector.
		void deleteAllEffects() {
			for(int i = 0; i<effects.size();i++) {
				delete effects[i];
				effects.erase(effects.begin()+i);
			}
		}

		/**
		 * Sets the current shader to be used in mesh rendering by passing the pointer to the shader object as a parameter.
		 * @param shader Pointer to the shader that will be used in mesh rendering.
		**/
		void setCurrentShader(Shader* shader) {
			currentShader = shader;
		}

		/**
		 * Sets the current shader to be used in mesh rendering by passing the shader's name as a parameter.
		 * @param name The name of the shader to be set as current shader.
		**/
		void setCurrentShader(string name) {
			for(vector<Shader*>::iterator it = shaders.begin(); it != shaders.end(); it++) {
				if ((*it)->getShaderName() == name) {
					setCurrentShader(*it);
				}
			}
		}

		/**
		 * Sets the active effect to be used in mesh rendering by passing the pointer to the effect as a parameter.
		 * @param effect Pointer to the effect that will be used in mesh rendering.
		**/
		void setActiveEffect(Effect* effect) {
			activeEffect = effect;
		}

		/**
		 * Sets the active effect to be used in mesh rendering by passing the effect's name as a parameter.
		 * @param name The name of the effect to be set as active effect.
		**/
		void setActiveEffect(const char* name) {
			for(vector<Effect*>::iterator it = effects.begin(); it!= effects.end(); it++) {
				if((*it)->getEffectName() == name) {
					setActiveEffect(*it);
				}
			}
		}

		/**
		 * Returns a pointer to the shader being currently used for mesh rendering.
		 * @return Returns a pointer to the current shader object.
		**/
		Shader* getCurrentShader() {
			return currentShader;
		}

		/**
		 * Returns a pointer to the effect being currently used for mesh rendering.
		 * @return Returns a pointer to the active effect object.
		**/
		Effect* getActiveEffect() {
			return activeEffect;
		}


		/**
		 * Returns the current window identification handle.
		 * @return Return the Window Handle.
		**/
		int getWindowHandle() {
			return windowHandle;
		}

		///Increments the frame counter for fps measuring.
		void incrementFrameCount() {
			++frameCount;
		}

		/**
		 * Returns the value of the frame counter.
		 * @return Returns the Frame Count.
		**/
		unsigned getFrameCount() {
			return frameCount;
		}

		/**
		 * Sets the frame counter to a specific value.
		 * @param fc the value that the frame count will be set.
		**/
		void setFrameCount(unsigned fc) {
			frameCount = fc;
		}

		/**
		 * Convert a given 2D int vector to Normalized Device Coordinates System.
		 * @param position The position vector to be converted.
		 * @return Returns the position in the normalized device coordinates system.
		**/
		Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);

		///GLFW Render Function. Increments framecounter, clear color and depth buffers and calls draw function.
		void mainLoop();

		///Deletes the camera and light trackballs.
		void deleteTrackball() {
			if(cameraTrackball) {
				delete cameraTrackball;
			}

			if(lightTrackball) {
				delete lightTrackball;
			}
		}

		///Terminates GLFW Window.
		void shutDown() {
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}

		///Default Destructor. Detaches current shaders from the shader program, deletes the shaders, mesh buffers, trackballs, windows, effects, terminates antTweakBar and closes GLFW window.
		~MainWindow() {

			//MainWindow* currentWindow = MainWindow::getCurrentWindow(); 

			GLuint shaderProgram = getCurrentShader()->getShaderProgram();
			GLuint vertexShader = getCurrentShader()->getVertexShader();
			GLuint fragmentShader = getCurrentShader()->getFragmentShader();	
	
    		glDetachShader(shaderProgram, fragmentShader);
    		glDetachShader(shaderProgram, vertexShader);
    		glDeleteShader(fragmentShader);
    		glDeleteShader(vertexShader);
    		glDeleteProgram(shaderProgram);
			errorCheckFunc(__FILE__, __LINE__);

			deleteTrackball();
	
			cout << TwTerminate() << endl << endl;


			//Delete Windows:
			for(vector<MainWindow*>::iterator it = windows.begin(); it != windows.end(); it++) {
				if ((*it)->getWindowHandle() == windowHandle) {
					windows.erase(it);
				}
			}

			//Delete Shaders:
			deleteAllShaders();

			//Delete Effects:
			deleteAllEffects();

			glfwCloseWindow();

		}

	private:

		///Resizes the current window.
		void resize(int width, int height);

		///Initializes all menus and buttons from antTweakBar.
		void initializeAntTweakBar();

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

		///Creates the Ray Casting Effect.
		void createRayCasting();

		//Won't be needing these two methods for now.
		///Enables the AntTweakBar menu related to the active effect.
		//void enableEffectMenu();

		///Disables the AntTweakBar menu related to the active effect.
		//void disableEffectMenu();

		///Vector containing all the windows created.
		static vector<MainWindow*> windows;

		///Trackball Object for the camera position.
		Trackball* cameraTrackball;

		///Trackball object for lights position.
		Trackball* lightTrackball;

		///The Current Shader object.
		Shader* currentShader;

		///The currently active effect. If this points to something other than NULL, the rendering will be done directly from the effect's object render function.
		Effect* activeEffect;

		///Vector storing all the shaders loaded.
		vector<Shader*> shaders;

		///Vector storing all the effects.
		vector<Effect*> effects;

		///The current effect being used:
		Effect* currentEffect;

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

		///Frame Counter used for FPS measuring.
		unsigned frameCount;

		///Window Title:
		string windowTitle;

		///Initializes GLFW window and sets all the callback functions.
		void initWindow();

		///Calls the ray casting render function.	
		void draw(void);
	
		///GLFW Reshape Function. It takes the new width and height as parameters, resize the window, creates the new projection matrix in the cameraTrackball and sends it to the shader program.
		static void GLFWCALL reshapeFunction(int width, int height);
	
		///GLFW Idle Function.
		static void GLFWCALL idleFunction(void);
		
		///GLFW Timer Function. Handles FPS Counting.
		static void GLFWCALL timerFunction(int value);
		
		///GLFW Mouse Position Function. Handles callbacks used for the trackballs' transformations.
		static void GLFWCALL mousePosFunction(int x, int y);

		///GLFW Mouse Button Function. Handles callbacks used for the trackballs' transformations.
		static void GLFWCALL mouseButtonFunction(int button, int action);

		///GLFW Mouse Wheel Function. Handles callbacks used for the trackballs' transformations.
		static void GLFWCALL mouseWheelFunction(int pos);
		
		///GLFW Keyboard handling function.
		static void GLFWCALL keyboardFunction(int key, int action);

		///GLFW Close Function.
		static int GLFWCALL closeFunction(void);

		///AntTweakBar menu:
		TwBar* menu;

		///Reload Shaders callback function.
		static void TW_CALL reloadShaders(void *clientData);

		///Callback to handle shader selection buttons:
		static void TW_CALL selectShader(void *clientData);

		///Sets the effect related to the current shader as active effect.
		void enableEffect();

};


#endif

