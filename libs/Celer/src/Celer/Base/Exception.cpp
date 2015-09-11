#include "Exception.hpp"

namespace Celer
{
	namespace Log
	{

		Exception::Exception ( const std::string& message ) throw ( )
		{			
			message_ = message;
		}

		Exception::Exception ( const std::string& name , const std::string& message ) throw ( )
		{
			message_ = name + ": " + message;
		}

		Exception::~Exception ( ) throw ( )
		{
		}

		const char* Exception::what ( ) const throw ( )
		{
			return message_.c_str ( );
		}

	}
}
