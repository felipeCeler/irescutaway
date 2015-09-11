#ifndef CELER_MATH_HPP_
#define CELER_MATH_HPP_


#include <cmath>

//-----------------------------------------------------------------------------
// Common math functions and constants.
//-----------------------------------------------------------------------------



namespace Celer {

/*! \brief The Math class represents some useful math function.
\class Math Math.hpp

<h3>Internal representation</h3>
	All constants are float type
\nosubgrouping */

	class Math
	{
	public:
		/*! @name Constants */
		//@{
		/*!  */
		static const float kPi;
		/*! */
	    static const float kHalfPi;
	    /*!
	     * \code
	     *  /f[
		 * \sqrt{ 2 }
		 * /f]
	     * \endcode
	     *  */

	    static const float kSqrtTwo;
	    /*!
	     * \code
	     *  /f[
		 * \sqrt{ 3 }
		 * /f]
	     * \endcode
	     * */
	    static const float kSqrtThree;
	    /*!
	     *  \code
	     *  /f[
		 * 1 / \sqrt{ 2 }
		 * /f]
	     * \endcode
	     * */
	    static const float kInvSqrtTwo;
	    /*!
	     * \code
	     *  /f[
		 * 1 / \sqrt{ 3 }
		 * /f]
	     * \endcode
	     *  */
	    static const float kInvSqrtThree;
	    /*!
	     * * \code
	     * /f[
		 * 1 / 3
		 * /f]
	     * \endcode
	     * */
	    static const float kInvThree;
	    /*! Dregrees to radians conversion
	     * Only multiply the number in degrees by it
	     * */
	    static const float	kDeg2Rad;
	    /*! Radians to dregrees conversion
	     * * Only multiply the number in radians by it
	     * */
	    static const float	kRad2Deg;
	    /*! */
	    static const float	kFloatInfinty;
	    /*! */
	    static const float 	kEpsilon;
	    //@}

		/*! @name functions ... */
		//@{
	    /*! */
	    static bool CloseEnough(float f1, float f2)
	    {
	        // Determines whether the two floating-point values f1 and f2 are
	        // close enough together that they can be considered equal.

	        return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < kEpsilon;
	    }
	    /*!
	     * \attention maybe move to another package
	     * */

		static float Hypotenuse (float x, float y)
		{
			return std::sqrt( (x * x) + (y * y) );
		}

	    //@}
		virtual ~Math();

	};


}/* Celer :: NAMESPACE */

#endif /*MATH_HPP_*/
