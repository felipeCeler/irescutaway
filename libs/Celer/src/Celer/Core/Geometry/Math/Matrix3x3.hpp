#ifndef CELER_MATRIX3X3_HPP_
#define CELER_MATRIX3X3_HPP_

#include <iostream>

#include <Celer/Core/Geometry/Math/Vector3.hpp>

namespace Celer {

/*! \brief The Matrix3x3 class .

*@class Matrix3x3.
*@brief Class that represent a dense 3x3 Matrix and Homogeneous Coordinate ..
*@details ...
*@author Felipe Moura.
*@email fmc@cos.ufrj.br
*@version 1.0.
*@todo OpenGL interface and a C style array.
*@todo Row or Column major ?

  \nosubgrouping */

	template<class Real> class Matrix3x3
	{
	private:

		Vector3<Real> m[ 3 ];


	public:

		friend class Vector3<Real>;
		/*! @name Defining a Matrix3x3 */
		//@{
		Matrix3x3();
		Matrix3x3( Real a11, Real a12, Real a13,Real a21, Real a22, Real a23,Real a31, Real a32, Real a33 );
		Matrix3x3( const Vector3<Real>& row1, const Vector3<Real>& row2, const Vector3<Real>& row3 );
		virtual ~Matrix3x3();
		Matrix3x3<Real>& 		operator=( const Matrix3x3<Real>& A);
		//@}
		// transpose
		Matrix3x3<Real> 		operator~() const;
		/*! @name Accessing the value */
		//@{
		const Vector3<Real>& 	operator[]( int rowIndex ) const;
		Vector3<Real>& 			operator[]( int rowIndex );

		Real 					operator()( int i, int j )  	const;
		Real& 					operator()( int i, int j );
		// Return Column
		Vector3<Real> 			Column( int index ) const;
		Vector3<Real> 			Row( int index ) const;
		//@}
		//
		/*! @name Algebraic computations */
		//@{
		Matrix3x3<Real>  		operator-() const;
		Matrix3x3<Real>  		operator+() const;

		template <class T>
		friend Matrix3x3<T> 	operator*( const T& factor, const Matrix3x3<T>& a );
		template <class T>
		friend Matrix3x3<T> 	operator*( const Matrix3x3<T>& a, const T& factor );
		template <class T>
		friend Matrix3x3<T> 	operator/( const Matrix3x3<T>& a, const T& factor );

		template <class T>
		friend Matrix3x3<T> 	operator+( const Matrix3x3<T>& a, const Matrix3x3<T>& b );
		template <class T>
		friend Matrix3x3<T> 	operator-( const Matrix3x3<T>& a, const Matrix3x3<T>& b );
		template <class T>
		friend Matrix3x3<T> 	operator*( const Matrix3x3<T>& a, const Matrix3x3<T>& b );

		template <class T>
		friend Vector3<T> 		operator*( const Matrix3x3<T>& a, const Vector3<T>& v );
		template <class T>
		friend Vector3<T>  		operator*( const Matrix3x3<T>& a, const Vector3<T>& p );
		//@}


		/*! @name Matrix Function  */
		//@{
		template <class T>
		friend  std::ostream& 	operator<< ( std::ostream & s, const Matrix3x3<T>& a );

		const Real* 			ToRealPtr( void ) const ;

		Real* 					ToRealPtr( void );

		bool 					IsSymetric();
		Matrix3x3<Real> 		Identity();

		//@}

	};

	template <class Real>
	Matrix3x3<Real>::Matrix3x3()
	{
		this->m[ 0 ].x = static_cast<Real>(1); this->m[ 0 ].y = static_cast<Real>(0); this->m[ 0 ].z = static_cast<Real>(0);
		this->m[ 1 ].x = static_cast<Real>(0); this->m[ 1 ].y = static_cast<Real>(1); this->m[ 1 ].z = static_cast<Real>(0);
		this->m[ 2 ].x = static_cast<Real>(0); this->m[ 2 ].y = static_cast<Real>(0); this->m[ 2 ].z = static_cast<Real>(1);
	};

