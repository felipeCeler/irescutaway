#ifndef __VOLUME__
#define __VOLUME__

#include "shaderlib/mesh.hpp"
#include <Eigen/Dense>

class Volume {

	public:

		///Default Constructor. Sets the number of layers in texture3D, The number of voxel elements (voxelArraySize), initializes the voxelArray, the depthStep, the mesh and generate the voxel texture.
		void Volume() {

			//Initializing the depth step:
			depthStep = 0.01;

			//Configuring the texture 3D:
			numberOfLayers[0] = 4; numberOfLayers[1] = 4; numberOfLayers[2] = 4;
			int voxelArraySize = numberOfLayers[0]*numberOfLayers[1]*numberOfLayers[2]*2;//Since each voxel consists in two bytes, the voxelArraySize is the number of voxels*2.

			//Initializing voxelArray:
			voxelArray = new GLubyte[voxelArraySize];

			//Generating texture:
			for(int i = 0; i < voxelArraySize; i+=2) {
				voxelArray[i] = 255*2/i; //Scalar
				voxelArray[i+1] = 0; //Gradient
			}

			//Initializing the mesh:
			mesh = new Mesh();

		}

		///Creates the mesh object, calls the texture 3D creation and loads the volume.
		void loadVolume();

		///Creates the openGL texture 3D object.
		void createTexture3D();

		///Default Destructor:
		void ~Volume() {

			if(voxelArray) {
				delete voxelArray;
			}

			if(mesh) {
				delete mesh;
			}

		}

	private:

		///Dimension of the volume in world space
		vector3i realDimension;

		///Mesh object.
		Mesh* mesh;

		///Array containing the voxels. It is important to notice that each voxel consists in two bytes: one scalar and one gradient.
		GLubyte* voxelArray;

		///Identification GLuint for the texture 3D.
		GLuint texID;

		///Number of layers in Texture 3D.
		vector3i numberOfLayers;


};

#endif
