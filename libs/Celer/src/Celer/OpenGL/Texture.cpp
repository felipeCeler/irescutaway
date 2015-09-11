/*
 * GLTexture2D.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: felipe
 */

#include <assert.h>
#include <iostream>

#include "Texture.hpp"

// TODO - Assert or Try Catch !!

namespace Celer
{
        namespace OpenGL
        {
                Texture2D::Texture2D()
                {
                        id_ 		= 0;
                        name_		= std::string("Nothing");
                        bound_ 		= 0;
                        isInitialized_ 	= 0;
                        width_ 		= 0;
                        height_		= 0;

                        internalFormat_ = GL_RGBA;
                        format_		= GL_RGBA;
                        type_ 		= GL_UNSIGNED_BYTE;
                        target_ 	= GL_TEXTURE_2D;

                }

                Texture2D::Texture2D( std::string name )
                {
                        id_ 		= 0;
                        name_ 		= name;
                        bound_ 		= 0;
                        isInitialized_ 	= 0;

                        internalFormat_ = GL_RGBA;
                        format_		= GL_RGBA;
                        type_ 		= GL_UNSIGNED_BYTE;
                        target_ 	= GL_TEXTURE_2D;

                }

                Texture2D::~Texture2D()
                {
                        if ( bound_ )
                        {
                                unbind ();
                        }
                        if ( id_ != 0 )
                        {
                                glDeleteTextures ( 1, &id_ );
                        }
                }

                GLuint Texture2D::id() const
                {
                        return id_;
                }

                bool Texture2D::isInitialized	( ) const
                {
                        return isInitialized_;
                }

                bool Texture2D::bind ()
                {
                        if ( id_ == 0 )
                        {
                                return 0;
                        }

                        //assert ( !bound_ );
                        bound_ = 1;

                        glBindTexture ( target_, id_ );

                        return 1;
                }

                bool Texture2D::unbind ()
                {
                        if ( id_ == 0 )
                        {
                                return 0;
                        }

                        //assert ( bound_ );

                        bound_ = 0;

                        glBindTexture ( target_, 0 );

                        return 1;
                }

                GLuint Texture2D::width () const
                {
                        return width_;
                }

                GLuint Texture2D::height () const
                {
                        return height_;
                }
                /// TODO: An way to fill this function taking advantage of GLEW
                std::string Texture2D::determineFormat ( GLint internalFormat ) const
                {
                        switch ( internalFormat )
                        {
                                case GL_RGB:
                                        return std::string ( "GL_RGB" );
                                case GL_RGBA:
                                        return std::string ( "GL_RGBA" );
                                case GL_DEPTH_COMPONENT:
                                        return std::string ( "GL_DEPTH_COMPONENT" );
                                default:
                                        return std::string ( "No internal format defined" );

                        }

                }
                /// TODO: An way to fill this function taking advantage of GLEW
                std::string Texture2D::determineType ( GLenum type ) const
                {
                        switch ( type )
                        {
                                case GL_UNSIGNED_BYTE:
                                        return std::string ( "GL_UNSIGNED_BYTE" );
                                case GL_BYTE:
                                        return std::string ( "GL_BYTE" );
                                case GL_UNSIGNED_INT:
                                        return std::string ( "GL_UNSIGNED_INT" );
                                case GL_INT:
                                        return std::string ( "GL_INT" );
                                case GL_FLOAT:
                                        return std::string ( "GL_FLOAT" );
                                default:
                                        return std::string ( "No type defined" );

                        }
                }
                /// TODO: An way to fill this function taking advantage of GLEW
                std::string Texture2D::determineTarget ( GLenum target ) const
                {
                        switch ( target )
                        {
                                case GL_TEXTURE_2D:
                                        return std::string ( "GL_TEXTURE_2D" );
                                case GL_TEXTURE_RECTANGLE:
                                        return std::string ( "GL_TEXTURE_RECTANGLE" );
                                default:
                                        return std::string ( "No target defined" );

                        }
                }


