/*
 * OpenGL.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: felipe
 */

#include "OpenGLContext.hpp"
// TODO Totally Crazy this Class , why not constructor !!!!
// TODO Getting better Watching Tublib from Labri Boudeaux.

Celer::OpenGL::OpenGLContext* Celer::OpenGL::OpenGLContext::instance_ = 0;

namespace Celer
{
        namespace OpenGL
        {
                OpenGLContext::OpenGLContext ( ) : isglewInitialized_(false)
                {

                }
                /**
                 * Return the current instance. If instance doesn't exist, create it.
                 */
                OpenGLContext* OpenGLContext::instance ( )
                {
                        if ( instance_ == 0 )
                        {
                                instance_ = new OpenGLContext ( );
                        }

                        return instance_;
                }

                void OpenGLContext::glewInitialize ( const std::string& who)
                {
                        if ( isglewInitialized_ )
                        {
                                return;
                        }

                        // Ensure OpenGL Context.
                        GLenum error = glewInit ( );
                        if ( error != GLEW_OK )
                        {
                                isglewInitialized_ = false;

                                std::cout << "Glew error : " << glewGetErrorString ( error ) << " from: " << who <<  " => OpenGL can't be used.";
                        }
                        else
                        {
                                std::cout << "GL VERSION : " + who + " " << glGetString ( GL_VERSION ) << std::endl;

                                isglewInitialized_ = true;
                        }

                }
        }
}
