#include "PixelBuffer.hpp"

namespace Celer
{
        namespace OpenGL
        {
                PixelBuffer::PixelBuffer ( std::string name )
                {
                        name_ = name;
                        // TODO Is the context created ?

                        Celer::OpenGL::OpenGLContext::instance( )->glewInitialize( name_ );

                        if ( !glewIsSupported ( "GL_ARB_vertex_buffer_object" ) )
                        {
                                std::cerr << "ERROR: Support for  GL_VERSION_2_0 GL_ARB_pixel_buffer_object OpenGL extensions missing." << std::endl;
                        }
                        else
                        {
                                glGenBuffersARB ( 1 , &id_ );

                                ok_ = ( glIsBufferARB ( id_ ) == GL_TRUE );
                                target_ = 0;
                                bound_ = false;
                        }

                }

                PixelBuffer::~PixelBuffer ( )
                {
                        // TODO
                        if ( bound_ )
                        {
                                unbind ( );
                        }
                        glDeleteBuffersARB ( 1 , &id_ );
                }

                GLuint PixelBuffer::id ( ) const
                {
                        return this->id_;
                }

                bool PixelBuffer::bind ( GLenum target )
                {
                        if ( id_ == 0 )
                        {
                                return false;
                        }

                        assert ( !bound_ );

                        bound_ = true;

                        glBindBufferARB ( target_ = target , id_ );

                        return bound_;

                }

                bool PixelBuffer::unbind ( )
                {
                        assert ( bound_ );

                        bound_ = false;

                        glBindBufferARB ( target_ , 0 );

                        return true;
                }

                void PixelBuffer::setData ( unsigned size , const void * data , GLenum usage )
                {
                        // TODO Tests if the Buffer is bound. Is this really important ?
                        glBufferDataARB ( target_ , size , data , usage );
                }

                void PixelBuffer::setSubData ( unsigned offs , unsigned size , const void * data )
                {
                        // TODO Tests if the Buffer is bound. Is this really important ?
                        glBufferSubDataARB ( target_ , offs , size , data );
                }

                void PixelBuffer::getSubData ( unsigned offs , unsigned size , void * data )
                {
                        // TODO Tests if the Buffer is bound. Is this really important ?
                        glGetBufferSubDataARB ( target_ , offs , size , data );
                }

                void * PixelBuffer::map ( GLenum access )
                {
                        // TODO Tests if the Buffer is bound. Is this really important ?
                        return glMapBufferARB ( target_ , access );
                }

                bool PixelBuffer::unmap ( )
                {
                        // TODO Tests if the Buffer is bound. Is this really important ?
                        return glUnmapBufferARB ( target_ ) == GL_TRUE;
                }

                bool PixelBuffer::isSupported ( )
                {
                        return glewIsSupported ( "GL_ARB_vertex_buffer_object" );
                }

        }
}
