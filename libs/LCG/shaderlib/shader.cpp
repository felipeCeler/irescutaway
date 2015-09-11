#include "shader.hpp"

void Shader::initialize() {

    if (debug_level > 1)
        cout << "initializing shaders" << endl;

  createShaders();

  if(!vertexShaderPath.empty()) {
    setVertexShader();
    // geom shader needs a vertex shader
    if(!geometryShaderPath.empty()) {
      setGeometryShader();
    }
  }
  if(!fragmentShaderPath.empty()) {
    setFragmentShader();
  }
  if(computeShaderPaths.size() > 0) {
    setComputeShaders();
  }
  glLinkProgram(shaderProgram);

}

void Shader::createShaders() {

  //Create Shader Program.
  shaderProgram = glCreateProgram();


  if(!fragmentShaderPath.empty()) {
    //Create Fragment Shader.
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  }

  if(!vertexShaderPath.empty()) {
    //Create Vertex Shader.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);


    // vertex shader is mandatory when using geom shader
    if(!geometryShaderPath.empty()) {
      //Create Geometry Shader.
      geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    }

  }

  if(computeShaderPaths.size() > 0) {
    //Create Compute Shaders.
    for(unsigned int i = 0; i<computeShaderPaths.size(); i++) {
      computeShaders.push_back(glCreateShader(GL_COMPUTE_SHADER));
    }
  }
}

void Shader::setVertexShader() {

  // Read the Vertex Shader code from the file
  string vertexShaderCode;

  ifstream vertexShaderStream(vertexShaderPath.c_str(), std::ios::in);

  if(vertexShaderStream.is_open())
    {
      string line = "";
      while(getline(vertexShaderStream, line))
	vertexShaderCode += "\n" + line;
      vertexShaderStream.close();
    }

  if (debug_level > 1)
    cout << vertexShaderCode << endl << endl << endl;

  GLint result = GL_FALSE;
  int infoLogLength;
 
  // Compile Vertex Shader
  if (debug_level > 0)
    cout << "Compiling vertex shader: " << vertexShaderPath << endl;
  char const * vertexSourcePointer = vertexShaderCode.c_str();
  glShaderSource(vertexShader, 1, &vertexSourcePointer , NULL);
  glCompileShader(vertexShader);


  // Check Vertex Shader
  if (debug_level > 0) {
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    char * vertexShaderErrorMessage = new char[infoLogLength];
    glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &vertexShaderErrorMessage[0]);
    delete [] vertexShaderErrorMessage;
  }

  glAttachShader(shaderProgram, vertexShader);



}


void Shader::setGeometryShader() {

  // Read the Geometry Shader code from the file
  string geometryShaderCode;

  ifstream geometryShaderStream(geometryShaderPath.c_str(), std::ios::in);

  if(geometryShaderStream.is_open())
    {
      string line = "";
      while(getline(geometryShaderStream, line))
	geometryShaderCode += "\n" + line;
      geometryShaderStream.close();
    }

  if (debug_level > 1)
    cout << geometryShaderCode << endl << endl << endl;

  GLint result = GL_FALSE;
  int infoLogLength;
 
  // Compile Geometry Shader
  if (debug_level > 0)
    cout << "Compiling Geom shader: " << geometryShaderPath << endl;
  char const * geometrySourcePointer = geometryShaderCode.c_str();
  glShaderSource(geometryShader, 1, &geometrySourcePointer , NULL);
  glCompileShader(geometryShader);


  // Check Geometry Shader
  if (debug_level > 0) {
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    char * geometryShaderErrorMessage = new char[infoLogLength];
    glGetShaderInfoLog(geometryShader, infoLogLength, NULL, &geometryShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &geometryShaderErrorMessage[0]);
    delete [] geometryShaderErrorMessage;
  }

  glAttachShader(shaderProgram, geometryShader);



}


void Shader::setFragmentShader() {
  
  // Read the Fragment Shader code from the file
  string fragmentShaderCode;
  ifstream fragmentShaderStream(fragmentShaderPath.c_str(), std::ios::in);
  if(fragmentShaderStream.is_open()){
    string line = "";
    while(getline(fragmentShaderStream, line))
      fragmentShaderCode += "\n" + line;
    fragmentShaderStream.close();
  }

  if (debug_level > 2)
    cout << fragmentShaderCode << endl;

  GLint result = GL_FALSE;
  int infoLogLength;
 
  // Compile Fragment Shader
  if (debug_level > 0)
    cout << "Compiling frag shader: " << fragmentShaderPath << endl;
  char const * fragmentSourcePointer = fragmentShaderCode.c_str();
  glShaderSource(fragmentShader, 1, &fragmentSourcePointer , NULL);
  glCompileShader(fragmentShader);

  // Check Fragment Shader
  if (debug_level > 0) {
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    char * fragmentShaderErrorMessage = new char[infoLogLength];
    glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &fragmentShaderErrorMessage[0]);
    delete [] fragmentShaderErrorMessage;
  }

  glAttachShader(shaderProgram, fragmentShader);

  if (debug_level > 0)
    cout << endl;


}

