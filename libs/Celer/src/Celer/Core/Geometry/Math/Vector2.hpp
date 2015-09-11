#ifndef CELER_VECTOR2_HPP_
#define CELER_VECTOR2_HPP_

#include <iostream>

#include <cassert>
#include <cmath>

namespace Celer
{

	/*!
	 *@class Vector2.
	 *@brief Class that represent a Vector in 2D.
	 *@details ...
	 *@author Felipe Moura.
	 *@email fmc@cos.ufrj.br
	 *@version 1.0.
	 *@date 25-Feb-2008.
	 *@todo OpenGL interface and a C style array.
	 */

	template < class Real > class Vector2
	{

		public:
			/*! @name Members  */
			//@{
			/*! An union. */
			/*!
			 * Shareed memory with abscissas atributes and the XY Array.
			 * The Struct has the same size of the \code xy[] \endcode.
			 */
			union
			{
					struct
					{
							Real x; /*!< X abscissa of space. */
							Real y; /*!< Y abscissa of space. */
					};
					Real array[2];
			};

			//@}

		public:

			// VETOR COLUNA OU VETOR LINHA ??
			/*! @name  Defining a Point2 */
			//@{
			/*! Default constructor. Value is set to (0,0,0). */
			Vector2 ( );

			template < class T >
			Vector2 ( const T* v );

			Vector2 ( const Vector2<Real>& v );

			Vector2 ( const Real& x , const Real& y );
			virtual ~Vector2 ( );
			void Set ( const Real& x , const Real& y );
			//@}
			//Operator

			/*! @name Accessing the value */
			//@{

			/*! Conversion operator returning the memory address of the Point.

			 Useful to pass as a parameter to OpenGL functions:
			 \code
			 Celer:Point2<float> pos, normal;
			 glNormal2fv(normal);
			 glVertex2fv(pos);
			 \endcode */

			Real operator[] ( unsigned int index ) const;
			Real& operator[] ( unsigned int index );
			//@}

			// Assignment Opertators
			//With Scalar
			/*! @name Algebraic computations */
			// @{
			Vector2<Real> operator+ ( ) const;
			Vector2<Real> operator- ( ) const;

			Vector2<Real>& operator+= ( const Real& factor );
			Vector2<Real>& operator-= ( const Real& factor );
			Vector2<Real>& operator*= ( const Real& factor );
			Vector2<Real>& operator/= ( const Real& factor );

			template < class T >
			friend Vector2<T> operator* ( const Vector2<T>& v , const T& factor );
			template < class T >
			friend Vector2<T> operator* ( const T& factor , const Vector2<T>& v );
			template < class T >
			friend Vector2<T> operator/ ( const Vector2<T>& v , const T& factor );
			template < class T >
			friend Vector2<T> operator+ ( const T& factor , const Vector2<T>& v );
			template < class T >
			friend Vector2<T> operator+ ( const Vector2<T>& v , const T& factor );
			template < class T >
			friend Vector2<T> operator- ( const T& factor , const Vector2<T>& v );
			template < class T >
			friend Vector2<T> operator- ( const Vector2<T>& v , const T& factor );

			// With Vector
			Vector2<Real>& operator= ( const Vector2<Real>& v );
			Vector2<Real>& operator+= ( const Vector2<Real>& v );
			Vector2<Real>& operator-= ( const Vector2<Real>& v );
			Vector2<Real>& operator/= ( const Vector2<Real>& v );

			bool operator== ( const Vector2<Real>& v ) const;
			bool operator!= ( const Vector2<Real>& v ) const;

			Vector2<Real> operator- ( const Vector2<Real>& v ) const;
			Vector2<Real> operator+ ( const Vector2<Real>& v ) const;

			Real operator* ( const Vector2<Real>& v ) const;
			//@}

			//@}
			/*! @name Output stream */
			//@{
			/*! Output stream operator. Enables debugging code like:
			 \code
			 Celer::Vector2<> p(...);
			 std::cout << "Vector2" << " x = " << v.x << " ,y = " << v.y << std::endl;
			 \endcode */
			template < class T >
			friend std::ostream& operator<< ( std::ostream & s , const Vector2<T>& v );
			//@}

			/*! @name Functions */
			//@{
			/*!  */
			Real Length ( void );
			void Normalize ( void );
			Vector2<Real> Norm ( void );
			const Real* ToRealPtr ( void ) const;
			Real* ToRealPtr ( void );

			operator const Real * ( void ) const;

			operator Real * ( void );
			//@}


	};

	template < class Real >
	Vector2<Real>::Vector2 ( )
	{
		this->x = static_cast<Real> ( 0 );
		this->y = static_cast<Real> ( 0 );
	}
	;

