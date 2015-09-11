#ifndef __COMPUTE__
#define __COMPUTE__
#include "shaderlib/effect.hpp"
//#include "utils.hpp"

using namespace std;

/**
 * Just beginning to play with Compute Shaders. 
**/
class ComputeShaderTest : public Effect {

	public:

	///Default Constructor:
	ComputeShaderTest(const char* name, int currentWidth, int currentHeight): ssbo(0){

		effectName = name;
		viewportSize = Eigen::Vector2i(currentWidth, currentHeight);	

	}

	///Initializes the shaders and creates the FBO.
	virtual void initialize();

	///Renders the scene.
	virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL);

	///Reloads all shaders, reading the files again.
	virtual void reloadShaders();

	Shader* getFirstPassShader() {
		return firstPassShader;
	}

	///Default destructor. Just deletes the shaders and destroy the buffers.
	~ComputeShaderTest() {

		if(firstPassShader) {
			delete firstPassShader;
		}

		if(shader) {
			delete shader;
		}

		if(thirdPassShader) {
			delete thirdPassShader;
		}

	}

	protected:

	///Set opengl matrices.
	void setOpenGLMatricesUniforms(Shader* currentShader, Trackball* cameraTrackball, Trackball* lightTrackball);

	///Creates the Shader Storage Buffer Object.
	void createSSBO();

	///Creates the final image to store compute shader manipulations.
	void createImage();

	///Shader used to offscreen rendering.
	Shader* firstPassShader;

	///Shader used in back-buffer rendering.
	Shader* thirdPassShader;

	///Shader Storage Buffer Object.
	GLuint ssbo;

	///Image to store the compute shader manipulations.
	GLuint finalImage;

};



#endif
