#ifndef CELER_POINT2_HPP_
#define CELER_POINT2_HPP_

// [System Include]
#include <iostream>    // standard IO interface

// [Project Includes]
#include <Celer/Core/Geometry/Math/Math.hpp>	   // Use sqrt()
#include <Celer/Core/Geometry/Math/Vector2.hpp> // Friend Class


namespace Celer
{

/*!
*@class Point2.
*@brief Class that represent a Point in 2D.
*@details ...
*@author Felipe Moura.
*@email fmc@cos.ufrj.br
*@version 1.0.
*@todo OpenGL interface and a C style array.
*/
      template <class Real> class Point2
      {

      public:

    	  friend class Vector2<Real>;

    	  /*! @name  Defining a Point2 */
    	  //@{
    	  /*! Default constructor. Value is set to (0,0,0). */
    	  Point2();
    	  /*! Standard constructor  with the x , y  values. */
    	  Point2 ( const Real& x, const Real& y );
    	  /*! Copy constructor. */
    	  Point2 ( const Point2<Real>& p);
    	  /*! Constructor by Vector 2. */
    	  Point2 ( const Vector2<Real>& v);
    	  /*! Constructor by a array of any type. */
    	  /*!@warning If the type is not a number, the construtor will store trash.*/
    	  template < class T >
    	  Point2 (const T* p);
    	  virtual ~Point2(){};

    	  void 					Set( const Real& x, const Real& y );
    	  //@}

  		  /*! @name Accessing the value */
  		  //@{

    	  /*! Conversion operator returning the memory address of the Point.

    	  Useful to pass as a parameter to OpenGL functions:
    	  \code
    	  Celer:Point2<float> pos, normal;
    	  glNormal2fv(normal);
    	  glVertex2fv(pos);
    	  \endcode */


    	  Real 					operator[]( unsigned int i)  const;

    	  Real& 				operator[]( unsigned int i);
    	  //@}

    	  /*! @name Algebraic computations */
    	  // @{
    	  /*! Unary plus operator. */
    	  Point2<Real>  	  	operator+() const;
    	  /*! Unary minus operator. */
    	  Point2<Real>  	  	operator-() const;

    	  /*! Adds the point by a scalar \p factor. */
    	  Point2<Real>& 		operator+=( const Real& factor );
    	  /*! Subtracts the point by a scalar \p factor. */
    	  Point2<Real>& 		operator-=( const Real& factor );
    	  /*! Multiply the vector by a scalar \p k. */
    	  Point2<Real>& 		operator*=( const Real& factor );

    	  /*! Divides the vector by a scalar \p k.

    	   @wanning A test is performed to avoide division by zero. */

    	  Point2<Real>& 		operator/=( const Real& factor );
    	  /*!@see operator+ */
    	  template <class T>
    	  friend Point2<T> 		operator+( const T& factor,const Point2<T>& p  ) ;
    	  /*!@see operator+ */
    	  template <class T>
    	  friend Point2<T> 		operator+( const Point2<T>& p, const T& factor ) ;
    	  /*!@see operator- */
    	  template <class T>
    	  friend Point2<T> 		operator-( const T& factor, const Point2<T>& p ) ;
    	  /*!@see operator- */
    	  template <class T>
    	  friend Point2<T>		operator-( const Point2<T>& p, const T& factor ) ;
    	  /*!@see operator* */
    	  template <class T>
    	  friend Point2<T> 		operator*( const T& factor, const Point2<T>& p ) ;
    	  /*!@see operator* */
    	  template <class T>
    	  friend Point2<T> 		operator*( const Point2<T>& p, const T& factor ) ;
    	  /*!@see operator/ */
    	  Point2<Real> 			operator/( const Real& factor ) const;

    	  /*! Assigment operator. */
    	  Point2<Real>& 		operator= ( const Point2<Real>& p );
    	  /*! Adds \p a to the point. */
    	  Point2<Real>& 		operator+=( const Point2<Real>& p );
    	  /*! Subtract \p a to the vector. */
    	  Point2<Real>& 		operator-=( const Point2<Real>& p );
    	  /*! Divide \p a to the point by atribute to atribute. */
    	  /*\code
        	  p1.x() / p2.x();
        	  p1.x() / p2.y();
        	  \endcode */
    	  Point2<Real>& 		operator/=( const Point2<Real>& p );
    	  /*!@see operator+= */
    	  Point2<Real> 			operator+ (const Point2<Real>& p ) const;
    	  /*! This operation return a vector that has direction ...*/
    	  template <class T>
    	  friend Vector2<T>  	operator- (const Point2<T>& p,const Point2<T>& q ) ;

    	  bool 					operator== ( const Point2<Real>& p) const;
    	  bool 					operator!= ( const Point2<Real>& p) const;

    	  template <class T>
    	  friend Point2<T> 		operator+ (const Point2<T>& p, const Vector2<T>& v) ;
    	  template <class T>
    	  friend Point2<T> 		operator+ (const Vector2<T>& v, const Point2<T>& p ) ;

    	  template <class T>
    	  friend Point2<T> 		operator-( const Point2<T>& p, const Vector2<T>& v ) ;
    	  template <class T>
    	  friend Point2<T> 		operator-( const Vector2<T>& v, const Point2<T>& p ) ;
    	  //@}
    	  /*! @name Output stream */
    	  //@{
    	  /*! Output stream operator. Enables debugging code like:
    	  \code
    	  Celer::Point2<> p(...);
    	  std::cout << "Point2" << " x = " << p.x() << " ,y = " << p.y() << std::endl;
    	  \endcode */
    	  template <class T>
    	  friend std::ostream& 	operator<< (std::ostream & s, const Point2<T>& p);
    	  //@}

    	  /*! @name Functions */
    	  //@{
    	  /*!  */
    	  Real 							SquaredDistance( const Point2<Real>& p ) ;
  		  const Real* 					ToRealPtr( void ) const;
  		  Real* 						ToRealPtr( void );

  		  operator const Real *( void ) const;

  		  operator Real * ( void );

		  //@}

      public:
    	  /*! @name Members  */
    	  //@{
    	  /*! An union. */
    	  /*!
    	   * Shareed memory with abscissas atributes and the XY Array.
    	   * The Struct has the same size of the \code xy[] \endcode.
    	   */
    	  Real x; /*!< X abscissa of space. */
    	  Real y; /*!< Y abscissa of space. */

    	  //@}

      };// End Interface

