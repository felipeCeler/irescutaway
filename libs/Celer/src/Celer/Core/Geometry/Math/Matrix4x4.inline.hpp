/*
 * Matrix4x4.inline.hpp
 *
 *  Created on: Nov 7, 2012
 *      Author: felipe
 */

#ifndef MATRIX4X4_INLINE_HPP_
#define MATRIX4X4_INLINE_HPP_

namespace Celer
{

	/*! @name Defining a Matrix4x4 */
	//@{
	/*! Default constructor. All value is set to 0. */
	template < class Real >
	Matrix4x4<Real>::Matrix4x4 ( )
	{
		m[0].x = static_cast<Real> ( 1.0 );
		m[0].y = static_cast<Real> ( 0.0 );
		m[0].z = static_cast<Real> ( 0.0 );
		m[0].w = static_cast<Real> ( 0.0 );

		m[1].x = static_cast<Real> ( 0.0 );
		m[1].y = static_cast<Real> ( 1.0 );
		m[1].z = static_cast<Real> ( 0.0 );
		m[1].w = static_cast<Real> ( 0.0 );

		m[2].x = static_cast<Real> ( 0.0 );
		m[2].y = static_cast<Real> ( 0.0 );
		m[2].z = static_cast<Real> ( 1.0 );
		m[2].w = static_cast<Real> ( 0.0 );

		m[3].x = static_cast<Real> ( 0.0 );
		m[3].y = static_cast<Real> ( 0.0 );
		m[3].z = static_cast<Real> ( 0.0 );
		m[3].w = static_cast<Real> ( 1.0 );
	}

	template < class Real >
	Matrix4x4<Real>::Matrix4x4 ( Real other00,
				     Real other01,
				     Real other02,
				     Real other03,
				     Real other10,
				     Real other11,
				     Real other12,
				     Real other13,
				     Real other20,
				     Real other21,
				     Real other22,
				     Real other23,
				     Real other30,
				     Real other31,
				     Real other32,
				     Real other33)
	{
		m[0].x = static_cast<Real> ( other00 );
		m[0].y = static_cast<Real> ( other01 );
		m[0].z = static_cast<Real> ( other02 );
		m[0].w = static_cast<Real> ( other03 );

		m[1].x = static_cast<Real> ( other10 );
		m[1].y = static_cast<Real> ( other11 );
		m[1].z = static_cast<Real> ( other12 );
		m[1].w = static_cast<Real> ( other13 );

		m[2].x = static_cast<Real> ( other20 );
		m[2].y = static_cast<Real> ( other21 );
		m[2].z = static_cast<Real> ( other22 );
		m[2].w = static_cast<Real> ( other23 );

		m[3].x = static_cast<Real> ( other30 );
		m[3].y = static_cast<Real> ( other31 );
		m[3].z = static_cast<Real> ( other32 );
		m[3].w = static_cast<Real> ( other33 );
	}

	template < class Real >
	Matrix4x4<Real>::Matrix4x4 ( const Matrix4x4<Real>& other )
	{
		m[0].x = other[0].x;
		m[0].y = other[0].y;
		m[0].z = other[0].z;
		m[0].w = other[0].w;

		m[1].x = other[1].x;
		m[1].y = other[1].y;
		m[1].z = other[1].z;
		m[1].w = other[1].w;

		m[2].x = other[2].x;
		m[2].y = other[2].y;
		m[2].z = other[2].z;
		m[2].w = other[2].w;

		m[3].x = other[3].x;
		m[3].y = other[3].y;
		m[3].z = other[3].z;
		m[3].w = other[3].w;

	}

	template < class Real >
	Matrix4x4<Real>::Matrix4x4 ( const Real other[4][4] )
	{
		m[0].x = other[0][0];
		m[0].y = other[0][1];
		m[0].z = other[0][2];
		m[0].w = other[0][3];

		m[1].x = other[1][0];
		m[1].y = other[1][1];
		m[1].z = other[1][2];
		m[1].w = other[1][3];

		m[2].x = other[2][0];
		m[2].y = other[2][1];
		m[2].z = other[2][2];
		m[2].w = other[2][3];

		m[3].x = other[3][0];
		m[3].y = other[3][1];
		m[3].z = other[3][2];
		m[3].w = other[3][3];

	}

