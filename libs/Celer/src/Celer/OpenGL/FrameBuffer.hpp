#ifndef FRAMEBUFFER_HPP_
#define FRAMEBUFFER_HPP_

//- Celer/OpenGL/FrameBuffer.hpp - FrameBuffer Module definition --------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                    The Celer OpenGL Framework
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. @link http://www.gnu.org/licenses/gpl.html.
//
// @file
// @created on: Mar 5, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the FrameBuffer class which
//        provides a wrapper to handle FrameBuffer Object.
// TODO: How I will handle the various COLOR_ATTACHMENT ?
//---------------------------------------------------------------------------//

/// OpenGL Extensions 		- Need a OpenGL context to use this class.
#include "Celer/OpenGL/OpenGLContext.hpp"
/// Celer OpenGL Framework	- Color Attachments.
#include "Celer/OpenGL/Texture.hpp"
/// Base			- This class can't be copied.
#include "Celer/Base/Base.hpp"
/// Standard C++ Library	- std::cout and std::string.
#include <iostream>
#include <string>
#include <vector>

namespace Celer
{
        namespace OpenGL
        {
                class FrameBuffer : public Celer::NonCopyable
                {
                        private:
                                GLuint 			frameBuffer_id_;
                                GLuint 			depthBuffer_id_;
                                GLuint			stencilBuffer_id_;

                                GLuint 			width_;
                                GLuint 			height_;
                                std::string 	        name_;		   ///< Something like that ... Why I'm using this framebuffer ?
                                bool			isInitialized_;
                                bool    		bound_;

                        public:

                                FrameBuffer             ( );
                                FrameBuffer             ( std::string name );
                                ~FrameBuffer            ( );
                                std::vector<std::pair<GLuint, Celer::OpenGL::Texture2D> > colorAttachtment;

                                GLuint id               ( ) const;
                                GLuint width            ( ) const;
                                GLuint height           ( ) const;
                                std::string name        ( ) const;

                                bool bind               ( );
                                bool unbind             ( );
                                bool isInitialized      ( ) const;

                                bool checkFramebufferStatus ( );
                                /// - Shows the current FrameBuffer parameters and Objects.
                                void whoIm                  ( ) const;

                                bool create ( const GLuint width ,
                                              const GLuint height ,
                                              GLenum internalFormat ,
                                              GLenum format ,
                                              GLenum type ,
                                              GLenum target );



                };
        }
}

#endif /* FRAMEBUFFER_HPP_ */

