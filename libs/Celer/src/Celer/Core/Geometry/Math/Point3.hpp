#ifndef CELER_POINT3_HPP_
#define CELER_POINT3_HPP_

#include <iostream>
// [C++ Header files]
#include <cmath>
// [C++ Header STL]

#include <Celer/Core/Geometry/Math/Vector3.hpp>


namespace Celer
{
/*!
*@class Point3.
*@brief Class that represent a Point in 3D.
*@details ...
*@author Felipe Moura.
*@email fmc@cos.ufrj.br
*@version 1.0.
*@date 25-Feb-2008.
*@todo OpenGL interface and a C style array.
*@todo Vector <-> Point Operator
*/

      template <class Real>   class Point3
      {

      public:
     	  /*! @name Members  */
     	  //@{
     	  /*! An union. */
     	  /*!
     	   * Shareed memory with abscissas atributes and the XYZ Array.
     	   * The Struct has the same size of the \code xyz[] \endcode.
     	   */
    	  Real x; /*!< X abscissa of space. */
    	  Real y; /*!< Y abscissa of space. */
    	  Real z; /*!< Y abscissa of space. */

     	  //@}

      public:

    	  friend class Vector3<Real>;

    	  /*! @name  Defining a Point3 */
    	  //@{
    	  /*! Default constructor. Value is set to (0,0,0). */
    	  Point3();
    	  Point3( const Real& x, const Real& y, const Real& z );
    	  Point3( const Point3<Real>& point );
    	  Point3( const Vector3<Real>& v );
    	  template < class T >
    	  Point3( const T* point );
    	  virtual ~Point3(){};

    	  void 					Set( const Real& x, const Real& y, const Real& z );
    	  //Operator
    	  //@}

  		  /*! @name Accessing the value */
  		  //@{

    	  /*! Conversion operator returning the memory address of the Point.

    	  Useful to pass as a parameter to OpenGL functions:
    	  \code
    	  Celer:Point3<float> pos, normal;
    	  glNormal3fv(normal);
    	  glVertex3fv(pos);
    	  \endcode */

    	  Real 					operator[]( int i ) const;
    	  Real& 				operator[]( int i );
    	  //@}

    	  /*! @name Algebraic computations */
    	  // @{
    	  // Point/Scalar operations
    	  Point3<Real>  	  	operator+() const;
    	  Point3<Real>  	  	operator-() const;

    	  Point3<Real>& 		operator+=( const Real& factor );
    	  Point3<Real>& 		operator-=( const Real& factor );
    	  Point3<Real>& 		operator*=( const Real& factor );
    	  Point3<Real>& 		operator/=( const Real& factor );
    	  template <class T>
    	  friend Point3<T> 		operator+( const T& factor,const Point3<T>& p  ) ;
    	  template <class T>
    	  friend Point3<T> 		operator+( const Point3<T>& p, const T& factor ) ;
    	  template <class T>
    	  friend Point3<T> 		operator-( const T& factor, const Point3<T>& p ) ;
    	  template <class T>
    	  friend Point3<T> 		operator-( const Point3<T>& p, const T& factor ) ;

    	  template <class T>
    	  friend Point3<T> 		operator*( const T& factor, const Point3<T>& p ) ;
    	  template <class T>
    	  friend Point3<T> 		operator*( const Point3<T>& p, const T& factor ) ;

    	   Point3<Real> 		operator/( const Real& factor ) const;

    	  // Point/Point operations
    	  Point3<Real>& 		operator= ( const Point3<Real>& p );
    	  Point3<Real>& 		operator+=( const Point3<Real>& p );
    	  Point3<Real>& 		operator-=( const Point3<Real>& p );
    	  Point3<Real>& 		operator/=( const Point3<Real>& p );

    	  Point3<Real> 			operator+( const Point3<Real>& p ) const;
    	  template <class T>
    	  friend  Vector3<T>  	operator-( const Point3<T>& source,const Point3<T>& target ) ;

    	  bool 					operator==( const Point3<Real>& p ) const;
    	  bool 					operator!=( const Point3<Real>& p ) const;

    	  // Point/Vector
    	  template <class T>
    	  friend Point3<T> 		operator+( const Point3<T>& p, const Vector3<T>& v ) ;
    	  template <class T>
    	  friend Point3<T> 		operator+( const Vector3<T>& v, const Point3<T>& p ) ;

    	  template <class T>
    	  friend Point3<T> 		operator-( const Point3<T>& p, const Vector3<T>& v ) ;
    	  template <class T>
    	  friend Point3<T> 		operator-( const Vector3<T>& v, const Point3<T>& p ) ;
      	  /*! @name Output stream */
      	  //@{
      	  /*! Output stream operator. Enables debugging code like:
      	  \code
      	  Celer::Point3<> p(...);
      	  std::cout << "Point3" << " x = " << p.x << " ,y = " << p.y << " ,z = " << p.z << std::endl;
      	  \endcode */
    	  template <class T>
    	  friend std::ostream& 	operator<< (std::ostream & s, const Point3<T>& p);

    	  //@}
    	  /*!@name Functions */
    	  //@{
    	  /*!  */
    	  Real 					EuclideanDistance ( const Point3<Real>& p )  const;
  		  const Real* 			ToRealPtr( void ) const;
  		  Real* 				ToRealPtr( void );

  		  inline operator const Real *( void ) const;

  		  inline operator Real * ( void );

		  //@}


      };

