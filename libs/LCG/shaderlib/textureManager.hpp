#ifndef __TEXTUREMANAGER__
#define __TEXTUREMANAGER__
#include <vector>
#include <iostream>
#include <GL/glew.h>

using namespace std;

//using TextureManager::Instance() textureManager;

class TextureManager {

	//Still got to think about an interesting way to set texture parameters here. Besides, got to see a way to decrease the number of parameters for the create texture method.

	//One problem: framebuffer has it's method for storing the free units. What will I do? Use the texture manager in the framebuffer?

	//I need to test the texturemanager in an application that uses more than one texture unit at a time.
	public:

	/** 
	 * This method is the only one allowed to call the Texture Manager constructor. It only constructs a new TextureManager object if there wasn't any created yet. Otherwise it just returns
	 * the TextureManager object.
	**/
	static TextureManager* Instance();

	///Creates a texture object and returns its handler.
	GLuint createTexture(GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data = NULL, int n = 1, int lod = 0, int depth = 256);

	///Updates the image on a texture.
	void updateTexture(GLuint texID, GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data, int n, int lod, int depth = 256);

	/**
	 * Binds a texture to a unit given by the user.
	**/
	void bindTexture(GLenum texType ,GLuint texID, int textureUnit);

	/**
	 * Binds a texture to the first free texture unit and returns the unit used.
	**/
	int bindTexture(GLenum texType ,GLuint texID);

	///Returns the first available texture unit.
	int getAvailableUnit();

	///Sets one unit as unavaiable, in case something besides the texture manager has bound a texture to it.
	void setUnavailableUnit(int unit);

	///Unbinds the texture from the specific texture unit.
	void unbindTexture(GLenum texType, int textureUnit);

	///Unbinds all textures in use of a specific type.
	void unbindAllTextures(GLenum texType);

	//THAT'S SOMETHING IMPORTANT. SHOULDN'T REALLY I DELETE THE INSTANCE? WON'T THERE BE A MEMORY LEAKING THERE?
	~TextureManager() {};

private:

	//I need to define the constructor, the copy constructor and the operator= as private in order to ensure that only one Texture Manager can be created, through the use of the Instance() method.

	///Default Constructor
	TextureManager() {

		    // get maximum number of texture units, and set all as not used
   			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnits);
			for (int i = 0; i<maxTexUnits; i++) {
				usedTextureUnits.push_back(0);
			}

	}; 

	///Copy Constructor
	TextureManager(TextureManager const&) {};

	///Assignment Operation
	TextureManager& operator=(TextureManager const&) {};

	///Pointer to the instance of the Texture Manager object.
	static TextureManager* pInstance;

	/// Maximum number of color attachments
	int maxTexUnits;

	///This set holds the texture units that are currently being used. 
	vector<bool> usedTextureUnits;

};


//classe texture
//método fill texture
//tirar o set<int> usedTextureUnits e passar pra vector[máximo_de_texturas] (olhar no fbo)
//criar método pra setar os parâmetros dentro da classe texture e colocar um default (ver ssao noise texture)
//tirar o int unit do ssao. Jogar direto o bindtexture denntro do setuniform
//fbo usar texturemanager

#endif
