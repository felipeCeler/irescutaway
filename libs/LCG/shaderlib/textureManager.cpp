#include "textureManager.hpp"

//Not sure why it is here.
//http://www.yolinux.com/TUTORIALS/C++Singleton.html says: "Global static pointer used to ensure a single instance of the class."
//Need to understand it better.
TextureManager* TextureManager::pInstance = NULL;

TextureManager* TextureManager::Instance() {

	if (!pInstance) {
		pInstance = new TextureManager;
	}

	return pInstance;

}

GLuint TextureManager::createTexture(GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data, int n, int lod, int depth) {
	//I still got to read better, see if texID must be a pointer or just an integer. I believe that perhaps it needs to be a pointer, if more than one texture is being created. For now I'll just keep returning just an integer. If i could be an array of integers, I got to see how to handle the binding and initializing.
	//I had some problems, so for now I can only create one texture at a time here. Later got to see how to make this work with an array of texIDs.
	if(n>0) {
		GLuint texID;
		glGenTextures(n, &texID);
		glBindTexture(texType, texID);
		if(texType == GL_TEXTURE_2D) {
			glTexImage2D(texType, lod, internalFormat,width,height,0,format,pixelType,data);
		}
		else if (texType == GL_TEXTURE_3D) {
			glTexImage3D(texType, lod, internalFormat,width,height, depth, 0,format,pixelType,data);
		}
		else if (texType == GL_TEXTURE_1D) {
			glTexImage1D(texType, lod, internalFormat,width,0,format,pixelType,data);
		}
		glBindTexture(texType, 0);

		return texID;
	}
	else {
		cout << "Unable to create texture: Invalid number of textures" << endl;
		return 0;
	}
}

void TextureManager::updateTexture(GLuint texID, GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data, int n, int lod, int depth) {

	glBindTexture(texType, texID);
	if(texType == GL_TEXTURE_2D) {
		glTexImage2D(texType, lod, internalFormat,width,height,0,format,pixelType,data);
	}
	else if (texType == GL_TEXTURE_3D) {
		glTexImage3D(texType, lod, internalFormat,width,height, depth, 0,format,pixelType,data);
	}
	else if (texType == GL_TEXTURE_1D) {
		glTexImage1D(texType, lod, internalFormat,width,0,format,pixelType,data);
	}
	glBindTexture(texType,0);

}

void TextureManager::bindTexture(GLenum texType ,GLuint texID, int textureUnit) {

	glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(texType, texID);
	if(usedTextureUnits[textureUnit]) {
		cout << "WARNING: Texture unit already used. Replacing bound texture..." << endl;
	}
	else {
		usedTextureUnits[textureUnit] = true;
	}	

}

int TextureManager::bindTexture(GLenum texType ,GLuint texID) {

	for (int i = 0; i<maxTexUnits; i++) {
		if(!usedTextureUnits[i]) {
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(texType,texID);
			usedTextureUnits[i] = true;
			return i;
		}
	}

}

int TextureManager::getAvailableUnit() {

	for (int i = 0; i<maxTexUnits; i++) {
		if(!usedTextureUnits[i]) {
			return i;
		}
	}

}

void TextureManager::setUnavailableUnit(int unit) {
	
	usedTextureUnits[unit] = true;

}

void TextureManager::unbindTexture(GLenum texType, int textureUnit) {
	glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(texType,0);
	usedTextureUnits[textureUnit] = false;
}

void TextureManager::unbindAllTextures(GLenum texType) {
	for (int i = 0; i<maxTexUnits; i++) {
		if(usedTextureUnits[i]) {
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(texType,0);
			usedTextureUnits[i] = false;
		}
	}
}