	template <class Real>
	Matrix3x3<Real>::Matrix3x3( Real a11, Real a12, Real a13,Real a21, Real a22, Real a23,Real a31, Real a32, Real a33 )
	{
		this->m[ 0 ].x = a11; this->m[ 0 ].y = a12; this->m[ 0 ].z = a13;
		this->m[ 1 ].x = a21; this->m[ 1 ].y = a22; this->m[ 1 ].z = a23;
		this->m[ 2 ].x = a31; this->m[ 2 ].y = a32; this->m[ 2 ].z = a33;
	};

	template <class Real>
	Matrix3x3<Real>::Matrix3x3( const Vector3<Real>& row1, const Vector3<Real>& row2, const Vector3<Real>& row3 )
	{
		this->m[ 0 ].x = row1.x; this->m[ 0 ].y = row1.y; this->m[ 0 ].z = row1.z;
		this->m[ 1 ].x = row2.x; this->m[ 1 ].y = row2.y; this->m[ 1 ].z = row2.z;
		this->m[ 2 ].x = row3.x; this->m[ 2 ].y = row3.y; this->m[ 2 ].z = row3.z;
	};

	// transpose
	template <class Real>
	inline Matrix3x3<Real> Matrix3x3<Real>::operator~() const
	{
		return ( Matrix3x3<Real>( m[ 0 ].x,m[ 1 ].x,m[ 2 ].x,
								  m[ 0 ].y,m[ 1 ].y,m[ 2 ].y,
								  m[ 0 ].z,m[ 1 ].z,m[ 2 ].z )
		);

	};

	//----------------------------------------------------------------------------

	template <class Real>
	inline const Vector3<Real>& Matrix3x3<Real>::operator[] (int rowIndex) const
	{
		return m[ rowIndex ];
	}
	//----------------------------------------------------------------------------

	template <class Real>
	inline Vector3<Real>& Matrix3x3<Real>::operator[] (int rowIndex)
	{
		return m[ rowIndex ];
	}
	template <class Real>
	Real Matrix3x3<Real>::operator()( int i, int j ) const
	{

		if( i<0 || j<0 || i>=3 || j>=3 ){
			std::cerr << "[ERROR] Matrix operator()" << std::endl
			<< " out of the matrix size."
			<< std::endl;

		}

		return ( m[ i ][ j ] );

	}

	template <class Real>
	Real& Matrix3x3<Real>::operator()( int i, int j )
	{

		if( i<0 || j<0 || i>=3 || j>=3 ){
			std::cerr << "[ERROR] Matrix operator()" << std::endl
			<< " out of the matrix size."
			<< std::endl;

		}

		return ( m[ i ][ j ] );

	}


	// Return Colum
	template <class Real>
	Vector3<Real> Matrix3x3<Real>::Column( int i ) const
	{

		if( i<0 || i>=3 ){
			std::cerr << "[ERROR] Matrix operator()" << std::endl
			<< " out of the matrix size."
			<< std::endl;

		}

		return ( Vector3<Real>( m[ 0 ][ i ] , m[ 1 ][ i ] , m[ 2 ][ i ] ) );

	}

	template <class Real>
	Vector3<Real> Matrix3x3<Real>::Row( int i ) const
	{

		if( i<0 || i>=3 ){
			std::cerr << "[ERROR] Matrix operator()" << std::endl
			<< " out of the matrix size."
			<< std::endl;

		}

		return ( Vector3<Real>( m[ i ].x , m[ i ].y , m[ i ].z ) );

	}

	template <class Real>
	inline Matrix3x3<Real>& Matrix3x3<Real>::operator=( const Matrix3x3<Real>& a )
	{

		this->m[0].x = a[ 0 ].x; this->m[0].y = a[ 0 ].y; this->m[0].z = a[ 0 ].z;
		this->m[1].x = a[ 1 ].x; this->m[1].y = a[ 1 ].y; this->m[1].z = a[ 1 ].z;
		this->m[2].x = a[ 2 ].x; this->m[2].y = a[ 2 ].y; this->m[2].z = a[ 2 ].z;

		return ( *this );
	};