	template < class Real >
	template < class T >
	Vector2<Real>::Vector2 ( const T* v )
	{
		assert ( v );

		this->x = static_cast<Real> ( v[0] );
		this->y = static_cast<Real> ( v[1] );

	}
	;

	template < class Real >
	Vector2<Real>::Vector2 ( const Vector2<Real>& v )
	{
		this->x = v.x;
		this->y = v.y;
	}
	;

	template < class Real >
	Vector2<Real>::Vector2 ( const Real& x , const Real& y )
	{
		this->x = x;
		this->y = y;
	}
	;

	template < class Real >
	inline void Vector2<Real>::Set ( const Real& x , const Real& y )
	{
		this->x = x;
		this->y = y;
	}
	;
	//Operator

	template < class Real >
	inline Real Vector2<Real>::operator[] ( unsigned int index ) const
	{
		assert ( ( index >= 0 ) && ( index < 2 ) );

		return ( ( &x )[index] );

	}
	;

	template < class Real >
	inline Real& Vector2<Real>::operator [] ( unsigned int index )
	{
		assert ( ( index >= 0 ) && ( index < 2 ) );

		return ( ( &x )[index] );
	}
	;

	// Assignment Opertators
	//With Scalar

	template < class Real >
	inline Vector2<Real> Vector2<Real>::operator+ ( ) const
	{
		return ( Vector2<Real> ( this->x , this->y ) );
	}
	;