      //============================= LIFECYCLE ====================================
      template<class Real>
      Point3<Real>::Point3()
      {
      	this->x = static_cast< Real > (0);
      	this->y = static_cast< Real > (0);
      	this->z = static_cast< Real > (0);

      };

      /*!@brief Constructor with X, Y and Z initialization.
       * @details Initialize all abscissas.
       * @param[in] x Real number.
       * @param[in] y Real number.
       * @param[in] z Real number.
       */
      template<class Real>
      Point3<Real>::Point3( const Real& x, const Real& y, const Real& z )
      : x(x), y(y), z(z) {};

      /*!@brief Constructor with X, Y and Z initialization with vector paramenter.
       * @param[in] u Vector of Real.
       */

      template<class Real>
      Point3<Real>::Point3( const Vector3<Real>& vector )
      : x(vector.x), y(vector.y), z(vector.z) {};

      /*!@brief Copy Constructor.
       *  @param[in] p Point Real.
       */
      template<class Real>
      Point3<Real>::Point3( const Point3<Real>& point )
      : x(point.x), y(point.y), z(point.z) {};

      /*!@brief Constructor with X, Y and Z initialization of any type.
       *  @details Initialize all abscissas of any. Try cast to the Real type of the class
       *  @param[in] array of any type.
       */
      template<class Real>
      template < typename P >
      Point3<Real>::Point3( const P* p )
      {
      	assert(p);

      	this->x = static_cast< Real > ( p[0] );
      	this->y = static_cast< Real > ( p[1] );
      	this->z = static_cast< Real > ( p[2] );

      };

      //============================= ACESS ======================================


      template<class Real>
      inline void Point3<Real>::Set( const Real& x, const Real& y, const Real& z )
      {
      	this->x = x;
      	this->y = y;
      	this->z = z;
      };

      //Operator

      template<class Real>
      inline  Real Point3<Real>::operator[]( int index )  const
      {
      	if ( (index > 2) || ( index < 0))
      	{
      		std::cerr << "[ERROR] Point3 operator[]"        << std::endl
      		<< "        Out of the Point size. " << std::endl
      		<< "        Accepts, 0 , 1 , 2 only." << std::endl;

      	}

      	return ( ( &x )[ index ] );
      };
      /*!@brief operator[]
       * @see operator[]
       * @note Acept assigement
       * @return Real.
       */
      template<class Real>
      inline  Real& Point3<Real>::operator[]( int index )

      {
      	if ( (index > 2) || ( index < 0))
      	{
      		std::cerr << "[ERROR] Point3 operator[]"        << std::endl
      		<< "        Out of the Point size. " << std::endl
      		<< "        Accepts, 0 , 1 , 2 only." << std::endl;

      	}


      	return ( ( &x )[ index ] );

      };// End Interface

      //============================= OPERATORS ====================================