	template < class Real >
	template < class Type >
	Matrix4x4<Real>::Matrix4x4 ( const Matrix4x4<Type>& other )
	{
		m[0].x = static_cast<Real> ( other[0].x );
		m[0].y = static_cast<Real> ( other[0].y );
		m[0].z = static_cast<Real> ( other[0].z );
		m[0].w = static_cast<Real> ( other[0].w );

		m[1].x = static_cast<Real> ( other[1].x );
		m[1].y = static_cast<Real> ( other[1].y );
		m[1].z = static_cast<Real> ( other[1].z );
		m[1].w = static_cast<Real> ( other[1].w );

		m[2].x = static_cast<Real> ( other[2].x );
		m[2].y = static_cast<Real> ( other[2].y );
		m[2].z = static_cast<Real> ( other[2].z );
		m[2].w = static_cast<Real> ( other[2].w );

		m[3].x = static_cast<Real> ( other[3].x );
		m[3].y = static_cast<Real> ( other[3].y );
		m[3].z = static_cast<Real> ( other[3].z );
		m[3].w = static_cast<Real> ( other[3].w );
	}

	template < class Real >
	template < class Type >
	Matrix4x4<Real>::Matrix4x4 ( const Type other[4][4] )
	{
		m[0].x = static_cast<Real> ( other[0][0] );
		m[0].y = static_cast<Real> ( other[0][1] );
		m[0].z = static_cast<Real> ( other[0][2] );
		m[0].w = static_cast<Real> ( other[0][3] );

		m[1].x = static_cast<Real> ( other[1][0] );
		m[1].y = static_cast<Real> ( other[1][1] );
		m[1].z = static_cast<Real> ( other[1][2] );
		m[1].w = static_cast<Real> ( other[1][3] );

		m[2].x = static_cast<Real> ( other[2][0] );
		m[2].y = static_cast<Real> ( other[2][1] );
		m[2].z = static_cast<Real> ( other[2][2] );
		m[2].w = static_cast<Real> ( other[2][3] );

		m[3].x = static_cast<Real> ( other[3][0] );
		m[3].y = static_cast<Real> ( other[3][1] );
		m[3].z = static_cast<Real> ( other[3][2] );
		m[3].w = static_cast<Real> ( other[3][3] );

	}


	template < class Real >
	Matrix4x4<Real>::Matrix4x4 (	const Vector4<Real>& firstRow ,
					const Vector4<Real>& secondRow ,
					const Vector4<Real>& thirdRow ,
					const Vector4<Real>& fourthRow )
	{
		m[0].x = firstRow.x;
		m[0].y = firstRow.y;
		m[0].z = firstRow.z;
		m[0].w = firstRow.w;

		m[1].x = secondRow.x;
		m[1].y = secondRow.y;
		m[1].z = secondRow.z;
		m[1].w = secondRow.w;

		m[2].x = thirdRow.x;
		m[2].y = thirdRow.y;
		m[2].z = thirdRow.z;
		m[2].w = thirdRow.w;

		m[3].x = fourthRow.x;
		m[3].y = fourthRow.y;
		m[3].z = fourthRow.z;
		m[3].w = fourthRow.w;
	}

	template < class Real >
	Matrix4x4<Real>::Matrix4x4 (	const Vector3<Real>& firstRow ,
					const Vector3<Real>& secondRow ,
					const Vector3<Real>& thirdRow )
	{
		m[0].x = firstRow.x;
		m[0].y = firstRow.y;
		m[0].z = firstRow.z;
		m[0].w = static_cast<Real> ( 0 );

		m[1].x = secondRow.x;
		m[1].y = secondRow.y;
		m[1].z = secondRow.z;
		m[1].w = static_cast<Real> ( 0 );

		m[2].x = thirdRow.x;
		m[2].y = thirdRow.y;
		m[2].z = thirdRow.z;
		m[2].w = static_cast<Real> ( 0 );

		m[3].x = static_cast<Real> ( 0 );
		m[3].y = static_cast<Real> ( 0 );
		m[3].z = static_cast<Real> ( 0 );
		m[3].w = static_cast<Real> ( 1 );
	}
	//@}

	// transpose
	template < class Real >
	inline Matrix4x4<Real> Matrix4x4<Real>::operator~ ( ) const
	{
		Matrix4x4 matrix;

		matrix[0].x = m[0].x;
		matrix[0].y = m[1].x;
		matrix[0].z = m[2].x;
		matrix[0].w = m[3].x;

		matrix[1].x = m[0].y;
		matrix[1].y = m[1].y;
		matrix[1].z = m[2].y;
		matrix[1].w = m[3].y;

		matrix[2].x = m[0].z;
		matrix[2].y = m[1].z;
		matrix[2].z = m[2].z;
		matrix[2].w = m[3].z;

		matrix[3].x = m[0].w;
		matrix[3].y = m[1].w;
		matrix[3].z = m[2].w;
		matrix[3].w = m[3].w;

		return matrix;

	}

