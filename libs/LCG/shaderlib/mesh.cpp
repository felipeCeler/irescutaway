#include "mesh.hpp"

void Mesh::createArrays(int vertNumber, int indexNumber, int colorNumber, int normalNumber, int texCoordNumber) {

  if(vertNumber > 0) {
    vertices = new float[vertNumber*4];
  }

  if(indexNumber > 0) {
    indices = new GLuint[indexNumber];
  }

  if(colorNumber > 0) {	
    colors = new float[colorNumber*4];
  }

  if(normalNumber > 0) {
    normals = new float[normalNumber*3];
  }

  if(texCoordNumber > 0) {
    texCoords = new float[texCoordNumber*2];
  }

}

void Mesh::saveDataToArrays(vector<Eigen::Vector4f> &vert, 
			    vector<Eigen::Vector3f> &norm, 
			    vector<GLuint> &ind, 
			    vector<Eigen::Vector2f> &tex) {
  int temp = 0;

  //Vertices:
  float xMax = 0; float xMin = 0; float yMax = 0; float yMin = 0; float zMax = 0; float zMin = 0;

  for(int i = 0; i < numberOfVertices*4; i+=4) {

    //X:
    vertices[i] = vert[temp][0];
    if(vert[temp][0] > xMax) {
      xMax = vert[temp][0];
    }
    if(vert[temp][0] < xMin) {
      xMin = vert[temp][0];
    }

    //Y:
    vertices[i+1] = vert[temp][1];
    if(vert[temp][1] > yMax) {
      yMax = vert[temp][1];
    }
    if(vert[temp][1] < yMin) {
      yMin = vert[temp][1];
    }

    //Z:
    vertices[i+2] = vert[temp][2];
    if(vert[temp][2] > zMax) {
      zMax = vert[temp][2];
    }
    if(vert[temp][2] < zMin) {
      zMin = vert[temp][2];
    }

    //W:
    vertices[i+3] = vert[temp][3];

    temp++;
  }

  temp = 0;

  //Normals:
  for(int i = 0; i < numberOfNormals*3; i+=3) {
    normals[i] = norm[temp][0];
    normals[i+1] = norm[temp][1];
    normals[i+2] = norm[temp][2];
    temp++;
  }

  temp = 0;
  //TexCoords:
  for(int i = 0; i < numberOfTexCoords*2; i+=2) {
    texCoords[i] = tex[temp][0];
    texCoords[i+1] = tex[temp][1];
    temp++;
  }

  //Indices:
  for(int i = 0; i < numberOfElements; i++) {
    indices[i] = ind[i];
  }

  scale = 1.0/max(max(xMax-xMin, yMax-yMin), zMax-zMin);
 
  float centerX = (xMax+xMin)/2.0;
  float centerY = (yMax+yMin)/2.0;
  float centerZ = (zMax+zMin)/2.0;
  //cout << "X: " << centerX << " Y: " << centerY << " Z: " << centerZ << endl;
  objectCenter = Eigen::Vector3f(centerX,centerY,centerZ);

  // compute the centroid (different from the box center)
  centroid = Eigen::Vector3f::Zero();
  for(int i = 0; i < numberOfVertices; i++) {
    centroid = centroid + Eigen::Vector3f(vert[i][0], vert[i][1], vert[i][2]);
  }
  centroid = centroid / numberOfVertices;
	
  // compute the radius of the bounding sphere
  // most distance point from the centroid
  radius = 0.0;
  for(int i = 0; i < numberOfVertices; i++) {
    Eigen::Vector3f pt (vert[i][0], vert[i][1], vert[i][2]);
    radius = max(radius, (pt-centroid).norm());
  }

  scale = 1.0/radius;

}

void Mesh::createBuffers() {

  glGenVertexArrays(1, &bufferIDs[0]);
  glGenBuffers(5, &bufferIDs[1]);


}

