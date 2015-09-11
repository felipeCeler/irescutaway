#ifndef __VOLUME__
#define __VOLUME__

#include "mesh.hpp"
#include <Eigen/Dense>


///Stores the Mesh volume and texture3D.
class Volume {

	public:

		///Default Constructor. Sets the number of layers in texture3D, The number of voxel elements (voxelArraySize), initializes the voxelArray, the depthStep, the mesh and generate the voxel texture.
		Volume() {

			//Configuring the texture 3D:
			//numberOfLayers[0] = 10; numberOfLayers[1] = 10; numberOfLayers[2] = 10;
			//int voxelArraySize = numberOfLayers[0]*numberOfLayers[1]*numberOfLayers[2]*2;//Since each voxel consists in two bytes, the voxelArraySize is the number of voxels*2.

			//Initializing voxelArray:
			//voxelArray = new GLubyte[voxelArraySize];

			numberOfLayers[0] = 64; numberOfLayers[1] = 64; numberOfLayers[2] = 64;
			int voxelArraySize = numberOfLayers[0]*numberOfLayers[1]*numberOfLayers[2]*2;//Since each voxel consists in two bytes, the voxelArraySize is the number of voxels*2.
			voxelArray = new GLubyte[voxelArraySize];

			ifstream file ("dataset/neghip64x64x64x1.raw", ios::in|ios::binary);
			
			char buff[128];
			double buff2;

			int i = 0;
 
			if(file.is_open())
			{
				while (!file.eof()) {
					file.read(buff, 1); //this will read one byte of data
					voxelArray[i] = (unsigned char) buff[0];//Scalar
					voxelArray[i+1] = (unsigned char)0;//Gradient
					i+=2;
				}
				file.close();

        	}
        	else cout << "Unable to open file"<<endl;


/*
			//Generating the texture3D parameters, i.e., the voxelArray:
			for(int i = 0; i < voxelArraySize; i+=2) {
				//voxelArray[i] = (unsigned char)(255/i); //Scalar
				voxelArray[i] = (unsigned char)(250); //Scalar
				voxelArray[i+1] = (unsigned char)0; //Gradient
			}

			for(int i = 0; i < numberOfLayers[0]*2; i+=2) {
				for(int j = 0; j < numberOfLayers[1]*2; j+=2) {
					voxelArray[numberOfLayers[0]*numberOfLayers[1]*0 + numberOfLayers[1]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[0]*numberOfLayers[1]*0 + numberOfLayers[1]*i + j+1] = (unsigned char)0; //Gradient
					voxelArray[numberOfLayers[0]*numberOfLayers[1]*(numberOfLayers[2]-1) + numberOfLayers[1]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[0]*numberOfLayers[1]*(numberOfLayers[2]-1) + numberOfLayers[1]*i + j+1] = (unsigned char)0; //Gradient
				}
			}
			for(int i = 0; i < numberOfLayers[1]*2; i+=2) {
				for(int j = 0; j < numberOfLayers[2]*2; j+=2) {
					voxelArray[numberOfLayers[2]*numberOfLayers[1]*0 + numberOfLayers[2]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[2]*numberOfLayers[1]*0 + numberOfLayers[2]*i + j+1] = (unsigned char)0; //Gradient
					voxelArray[numberOfLayers[2]*numberOfLayers[1]*(numberOfLayers[0]-1) + numberOfLayers[2]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[2]*numberOfLayers[1]*(numberOfLayers[0]-1) + numberOfLayers[2]*i + j+1] = (unsigned char)0; //Gradient
				}
			}			
			for(int i = 0; i < numberOfLayers[2]*2; i+=2) {
				for(int j = 0; j < numberOfLayers[0]*2; j+=2) {		
					voxelArray[numberOfLayers[0]*numberOfLayers[2]*0 + numberOfLayers[0]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[0]*numberOfLayers[2]*0 + numberOfLayers[0]*i + j+1] = (unsigned char)0; //Gradient
					voxelArray[numberOfLayers[0]*numberOfLayers[2]*(numberOfLayers[1]-1) + numberOfLayers[0]*i + j] = (unsigned char)0; //Scalar
					voxelArray[numberOfLayers[0]*numberOfLayers[2]*(numberOfLayers[1]-1) + numberOfLayers[0]*i + j+1] = (unsigned char)0; //Gradient
				}
			}
*/
			realDimension << 10.0, 10.0, 20.0;
			
			//Initializing the mesh:
			mesh = new Mesh();

			//Loads the volume:
			loadVolume();

		}

		Mesh* getMesh() {
			return mesh;
		}

		///Creates the mesh object, calls the texture 3D creation and loads the volume.
		void loadVolume();

		///Creates the openGL texture 3D object.
		void createTexture3D();

		GLuint getTexture3D() {
			return texID;
		}

		///Default Destructor:
		~Volume() {

			if(voxelArray) {
				delete voxelArray;
			}

			if(mesh) {
				delete mesh;
			}

		}

	private:

		///Dimension of the volume in world space
		Eigen::Vector3f realDimension;

		///Mesh object.
		Mesh* mesh;

		///Array containing the voxels. It is important to notice that each voxel consists in two bytes: one scalar and one gradient.
		GLubyte* voxelArray;

		///Identification GLuint for the texture 3D. The elements of this texture are stored in voxelArray.
		GLuint texID;

		///Number of layers in Texture 3D.
		Eigen::Vector3i numberOfLayers;


};

#endif
