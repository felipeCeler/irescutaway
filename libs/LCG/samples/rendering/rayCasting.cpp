#include "rayCasting.hpp"

void RayCasting::initializeTransferFunction() {

	float values[256*4];

	for(int i = 0; i<256*4; i+=4) {//*4 because of 256 RGBA values
			values[i] = i;
			values[i+1] = i;
			values[i+2] = i;
			values[i+3] = 0.05;
	}

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, &values[0]);
	

}

void RayCasting::createTexture2D(GLuint texID) {

	glGenTextures(1,&texID);
	glBindTextures(GL_TEXTURE_2D,texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportSize[0], viewportSize[1], 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(0,&texID);

}

void RayCasting::createTexture1D(GLuint texID) {

	glGenTextures(1,&texID);
	glBindTextures(GL_TEXTURE_1D,texID);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(0,&texID);

}

float RayCasting::computeDistanceFromPointToPlane(Eigen::Vector4f planePoint, Eigen::Vector4f planeNormal, Eigen::Vector4f point) {

	return fabs( (planeNormal[0] * point[0] + planeNormal[1] * point[1] + planeNormal[2] * point[2] - 
			(planeNormal[0] * planePoint[0] + planeNormal[1] * planePoint[1] + planeNormal[2] * planePoint[2])) );

}

Eigen::Vector4f RayCasting::computeInitialTranslation(){

	//Compute Initial Translation:

	//Get the viewport plane equation:
	Eigen::Vector4f planePoint, planeNormal, transformedPlanePoint, transformedPlaneNormal, cubeVertex;
	planePoint << 0.0, 0.0, -20.0, 1.0;
	planeNormal << 0.0, 0.0, -1.0, 0.0;

	Eigen::Affine4f modelViewMatrix;
	modelViewMatrix = cameraTrackball->getViewMatrix() * cameraTrackball->getModelMatrix();

	//Transforms the plane point and normal to eye space:
	transformedPlanePoint = planePoint * modelViewMatrix();
	transformedPlaneNormal = planeNormal * modelViewMatrix();

	float distance, minDistance;
	int vertexIndex;

	//Test all volumes' vertex to get the one nearest to the viewport plane and start ray casting at this vertex.
	for (int i=0; i<8; i++) {
		cubeVertex = volume->getMesh()->getCubeVertex(i);
		distance = computeDistanceFromPointToPlane(transformedPlanePoint, transformedPlaneNormal, cubeVertex);
		if ( (distance < minDistance) || i == 0 ) {
			minDistance = distance;
			vertexIndex = i;
		}
	}

	//Translation that will be applied to modelView matrix in order to start ray casting at the volume's boundary.
	Eigen::Vector4f translationVector;
	translationVector << minDistance * transformedPlaneNormal[0], minDistance * transformedPlaneNormal[1],
						 minDistance * transformedPlaneNormal[2], 1.0;

	//Translate View Matrix:
	cameraTrackball->translateViewMatrix(translationVector);

	//Computing number of Iterations:

	//Get the final vertex covered by the ray casting:
	cubeVertex = volume->getMesh()->getCubeVertex(vertexIndex);
	cubeVertex[3] = 0.0;//Transforms point to vector.
	cubeVertex *= 2; //2 or -2? Depends on the scalar signal to define the step direction.

	//Compute Initial Translation in z axis = Distance between origin and tangent vertex to the view plane.:
	transformedPlaneNormal.normalize();
	float initialTranslation = transformedPlaneNormal.dot(cubeVertex);

	//Projects the cubeVertex vector in the plane normal to get the total length covered by the ray casting. Then divide it by the depthStep to get the number of iterations needed:
	numberOfIterations = initialTranslation/depthStep;


	return volume->getMesh()->getCubeVertex(vertexIndex);

}

void RayCasting::rayCastingRender() {

	//We will use ping pong structure to use the shaders alternating the reading and writing textures. Each step will be duplicated.
	//First we define the start point and inside the fragment shader we will increment each step.


	//Move screen plane to nearest cube vertex:
	Eigen::Vector4f translation = computeInitialTranslation();

	// compute P0
	// uniform volumedim -->> shader ([0,1], [0,1], [0,1])
	// uniform texture3D -->> shader
	// screensize = ( sqrt(3.0) , sqrt(3.0) )
	// vector e_x , e_y -->> shader  (e_x = View * (1,0,0) * screensize.x , e_y = View * (0,1,0) * screensize.y )

	int i = 0;

	while (i < numberOfIterations){

		// Pi = P0 + viewvector*step*i;
		// uniform Pi -->> shader

		//============Ray cast===================:
		//fragmentShader(onde_estou, flag);


		//==========Call Compute Shaders=================:
		//... volumeSize = [5, 5, 5]. 
		// [0,0,0] 
		//glDispatchCompute(viewportSize[0],viewportSize[1],0); <<-- screenresolution

		i++;
	}
}