void Mesh::bufferData() {

  //VAO:
  glBindVertexArray(bufferIDs[0]);

  //Vertices:
  if(vertices) {
    int vboSize = numberOfVertices*4*sizeof(float);
    cout << "VBO Size: " << vboSize << endl;
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[1]);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
  }

  //Normals:
  if(normals) {
    int normalBufferSize = numberOfNormals*3*sizeof(float);
    cout << "Normal Buffer Size: " << normalBufferSize << endl;
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[4]);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);

  }

  //Colors:
  if(colors) {
    int colorBufferSize = numberOfVertices*4*sizeof(float);
    cout << "Color Buffer Size: " << colorBufferSize << endl;
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[3]);
    glBufferData(GL_ARRAY_BUFFER, colorBufferSize, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    //glEnableVertexAttribArray(2);

  }

  //TexCoords:
  if(texCoords) {
    int texCoordsBufferSize = numberOfTexCoords*2*sizeof(float);
    cout << "Texture Coordinates Buffer Size: " << texCoordsBufferSize << endl;
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[5]);
    glBufferData(GL_ARRAY_BUFFER, texCoordsBufferSize, texCoords, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(3);

  }

  //Indices:
  if(indices){
    int iboSize = numberOfElements*sizeof(GLuint);
    cout << "IBO Size: " << iboSize << endl << endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, indices, GL_STATIC_DRAW);

  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

}


void Mesh::bindBuffers() {
  glBindVertexArray(bufferIDs[0]);//VAO

  //Vertices Buffer:
  if(vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[1]);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

  }

  //Colors Buffer:
  if(colors) {
    glBindBuffer(GL_ARRAY_BUFFER,bufferIDs[3]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
  }
	
  //Normals Buffer:
  if(normals) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[4]);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);

  }

  //Texture Coordinates Buffer:
  if(texCoords) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[5]);
    glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(3);

  }

  if(indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[2]);

  }

}



void Mesh::unbindBuffers() {

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

}

void Mesh::renderElements() {
  glDrawElements(GL_TRIANGLES, numberOfElements, GL_UNSIGNED_INT, (GLvoid*)0);
}

void Mesh::render() {
  bindBuffers();
  renderElements();
  unbindBuffers();
}


void Mesh::createParallelepiped(float x, float y, float z)
{

	numberOfVertices = 32;
	numberOfElements = 36;
	//numberOfColors = 32;

	createBuffers();
	createArrays(numberOfVertices,numberOfElements,0);

	//Normalizing the cube coordinates:
	float scale = x;
	if (y > x) scale = y;
	if (z > scale) scale = z;

	float tempVert[32] = {
		 -.5f*x/scale, -.5f*y/scale,  .5f*z/scale, 1,
		 -.5f*x/scale,  .5f*y/scale,  .5f*z/scale, 1,
	      .5f*x/scale,  .5f*y/scale,  .5f*z/scale, 1,
		  .5f*x/scale, -.5f*y/scale,  .5f*z/scale, 1,
		 -.5f*x/scale, -.5f*y/scale, -.5f*z/scale, 1,
		 -.5f*x/scale,  .5f*y/scale, -.5f*z/scale, 1,
		  .5f*x/scale,  .5f*y/scale, -.5f*z/scale, 1,
		  .5f*x/scale, -.5f*y/scale, -.5f*z/scale, 1
	};

	for(int i=0; i<numberOfVertices; i++) {
		vertices[i] = tempVert[i];
	}
	
	GLuint tempIndices[36] = {
    	0,2,1,  0,3,2, 
    	4,3,0,  4,7,3,
    	4,1,5,  4,0,1,
    	3,6,2,  3,7,6,
    	1,6,5,  1,2,6,
    	7,5,6,  7,4,5
	};
	
	for(int i=0; i<numberOfElements;i++) {
		indices[i] = tempIndices[i];
	}
	
	/*float colors[32] =
	{
		0, 0, 1, 1,
		1, 0, 0, 1,
		0, 1, 0, 1,
		1, 1, 0, 1,
		1, 1, 1, 1,
		1, 0, 0, 1,
		1, 0, 1, 1,
		0, 0, 1, 1
	};*/

	bufferData();

}


