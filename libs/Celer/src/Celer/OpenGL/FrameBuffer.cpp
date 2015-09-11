#include "FrameBuffer.hpp"

namespace Celer
{
        namespace OpenGL
        {

                // TODO Por enquanto é o básico do FrameBuffer.
                // TODO Próxima extensão é fazer mais de um Color Attachment.

                FrameBuffer::FrameBuffer ( std::string name )
                {
                        name_             = name;
                        bound_            = false;
                        width_            = 0;
                        height_           = 0;
                        isInitialized_    = false;

                        frameBuffer_id_   = 0;
                        depthBuffer_id_   = 0;
                        stencilBuffer_id_ = 0;

                        Celer::OpenGL::OpenGLContext::instance( )->glewInitialize( name_ );

                        if ( !glewIsSupported ( "GL_EXT_framebuffer_object" ) )
                        {
                                std::cerr << "ERROR" + name_ + ": Support for  GL_EXT_framebuffer_object OpenGL extensions missing." << std::endl;
                        }else
                        {
                                int max_attachments;
                                // Get the maximum number of color attachments.
                                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attachments);

                                colorAttachtment.resize( max_attachments );
                        }
                }

                FrameBuffer::~FrameBuffer ( )
                {
                        if ( bound_ )
                        {
                                unbind ( );
                        }

                        if ( depthBuffer_id_ != 0 )
                        {
                                glDeleteRenderbuffersEXT ( 1 , &depthBuffer_id_ );
                        }

                        if ( frameBuffer_id_ != 0 )
                        {
                                glDeleteFramebuffersEXT ( 1 , &frameBuffer_id_ );
                        }
                }

                bool FrameBuffer::create ( const GLuint width , const GLuint height , GLenum internalFormat, GLenum format , GLenum type, GLenum target )
                {
                        /// XXX: How creat color Attachments and Depth and Stencil Buffer
                        /// XXX: This function needs too much attention !!
                        /// FIXME: hasDepht;hasStencil,hasTexture: How many and so one ...

                        width_  = width;
                        height_ = height;

                        glGenFramebuffersEXT(1, &frameBuffer_id_ );
                        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frameBuffer_id_ );


                        for ( int i = 0; i < colorAttachtment.size() ; i++ )
                        {
                                colorAttachtment[i].first 		  = GL_COLOR_ATTACHMENT0 + i;
                                colorAttachtment[i].second.create (width_,height_,internalFormat,format,type,target);
                                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, colorAttachtment[i].second.id(), 0);
                        }

                        // create the depth buffer
                        glGenRenderbuffers( 1, &depthBuffer_id_ );
                        glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer_id_ );
                        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_ , height_ );
                        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_id_ );

                        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

                        return checkFramebufferStatus ( );
                }

                bool FrameBuffer::bind ( )
                {
                        if ( frameBuffer_id_ == 0 )
                        {
                                return false;
                        }

                        glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT , frameBuffer_id_ );

                        return bound_;
                }

                bool FrameBuffer::checkFramebufferStatus ( )
                {
                        // check FBO status
                        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frameBuffer_id_ );
                        GLenum status = glCheckFramebufferStatusEXT ( GL_FRAMEBUFFER_EXT );
                        switch ( status )
                        {
                                case GL_FRAMEBUFFER_COMPLETE_EXT:
                                        std::cout << name_ + " : Framebuffer complete." << std::endl;
                                        return true;

                                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                                        std::cout << name_ + " : [ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                                        std::cout << name_ + " : [ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                                        std::cout << name_ + " : [ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                                        std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                                        std::cout << name_ + " : [ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                                        std::cout << name_ + " : [ERROR] Framebuffer incomplete: Read buffer." << std::endl;
                                        return false;

                                case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                                        std::cout << name_ + " : [ERROR] Unsupported by FBO implementation." << std::endl;
                                        return false;

                                default:
                                        std::cout << name_ + " : [ERROR] Unknow error." << std::endl;
                                        return false;
                        }

                        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
                }

                bool FrameBuffer::unbind ( )
                {

                        glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT , 0 );

                        return true;
                }

                GLuint FrameBuffer::width ( ) const
                {
                        return width_;
                }


                GLuint FrameBuffer::height ( ) const
                {
                        return height_;
                }

                std::string FrameBuffer::name ( ) const
                {
                        return name_;
                }


        }
}