void Shader::setComputeShaders() {

  int position = 0;//Position of the shader in computeShaders vector.

  for(vector<string>::iterator it = computeShaderPaths.begin(); it != computeShaderPaths.end(); it++) {
	
    // Read the Fragment Shader code from the file
    string computeShaderCode;
    ifstream computeShaderStream(it->c_str(), std::ios::in);
    if(computeShaderStream.is_open()){
      string line = "";
      while(getline(computeShaderStream, line))
	computeShaderCode += "\n" + line;
      computeShaderStream.close();
    }

    if (debug_level > 1)
      cout << computeShaderCode << endl;
	
    GLint result = GL_FALSE;
    int infoLogLength;
 	
    // Compile Compute Shader
    if (debug_level > 0)
      cout << "Compiling compute shader: " << *it << endl;
    char const * computeSourcePointer = computeShaderCode.c_str();
    glShaderSource(computeShaders[position], 1, &computeSourcePointer , NULL);
    glCompileShader(computeShaders[position]);
	
    // Check Fragment Shader
    if (debug_level > 0) {  
      glGetShaderiv(computeShaders[position], GL_COMPILE_STATUS, &result);
      glGetShaderiv(computeShaders[position], GL_INFO_LOG_LENGTH, &infoLogLength);
      char * computeShaderErrorMessage = new char[infoLogLength];
      glGetShaderInfoLog(computeShaders[position], infoLogLength, NULL, &computeShaderErrorMessage[0]);
      fprintf(stdout, "%s\n", &computeShaderErrorMessage[0]);
      delete [] computeShaderErrorMessage;
    }

    glAttachShader(shaderProgram, computeShaders[position]);
	
    if (debug_level > 0)
      cout << endl;
	


    position++;
  }
}

void Shader::reloadShaders() {

    if (debug_level > 1)
        cout << "reloading shaders" << endl;

  if(fragmentShader != 0) {
    glDetachShader(shaderProgram, fragmentShader);
    setFragmentShader();
  }
  if(vertexShader != 0) {
    glDetachShader(shaderProgram, vertexShader);
    setVertexShader();
  }
  if(geometryShader != 0) {
    glDetachShader(shaderProgram, geometryShader);
    setGeometryShader();
  }
  if(computeShaders.size() > 0) {
    for(unsigned int i = 0; i<computeShaders.size(); i++) {
      glDetachShader(shaderProgram, computeShaders[i]);
    }
    setComputeShaders();
  }


  glLinkProgram(shaderProgram);


}

void Shader::enable() {
  glUseProgram(shaderProgram);

}

void Shader::disable() {
  glUseProgram(0);
}

void Shader::deleteShaders() {
 glDetachShader(shaderProgram, fragmentShader);
 glDetachShader(shaderProgram, vertexShader);
 glDeleteShader(fragmentShader);
 glDeleteShader(vertexShader);
 glDeleteProgram(shaderProgram);
}

void errorCheckFunc(string file, int line, string message){
  //OpenGL Error Handling Function:
  GLenum ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR)
    {
      cerr << " GL error in " << file << "  line " << line << " : " << gluErrorString(ErrorCheckValue) << " " << message << endl;
    }
}

//============================Uniforms Setters==========================================================

