#ifndef __SSAO__
#define __SSAO__
#include "shaderlib/effect.hpp"
#include "shaderlib/textureManager.hpp"
using namespace std;

/**
 * Screen Space Ambient Occlusion effect class. Handles the pre-computations needed in order to use this effect, as well as handles the rendering with this effect. 
 * This class handles sampling kernel generation used in occlusion computation, noise texture generation used to remove banding caused by the low number of sample points.
 * Three rendering passes are needed in order to use this effect: in the first pass, the eye-space depth information of the mesh is stored in one channel of a 2D texture. In the second pass, using this
 * depth information, the occlusion factor is computed and used to scale the ambient light. A small noise texture is tiled through thw whole mesh to remove banding due to low number of samples needed to 
 * keep computational cost low. In the third pass, a gaussian blur is applied in order to remove the high frequency noise.
**/
class SSAO: public Effect {

	public:

	/** Default constructor. User must pass the viewport width and height used for framebuffer initialization and noise scale factor computation and can optionally pass the number of samples used for 
	 * occlusion computation, dimension of the noise texture and kernel radius. It's important to notice that if the number of samples is changed here, it also needs to be changed in the shader code. It 
	 * can't be passed as a uniform to the shader because it represents the size of the sample points' array.
	 * @param name The name of the effect.
	 * @param currentWidth the current Viewport Width.
	 * @param currentHeight the current Viewport Height.
	 * @param noiseTextureDimension The dimension of the noise texture to be generated.
	 * @param sampleKernelSize The size of the kernel array that will store the sample points. This means that a number of points equal to
	 * sampleKernelSize will be sampled in order to compute occlusion.
	 * @param rad The kernel radius. This is used to define the max distance between the current point and the samples that will be considered for occlusion computation.
	**/
	SSAO(const char* name, int currentWidth, int currentHeight, int noiseTextureDimension = 4, int sampleKernelSize = 16, float rad = 0.0001):
		 noiseSize(noiseTextureDimension), numberOfSamples(sampleKernelSize), radius(rad), applyBlur(true), displayAmbientPass(false), blurRange(3), depthTextureID(0), blurTextureID(1){

		effectName = name;
		viewportSize = Eigen::Vector2i(currentWidth, currentHeight);	
		computeNoiseScale();

	}

	///Initializes the SSAO effects, generating the sample kernel, creating and loading the shaders and generating the noise texture.
	virtual void initialize();