      /*!@brief operator+
       * @details Unary operator tha return itself.
       * @note To be clearer the arithmetic operations.
       * @return Point3.
       */
      template<class Real>
      inline Point3<Real> Point3<Real>::operator+()  const
      {
      	return ( Point3<Real> (this->x, this->y, this->z)  );
      };

      /*!@brief operator-
       * @details Unary operator tha return the negative of the point.
       * @note To be clearer the arithmetic operations.
       * @return Point3.
       */
      template<class Real>
      inline Point3<Real>  Point3<Real>::operator-() const
      {

      	return ( Point3<Real> (-this->x, -this->y, -this->z) );

      };

      /*!@brief operator+=
       * @details Add the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3< Real >& Point3<Real>::operator+=( const Real&  factor )
      {
      	this->x += factor;
      	this->y += factor;
      	this->z += factor;

      	return ( *this );
      }
      /*!@brief operator-=
       * @details Subtract the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3< Real >& Point3<Real>::operator-=( const Real&  factor )
      {
      	this->x -= factor;
      	this->y -= factor;
      	this->z -= factor;

      	return ( *this );
      }
      /*!@brief operator*=
       * @details Multiply the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3< Real >& Point3<Real>::operator*=( const Real& factor )
      {

      	this->x  *= factor;
      	this->y  *= factor;
      	this->z  *= factor;

      	return ( *this );
      }
      /*!@brief operator/=
       * @details Divide the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3< Real >& Point3<Real>::operator/=( const Real& factor )
      {
      	Real lFactor = (Real)1 / factor;

      	this->x *= lFactor;
      	this->y *= lFactor;
      	this->z *= lFactor;

      	return ( *this );
      }

      /*!@brief operator+
       * @details Binary operator tha add a poitn with number.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator+( const Real& factor,const Point3<Real>& p )
      {
      	return ( Point3<Real> ( p.x + factor, p.y + factor, p.z + factor ) );

      };
      /*!@brief operator+
       * @see operator+
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator+( const Point3<Real>& p, const Real& factor )
      {
      	return ( Point3<Real> ( p.x + factor, p.y + factor, p.z + factor ) );

      };

      /*!@brief operator-
       * @details Binary operator tha subtratc a poitn and number.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator-( const Real& factor,const Point3<Real>& p )
      {
      	return ( Point3<Real> ( factor - p.x , factor - p.y , factor - p.z  ) );

      };

      /*!@brief operator-
       * @see operator-
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator-( const Point3<Real>& p, const Real& factor )
      {
      	return ( Point3<Real> ( p.x - factor, p.y - factor, p.z - factor )	 );

      };

      /*!@brief operator*
       * @details Binary operator tha multi the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator*( const Real& factor,const Point3<Real>& p )
      {
      	return ( Point3<Real> ( p.x * factor, p.y * factor, p.z * factor ) );

      };

      template<class Real>
      inline Point3<Real> operator*( const Point3<Real>& p, const Real& factor )
      {
      	return ( Point3<Real> ( p.x * factor, p.y * factor, p.z * factor ) );

      };

      /*!@brief operator/
       * @details Binary operator tha divide the value of the second operand to the value of the all abscissa of the point.
       * @param[in] factor Real
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> Point3<Real>::operator/( const Real& factor )  const
      {
      	return ( Point3<Real> ( x / factor, y/ factor, z / factor ) );

      };

      /*!@brief operator=
       * @details Assigment operator
       * @return Point3
       */
      template<class Real>
      inline Point3<Real>& Point3<Real>::operator=( const Point3<Real>& p )
      {
      	this->x = p.x;
      	this->y = p.y;
      	this->z = p.z;

      	return ( *this );
      };