	// FRIEND FUNCRealIONS

	template <class Real>
	inline Matrix3x3<Real>  Matrix3x3<Real>::operator-() const
	{

		return ( Matrix3x3<Real>
		( -m[ 0 ].x,-m[ 0 ].y,-m[ 0 ].z,
		  -m[ 1 ].x,-m[ 1 ].y,-m[ 1 ].z,
		  -m[ 2 ].x,-m[ 2 ].y,-m[ 2 ].z )
		);

	};

	template <class Real>
	inline Matrix3x3<Real>  Matrix3x3<Real>::operator+() const
	{
		return ( Matrix3x3<Real>
		( m[ 0 ].x,m[ 0 ].y,m[ 0 ].z,
		  m[ 1 ].x,m[ 1 ].y,m[ 1 ].z,
		  m[ 2 ].x,m[ 2 ].y,m[ 2 ].z )
		);
	};

	template <class Real>
	inline Matrix3x3<Real> operator+( const Matrix3x3<Real>& a, const Matrix3x3<Real>& b )
	{

		return ( Matrix3x3<Real>
		( a[ 0 ].x + b[ 0 ].x, a[ 0 ].y + b[ 0 ].y, a[ 0 ].z + b[ 0 ].z,
		  a[ 1 ].x + b[ 1 ].x, a[ 1 ].y + b[ 1 ].y, a[ 1 ].z + b[ 1 ].z,
		  a[ 2 ].x + b[ 2 ].x, a[ 2 ].y + b[ 2 ].y, a[ 2 ].z + b[ 2 ].z )
		);
	};

	template <class Real>
	inline Matrix3x3<Real> operator-( const Matrix3x3<Real>& a, const Matrix3x3<Real>& b )
	{

		return ( Matrix3x3<Real>
		( a[ 0 ].x - b.m[0].x, a[ 0 ].y - b.m[0].y, a[ 0 ].z - b.m[0].z,
		  a[ 1 ].x - b.m[1].x, a[ 1 ].y - b.m[1].y, a[ 1 ].z - b.m[1].z,
		  a[ 2 ].x - b.m[2].x, a[ 2 ].y - b.m[2].y, a[ 2 ].z - b.m[2].z )
		);
	};

	template <class Real>
	inline Matrix3x3<Real> operator*( const Real& factor, const Matrix3x3<Real>& a )
	{

		return ( Matrix3x3<Real>
		( a[ 0 ].x * factor, a[ 0 ].y * factor, a[ 0 ].z * factor,
		  a[ 1 ].x * factor, a[ 1 ].y * factor, a[ 1 ].z * factor,
		  a[ 2 ].x * factor, a[ 2 ].y * factor, a[ 2 ].z * factor )
		);

	};

	template <class Real>
	inline Matrix3x3<Real> operator*( const Matrix3x3<Real>& a, const Real& factor )
	{

		return ( Matrix3x3<Real>
		( a[ 0 ].x * factor, a[ 0 ].y * factor, a[ 0 ].z * factor,
		  a[ 1 ].x * factor, a[ 1 ].y * factor, a[ 1 ].z * factor,
		  a[ 2 ].x * factor, a[ 2 ].y * factor, a[ 2 ].z * factor )
		);

	};

	template <class Real>
	inline Matrix3x3<Real> operator/ ( const Matrix3x3<Real>& a, const Real& factor)
	{
		Matrix3x3<Real> b;

		if( factor == 0 ){
			std::cerr << "[ERROR]" << std::endl
			<< " division by zero."
			<< std::endl;

		}

		Real d = 1 / factor;

		return ( Matrix3x3<Real>
		( a[ 0 ].x * d, a[ 0 ].y * d, a[ 0 ].z * d,
		  a[ 1 ].x * d, a[ 1 ].y * d, a[ 1 ].z * d,
		  a[ 2 ].x * d, a[ 2 ].y * d, a[ 2 ].z * d )
		);
	};


