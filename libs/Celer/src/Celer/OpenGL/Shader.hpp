#ifndef SHADER_HPP_
#define SHADER_HPP_

//- Celer/OpenGL/Shader.hpp - Shader Module definition ----------------------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                     The Celer OpenGL Framework
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. @link http://www.gnu.org/licenses/gpl.html.
//
// @file
// @created on: Sep 23, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the Shader class which provides
//        a wrapper to handle GLSL shader programs.
//
// TODO: C++ Class Inheritance Concepts:
//	 Virtual functions, public inheritance ...
//---------------------------------------------------------------------------//


/// OpenGL Extensions 		- Need an OpenGL Context to use this class.
#include "OpenGLContext.hpp"
/// Base			- This class can't be copied.
#include "Celer/Base/Base.hpp"
/// Standard C++ Library        - Show text and type String
#include <iostream> ///- std::cout
#include <string>   ///- std::string



namespace Celer
{

        namespace OpenGL
        {

                class Shader
                {
                        private:
                                GLenum			shaderType_;    ///< VERTEX, GEOMETRY or FRAGMENT shader.
                                std::string 		fileName_;	///< Something.glsl
                                std::string 		description_;	///< Something like ... Phong Lighting Model ?

                        public:

                                Shader&     operator= 			( const Shader & );

                                Shader 			( GLenum      shadertype,
                                       			  std::string fileName,
                                       			  std::string description );

                                std::string showShaderType 		( ) const;
                                GLenum 	    shaderType 			( ) const;
                                std::string fileName 			( ) const;
                                std::string description 		( ) const;
                                void        showShaderInformation	( ) const;

                                virtual ~Shader ( );
                };

                class VertexShader : public Shader
                {
                        public:
                                VertexShader (std::string fileName, std::string description );
                };
                class FragmentShader : public Shader
                {
                        public:
                                FragmentShader ( std::string fileName , std::string description );
                };
                class GeometryShader : public Shader
                {
                        public:
                                GeometryShader ( std::string fileName , std::string description );
                };

        }

}

#endif /* SHADER_HPP_ */
