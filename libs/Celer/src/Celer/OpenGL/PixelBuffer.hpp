#ifndef PIXELBUFFER_HPP_
#define PIXELBUFFER_HPP_

//- Celer/Base/PixelBuffer.hpp - PixelBuffer.hpp Module definition ----------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                     The Celer OpenGL Framework
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. @link http://www.gnu.org/licenses/gpl.html.
//
// @file
// @created on: Mar 5, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the PixelBuffer class which
//                provides a wrapper to the OpenGL Pixel Buffer Object.
//
//---------------------------------------------------------------------------//

/// System
#include <assert.h>
#include <iostream>
#include <cstdio>
#include <string>

#include "Celer/OpenGL/OpenGLContext.hpp"

/// Base			- This class can't be copied.
#include "Celer/Base/Base.hpp"

namespace Celer
{
        namespace OpenGL
        {
                class PixelBuffer : public Celer::NonCopyable
                {

                        private:

                                GLuint 			id_;
                                GLenum 			target_;
                                bool 			ok_;
                                bool 			bound_;
                                std::string		name_;

                        public:
                                PixelBuffer                          ( std::string name );
                                ~PixelBuffer                         ( );

                                GLuint 			id           ( ) const;
                                bool 			bind         ( GLenum target );
                                bool 			unbind       ( );
                                void 			setData      ( unsigned size , const void * ptr , GLenum usage );
                                void 			setSubData   ( unsigned offs , unsigned size , const void * ptr );
                                void 			getSubData   ( unsigned offs , unsigned size , void * ptr );
                                void* 			map          ( GLenum access );
                                bool 			unmap        ( );
                                void 			clear        ( );

                                static bool isSupported      ( );
                                std::string name             ( );
                };
        }
}
#endif /* PIXELBUFFER_HPP_ */
