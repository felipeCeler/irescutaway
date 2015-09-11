#ifndef __MESH__
#define __MESH__

#include <GL/glew.h>
#include <GL/glu.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>

using namespace std;

/**
 * The Mesh class handles the buffers generation and usage, such as VBO, IBO, TBO... It also contains methods for simple meshes generation, (e.g. a quadrilateral or a cube) and a wavefront .obj 
 * loader. When a mesh is generated, geometrical computations are performed, detecting the center of the mesh through the axis-aligned bounding box and computing normalizating scale factors.
 * The attribute locations are predefined in this class, being the location 0 used for Vertex Buffer, 1 for Normals Buffer, 2 for Color Buffer and 3 for TexCoord Buffer. 
 **/
class Mesh {

public:

  ///Default Constructor. Initializes the buffers arrays with zeroes.
  Mesh() {

    //Initialize Buffers:
    bufferIDs = new GLuint[6];
    for(int i = 0; i < 6; i++) {
      bufferIDs[i] = 0;
    }

    vertices = 0;
    normals = 0;
    colors = 0;
    texCoords = 0;
    indices = 0;

    radius = 1.0;
    scale = 1.0;
  }

  /**
   * @return The number of elements (primitives) of the mesh.
   **/
  int getNumberOfElements() {
    return numberOfElements;
  }

  /**
   * @return The number of vertices of the mesh.
   **/
  int getNumberOfVertices() {
    return numberOfVertices;
  }


  /**
   * @return The scale factor to be applied in view Matrix.
   **/
  float getScale() {
    return scale;
  }

  /**
   * @return The center of the mesh object computed by axis-aligned bounding box algorithm.
   **/
  Eigen::Vector3f getObjectCenter() {
    return objectCenter;
  }

  /**
   * @return The centroid of the mesh object, given by the mean position of all vertices.
   **/
  Eigen::Vector3f getCentroid() {
    return centroid;
  }


  /**
   * Returns the radius of the bounding sphere, computed as the distance from the farthest point to the object center (centroid).
   * @return Radius of the mesh bounding sphere.
   **/
  float getRadius() {
    return radius;
  }

  /**
   * Creates vertices, indices, colors, normals and texCoords arrays.
   * @param vertNumber Number of vertices needed to represent the mesh. 
   * 		 			 It is important to notice that the vertices array's size will be 4*vertNumber, since each vertex is represented by its homogeneous coordinates (x,y,z,w).
   * @param indexNumber Number of indices needed to represent the mesh's elements. 
   *					  As we are using triangles for standard primitives, each 3 indices in this array represents one element.
   * @param colorNumber Number of colors needed to represent mesh's per vertex color. 
   * 					  It is important to notice that the colors array's size will be 4*colorNumber, since each color is represented by 4 integer numbers (r,g,b,a).
   * @param normalNumber Number of normals needed to represent the mesh's per vertex normals. 
   * 					   It is important to notice that the normals array's size will be 3*normalNumber, since each normal vector is represented by its tridimensional coordinates (x,y,z). 
   * @param texCoordNumber Number of texture coordinates needed for mesh's texture mapping.
   *						 It is important to notice that the texCoords array size will be 2*texCoordNumber, since each texCoord vector is represented by it's uv coordinates (u,v).
   **/
  void createArrays(int vertNumber = 0, int indexNumber = 0, int colorNumber = 0, int normalNumber = 0, int texCoordNumber = 0);

  /**
   * Stores data passed in parameters to the object's vertices, indices, normals and texCoords arrays. Also calculates the scale and translation needed for mesh normalization.
   * @param vert Vector containing all the vertices that will be saved to the vertices array. Each vertex is represented by one vector in homogeneous coordinates (x,y,z,w).
   * @param norm Vector containing all the normal vectors that will be saved to the normals array. Each normal vector is represented by one tridimensional vector (x,y,z).
   * @param indices Vector containing all the indices that will be saved to the indices array. Each index is a reference to one vertex in vertex buffer object.
   * @param texCoord Vector containing all the texCoords that will be saved to the texCoords array. Each texCoord vector is represented by it's uv coordinates (u,v).
   **/
  void saveDataToArrays(vector<Eigen::Vector4f> &vert, vector<Eigen::Vector3f> &norm, 
			vector<GLuint> &indices, vector<Eigen::Vector2f> &texCoord);


  ///Creates VAO, VBO, IBO, TBO, Normals and Colors buffer and stores them in the BufferIDs array.
  void createBuffers();

  ///Buffer the data in the vertices, normals, colors, indices and texCoords arrays.
  void bufferData();

  ///Binds all buffers for OpenGL Usage.
  virtual void bindBuffers();

  ///Unbinds all buffers.
  virtual void unbindBuffers();

  /**
   * Renders the mesh.
   *  Binds buffers, call draw method, and unbinds buffer.  
   **/
  virtual void render();

  ///Renders the triangles calling appropriate GLDraw method
  virtual void renderElements();

  ///Creates a standard parallelepiped centered in origin with sides x, y and z.
  void createParallelepiped(float x, float y, float z);

  /**
	* Returns one vertex of the standard cube.
	* @param index The index of the vertex to be returned.
	* @return The coordinates of the cube's vertex.
  **/
  Eigen::Vector4f getCubeVertex(int index);

  ///Creates a quad. Useful for offscreen rendering.
  void createQuad(void);

  /**
   * Loads a wavefront obj file from an external file.
   * @param filename The complete path for the file to be loaded.
   **/
  void loadObjFile(const char* filename);

  ///Deletes all the buffers and VAO.
  void deleteBuffers() {
    glDeleteBuffers(5, &bufferIDs[1]);
    glDeleteVertexArrays(1, &bufferIDs[0]);
  }

  ///Default Destructor. Deletes the bufferIDs, vertices, normals, colors, indices and texCoords arrays.
  ~Mesh() {
    //Delete Buffers:
    if(bufferIDs) {
      delete [] bufferIDs;
    }

    //Delete Arrays:
    if(vertices) delete [] vertices;
    if(normals) delete [] normals;
    if(colors) delete [] colors;
    if(indices) delete [] indices;
    if(texCoords) delete [] texCoords;
  }

protected:

  ///Array of mesh vertices.
  float* vertices;

  ///Array of mesh normals.
  float* normals;

  ///Array of mesh indices.
  GLuint* indices;

  ///Array of mesh colors.
  float* colors;

  ///Array of mesh TexCoords.
  float* texCoords;

  ///Center of the mesh object.
  Eigen::Vector3f objectCenter;

  ///Object's centroid (different from center of bounding box)
  Eigen::Vector3f centroid;

  ///Radius of the mesh bounding sphere.
  float radius;

  ///Number of vertices in vertices array.
  int numberOfVertices;

  ///Number of normals in normals array.
  int numberOfNormals;

  ///Number of indices in indices array
  int numberOfElements;

  ///Number of texture coordinates in texCoords array.
  int numberOfTexCoords;

  /**Array containing VAO, VBO, IBO, TBO, Colors and Normals Buffer. 
   * bufferIDs[0] = VAO, 
   * bufferIDs[1] = VBO, 
   * bufferIDs[2] = IBO, 
   * bufferIDs[3] = Colors buffer,
   * bufferIDs[4] = Normals buffer,
   * bufferIDs[5] = TBO
   **/
  GLuint*  bufferIDs;//Change to vector if possible.

  ///The normalization scale factor computed when the mesh is loaded. This parameter might be applied to the view matrix if the user wants the mesh to be scaled to [0,1] range.
  float scale;

  ///Function used for openGL error handling.
  void errorCheckFunc(string file, int line);

};

#endif