	/**
	 * Renders the mesh with the desired effect.
	 * @param mesh A pointer to the mesh that will be rendered.
	 * @param cameraTrackball A pointer to the camera trackball object.
	 * @param lightTrackball A pointer to the light trackball object.
	**/
	virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL);

	///Change the current state of applyBlur flag.
	void changeBlurFlag();

	///Increases blur range.
	void incrementBlurRange() {
		blurRange += 2;
	}

	///Decreases blur range.
	void decrementBlurRange() {
		if(blurRange >=5) {
			blurRange -= 2;
		}
		else {
			blurRange = 3;
		}
	}

	/**
	 * Returns a pointer to the apply blur flag. It is used to create the button to select wether the blur will or won't be applied through GUI.
	 * @return Return a pointer to the apply blur flag.
	**/
	bool* getBlurFlagPointer() {
		return &applyBlur;
	}

	/**
	 * Returns a pointer to the blur range attribute. It is used to create the button to adjust the blur intensity through GUI.
	 * @return Return a pointer to the blur range attribute.
	**/
	int* getBlurRangePointer() {
		return &blurRange;
	}

	/**
	 * Returns a pointer to the display ambient pass flag. It is used to create the button to select through GUI wether the mesh will show only the AO pass or the full illumination.
	 * @return Return a pointer to the display ambient pass flag.
	**/
	bool* getAmbientPassFlagPointer() {
		return &displayAmbientPass;
	}

	/**
	 * Returns a pointer to the number of samples attribute. It is used to create the button displaying the number of samples in GUI.
	 * @return Return a pointer to the number of samples attribute.
	**/
	int* getNumberOfSamplesPointer() {
		return &numberOfSamples;
	}

	/**
	 * Returns a pointer to the sampling radius attribute. It is used to create the button displaying the sampling radius in GUI.
	 * @return Return a pointer to the sampling radius attribute.
	**/
	float* getSamplingRadiusPointer() {
		return &radius;
	}

	///Change the current state of displayAmbientPass flag.
	void changeAmbientPassFlag();

	///Reload all shaders needed for SSAO effect.
	virtual void reloadShaders();

	///Default destructor. Destroy the FBO, deletes the shaders and the sampling kernel.
	~SSAO() {
/* Got to see if Ricardo is calling these functions anywhere. 
		if(framebuffer) {
			glDeleteFramebuffers(1, &framebuffer);
		}

		if(depthbuffer) {
			glDeleteRenderbuffers(1,&depthbuffer);
		}



		if(fboTexture) {
			glDeleteTextures(1,&fboTexture);
		}

*/

		if(shader) {
			delete shader;
		}

		if(deferredShader) {
			delete deferredShader;
		}

		if(blurShader) {
			delete blurShader;
		}

		if(kernel) {
			delete kernel;
		}

	}

	private:

	///Computes the noise scale factor, that will be used for tiling the noise texture through the screen.
	void computeNoiseScale();

	///Creates and loads all shaders.
	void initializeShaders();

	/**
	 *Sets Model, View, Projection and Light Rotation matrices uniforms to a current shader.
	 *@param currentShader A pointer to the shader to which the uniforms will be set.
	 *@param cameraTrackball A pointer to the trackball object for the camera. It stores the model, view and projection matrices.
	 *@param lightTrackball A pointer to the trackball object for the light. It stores the light rotation matrix.
	**/
	void setOpenGLMatricesUniforms(Shader* currentShader, Trackball* cameraTrackball, Trackball* lightTrackball);

	///Generates a sampling kernel.
	void generateKernel();

	///Generates a random noise texture.
	void generateNoiseTexture();

	/**Linear interpolation function. Interpolates between a and b with a scale factor s.
	 * @param a The minimum point for interpolation.
	 * @param b The maximum point for interpolation. 
	 * @param s The scale factor used for interpolation.
	**/
	float lerp(const float a, const float b, const float s);

	/**Generate random number in range [min,max].
	 * @param min The minimum value for random number generation.
	 * @param max The maximum value for random number generation.
	**/
	float random(int min, int max);

	///Noise texture dimension. It will be a noiseSize x noiseSize texture.
	int noiseSize;

	///Scale used to tile the noise texture through screen.
	Eigen::Vector2f noiseScale;

	///Noise texture identification.
	GLuint noiseTexture;

	///Array of sample points generated inside a unit hemisphere around z axis.
	float *kernel;

	///Kernel radius. If the distance between a sample point and the point for which the occlusion is being computed is larger than radius, the occlusion for this sample will be neglected.
	float radius;

	///Number of sample points that will be used per fragment for occlusion computation.
	int numberOfSamples;

	///The shader used to store the depth information in the framebuffer.
	Shader* deferredShader;

	///The shader used to blur the result from the second pass, removing the noise pattern.
	Shader* blurShader;

	///Flag indicating wether blur shall be applied or not.
	bool applyBlur;

	///Flag indicating if the mesh should be rendered only with ambient occlusion pass or with full illumination. If True, mesh will be rendered only with the ambient occlusion pass.
	bool displayAmbientPass;

	///Number of neighbour pixels used in blurring. The blur will be applied to a blurRange x blurRange square around the current pixel. It's important to notice that blurRange must be an odd number.
	int blurRange;

	///The ID defining the color attachment to which the depth texture is bound in the framebuffer.
	int depthTextureID;

	///The ID defining the color attachment to which the blur texture is bound in the framebuffer.
	GLuint blurTextureID;

};

#endif