void Mesh::createQuad(void)
{

  vector<Eigen::Vector4f> vert; 
  vector<Eigen::Vector3f> norm;
  vector<Eigen::Vector2f> texCoord;
  vector<GLuint> elementsVertices;

  vert.push_back ( Eigen::Vector4f(-1.0, -1.0, 0.0, 1.0) );
  vert.push_back ( Eigen::Vector4f( 1.0, -1.0, 0.0, 1.0) );
  vert.push_back ( Eigen::Vector4f( 1.0,  1.0, 0.0, 1.0) );
  vert.push_back ( Eigen::Vector4f(-1.0,  1.0, 0.0, 1.0) );

  elementsVertices.push_back(0);
  elementsVertices.push_back(1);
  elementsVertices.push_back(2);
  elementsVertices.push_back(2);
  elementsVertices.push_back(3);
  elementsVertices.push_back(0);

  texCoord.push_back ( Eigen::Vector2f(0.0, 0.0) );
  texCoord.push_back ( Eigen::Vector2f(1.0, 0.0) );
  texCoord.push_back ( Eigen::Vector2f(1.0, 1.0) );
  texCoord.push_back ( Eigen::Vector2f(0.0, 1.0) );

  numberOfVertices = vert.size();
  numberOfNormals = norm.size();
  numberOfTexCoords = texCoord.size();
  numberOfElements = elementsVertices.size();

  createBuffers();

  createArrays(numberOfVertices, numberOfElements, 0, numberOfNormals, numberOfTexCoords);

  saveDataToArrays(vert, norm, elementsVertices, texCoord);

  bufferData();
}

Eigen::Vector4f Mesh::getCubeVertex(int index) {

	Eigen::Vector4f ret;

	ret << vertices[4*index], vertices[(4*index)+1], vertices[(4*index)+2], vertices[(4*index)+3];

	return ret;

}

void Mesh::loadObjFile(const char* filename) {

  vector<Eigen::Vector4f> vert; 
  vector<Eigen::Vector3f> norm;
  vector<Eigen::Vector2f> texCoord;
  vector<GLuint> elementsVertices;
  vector<GLuint> elementsNormals;
  vector<GLuint> elementsTexIDs;

  //Opening file:
  cout << "Opening Wavefront obj file..." << endl << endl;
  ifstream in(filename,ios::in);
  if (!in) { 
    cerr << "Cannot open " << filename << endl; exit(1); 
  }

  //Reading file:	
  string line;
  while(getline(in,line)) {
		
    //Vertices reading:
    if(line.substr(0,2) == "v ") {

      istringstream s(line.substr(2));
      Eigen::Vector4f v;
      s >> v[0]; s >> v[1]; s >> v[2]; v[3] = 1.0f;
      vert.push_back(v);
    }

    //Normals reading:
    else if(line.substr(0,2) == "vn") {
      istringstream s(line.substr(3));
      Eigen::Vector3f vn;
      s >> vn[0]; s >> vn[1]; s >> vn[2];
      norm.push_back(vn);
    }

    //Texture Coordinates reading:
    else if(line.substr(0,2) == "vt") {
      istringstream s(line.substr(2));
      Eigen::Vector2f vt;
      s >> vt[0]; s >> vt[1];
      texCoord.push_back(vt);
    }

    //Elements reading: Elements are given through a string: "f vertexID/TextureID/NormalID". If no texture is given, then the string will be: "vertexID//NormalID".
    else if(line.substr(0,2) == "f ") {
      GLuint vertexID1, normalID1, textureID1;
      istringstream s(line.substr(2));
      while(!s.eof()){
	s >> vertexID1; 
	elementsVertices.push_back(vertexID1-1); 
	if(s.peek() == '/') {
	  s.get();
	  if(s.peek() == '/') {
	    s.get();
	    s >> normalID1; 
	    elementsNormals.push_back(normalID1-1); 
	  }
	  else {
	    s >> textureID1; 
	    elementsTexIDs.push_back(textureID1-1); 
	    if(s.peek() == '/') {
	      s.get();
	      s >> normalID1; 
	      elementsNormals.push_back(normalID1-1); 
	    }
	  }
	
	}
      }
    }

    //Ignoring comment lines:
    else if(line[0] == '#') { }

    //Ignoring any other lines:
    else {};
  }
  if(in.is_open()) {
    in.close();
  }
  //Saving Data to arrays:

  numberOfVertices = vert.size();
  numberOfNormals = norm.size();
  numberOfTexCoords = texCoord.size();
  numberOfElements = elementsVertices.size();

  createArrays(numberOfVertices,numberOfElements,0,numberOfNormals,numberOfTexCoords);

  saveDataToArrays(vert,norm,elementsVertices,texCoord);

  bufferData();

}

void Mesh::errorCheckFunc(string file, int line){
  //OpenGL Error Handling Function:
  GLenum ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR)
    {
      cerr << "GL error in " << file << "  line " << line << " : " << gluErrorString(ErrorCheckValue) << endl;
      exit(-1);
    }
}
