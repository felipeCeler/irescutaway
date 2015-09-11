#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

//- Celer/Base/Exception.hpp - Exception.hpp Module definition --------------//
//- Celer Graphics
//  Copyrights (c) 2008-2013 - Felipe de Carvalho
//
//                     The Celer Base Module
//
// This file is distributed under GNU General Public License as published by
// the Free Software Foundation. See LICENSE.TXT for details.
//
// @file
// @created on: Mar 23, 2012
// @version   : 0.1.0 Initial Release
// @brief This file contains the declaration of the Exception class which 
//		  provides functions to throw exceptions.
//        Not used, just for future plans.
//
//---------------------------------------------------------------------------//

/// Standard C++ library
#include <exception>
#include <string>

namespace Celer
{
        namespace Log
        {

                class Exception: public std::exception
                {

                        private:
                                std::string             message_; ///< the message of the exception

                        public:
                                                        Exception ( const std::string &message ) throw ();

                                                        Exception ( const std::string &name ,
                                                                    const std::string &message ) throw ();

                                virtual                 ~Exception ( ) throw ();
                                virtual const char *    what       ( ) const throw ();

                };

        }
}

#endif /* EXCEPTION_HPP_ */

