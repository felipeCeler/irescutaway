#ifndef __TOON__
#define __TOON__
#include "shaderlib/effect.hpp"

using namespace std;

/**
 * Toon effect class. It is used to store the quantization level for the effect and to handle the mesh rendering.
**/
class Toon: public Effect {

	public:

	/**
	 * Constructor. Just initializes the quantization level and effect name.
	 * @param name The effect's name.
	**/
	Toon(const char* name): quantizationLevel(8) {
		effectName = name;
	}

	///Initializes the effect, creating and loading the shader.
	virtual void initialize();

	/**
	 * Renders the mesh with the Toon effect.
	 * @param mesh A pointer to the mesh object.
	 * @param cameraTrackball A pointer to the camera trackball object.
	 * @param lightTrackball A pointer to the light trackball object.
	 **/
	virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL);

	///Reloads the shader, reading the files again.
	virtual void reloadShaders();

	/**
	 * Returns a pointer to the quantization level attribute. Used to create a button to adjust this attribute within the GUI.
	 * @return Returns a pointer to the quantization level.
	**/
	float* getQuantizationLevelPointer() {
		return &quantizationLevel;
	}

	private:

	///Represents the number of colors that will be used in color quantization to create the toonish effect.
	float quantizationLevel;

};

#endif
