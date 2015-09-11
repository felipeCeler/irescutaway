#ifndef SHADERMANAGER_HPP_
#define SHADERMANAGER_HPP_

//- Celer/OpenGL/ShaderManager.hpp - ShaderManager Module definition --------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                    The Celer OpenGL Framework
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. @link http://www.gnu.org/licenses/gpl.html.
//
// @file
// @created on: Sep 26, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the ShaderManager class which
//        provides a wrapper to handle GLSL shader programs.
//
//---------------------------------------------------------------------------//


/// OpenGL Extensions 		- Need an OpenGL context to use this class.
#include "Celer/OpenGL/OpenGLContext.hpp"
/// Base			- This class can't be copied.
#include <Celer/Base/Base.hpp>
/// OpenGL Shaders
#include <Celer/OpenGL/Shader.hpp>
/// Standard C++ Library	- std::cout and std::string.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>


namespace Celer
{

        namespace OpenGL
        {

                class ShaderManager
                {
                        private:

                                struct Uniform
                                {
                                        GLenum type;
                                        GLint location;
                                        GLint array_size;
                                        GLint array_stride;
                                        GLint offset;
                                };

                                /// @link http://www.opengl.org/wiki/Interface_Block_(GLSL)
                                /// If you are using CelerGL, make sure to translate the matrix after upload to GPU
                                /// The way that uniform block carrier data is complex.
                                /// This seems to work with a small blocks definition.
                                struct UniformBlock
                                {
                                                std::string 	               name;
                                                GLuint 		               index;
                                                GLuint		               size;
                                                std::map<std::string, Uniform> uniform;
                                };

                                struct SubRoutine
                                {
                                                // @brief SubRoutine belong to a shader stage.
                                                //        There is only one uniform location for them.
                                                //        One index for each subroutine.
                                                //
                                                GLenum 		shader_type;
                                                GLuint 		index;
                                                std::string 	name;

                                                GLuint 		uniform_location;
                                                std::string     uniform_name;
                                };

                                GLuint                  id_;        ///< Identification
                                std::string             name_;      ///< Some name that remember the purpose of the shader.
                                bool                    isLinked_;  ///< Is it successfully complied

                                ///TODO: Will be a list to take advantage of subroutine.
                                std::vector<Celer::OpenGL::VertexShader>   vertexShaders_;
                                std::vector<Celer::OpenGL::FragmentShader> fragmentShaders_;


                                bool 	compile 		( );
                                GLuint 	loadShaderFromFile 	( GLenum shaderType , std::string fileName );
                                void 	shaderLogInformation 	( GLuint id );
                                void 	programLogInformation	( );

                        public:

                                std::map<std::string, Uniform> 		                        uniforms_;
                                std::map<std::string, UniformBlock>                             uniform_blocks_;
                                // FIXME <Uniform name, SubRoutine informations>
                                std::map<std::string, std::map <std::string , SubRoutine> > 	subroutines_;

                                ShaderManager 	   ( );
                                ShaderManager 	   ( std::string name );
                                GLint    id	   ( );

                                void create 	   ( const std::string name , const std::string& vertexShader , const std::string& fragmentShader );
                                void create 	   ( const std::string name , const std::string& vertexShader , const std::string& geometryShader , const std::string& fragmentShader );

                                void addUniforms ( );
                                void addUniform ( std::string name , GLenum type , GLint location, GLint array_size , GLint array_stride, GLint offset );

                                void addSubRoutines ( GLenum shader_type );
                                void addSubRoutine  ( std::string name , GLenum shader_type, GLint index, GLint uniform_location, std::string uniform_name );

                                void addUniformBlocks ( );

                                void active   	   ( );
                                void deactive 	   ( );

                                virtual ~ShaderManager ( );
                };

        }

}

#endif /* SHADERMANAGER_HPP_ */