      //============================= LIFECYCLE ====================================

      template<class Real>
      Point2<Real>::Point2()
      {
      	this->x = static_cast< Real > (0);
      	this->y = static_cast< Real > (0);
      };


      template<class Real>
      Point2<Real>::Point2( const Real& x, const Real& y )
      : x(x), y(y) {};


      template<class Real>
      Point2<Real>::Point2( const Vector2<Real>& v )
      : x(v.x), y(v.y) {};

      template<class Real>
      Point2<Real>::Point2( const Point2<Real>& p )
      : x(p.x), y(p.y) {};


      template< class Real>
      template <class T >
      Point2<Real>::Point2 (const T* point)
      {
      	assert(point);

      	this->x = static_cast< Real > ( point[0] );
      	this->y = static_cast< Real > ( point[1] );

      };

      template<class Real>
      inline void Point2<Real>::Set( const Real& x, const Real& y )
      {
      	this->x = x;
      	this->y = y;
      };


      //============================= ACESS ======================================


      template<class Real>
      inline  Real Point2<Real>::operator[]( unsigned int index )  const
      {
      	assert( ( index >= 0 ) && ( index < 2 ) );

      	return ( ( &x )[ index ] );
      };

      template<class Real>
      inline  Real& Point2<Real>::operator[]( unsigned int index )

      {
      	assert( ( index >= 0 ) && ( index < 2 ) );

      	return ( ( &x )[ index ] );
      };

      //============================= OPERATORS ====================================


      template<class Real>
      inline Point2<Real> Point2<Real>::operator+()  const
      {
      	return ( Point2<Real> (this->x, this->y)  );
      };


      template<class Real>
      inline Point2<Real>  Point2<Real>::operator-() const
      {

      	return ( Point2<Real> (-this->x, -this->y) );

      };


      template<class Real>
      inline Point2< Real >& Point2<Real>::operator+=( const Real&  factor )
      {
      	this->x += factor;
      	this->y += factor;

      	return ( *this );
      }

      template<class Real>
      inline Point2< Real >& Point2<Real>::operator-=( const Real&  factor )
      {
      	this->x -= factor;
      	this->y -= factor;

      	return ( *this );
      }

      template<class Real>
      inline Point2< Real >& Point2<Real>::operator*=( const Real& factor )
      {

      	this->x  *= factor;
      	this->y  *= factor;

      	return ( *this );
      }

      template<class Real>
      inline Point2< Real >& Point2<Real>::operator/=( const Real& factor )
      {
      	Real f = (Real)1 / factor;

      	this->x *= f;
      	this->y *= f;

      	return ( *this );
      }


      template<class Real>
      inline Point2<Real> operator+( const Real& factor,const Point2<Real>& point )
      {
      	return ( Point2<Real> ( point.x + factor, point.y + factor ) );

      };