	template < class Real >
	inline Vector2<Real> Vector2<Real>::operator- ( ) const
	{

		return ( Vector2<Real> ( -this->x , -this->y ) );

	}
	;

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator+= ( const Real& factor )
	{
		this->x += factor;
		this->y += factor;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator-= ( const Real& factor )
	{
		this->x -= factor;
		this->y -= factor;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator*= ( const Real& factor )
	{

		this->x *= factor;
		this->y *= factor;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator/= ( const Real& factor )
	{
		Real f = (Real) 1 / factor;

		this->x *= f;
		this->y *= f;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real> operator* ( const Vector2<Real>& v , const Real& factor )
	{

		return ( Vector2<Real> ( v.x * factor , v.y * factor ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator* ( const Real& factor , const Vector2<Real>& v )
	{
		return ( Vector2<Real> ( v.x * factor , v.y * factor ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator/ ( const Vector2<Real>& v , const Real& factor )
	{

		return ( Vector2<Real> ( v.x / factor , v.y / factor ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator+ ( const Real& factor , const Vector2<Real>& v )
	{
		return ( Vector2<Real> ( v.x + factor , v.y + factor ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator+ ( const Vector2<Real>& v , const Real& factor )
	{
		return ( Vector2<Real> ( v.x + factor , v.y + factor ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator- ( const Real& factor , const Vector2<Real>& v )
	{
		return ( Vector2<Real> ( factor - v.x , factor - v.y ) );

	}
	;

	template < class Real >
	inline Vector2<Real> operator- ( const Vector2<Real>& v , const Real& factor )
	{
		return ( Vector2<Real> ( v.x - factor , v.y - factor ) );

	}
	;

	// With Vector

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator= ( const Vector2<Real>& v )
	{
		this->x = v.x;
		this->y = v.y;

		return ( *this );
	}
	;

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator+= ( const Vector2<Real>& v )
	{
		this->x += v.x;
		this->y += v.y;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator-= ( const Vector2<Real>& v )
	{
		this->x -= v.x;
		this->y -= v.y;

		return ( *this );
	}

	template < class Real >
	inline Vector2<Real>& Vector2<Real>::operator/= ( const Vector2<Real>& v )
	{

		this->x /= v.x;
		this->y /= v.y;

		return ( *this );
	}

	template < class Real >
	inline bool Vector2<Real>::operator== ( const Vector2<Real>& v ) const
	{
		return ( ( this->x == v.x ) && ( this->y == v.y ) );
	}
	;

	template < class Real >
	inline bool Vector2<Real>::operator!= ( const Vector2<Real>& v ) const
	{
		return ! ( *this == v );
	}
	;

	template < class Real >
	inline Vector2<Real> Vector2<Real>::operator- ( const Vector2<Real>& v ) const
	{

		return ( Vector2 ( this->x - v.x , this->y - v.y ) );
	}
	;

	template < class Real >
	inline Vector2<Real> Vector2<Real>::operator+ ( const Vector2<Real>& v ) const
	{

		return ( Vector2 ( this->x + v.x , this->y + v.y ) );

	}
	;

	template < class Real >
	inline Real Vector2<Real>::operator* ( const Vector2<Real>& v ) const
	{

		return ( ( v.x * x ) + ( v.y * y ) );

	}
	;

	template < class Real >
	inline std::ostream& operator<< ( std::ostream & s , const Vector2<Real>& v )
	{
		s << "Vector2" << " x = " << v.x ( ) << " ,y = " << v.y ( ) << std::endl;

		return s;
	}
	;

	// AUXILIAR FUNCTIONS
	template < class Real >
	inline Real Vector2<Real>::Length ( void )
	{
		return sqrt ( ( this->x * this->x ) + ( this->y * this->y ) );
	}
	;

	template < class Real >
	inline void Vector2<Real>::Normalize ( void )
	{
		Real factor = sqrt ( ( x * x ) + ( y * y ) );

		assert ( factor );

		Real lFactor = 1 / factor;

		x *= lFactor;
		y *= lFactor;

	}
	;

	template < class Real >
	inline Vector2<Real> Vector2<Real>::Norm ( void )
	{
		Real factor = sqrt ( ( x * x ) + ( y * y ) );

		assert ( factor );

		Real f = 1 / factor;

		return ( Vector2 ( x * f , y * f ) );

	}
	;

	template < class Real >
	inline const Real* Vector2<Real>::ToRealPtr ( void ) const
	{
		return &x;
	}

	template < class Real >
	inline Real* Vector2<Real>::ToRealPtr ( void )
	{
		return &x;
	}

	template < class Real >
	inline Vector2<Real>::operator const Real * ( void ) const
	{
		return &x;
	}

	template < class Real >
	inline Vector2<Real>::operator Real * ( void )
	{
		return &x;
	}

	template < class Real >
	Vector2<Real>::~Vector2 ( )
	{
	}
	;

} /* Celer :: NAMESPACE */

#endif /*VECTOR2_HPP_*/

// CASOS DE TESTE


/* Teste operator+ e operator-
 Celer::Vector2<double> v1(1.0,1.0,1.0);
 Celer::Vector2<double> v2(-1.0,-1.0,-1.0);
 Celer::Vector2<double> v3(0.0,0.0,0.0);

 v3 = v1 + v2;
 std::cout << v3.x << v3.y << v3.z << "#v1+v2#"<<std::endl;
 v3 = v1 - v2;
 std::cout << v3.x << v3.y << v3.z << "#v1-v2#"<<std::endl;
 v3 = -v1 - v2;
 std::cout << v3.x << v3.y << v3.z << "#-v1-v2#"<<std::endl;
 v3 = -v1 + v2;
 std::cout << v3.x << v3.y << v3.z << "#-v1+v2#"<<std::endl;
 v3 = + v1 - v2;
 std::cout << v3.x << v3.y << v3.z << "#+v1-v2#"<<std::endl;
 v3 = + v1 + v2;
 std::cout << v3.x << v3.y << v3.z << "#+v1+v2#"<<std::endl;
 */

/*Teste operator*
 Celer::Vector2<double> v1(1.0,1.0,1.0);
 Celer::Vector2<double> v3(0.0,0.0,0.0);

 v3 = 3.0 * v1;
 std::cout << v3.x << v3.y << v3.z << "#3*v1#"<<std::endl;
 v3 = 3.0 * (-v1);
 std::cout << v3.x << v3.y << v3.z << "#3*(-v1)#"<<std::endl;
 v3 = 3.0 * (+v1);
 std::cout << v3.x << v3.y << v3.z << "#3*(+v1)#"<<std::endl;
 v3 = (-v1) * 3.0;
 std::cout << v3.x << v3.y << v3.z << "#(-v1)*3#"<<std::endl;
 v3 = (+v1) * 3.0;
 std::cout << v3.x << v3.y << v3.z << "#(+v1)*3#"<<std::endl;
 v3 = (+v1) * 0.0;
 std::cout << v3.x << v3.y << v3.z << "#(+v1)*0.0#"<<std::endl;
 v3 = (+v1) * v2[1];
 std::cout << v3.x << v3.y << v3.z << "#(+v1)*0.0#"<<std::endl;
 */

/* Teste operator* (puduto interno ou  produto escalar)
 Celer::Vector2<double> v1(6.0,-1.0,0.0);
 Celer::Vector2<double> v2(0.5,-4.0,0.0);
 double t = 0.0 ;
 t = v1 * v2
 std::cout << t << "#(v1)*(v2)#"<<std::endl;
 v1[1] = v1*(-v2)
 */

/*
 * solucão para não ter warnning no retorno de uma referencia
 Vector2<T>&  operator- ( const Vector2<T>& u) const
 {
 Vector2<T> w;

 w.x = this->x - u.x;
 w.y = this->y - u.y;
 w.z = this->z - u.z;

 Vector2<T>& h = w;

 return ( h );
 };
 */