	template <class Real>
	inline Matrix3x3<Real> operator* ( const Matrix3x3<Real>& a, const Matrix3x3<Real>& b)
	{
		return ( Matrix3x3<Real>
		( a[ 0 ].x * b[ 0 ].x + a[ 0 ].y * b[ 1 ].x + a[ 0 ].z * b[ 2 ].x,
		  a[ 0 ].x * b[ 0 ].y + a[ 0 ].y * b[ 1 ].y + a[ 0 ].z * b[ 2 ].y,
		  a[ 0 ].x * b[ 0 ].z + a[ 0 ].y * b[ 1 ].z + a[ 0 ].z * b[ 2 ].z,

		  a[ 1 ].x * b[ 0 ].x + a[ 1 ].y * b[ 1 ].x + a[ 1 ].z * b[ 2 ].x,
		  a[ 1 ].x * b[ 0 ].y + a[ 1 ].y * b[ 1 ].y + a[ 1 ].z * b[ 2 ].y,
		  a[ 1 ].x * b[ 0 ].z + a[ 1 ].y * b[ 1 ].z + a[ 1 ].z * b[ 2 ].z,

		  a[ 2 ].x * b[ 0 ].x + a[ 2 ].y * b[ 1 ].x + a[ 2 ].z * b[ 2 ].x,
		  a[ 2 ].x * b[ 0 ].y + a[ 2 ].y * b[ 1 ].y + a[ 2 ].z * b[ 2 ].y,
		  a[ 2 ].x * b[ 0 ].z + a[ 2 ].y * b[ 1 ].z + a[ 2 ].z * b[ 2 ].z )
		);

	};

	template <class Real>
	inline Vector3<Real> operator* ( const Matrix3x3<Real>& a, const Vector3<Real>& p)
	{
		return ( Vector3<Real>
		( a[ 0 ].x * p.x + a[ 0 ].y * p.y + a[ 0 ].z * p.z,
	      a[ 1 ].x * p.x + a[ 1 ].y * p.y + a[ 1 ].z * p.z,
	      a[ 2 ].x * p.x + a[ 2 ].y * p.y + a[ 2 ].z * p.z )
		);

	};

	template <class Real>
	inline std::ostream& operator<< (std::ostream & s, const Matrix3x3<Real>& a)
	{
		s << "Matrix 3x3" << std::endl
		<< " a11 = " << a[ 0 ].x << " ,a12 = " << a[ 0 ].y << " ,a13 = " << a[ 0 ].z << std::endl
		<< " a21 = " << a[ 1 ].x << " ,a22 = " << a[ 1 ].y << " ,a23 = " << a[ 1 ].z << std::endl
		<< " a31 = " << a[ 2 ].x << " ,a32 = " << a[ 2 ].y << " ,a33 = " << a[ 2 ].z << std::endl;

		return ( s );
	};

	template <class Real>
	const Real* Matrix3x3<Real>::ToRealPtr( void ) const
	{
		return m[0].ToFloatPtr();
	}

	template <class Real>
	Real* Matrix3x3<Real>::ToRealPtr( void )
	{
		return m[0].ToRealPtr();
	}

	template <class Real>
	bool Matrix3x3<Real>::IsSymetric ()
	{
		if ( (m[0].y == m[1].x) and
				(m[0].z == m[2].x) and
				(m[2].y == m[1].z) )
		{
			return true;
		}

		return false;
	}

	template <class Real>
	Matrix3x3<Real> Matrix3x3<Real>::Identity ()
	{
		return Matrix3x3<Real>(1.0,0.0,0.0,
				0.0,1.0,0.0,
				0.0,0.0,1.0);
	}
	template <class Real>
	Matrix3x3<Real>::~Matrix3x3(){};




}/* Celer :: NAMESPACE */

#endif /*MATRIX3X3_HPP_*/

// Tentar fazer operator [][] , ver geometric Tools Wm4::Math.h
//inline operator const Real* () const
//{
//return a;
//}
//----------------------------------------------------------------------------

//inline operator Real* ()
//{
//return a;
//}
