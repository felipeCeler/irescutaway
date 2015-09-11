#ifndef __SHADER__
#define __SHADER__

#include <GL/glew.h>
#include <GL/glu.h>
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

void errorCheckFunc(string file, int line, string message = "");

/**
 * Shader class. Each Shader object represents one OpenGL Shader program. This means one object can store either the standard shaders used for rendering (vertex, geometry, fragment...) or a group of
 * compute shaders. For convenience, it also stores a user defined name, making it easier to access the shaders from within the main program. The shader's name is the same as the shaders filenames,
 * without the extensions. 
 **/
class Shader {

public:

  /**
   * This constructor receives the pathes to each shader separately. It must receive a vertex and a fragment shader, and optionally can receive a geometry shader. The shader name is defined by the 
	 * user. 
   * @param name The string to be set as shader identification. User manteined.
   * @param vertex_shader_path String giving the path to the external file containing the vertex shader.
   * @param fragment_shader_path String giving the path to the external file containing the fragment shader.
	 * @param geometry_shader_path String giving the path to the external file containing the geometry shader.
	 * @param dbg Debug level. 0 mean the debug is disabled; 1 means the shader name will be printed on the terminal and 2 means the whole shader code will be printed on the terminal.
   **/
  Shader(string name, string vertex_shader_path, string fragment_shader_path, string geometry_shader_path = "", int dbg = 0) : debug_level(dbg) {

    shaderName = name;
    vertexShaderPath = vertex_shader_path;
    geometryShaderPath = geometry_shader_path;
    fragmentShaderPath = fragment_shader_path;

    vertexShader = 0; fragmentShader = 0; geometryShader = 0; shaderProgram = 0; computeShaders = vector<GLuint>();

  }

  /**
   * Auto load constructor.
   * Receives a directory and a shader name, searches for files with the same name and extesions vert, frag, geom and compute to auto load shaders.
   * @param dir Directory containing shaders
   * @param name Shader name, must be the same as the files name without extensions
	 * @param dbg Debug level. 0 mean the debug is disabled; 1 means the shader name will be printed on the terminal and 2 means the whole shader code will be printed on the terminal.
   **/
  Shader(string shader_dir, string name, int dbg = 0) : debug_level(dbg) {

    shaderName = name;

    bool found = false;

	//Vertex:
    string vs_name = shader_dir + name + ".vert";
    ifstream vertex_file(vs_name.c_str());
    if (vertex_file.good()) {
      vertexShaderPath = vs_name;
      found = true;
    }

	//Fragment:
    string fs_name = shader_dir + name + ".frag";
    ifstream fragment_file(fs_name.c_str());
    if (fragment_file.good()) {
      fragmentShaderPath = fs_name;
      found = true;
    }
    
	//Geometry:
    string gs_name = shader_dir + name + ".geom";
    ifstream geom_file(gs_name.c_str());
    if (geom_file.good()) {
      geometryShaderPath = gs_name;
      found = true;
    }


	//Compute:
    string cs_name = shader_dir + name + ".comp";
    ifstream comp_file(cs_name.c_str());
    if (comp_file.good()) {
      computeShaderPaths.push_back(cs_name);
      found = true;
    }

    if (!found) {
        cout << "warning: no shader file found in directory : " << shader_dir.c_str() << endl;
    }

    vertexShader = 0; fragmentShader = 0; geometryShader = 0; shaderProgram = 0; computeShaders = vector<GLuint>();
  }


  /**
   * This constructor receives the pathes to the compute shaders from a vector. It must receive a vector containing at least one compute shader path. The shader name is defined by the user. 
   * @param name The string to be set as shader identification. User manteined.
   * @param compute_shader_paths Vector of strings giving the paths to the external files containing the compute shaders.
	 * @param dbg Debug level. 0 mean the debug is disabled; 1 means the shader name will be printed on the terminal and 2 means the whole shader code will be printed on the terminal.
   **/
  Shader(string  name, vector<string> compute_shader_paths, int dbg = 0) : debug_level(dbg) {

    shaderName = name;
    computeShaderPaths = compute_shader_paths;

    vertexShader = 0; fragmentShader = 0; geometryShader = 0; shaderProgram = 0; computeShaders = vector<GLuint>();

  }

  /**
   * Calls all the functions related to the shader initialization, i.e., creates, loads the shaders from the external files and links the shader program.
   **/
  void initialize();

  /**
	 * Given the name of a uniform used inside the shader, returns it's location.
   * @param name Name of the uniform variable in shader.
   * @return The uniform location.
   **/
  GLint getUniformLocation(const GLchar* name) {
    return glGetUniformLocation(shaderProgram,name);
  }