                /// TODO: getTexture GLuint and GLenum as String like @kglib.
                void Texture2D::whoIm( ) const
                {
                        std::cout << "| -------------------------------------------------------------- |"	<< std::endl;
                        std::cout << "| --- Texture propose : " << name_		   			<< std::endl;
                        std::cout << "| --- target          : " << determineTarget(target_)			<< std::endl;
                        std::cout << "| --- internalFormat  : " << determineFormat(internalFormat_) 		<< std::endl;
                        std::cout << "| --- format          : " << determineFormat(format_)	   		<< std::endl;
                        std::cout << "| --- type            : " << determineType(type_)	  			<< std::endl;
                        std::cout << "| --- id              : " << id_ 		   				<< std::endl;
                        std::cout << "| --- isOk ?          : " << isInitialized_	   			<< std::endl;
                        std::cout << "| -------------------------------------------------------------- |"	<< std::endl;

                }

                /// Here where are things happen !!
                ///	- create a normal GL_TEXTURE_2D or a GL_TEXTURE_RECTANGLE.
                /// TODO: Setup filters ... I don't know the real importance of them.
                int Texture2D::create(const GLuint width, const GLuint height)
                {
                        // TODO this is just the beginning
                        // Create a texture as attachment
                        if ( id_ != 0 )
                        {
                                glDeleteTextures ( 1 , &id_ );
                                id_ = 0;
                        }
                        else
                        {
                                glGenTextures ( 1 , &id_ );
                        }

                        this->bind ( );
                        // Set basic parameters
                        glTexParameteri ( target_ , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
                        glTexParameteri ( target_ , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
                        glTexParameteri ( target_ , GL_TEXTURE_MIN_FILTER , GL_NEAREST );
                        glTexParameteri ( target_ , GL_TEXTURE_MAG_FILTER , GL_NEAREST );
                        // Allocate memory
                        glTexImage2D ( target_ , 0 , internalFormat_ , width , height , 0 , format_ , type_ , NULL );
                        this->unbind ( );

                        width_ = width;
                        height_ = height;

                        isInitialized_ = 1;


                        this->whoIm();
                        return id_;
                }

                int Texture2D::create( const GLuint width ,
                                       const GLuint height,
                                       GLuint internalFormat,
                                       GLenum format,
                                       GLenum type,
                                       GLenum target	    )
                {

                        internalFormat_ = internalFormat;
                        format_ 	= format;
                        type_ 		= type;
                        target_ 	= target;


                        // TODO this is just the beginning
                        // Create a texture as attachment
                        if ( id_ != 0 )
                        {
                                glDeleteTextures ( 1 , &id_ );
                        }
                        else
                        {
                                glGenTextures ( 1 , &id_ );
                        }

                        this->bind ( );
                        // Set basic parameters
                        glTexParameteri ( target_ , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
                        glTexParameteri ( target_ , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
                        glTexParameteri ( target_ , GL_TEXTURE_MIN_FILTER , GL_NEAREST );
                        glTexParameteri ( target_ , GL_TEXTURE_MAG_FILTER , GL_NEAREST );
                        // Allocate memory
                        glTexImage2D ( target , 0 , internalFormat , width , height , 0 , format , type , NULL );
                        this->unbind ( );

                        width_ = width;
                        height_ = height;

                        internalFormat_ = internalFormat;
                        format_ = format;
                        type_ = type;
                        target_ = target;

                        isInitialized_ = 1;


                        return id_;
                }

                void Texture2D::loadFromRawData ( const GLubyte* pixels )
                {
                        if ( isInitialized_ )
                        {
                                bind ( );
                                glTexSubImage2D ( target_ , 0 , 0 , 0 , width_ , height_ , internalFormat_ , type_ , pixels );
                                unbind ( );
                        }
                        else
                        {
                                std::cerr << "Texture not created" << std::endl;
                        }
                }
        }
}