	//----------------------------------------------------------------------------
	template < class Real >
	inline const Vector4<Real>& Matrix4x4<Real>::operator[] ( int rowIndex ) const
	{
		return m[rowIndex];
	}

	template < class Real >
	inline Vector4<Real>& Matrix4x4<Real>::operator[] ( int rowIndex )
	{
		return m[rowIndex];
	}

	template < class Real >
	Real Matrix4x4<Real>::operator() ( int i , int j ) const
	{

		if ( i < 0 || j < 0 || i >= 4 || j >= 4 )
		{
			std::cerr << "[ERROR] Matrix operator()" << std::endl << " out of the matrix size." << std::endl;

		}

		return ( m[i][j] );

	}

	template < class Real >
	Real& Matrix4x4<Real>::operator() ( int i , int j )
	{

		if ( i < 0 || j < 0 || i >= 4 || j >= 4 )
		{
			std::cerr << "[ERROR] Matrix operator()" << std::endl << " out of the matrix size." << std::endl;

		}

		return ( m[i][j] );

	}

	// Return Colum
	template < class Real >
	Vector4<Real> Matrix4x4<Real>::column ( int i ) const
	{

		if ( i < 0 || i >= 4 )
		{
			std::cerr << "[ERROR] Matrix operator()" << std::endl << " out of the matrix size." << std::endl;

		}

		return ( Vector4<Real> ( m[0][i] , m[1][i] , m[2][i] , m[3][i] ) );

	}