	//@{
  /**
   Sets {1|2|3|4}-{integer|float|double} uniform values by {name|location}. It converts double to float.
   *@param name Name of uniform variable.
   *@param location Location handle of uniform variable.
   *@param a First value.
   *@param b Second value.
   *@param c Third value.
   *@param d Fourth value.
   **/
  void setUniform(GLint location, GLint a, GLint b, GLint c, GLint d);
  void setUniform(GLint location, GLint a, GLint b, GLint c);
  void setUniform(GLint location, GLint a, GLint b);
  void setUniform(GLint location, GLint a);
  void setUniform(const GLchar* name, GLint a, GLint b, GLint c, GLint d);
  void setUniform(const GLchar* name, GLint a, GLint b, GLint c);
  void setUniform(const GLchar* name, GLint a, GLint b);
  void setUniform(const GLchar* name, GLint a);
  void setUniform(GLint location, GLfloat a, GLfloat b, GLfloat c, GLfloat d);
  void setUniform(GLint location, GLfloat a, GLfloat b, GLfloat c);
  void setUniform(GLint location, GLfloat a, GLfloat b);
  void setUniform(GLint location, GLfloat a);
  void setUniform(const GLchar* name, GLfloat a, GLfloat b, GLfloat c, GLfloat d);
  void setUniform(const GLchar* name, GLfloat a, GLfloat b, GLfloat c);
  void setUniform(const GLchar* name, GLfloat a, GLfloat b);
  void setUniform(const GLchar* name, GLfloat a);
  void setUniform(GLint location, GLdouble a, GLdouble b, GLdouble c, GLdouble d);
  void setUniform(GLint location, GLdouble a, GLdouble b, GLdouble c);
  void setUniform(GLint location, GLdouble a, GLdouble b);
  void setUniform(GLint location, GLdouble a);
  void setUniform(const GLchar* name, GLdouble a, GLdouble b, GLdouble c, GLdouble d);
  void setUniform(const GLchar* name, GLdouble a, GLdouble b, GLdouble c);
  void setUniform(const GLchar* name, GLdouble a, GLdouble b);
  void setUniform(const GLchar* name, GLdouble a);
	//@}

	//@{
  /**
   Sets a {integer|float} uniform vector value by {name|location}.
   *@param name Name of uniform variable.
   *@param location  Location handle of uniform variable
   *@param v  Vector value.
   *@param nvalues  Number of values in the vector v.
   *@param count  Number of elements of the uniform vector array to be modified.
   *			    a count of 1 should be used if modifying the value of a single vector. 
   **/
  void setUniform(const GLchar* name, const GLint* v, GLuint nvalues, GLsizei count=1);
  void setUniform (const GLchar* name, const GLfloat* v, GLuint nvalues, GLsizei count=1);
  void setUniform (GLint location, const GLint* v, GLuint nvalues, GLsizei count=1);
  void setUniform (GLint location, const GLfloat* v, GLuint nvalues, GLsizei count=1);
	//@}

	//@{
  /**
   Sets a uniform matrix value by {name|location}.
   *@param name  Name of uniform variable.
   *@param location  Location handle of uniform variable.
   *@param m  Matrix value with dim^2 values
   *@param dim  Dimension of the matrix m.
   *@param transpose If transpose is GL_FALSE, each matrix is assumed to be supplied in
   *					in column major order, otherwise is in row major order.
   *@param count  Number of elements of the uniform matrix array to be modified.
   *				a count of 1 should be used if modifying the value of a single matrix.
   **/
  void setUniform(const GLchar* name, const GLfloat* m, GLuint dim, GLboolean transpose = GL_FALSE, GLsizei count=1);
  void setUniform(GLint location, const GLfloat* m, GLuint dim, GLboolean transpose = GL_FALSE, GLsizei count=1);
	//@}

  /**
   * @return The shader name. If the shader was created with the auto-loader, the name is the same as the shader file name. If not, it's user manteined.
   **/
  string  getShaderName() {
    return shaderName;
  }
				
  /**
   *	@return The Shader Program identification handle.
   **/
  GLuint getShaderProgram() {
    return shaderProgram;
  }

  /**
   * Reads the external file containing the fragment shader and loads it into the shader program.
   **/
  void setFragmentShader();

  /**
   * @return The Fragment Shader identification handle.
   **/
  GLuint getFragmentShader() {
    return fragmentShader;
  }

  /**
   * Reads the external file containing the vertex shader and loads it into the shader program.
   **/
  void setVertexShader();

  /**
   * @return The Vertex Shader identification handle.
   **/
  GLuint getVertexShader() {
    return vertexShader;
  }

  /**
   * Reads the external file containing the geometry shader and loads it into the shader program.
   **/
  void setGeometryShader();

  /**
   * @return Return the Geometry Shader identification handle.
   **/
  GLuint getGeometryShader() {
    return geometryShader;
  }

  /**
   * Reads the external files containing the compute shaders and loads them into the shader program.
   **/
  void setComputeShaders();

  /**
   * @return Return the vector of Compute Shaders identification handles.
   **/
  vector<GLuint> getComputeShaders() {
    return computeShaders;
  }
		
  ///Enable the shader program.
  void enable();

  ///Disables the shader program.
  void disable();

  ///Creates the OpenGL shader objects, storing their identification handles properly.
  void createShaders();

  ///Reloads all shaders, reading the files again. 
  void reloadShaders();

  ///Detaches and deletes the shaders and the shader program.
  void deleteShaders();

private:

  ///Stores an user mainteined identification for the shader. If the shader is created with the autoloader, the name is equal to the filename, without extensions.
  string shaderName;

  ///Stores the path to the vertex shader file.
  string vertexShaderPath;

  ///Stores the path to the geometry shader file.
  string geometryShaderPath;

  ///Stores the path to the fragment shader file.
  string  fragmentShaderPath;

  ///Stores the paths to the compute shaders files.
  vector<string> computeShaderPaths;
	
  ///The Projection Matrix uniform location.
  GLuint projectionMatrixUniformLocation;

  ///The View Matrix uniform location.
  GLuint viewMatrixUniformLocation;
		
  ///The Model Matrix uniform location.
  GLuint modelMatrixUniformLocation;

  ///Mesh's Color uniform location.
  GLuint colorUniformLocation;

  ///Array that contains all the compute shaders identifications.
  vector<GLuint> computeShaders;

  ///Vertex Shader identification.
  GLuint vertexShader;

  ///Geometry Shader identification.
  GLuint geometryShader;

  ///Fragment Shader identification.
  GLuint fragmentShader;

  ///Shader program identification.
  GLuint shaderProgram;

  ///Debug level for outputing warnings and messages
  int debug_level;

};

#endif
