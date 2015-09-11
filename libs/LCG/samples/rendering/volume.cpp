#include "volume.hpp"

void Volume::loadVolume() {

	mesh->createCube(realDimension[0],realDimension[1],realDimension[2]);
	createTexture3D();

}

void Volume::createTexture3D() {

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_3D, texID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RG8, numberOfLayersX, numberOfLayersY, numberOfLayersZ, 0, GL_RG, 
          		 GL_UNSIGNED_BYTE, voxelArray);

}
