#ifndef __RAYCASTING__
#define __RAYCASTING__

#include "shaderlib/shader.hpp"
#include "shaderlib/trackball.hpp"
#include "shaderlib/effect.hpp"

using namespace std;

class RayCasting : public Effect{

	public:

	///Default Constructor. Initializes the shaders, volume, textures, transfer function and the trackball.
	void RayCasting() {

		//Sets the folder in which the shaders programs are stored.
		setShadersDir("shaders/raycasting");

		//Initialize Shaders:
		Shader computeShader = new Shader(shaders_dir, "computeShader");
		computeShader->initialize();
		Shader shaderProgram = new Shader(shaders_dir, "shaderProgram");
		shaderProgram->initialize();

		//Initialize Volume:
		Volume volume = new Volume();

		//Initialize Textures:
		//createTexture2D(currentImage);
		createTexture2D(accumulatedColorsTexture);
		createTexture1D(transferFunction);

		//Initialize transfer function:
		initializeTransferFunction();

		//Initialize trackball:
		cameraTrackball = new Trackball;
		cameraTrackball->initOpenGLMatrices();
		cameraTrackball->translateViewMatrix(Eigen::Vector3f(0.0,0.0,-18.0));//

	}

	/**
	 * Sets the step used to pass through the volume during the raycasting.
	 * @param step The step used to pass through the volume.
	**/
	void setDepthStep(float step) {
		depthStep = step;
	}

	/**
	 * Returns the depth step that is used to pass through the volume during the raycasting.
	 * @return Returns the depth step used to pass through the volume.
	**/
	float getDepthStep() {
		return depthStep;
	}

	///Renders the volume using the ray casting.
	void rayCastingRender();

	private:

	/**
	 * Computes the initial translation necessary in order not to waste computational power ray casting in the empty space, before the volume borders.
	 * @return Returns the vertex that is nearest to the screen position. That vertex will be the starting point for the ray casting.
	**/
	Eigen::Vector4f computeInitialTranslation();

	/**
	 * Given a point that belongs to the plane, the plane's normal and another arbitrary point, computes the distance between the plane and the point.
	 * @param planePoint The point that belongs to the plane.
	 * @param planeNormal The plane's normal vector.
	 * @param point An arbitrary point to which the distance from the plane will be computed.
	**/
	float computeDistanceFromPointToPlane(Eigen::Vector4f planePoint, Eigen::Vector4f planeNormal, Eigen::Vector4f point);

	///Initializes the transfer function of the Ray Casting.
	void initializeTransferFunction();

	///Shader used for ray-casting computation.
	Shader* computeShader;
	
	///Shader used for renderization.
	Shader* shaderProgram;

	///The current size of the viewport.
	vector2i viewportSize;

	///TexID representing the accumulated colors of the volume.
	GLuint accumulatedColorsTexture;

	///TexID used for the current color of the volume.
	//GLUint currentColorsTexture;

	///TexID used for transfer function computations. Initially a texture1D, later a texture2D.
	GLuint transferFunction;

	///Volume object.
	Volume* volume;

	///Step iteration distance.
	float depthStep;

	///Number of iterations needed to cover the whole volume with the ray casting.
	int numberOfIterations;

	///The trackball object representing the camera of the scene.
	Trackball* cameraTrackball;



};

#endif