      /*!@brief operator+=
       * @see operator+=
       * @note Add the correspondents abscissa of the left point to the right.
       * @return Point3
       */
      template<class Real>
      inline Point3< Real >& Point3<Real>::operator+=( const Point3<Real>& p )
      {
      	this->x += p.x;
      	this->y += p.y;
      	this->z += p.z;

      	return ( *this );
      }
      /*!@brief operator-=
       * @see operator-=
       * @note Subtract the correspondents abscissa of the left point to the right.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real>& Point3<Real>::operator-= ( const Point3<Real>& p )
      {
      	this->x -= p.x;
      	this->y -= p.y;
      	this->z -= p.z;

      	return ( *this );
      }

      /*!@brief operator/=
       * @see operator/=
       * @note Subtract the correspondents abscissa of the left point to the right.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real>& Point3<Real>::operator/=( const Point3<Real>& p )
      {

      	this->x /= p.x;
      	this->y /= p.y;
      	this->z /= p.z;

      	return ( *this );
      }

      /*!@brief operator==
       * @details Two points are identical whether your correspondents abscissa are equal.
       * @return bool.
       */
      template<class Real>
      inline bool Point3<Real>::operator==( const Point3<Real>& p ) const
      {
      	return ( ( this->x == p.x ) && ( this->y == p.y ) && ( this->z == p.z ) );
      };
      /*!@brief operator!=
       * @details Opposite to the operator==
       * @see operator==
       * @return bool.
       */
      template<class Real>
      inline bool Point3<Real>::operator!=( const Point3<Real>& p ) const
      {
      	return  !(*this == p) ;
      };


      /*!@brief operator-
       * @details Binary operator tha subtract two point and return a vector.
       * @note First parameter is the origin and the second a target.
       * @return Vector3
       */
      template<class Real>
      inline Vector3<Real> operator-( const Point3<Real>& source, const Point3<Real>& target )
      {
      	return ( Vector3<Real>( 	source.x - target.x,
      			source.y - target.y,
      			source.z - target.z)
      	);

      };

      /*!@brief operator-
       * @details Binary operator tha subtract a poitn and a vecctor.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator-( const Point3<Real>& p, const Vector3<Real>& v )
      {
      	return ( Point3<Real>( p.x - v.x, p.y - v.y, p.z - v.z )  );
      };


      /*!@brief operator-
       * @see operator-
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator-( const Vector3<Real>& v,const Point3<Real>& p )
      {

      	return ( Point3<Real>( v.x - p.x, v.y - p.y, v.z - p.z ) );

      };


      /*!@brief operator+
       * @details Binary operator tha add a poitn and another point.
       * @return Point3
       */

      template<class Real>
      inline Point3<Real> Point3<Real>::operator+( const Point3<Real>& p )  const
      {
      	return ( Point3<Real>( x + p.x, y + p.y, z + p.z ) );

      };


      /*!@brief operator+
       * @details Binary operator tha add a poitn and a vecctor.
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator+( const Point3<Real>& p, const Vector3<Real>& v )
      {
      	return ( Point3<Real> ( p.x + v.x, p.y + v.y, p.z + v.z ) );

      };
      /*!@brief operator+
       * @see operator+
       * @return Point3
       */
      template<class Real>
      inline Point3<Real> operator+( const Vector3<Real>& v,const Point3<Real>& p )
      {
      	return ( Point3<Real> ( p.x + v.x,p.y + v.y, p.z + v.z ) );

      };

      //============================= OPERATIONS ===================================

      /*!@brief operator<<
       * @details For debug
       */
      template<class Real>
      inline std::ostream& operator<<( std::ostream & s, const Point3<Real>& p )
      {
      	s << "Point3" << " x = " << p.x << " ,y = " << p.y << " ,z = " << p.z << std::endl;


      	return ( s );
      };
      /*!@fn squareDistance
       * @details return the distance of two points
       */
      template<class Real>
      inline Real Point3<Real>::EuclideanDistance( const Point3<Real>& p ) const
      {
      	return (  std::sqrt( 	(p.x - this->x) * (p.x - this->x) +
      			  				(p.y - this->y) * (p.y - this->y) +
      			  				(p.z - this->z) * (p.z - this->z) ) );
      }

      template<class Real>
      inline const Real* Point3<Real>::ToRealPtr( void ) const
      {
      	return &x;
      }

      template<class Real>
      inline Real* Point3<Real>::ToRealPtr( void )
      {
      	return &x;
      }

      template<class Real>
      inline Point3<Real>::operator const Real *( void ) const
      {
    	  return &x;
      }

      template<class Real>
      inline Point3<Real>::operator Real * ( void )
      {
    	  return &x;
      }



} /* Celer :: NAMESPACE */

#endif /*POINT3_HPP_*/

