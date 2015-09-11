#include "Shader.hpp"

namespace Celer
{

        namespace OpenGL
        {

                Shader& Shader::operator= ( const Shader& shader )
                {
                        shaderType_ 	= shader.shaderType();
                        fileName_   	= shader.fileName();
                        description_ 	= shader.description();

                        return *this;
                }

                Shader::Shader ( GLenum shadertype , std::string fileName , std::string description )
                {
                        shaderType_ 	= shadertype;
                        fileName_ 	= fileName;
                        description_ 	= description;
                }

                Shader::~Shader ( )
                {

                }

                /// TODO: An way to fill this function taking advantage of GLEW
                std::string Shader::showShaderType ( ) const
                {
                        switch ( shaderType_ )
                        {
                                case GL_VERTEX_SHADER:
                                        return std::string ( "GL_VERTEX_SHADER" );
                                case GL_GEOMETRY_SHADER:
                                        return std::string ( "GL_GEOMETRY_SHADER" );
                                case GL_FRAGMENT_SHADER:
                                        return std::string ( "GL_FRAGMENT_SHADER" );
                                default:
                                        return std::string ( "No shader defined" );
                        }
                }

                GLenum Shader::shaderType ( ) const
                {
                        return shaderType_;
                }

                std::string Shader::fileName ( ) const
                {
                        return fileName_;
                }

                std::string Shader::description ( ) const
                {
                        return description_;
                }


                void Shader::showShaderInformation ( ) const
                {
                        std::cout << " Shader ---"       << showShaderType() << std::endl;
                        std::cout << " fileName ---" 	 << fileName() 	     << std::endl;
                        std::cout << " description ---"  <<  description()   << std::endl;
                }

                VertexShader::VertexShader ( std::string fileName , std::string description ):
				                                                                                                                                Shader ( GL_VERTEX_SHADER ,fileName , description )
                {

                }

                FragmentShader::FragmentShader ( std::string fileName , std::string description ) :
				                                                                                                                                Shader ( GL_FRAGMENT_SHADER ,fileName , description )
                {

                }
        }

}