      template<class Real>
      inline Point2<Real> operator+( const Point2<Real>& point, const Real& factor )
      {
      	return ( Point2<Real> ( point.x + factor, point.y + factor ) );

      };


      template<class Real>
      inline  Point2<Real> operator-( const Real& factor,const Point2<Real>& p )
      {
      	return ( Point2<Real> ( factor - p.x , factor - p.y )
      	);

      };


      template<class Real>
      inline Point2<Real> operator-( const Point2<Real>& p, const Real& factor )
      {
      	return ( Point2<Real> ( p.x - factor, p.y - factor ) );

      };


      template<class Real>
      inline Point2<Real> operator*( const Real& factor,const Point2<Real>& p )
      {
      	return ( Point2<Real> ( p.x * factor, p.y * factor ) );

      };

      template<class Real>
      inline Point2<Real> operator*( const Point2<Real>& p, const Real& factor )
      {
      	return ( Point2<Real> ( p.x * factor, p.y * factor ) );

      };


      template<class Real>
      inline Point2<Real> Point2<Real>::operator/( const Real& factor )  const
      {
      	return ( Point2<Real> ( x / factor, y/ factor ) );

      };


      template<class Real>
      inline Point2<Real>& Point2<Real>::operator=( const Point2<Real>& p )
      {
      	this->x = p.x;
      	this->y = p.y;

      	return ( *this );
      };



      template<class Real>
      inline Point2< Real >& Point2<Real>::operator+=( const Point2<Real>& p )
      {
      	this->x += p.x;
      	this->y += p.y;

      	return ( *this );
      }

      template<class Real>
      inline Point2<Real>& Point2<Real>::operator-=( const Point2<Real>& p )
      {
      	this->x -= p.x;
      	this->y -= p.y;

      	return ( *this );
      }


      template<class Real>
      inline Point2<Real>& Point2<Real>::operator/=( const Point2<Real>& p )
      {

      	this->x /= p.x;
      	this->y /= p.y;

      	return ( *this );
      }

      template<class Real>
      inline bool Point2<Real>::operator==( const Point2<Real>& p ) const
      {
      	return ( ( this->x == p.x ) and ( this->y == p.y ) );
      };

      template<class Real>
      inline bool Point2<Real>::operator!=( const Point2<Real>& p ) const
      {
      	return  !(*this == p) ;
      };



      template<class Real>
      inline Vector2<Real> operator-( const Point2<Real>& source, const Point2<Real>& target )
      {
      	return ( Vector2<Real>( source.x - target.x,  source.y - target.y ) );

      };


      template<class Real>
      inline Point2<Real> operator-( const Point2<Real>& p, const Vector2<Real>& v )
      {
      	return ( Point2<Real>( p.x - v.x, p.y - v.y)  );
      };


      template<class Real>
      inline Point2<Real> operator-( const Vector2<Real>& v,const Point2<Real>& p )
      {

      	return ( Point2<Real>( v.x - p.x, v.y - p.y ) );

      };

      template<class Real>
      inline Point2<Real> Point2<Real>::operator+( const Point2<Real>& p )  const
      {
      	return ( Point2<Real>( x + p.x, y + p.y ) );

      };


      template<class Real>
      inline Point2<Real> operator+( const Point2<Real>& p, const Vector2<Real>& v )
      {
      	return ( Point2<Real> ( p.x + v.x, p.y + v.y ) );

      };

      template<class Real>
      inline Point2<Real> operator+( const Vector2<Real>& v,const Point2<Real>& p )
      {
      	return ( Point2<Real> ( p.x + v.x, p.y + v.y ) );

      };

      //============================= OPERATIONS ===================================


      template<class Real>
      inline std::ostream& operator<<( std::ostream & s, const Point2<Real>& point )
      {
      	s << "Point2" << " x = " << point.x << " ,y = " << std::endl;


      	return ( s );
      };

      template<class Real>
      Real Point2<Real>::SquaredDistance( const Point2<Real>& p )
      {
      	return (  (p.x - this->x ) * ( p.x - this->x ) +  ( p.y - this->y) * ( p.y - this->y) );
      }

      template<class Real>
      inline const Real* Point2<Real>::ToRealPtr( void ) const
      {
      	return &x;
      }

      template<class Real>
      inline Real* Point2<Real>::ToRealPtr( void )
      {
      	return &x;
      }

      template<class Real>
      inline Point2<Real>::operator const Real *( void ) const { return &x; }

      template<class Real>
      inline Point2<Real>::operator Real * ( void ) { return &x; }

} /* Celer :: NAMESPACE */

#endif /*POINT2_HPP_*/