	template < class Real >
	Vector4<Real> Matrix4x4<Real>::row ( int i ) const
	{

		if ( i < 0 || i >= 4 )
		{
			std::cerr << "[ERROR] Matrix operator()" << std::endl << " out of the matrix size." << std::endl;

		}

		return ( Vector4<Real> ( m[i].x , m[i].y , m[i].z , m[i].w ) );

	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator= ( const Matrix3x3<Real>& other )
	{

		m[0].x = other[0].x;
		m[0].y = other[0].y;
		m[0].z = other[0].z;
		m[0].w = static_cast<Real> ( 0 );

		m[1].x = other[1].x;
		m[1].y = other[1].y;
		m[1].z = other[1].z;
		m[1].w = static_cast<Real> ( 0 );

		m[2].x = other[2].x;
		m[2].y = other[2].y;
		m[2].z = other[2].z;
		m[2].w = static_cast<Real> ( 0 );

		m[3].x = static_cast<Real> ( 0 );
		m[3].y = static_cast<Real> ( 0 );
		m[3].z = static_cast<Real> ( 0 );
		m[3].w = static_cast<Real> ( 1 );

		return ( *this );
	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator= ( const Matrix4x4<Real>& other )
	{

		m[0].x = other[0].x;
		m[0].y = other[0].y;
		m[0].z = other[0].z;
		m[0].w = other[0].w;

		m[1].x = other[1].x;
		m[1].y = other[1].y;
		m[1].z = other[1].z;
		m[1].w = other[1].w;

		m[2].x = other[2].x;
		m[2].y = other[2].y;
		m[2].z = other[2].z;
		m[2].w = other[2].w;

		m[3].x = other[3].x;
		m[3].y = other[3].y;
		m[3].z = other[3].z;
		m[3].w = other[3].w;

		return ( *this );
	}

	// FRIEND FUNCRealIONS

	template < class Real >
	inline Matrix4x4<Real> Matrix4x4<Real>::operator- ( ) const
	{

		Matrix4x4 matrix;

		matrix[0].x = -m[0].x;
		matrix[0].y = -m[0].y;
		matrix[0].z = -m[0].z;
		matrix[0].w = -m[0].w;

		matrix[1].x = -m[1].x;
		matrix[1].y = -m[1].y;
		matrix[1].z = -m[1].z;
		matrix[1].w = -m[1].w;

		matrix[2].x = -m[2].x;
		matrix[2].y = -m[2].y;
		matrix[2].z = -m[2].z;
		matrix[2].w = -m[2].w;

		matrix[3].x = -m[3].x;
		matrix[3].y = -m[3].y;
		matrix[3].z = -m[3].z;
		matrix[3].w = -m[3].w;

		return matrix;
	}

	template < class Real >
	inline Matrix4x4<Real> Matrix4x4<Real>::operator+ ( ) const
	{
		return Matrix4x4<Real> (
						m[0].x ,
						m[0].y ,
						m[0].z ,
						m[0].w ,

						m[1].x ,
						m[1].y ,
						m[1].z ,
						m[1].w ,

						m[2].x ,
						m[2].y ,
						m[2].z ,
						m[2].w ,

						m[3].x ,
						m[3].y ,
						m[3].z ,
						m[3].w );
	}

	template < class Real >
	inline Matrix4x4<Real> operator+ (	const Matrix4x4<Real>& a ,
						const Matrix4x4<Real>& b )
	{

		return Matrix4x4<Real> (
						a[0].x + b[0].x ,
						a[0].y + b[0].y ,
						a[0].z + b[0].z ,
						a[0].w + b[0].w ,

						a[1].x + b[1].x ,
						a[1].y + b[1].y ,
						a[1].z + b[1].z ,
						a[1].w + b[1].w ,

						a[2].x + b[2].x ,
						a[2].y + b[2].y ,
						a[2].z + b[2].z ,
						a[2].w + b[2].w ,

						a[3].x + b[3].x ,
						a[3].y + b[3].y ,
						a[3].z + b[3].z ,
						a[3].w + b[3].w );
	}

	template < class Real >
	inline Matrix4x4<Real> operator- ( const Matrix4x4<Real>& a , const Matrix4x4<Real>& b )
	{

		return Matrix4x4<Real> (
						a[0].x - b[0].x ,
						a[0].y - b[0].y ,
						a[0].z - b[0].z ,
						a[0].w - b[0].w ,

						a[1].x - b[1].x ,
						a[1].y - b[1].y ,
						a[1].z - b[1].z ,
						a[1].w - b[1].w ,

						a[2].x - b[2].x ,
						a[2].y - b[2].y ,
						a[2].z - b[2].z ,
						a[2].w - b[2].w ,

						a[3].x - b[3].x ,
						a[3].y - b[3].y ,
						a[3].z - b[3].z ,
						a[3].w - b[3].w );
	}

	template < class Real >
	inline Matrix4x4<Real> operator* ( const Real& factor , const Matrix4x4<Real>& a )
	{

		return Matrix4x4<Real> (
						a[0].x * factor ,
						a[0].y * factor ,
						a[0].z * factor ,
						a[0].w * factor ,

						a[1].x * factor ,
						a[1].y * factor ,
						a[1].z * factor ,
						a[1].w * factor ,

						a[2].x * factor ,
						a[2].y * factor ,
						a[2].z * factor ,
						a[2].w * factor ,

						a[3].x * factor ,
						a[3].y * factor ,
						a[3].z * factor ,
						a[3].w * factor );
	}

	template < class Real >
	inline Matrix4x4<Real> operator* ( const Matrix4x4<Real>& a , const Real& factor )
	{
		return Matrix4x4<Real> (
						a[0].x * factor ,
						a[0].y * factor ,
						a[0].z * factor ,
						a[0].w * factor ,

						a[1].x * factor ,
						a[1].y * factor ,
						a[1].z * factor ,
						a[1].w * factor ,

						a[2].x * factor ,
						a[2].y * factor ,
						a[2].z * factor ,
						a[2].w * factor ,

						a[3].x * factor ,
						a[3].y * factor ,
						a[3].z * factor ,
						a[3].w * factor );
	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator+= ( const Real& factor )
	{

		m[0].x += factor;
		m[0].y += factor;
		m[0].z += factor;
		m[0].w += factor;

		m[1].x += factor;
		m[1].y += factor;
		m[1].z += factor;
		m[1].w += factor;

		m[2].x += factor;
		m[2].y += factor;
		m[2].z += factor;
		m[2].w += factor;

		m[3].x += factor;
		m[3].y += factor;
		m[3].z += factor;
		m[3].w += factor;

		return ( *this );
	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator-= ( const Real& factor )
	{

		m[0].x -= factor;
		m[0].y -= factor;
		m[0].z -= factor;
		m[0].w -= factor;

		m[1].x -= factor;
		m[1].y -= factor;
		m[1].z -= factor;
		m[1].w -= factor;

		m[2].x -= factor;
		m[2].y -= factor;
		m[2].z -= factor;
		m[2].w -= factor;

		m[3].x -= factor;
		m[3].y -= factor;
		m[3].z -= factor;
		m[3].w -= factor;

		return ( *this );
	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator/= ( const Real& factor )
	{

		if ( factor == 0 )
		{
			std::cerr << "[ERROR]" << std::endl << " division by zero." << std::endl;

		}

		Real f = 1.0 / factor;

		m[0].x *= f;
		m[0].y *= f;
		m[0].z *= f;
		m[0].w *= f;

		m[1].x *= f;
		m[1].y *= f;
		m[1].z *= f;
		m[1].w *= f;

		m[2].x *= f;
		m[2].y *= f;
		m[2].z *= f;
		m[2].w *= f;

		m[3].x *= f;
		m[3].y *= f;
		m[3].z *= f;
		m[3].w *= f;

		return ( *this );
	}

	template < class Real >
	inline Matrix4x4<Real>& Matrix4x4<Real>::operator*= ( const Real& factor )
	{

		m[0].x *= factor;
		m[0].y *= factor;
		m[0].z *= factor;
		m[0].w *= factor;

		m[1].x *= factor;
		m[1].y *= factor;
		m[1].z *= factor;
		m[1].w *= factor;

		m[2].x *= factor;
		m[2].y *= factor;
		m[2].z *= factor;
		m[2].w *= factor;

		m[3].x *= factor;
		m[3].y *= factor;
		m[3].z *= factor;
		m[3].w *= factor;

		return ( *this );
	}

	template < class Real >
	inline Matrix4x4<Real> operator/ ( const Matrix4x4<Real>& a , const Real& factor )
	{

		if ( factor == static_cast<Real> ( 0 ) )
		{
			std::cerr << "[ERROR]" << std::endl << " division by zero." << std::endl;

		}

		Real f = 1.0 / factor;

		return Matrix4x4<Real> (
						a[0].x * f ,
						a[0].y * f ,
						a[0].z * f ,
						a[0].w * f ,

						a[1].x * f ,
						a[1].y * f ,
						a[1].z * f ,
						a[1].w * f ,

						a[2].x * f ,
						a[2].y * f ,
						a[2].z * f ,
						a[2].w * f ,

						a[3].x * f ,
						a[3].y * f ,
						a[3].z * f ,
						a[3].w * f );
	}

	template < class Real >
	inline Matrix4x4<Real> operator* ( const Matrix4x4<Real>& a , const Matrix4x4<Real>& b )
	{
		return Matrix4x4<Real> (
						a[0].x * b[0].x + a[0].y * b[1].x + a[0].z * b[2].x + a[0].w * b[3].x ,
						a[0].x * b[0].y + a[0].y * b[1].y + a[0].z * b[2].y + a[0].w * b[3].y ,
						a[0].x * b[0].z + a[0].y * b[1].z + a[0].z * b[2].z + a[0].w * b[3].z ,
						a[0].x * b[0].w + a[0].y * b[1].w + a[0].z * b[2].w + a[0].w * b[3].w ,
						// Row 2
						a[1].x * b[0].x + a[1].y * b[1].x + a[1].z * b[2].x + a[1].w * b[3].x ,
						a[1].x * b[0].y + a[1].y * b[1].y + a[1].z * b[2].y + a[1].w * b[3].y ,
						a[1].x * b[0].z + a[1].y * b[1].z + a[1].z * b[2].z + a[1].w * b[3].z ,
						a[1].x * b[0].w + a[1].y * b[1].w + a[1].z * b[2].w + a[1].w * b[3].w ,
						// Row 3
						a[2].x * b[0].x + a[2].y * b[1].x + a[2].z * b[2].x + a[2].w * b[3].x ,
						a[2].x * b[0].y + a[2].y * b[1].y + a[2].z * b[2].y + a[2].w * b[3].y ,
						a[2].x * b[0].z + a[2].y * b[1].z + a[2].z * b[2].z + a[2].w * b[3].z ,
						a[2].x * b[0].w + a[2].y * b[1].w + a[2].z * b[2].w + a[2].w * b[3].w ,
						// Row 4
						a[3].x * b[0].x + a[3].y * b[1].x + a[3].z * b[2].x + a[3].w * b[3].x ,
						a[3].x * b[0].y + a[3].y * b[1].y + a[3].z * b[2].y + a[3].w * b[3].y ,
						a[3].x * b[0].z + a[3].y * b[1].z + a[3].z * b[2].z + a[3].w * b[3].z ,
						a[3].x * b[0].w + a[3].y * b[1].w + a[3].z * b[2].w + a[3].w * b[3].w );

	}

	template < class Real >
	inline Vector4<Real> operator* ( const Matrix4x4<Real>& matrix4x4 , const Vector4<Real>& vector4 )
	{
		return ( Vector4<Real> (
						matrix4x4[0].x * vector4.x + matrix4x4[0].y * vector4.y + matrix4x4[0].z * vector4.z + matrix4x4[0].w * vector4.w ,
						matrix4x4[1].x * vector4.x + matrix4x4[1].y * vector4.y + matrix4x4[1].z * vector4.z + matrix4x4[1].w * vector4.w ,
						matrix4x4[2].x * vector4.x + matrix4x4[2].y * vector4.y + matrix4x4[2].z * vector4.z + matrix4x4[2].w * vector4.w ,
						matrix4x4[3].x * vector4.x + matrix4x4[3].y * vector4.y + matrix4x4[3].z * vector4.z + matrix4x4[3].w * vector4.w ) );

	}

	template < class Real >
	inline Vector3<Real> operator* ( const Matrix4x4<Real>& matrix4x4 , const Vector3<Real>& vector3 )
	{
		return ( Vector3<Real> ( 	matrix4x4[0].x * vector3.x +
		                         	matrix4x4[0].y * vector3.y +
		                         	matrix4x4[0].z * vector3.z ,

		                         	matrix4x4[1].x * vector3.x +
		                         	matrix4x4[1].y * vector3.y +
		                         	matrix4x4[1].z * vector3.z ,

		                         	matrix4x4[2].x * vector3.x +
		                         	matrix4x4[2].y * vector3.y +
		                         	matrix4x4[2].z * vector3.z ) );
	}

	template < class Real >
	inline std::ostream& operator<< ( std::ostream & s , const Matrix4x4<Real>& matrix )
	{
		s << "Matrix 4x4" << std::endl
                                  << " m[ 1 ].x = " << matrix[0].x << " ,m[ 1 ].y = " << matrix[0].y << " ,m[ 1 ].z = " << matrix[0].z << " ,m[ 4 ].w = " << matrix[0].w << std::endl
                                  << " m[ 2 ].x = " << matrix[1].x << " ,m[ 2 ].y = " << matrix[1].y << " ,m[ 2 ].z = " << matrix[1].z << " ,m[ 4 ].w = " << matrix[1].w << std::endl
                                  << " m[ 3 ].x = " << matrix[2].x << " ,m[ 3 ].y = " << matrix[2].y << " ,m[ 3 ].z = " << matrix[2].z << " ,m[ 4 ].w = " << matrix[2].w << std::endl
                                  << " m[ 4 ].x = " << matrix[3].x << " ,m[ 4 ].y = " << matrix[3].y << " ,m[ 4 ].z = " << matrix[3].z << " ,m[ 4 ].w = " << matrix[3].w << std::endl;

		return  s ;
	}

	template < class Real >
	bool Matrix4x4<Real>::isSymetric ( )
	{
		if ( ( m[0].y == m[1].x ) && ( m[0].z == m[2].x ) && ( m[0].w == m[3].x ) && ( m[2].y == m[1].z ) && ( m[3].y == m[1].w ) && ( m[3].z == m[2].w ) )
		{
			return true;
		}

		return false;
	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::inverse ( ) const
	{
		Real matrix[4][4];

		matrix[0][0] = m[1].z * m[2].w * m[3].y - m[1].w * m[2].z * m[3].y + m[1].w * m[2].y * m[3].z - m[1].y * m[2].w * m[3].z - m[1].z * m[2].y * m[3].w + m[1].y * m[2].z * m[3].w;
		matrix[0][1] = m[0].w * m[2].z * m[3].y - m[0].z * m[2].w * m[3].y - m[0].w * m[2].y * m[3].z + m[0].y * m[2].w * m[3].z + m[0].z * m[2].y * m[3].w - m[0].y * m[2].z * m[3].w;
		matrix[0][2] = m[0].z * m[1].w * m[3].y - m[0].w * m[1].z * m[3].y + m[0].w * m[1].y * m[3].z - m[0].y * m[1].w * m[3].z - m[0].z * m[1].y * m[3].w + m[0].y * m[1].z * m[3].w;
		matrix[0][3] = m[0].w * m[1].z * m[2].y - m[0].z * m[1].w * m[2].y - m[0].w * m[1].y * m[2].z + m[0].y * m[1].w * m[2].z + m[0].z * m[1].y * m[2].w - m[0].y * m[1].z * m[2].w;

		matrix[1][0] = m[1].w * m[2].z * m[3].x - m[1].z * m[2].w * m[3].x - m[1].w * m[2].x * m[3].z + m[1].x * m[2].w * m[3].z + m[1].z * m[2].x * m[3].w - m[1].x * m[2].z * m[3].w;
		matrix[1][1] = m[0].z * m[2].w * m[3].x - m[0].w * m[2].z * m[3].x + m[0].w * m[2].x * m[3].z - m[0].x * m[2].w * m[3].z - m[0].z * m[2].x * m[3].w + m[0].x * m[2].z * m[3].w;
		matrix[1][2] = m[0].w * m[1].z * m[3].x - m[0].z * m[1].w * m[3].x - m[0].w * m[1].x * m[3].z + m[0].x * m[1].w * m[3].z + m[0].z * m[1].x * m[3].w - m[0].x * m[1].z * m[3].w;
		matrix[1][3] = m[0].z * m[1].w * m[2].x - m[0].w * m[1].z * m[2].x + m[0].w * m[1].x * m[2].z - m[0].x * m[1].w * m[2].z - m[0].z * m[1].x * m[2].w + m[0].x * m[1].z * m[2].w;

		matrix[2][0] = m[1].y * m[2].w * m[3].x - m[1].w * m[2].y * m[3].x + m[1].w * m[2].x * m[3].y - m[1].x * m[2].w * m[3].y - m[1].y * m[2].x * m[3].w + m[1].x * m[2].y * m[3].w;
		matrix[2][1] = m[0].w * m[2].y * m[3].x - m[0].y * m[2].w * m[3].x - m[0].w * m[2].x * m[3].y + m[0].x * m[2].w * m[3].y + m[0].y * m[2].x * m[3].w - m[0].x * m[2].y * m[3].w;
		matrix[2][2] = m[0].y * m[1].w * m[3].x - m[0].w * m[1].y * m[3].x + m[0].w * m[1].x * m[3].y - m[0].x * m[1].w * m[3].y - m[0].y * m[1].x * m[3].w + m[0].x * m[1].y * m[3].w;
		matrix[2][3] = m[0].w * m[1].y * m[2].x - m[0].y * m[1].w * m[2].x - m[0].w * m[1].x * m[2].y + m[0].x * m[1].w * m[2].y + m[0].y * m[1].x * m[2].w - m[0].x * m[1].y * m[2].w;

		matrix[3][0] = m[1].z * m[2].y * m[3].x - m[1].y * m[2].z * m[3].x - m[1].z * m[2].x * m[3].y + m[1].x * m[2].z * m[3].y + m[1].y * m[2].x * m[3].z - m[1].x * m[2].y * m[3].z;
		matrix[3][1] = m[0].y * m[2].z * m[3].x - m[0].z * m[2].y * m[3].x + m[0].z * m[2].x * m[3].y - m[0].x * m[2].z * m[3].y - m[0].y * m[2].x * m[3].z + m[0].x * m[2].y * m[3].z;
		matrix[3][2] = m[0].z * m[1].y * m[3].x - m[0].y * m[1].z * m[3].x - m[0].z * m[1].x * m[3].y + m[0].x * m[1].z * m[3].y + m[0].y * m[1].x * m[3].z - m[0].x * m[1].y * m[3].z;
		matrix[3][3] = m[0].y * m[1].z * m[2].x - m[0].z * m[1].y * m[2].x + m[0].z * m[1].x * m[2].y - m[0].x * m[1].z * m[2].y - m[0].y * m[1].x * m[2].z + m[0].x * m[1].y * m[2].z;

		Real factor = static_cast<Real> ( 1.0 ) / this->determinant ( );

		return  Matrix4x4<Real> ( 	matrix[0][0] * factor ,
		                           	matrix[0][1] * factor ,
		                           	matrix[0][2] * factor ,
		                           	matrix[0][3] * factor ,

		                           	matrix[1][0] * factor ,
		                           	matrix[1][1] * factor ,
		                           	matrix[1][2] * factor ,
		                           	matrix[1][3] * factor ,

		                           	matrix[2][0] * factor ,
		                           	matrix[2][1] * factor ,
		                           	matrix[2][2] * factor ,
		                           	matrix[2][3] * factor ,

		                           	matrix[3][0] * factor ,
		                           	matrix[3][1] * factor ,
		                           	matrix[3][2] * factor ,
		                           	matrix[3][3] * factor );

	}

	template < class Real >
	Real Matrix4x4<Real>::determinant ( ) const
	{
		Real determinant;

		determinant =
                        m[0].w * m[1].z * m[2].y * m[3].x - m[0].z * m[1].w * m[2].y * m[3].x - m[0].w * m[1].y * m[2].z * m[3].x +
                        m[0].y * m[1].w * m[2].z * m[3].x + m[0].z * m[1].y * m[2].w * m[3].x - m[0].y * m[1].z * m[2].w * m[3].x -
                        m[0].w * m[1].z * m[2].x * m[3].y + m[0].z * m[1].w * m[2].x * m[3].y + m[0].w * m[1].x * m[2].z * m[3].y -
                        m[0].x * m[1].w * m[2].z * m[3].y - m[0].z * m[1].x * m[2].w * m[3].y + m[0].x * m[1].z * m[2].w * m[3].y +
                        m[0].w * m[1].y * m[2].x * m[3].z - m[0].y * m[1].w * m[2].x * m[3].z - m[0].w * m[1].x * m[2].y * m[3].z +
                        m[0].x * m[1].w * m[2].y * m[3].z + m[0].y * m[1].x * m[2].w * m[3].z - m[0].x * m[1].y * m[2].w * m[3].z -
                        m[0].z * m[1].y * m[2].x * m[3].w + m[0].y * m[1].z * m[2].x * m[3].w + m[0].z * m[1].x * m[2].y * m[3].w -
                        m[0].x * m[1].z * m[2].y * m[3].w - m[0].y * m[1].x * m[2].z * m[3].w + m[0].x * m[1].y * m[2].z * m[3].w;

		return determinant;
	}

	template < class Real >
	void Matrix4x4<Real>::identity ( )
	{

		m[0].x = static_cast<Real> (1.0);
		m[0].y = static_cast<Real> (0.0);
		m[0].z = static_cast<Real> (0.0);
		m[0].w = static_cast<Real> (0.0);

		m[1].x = static_cast<Real> (0.0);
		m[1].y = static_cast<Real> (1.0);
		m[1].z = static_cast<Real> (0.0);
		m[1].w = static_cast<Real> (0.0);

		m[2].x = static_cast<Real> (0.0);
		m[2].y = static_cast<Real> (0.0);
		m[2].z = static_cast<Real> (1.0);
		m[2].w = static_cast<Real> (0.0);

		m[3].x = static_cast<Real> (0.0);
		m[3].y = static_cast<Real> (0.0);
		m[3].z = static_cast<Real> (0.0);
		m[3].w = static_cast<Real> (1.0);
	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeIdentity ( )
	{
		Matrix4x4<Real> matrix;

		matrix[0].x = static_cast<Real> (1.0);
		matrix[0].y = static_cast<Real> (0.0);
		matrix[0].z = static_cast<Real> (0.0);
		matrix[0].w = static_cast<Real> (0.0);

		matrix[1].x = static_cast<Real> (0.0);
		matrix[1].y = static_cast<Real> (1.0);
		matrix[1].z = static_cast<Real> (0.0);
		matrix[1].w = static_cast<Real> (0.0);

		matrix[2].x = static_cast<Real> (0.0);
		matrix[2].y = static_cast<Real> (0.0);
		matrix[2].z = static_cast<Real> (1.0);
		matrix[2].w = static_cast<Real> (0.0);

		matrix[3].x = static_cast<Real> (0.0);
		matrix[3].y = static_cast<Real> (0.0);
		matrix[3].z = static_cast<Real> (0.0);
		matrix[3].w = static_cast<Real> (1.0);

		return matrix;

	}

	template < class Real >
	inline Matrix4x4<Real>::operator const Real* ( ) const
	{
		return m[0];
	}

	template < class Real >
	inline Matrix4x4<Real>::operator Real* ( )
	{
		return m[0];
	}

	template < class Real >
	Matrix4x4<Real>::~Matrix4x4 ( )
	{
	}

}

#endif /* MATRIX4X4_INLINE_HPP_ */