//Setting uniforms by value:
//Integer & Location:
void Shader::setUniform(GLint location, GLint a, GLint b, GLint c, GLint d) {
  //assert(location != -1);
  glUniform4i(location, a,b,c,d);

}
void Shader::setUniform(GLint location, GLint a, GLint b, GLint c) {
  //(location != -1);
  glUniform3i(location, a,b,c);

}
void Shader::setUniform(GLint location, GLint a, GLint b) {
  //assert(location != -1);
  glUniform2i(location, a,b);

}
void Shader::setUniform(GLint location, GLint a) {
  //assert(location != -1);
  glUniform1i(location, a);

}
//Integer & Name:
void Shader::setUniform(const GLchar* name, GLint a, GLint b, GLint c, GLint d) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b,c,d);
}
void Shader::setUniform(const GLchar* name, GLint a, GLint b, GLint c) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b,c);
}
void Shader::setUniform(const GLchar* name, GLint a, GLint b) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b);
}
void Shader::setUniform(const GLchar* name, GLint a) {
  GLint location = getUniformLocation(name);
  setUniform(location,a);
}
//Float & Location:
void Shader::setUniform(GLint location, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
  //assert(location != -1);
  glUniform4f(location,a,b,c,d);

}
void Shader::setUniform(GLint location, GLfloat a, GLfloat b, GLfloat c) {
  //assert(location != -1);
  glUniform3f(location,a,b,c);

}
void Shader::setUniform(GLint location, GLfloat a, GLfloat b) {
  //assert(location != -1);
  glUniform2f(location,a,b);

}
void Shader::setUniform(GLint location, GLfloat a) {
  //assert(location != -1);
  glUniform1f(location,a);

}
//Float & Name:
void Shader::setUniform(const GLchar* name, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b,c,d);
}
void Shader::setUniform(const GLchar* name, GLfloat a, GLfloat b, GLfloat c) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b,c);
}
void Shader::setUniform(const GLchar* name, GLfloat a, GLfloat b) {
  GLint location = getUniformLocation(name);
  setUniform(location,a,b);
}
void Shader::setUniform(const GLchar* name, GLfloat a) {
  GLint location = getUniformLocation(name);
  setUniform(location,a);
}
//Double & Location:
void Shader::setUniform(GLint location, GLdouble a, GLdouble b, GLdouble c, GLdouble d) {
  //assert(location != -1);
  glUniform4f(location, (GLfloat)a, (GLfloat)b, (GLfloat)c, (GLfloat)d);

}
void Shader::setUniform(GLint location, GLdouble a, GLdouble b, GLdouble c) {
  //assert(location != -1);
  glUniform3f(location, (GLfloat)a, (GLfloat)b, (GLfloat)c);

}
void Shader::setUniform(GLint location, GLdouble a, GLdouble b) {
  //assert(location != -1);
  glUniform2f(location, (GLfloat)a, (GLfloat)b);

}
void Shader::setUniform(GLint location, GLdouble a) {
  //assert(location != -1);
  glUniform1f(location, (GLfloat)a);

}
//Double & name:
void Shader::setUniform(const GLchar* name, GLdouble a, GLdouble b, GLdouble c, GLdouble d) {
  GLint location = getUniformLocation(name);
  setUniform(location, a, b, c, d);
}
void Shader::setUniform(const GLchar* name, GLdouble a, GLdouble b, GLdouble c) {
  GLint location = getUniformLocation(name);
  setUniform(location, a, b, c);
}
void Shader::setUniform(const GLchar* name, GLdouble a, GLdouble b) {
  GLint location = getUniformLocation(name);
  setUniform(location, a, b);
}
void Shader::setUniform(const GLchar* name, GLdouble a) {
  GLint location = getUniformLocation(name);
  setUniform(location, a);
}
//Setting Uniforms by vector:
//Location & Integer:
void Shader::setUniform(GLint location, const GLint* v, GLuint nvalues, GLsizei count) {
  //assert(location != -1);
  //assert((nvalue > 0) && (nvalues < 5));

  switch (nvalues) {
  case 1: glUniform1iv(location,count,v); break;
  case 2: glUniform2iv(location,count,v); break;
  case 3: glUniform3iv(location,count,v); break;
  case 4: glUniform4iv(location,count,v); break;
  }

}
//Location & Float:
void Shader::setUniform(GLint location, const GLfloat* v, GLuint nvalues, GLsizei count) {
	
  //assert(location != -1);
  //assert((nvalues > 0) && (nvalues < 5));

  switch (nvalues) {
  case 1: glUniform1fv(location, count, v); break;
  case 2: glUniform2fv(location, count, v); break;
  case 3: glUniform3fv(location, count, v); break;
  case 4: glUniform4fv(location, count, v); break;
  }

}
//Name & Integer:
void Shader::setUniform(const GLchar* name, const GLint* v, GLuint nvalues, GLsizei count) {
  GLint location = getUniformLocation(name);
  setUniform(location,v,nvalues,count);
}
//Name & Float:
void Shader::setUniform(const GLchar* name, const GLfloat* v, GLuint nvalues, GLsizei count) {
  GLint location = getUniformLocation(name);
  setUniform(location, v, nvalues, count);
}
//Setting uniform matrix value:
//Location:
void Shader::setUniform(GLint location, const GLfloat* m, GLuint dim, GLboolean transpose, GLsizei count) {
  //assert(location != -1);
  //assert((dim > 1) && (dim < 5));

  switch(dim) {
  case 2: glUniformMatrix2fv(location, count, transpose, m); break;
  case 3: glUniformMatrix3fv(location, count, transpose, m); break;
  case 4: glUniformMatrix4fv(location, count, transpose, m); break;
  }

}
//Name:
void Shader::setUniform(const GLchar* name, const GLfloat* m, GLuint dim, GLboolean transpose, GLsizei count) {
  GLint location = getUniformLocation(name);
  setUniform(location, m, dim, transpose, count);
}
