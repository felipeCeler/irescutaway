#ifndef OPENGL_CELER_HPP_
#define OPENGL_CELER_HPP_

//- Celer/OpenGL/OpenGLContext.hpp - OpenGLContext Module definition --------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                     The Celer OpenGL Framework
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. @link http://www.gnu.org/licenses/gpl.html.
//
// @file
// @created on: Mar 21, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the OpenGLContext class which
//        provides guarantees that OpenGL context has been created
//
//---------------------------------------------------------------------------//

#define GLEW_STATIC

// Unix Like OpenGL inlcudes.
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

// Standart Library
#include <iostream>
#include <sstream>


namespace Celer
{
        namespace OpenGL
        {
                class OpenGLContext
                {
                        private:
                                OpenGLContext         ( );
                                static OpenGLContext* instance_;
                                bool isglewInitialized_;

                        public:
                                static OpenGLContext* instance ( );
                                void glewInitialize            ( const std::string& who );

                };

        }
}
#endif /* OPENGL_CELER_HPP_ */
