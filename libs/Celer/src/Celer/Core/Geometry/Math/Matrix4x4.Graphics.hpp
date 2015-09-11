/*
 * Matrix4x4.inline.Graphics.hpp
 *
 *  Created on: Nov 7, 2012
 *      Author: felipe
 */

#ifndef MATRIX4X4_INLINE_GRAPHICS_HPP_
#define MATRIX4X4_INLINE_GRAPHICS_HPP_

namespace Celer
{

	/// ----------- Afine Transformation ------------------------------------------------------------------------------------------

	//@}
	/*! @name Accessing the value */
	//@{

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeTranslate ( const Vector3<Real>& v ) const
	{
		return ( Matrix4x4<Real> (	0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						v.x ,
						v.y ,
						v.z ,
						1.0 ) );
	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeTranslate ( const Vector4<Real>& v ) const
	{
		return Matrix4x4<Real> (	0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						v.x ,
						v.y ,
						v.z ,
						v.w );

	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeScalar ( const Vector3<Real>& v ) const
	{
		return ( Matrix4x4<Real> ( 	v.x ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						v.y ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						v.z ,
						0.0 ,
						0.0 ,
						0.0 ,
						0.0 ,
						1.0 ) );

	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makePerspectiveProjectionMatrix (	const Real& pFov ,
										const Real& pAspectRation ,
										const Real& pNear ,
										const Real& pFar )
	{

		Real c = static_cast<Real> ( 1.0 / std::tan ( Celer::Math::kDeg2Rad * pFov * 0.5 ) );
		Real aspectRationInv = static_cast<Real> ( 1.0 / pAspectRation );
		Real fovy = static_cast<Real> ( 2.0 * std::atan ( aspectRationInv / c ) );
		Real xScale = static_cast<Real> ( 1.0 / std::tan ( 0.5 * fovy ) );
		Real yScale = static_cast<Real> ( xScale / aspectRationInv );

		Matrix4x4<Real> pespectiveProjectionMatrix;

		pespectiveProjectionMatrix[0].x = xScale;
		pespectiveProjectionMatrix[0].y = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[0].z = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[0].w = static_cast<Real> ( 0 );

		pespectiveProjectionMatrix[1].x = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[1].y = yScale;
		pespectiveProjectionMatrix[1].z = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[1].w = static_cast<Real> ( 0 );

		pespectiveProjectionMatrix[2].x = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[2].y = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[2].z = static_cast<Real> ( ( pFar + pNear ) / ( pNear - pFar ) );
		pespectiveProjectionMatrix[2].w = static_cast<Real> ( ( 2.0 * pFar * pNear ) / ( pNear - pFar ) );

		pespectiveProjectionMatrix[3].x = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[3].y = static_cast<Real> ( 0 );
		pespectiveProjectionMatrix[3].z = static_cast<Real> ( -1 );
		pespectiveProjectionMatrix[3].w = static_cast<Real> ( 0 );

		return pespectiveProjectionMatrix;

	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeOrthographicProjectionMatrix (	const Real& left ,
										const Real& right ,
										const Real& bottom ,
										const Real& top ,
										const Real& near ,
										const Real& far )
	{
		Matrix4x4<Real> orthographicProjectionMatrix;

		orthographicProjectionMatrix[0].x = ( 2.0 / ( right - left ) );
		orthographicProjectionMatrix[0].y = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[0].z = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[0].w = - ( right + left ) / ( right - left );

		orthographicProjectionMatrix[1].x = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[1].y = ( 2.0 / ( top - bottom ) );
		orthographicProjectionMatrix[1].z = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[1].w = - ( top + bottom ) / ( top - bottom );

		orthographicProjectionMatrix[2].x = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[2].y = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[2].z = - ( 2.0 / ( far - near ) );
		orthographicProjectionMatrix[2].w = - (( far + near ) / ( far - near ));

		orthographicProjectionMatrix[3].x = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[3].y = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[3].z = static_cast<Real> ( 0 );
		orthographicProjectionMatrix[3].w = static_cast<Real> ( 1 );

		return orthographicProjectionMatrix;

	}

	template < class Real >
	Matrix4x4<Real> Matrix4x4<Real>::makeViewMatrix (	const Vector3<Real>& position ,
								const Vector3<Real>& target ,
								const Vector3<Real>& up )
	{

		Vector3<Real> zAxis = ( position - target ).norm ( );
		Vector3<Real> xAxis = ( up ^ zAxis ).norm ( );
		Vector3<Real> yAxis = zAxis ^ xAxis;

		Matrix4x4<Real> viewMatrix;

		viewMatrix[0].x = xAxis.x;
		viewMatrix[0].y = xAxis.y;
		viewMatrix[0].z = xAxis.z;
		viewMatrix[0].w = - ( xAxis * position );

		viewMatrix[1].x = yAxis.x;
		viewMatrix[1].y = yAxis.y;
		viewMatrix[1].z = yAxis.z;
		viewMatrix[1].w = - ( yAxis * position );

		viewMatrix[2].x = zAxis.x;
		viewMatrix[2].y = zAxis.y;
		viewMatrix[2].z = zAxis.z;
		viewMatrix[2].w = - ( zAxis * position );

		viewMatrix[3].x = static_cast<Real> ( 0 );
		viewMatrix[3].y = static_cast<Real> ( 0 );
		viewMatrix[3].z = static_cast<Real> ( 0 );
		viewMatrix[3].w = static_cast<Real> ( 1 );

		return viewMatrix;
	}

	template < class Real >
	void Matrix4x4<Real>::fromHeadPitchRoll ( Real headDegrees , Real pitchDegrees , Real rollDegrees )
	{
		// Constructs a rotation matrix based on a Euler Transform.
		// I use the popular NASA standard airplane convention of
		// heading-pitch-roll (i.e., RzRxRy).

		headDegrees = Math::kDeg2Rad * headDegrees;
		pitchDegrees = Math::kDeg2Rad * pitchDegrees;
		rollDegrees = Math::kDeg2Rad * rollDegrees;

		Real cosH = std::cos ( headDegrees );
		Real cosP = std::cos ( pitchDegrees );
		Real cosR = std::cos ( rollDegrees );
		Real sinH = std::sin ( headDegrees );
		Real sinP = std::sin ( pitchDegrees );
		Real sinR = std::sin ( rollDegrees );

		m[0].x = cosR * cosH - sinR * sinP * sinH;
		m[0].y = sinR * cosH + cosR * sinP * sinH;
		m[0].z = -cosP * sinH;
		m[0].w = static_cast<Real> ( 0 );

		m[1].x = -sinR * cosP;
		m[1].y = cosR * cosP;
		m[1].z = sinP;
		m[1].w = static_cast<Real> ( 0 );

		m[2].x = cosR * sinH + sinR * sinP * cosH;
		m[2].y = sinR * sinH - cosR * sinP * cosH;
		m[2].z = cosP * cosH;
		m[2].w = static_cast<Real> ( 0 );

		m[3].x = static_cast<Real> ( 0 );
		m[3].y = static_cast<Real> ( 0 );
		m[3].z = static_cast<Real> ( 0 );
		m[3].w = static_cast<Real> ( 1 );
	}

	template < class Real >
	void Matrix4x4<Real>::rotate ( const Vector3<Real>& axis , Real& degrees )
	{
		degrees = Math::kDeg2Rad * degrees;

		Real x = axis.x;
		Real y = axis.y;
		Real z = axis.z;
		Real c = std::cos ( degrees );
		Real s = std::sin ( degrees );

		m[0].x = ( x * x ) * ( 1.0 - c ) + c;
		m[0].y = ( x * y ) * ( 1.0 - c ) + ( z * s );
		m[0].z = ( x * z ) * ( 1.0 - c ) - ( y * s );
		m[0].w = static_cast<Real> ( 0 );

		m[1].x = ( y * x ) * ( 1.0 - c ) - ( z * s );
		m[1].y = ( y * y ) * ( 1.0 - c ) + c;
		m[1].z = ( y * z ) * ( 1.0 - c ) + ( x * s );
		m[1].w = 0.0;

		m[2].x = ( z * x ) * ( 1.0 - c ) + ( y * s );
		m[2].y = ( z * y ) * ( 1.0 - c ) - ( x * s );
		m[2].z = ( z * z ) * ( 1.0 - c ) + c;
		m[2].w = static_cast<Real> ( 0 );

		m[3].x = static_cast<Real> ( 0 );
		m[3].y = static_cast<Real> ( 0 );
		m[3].z = static_cast<Real> ( 0 );
		m[3].w = static_cast<Real> ( 1 );
	}

	template < class Real >
	void Matrix4x4<Real>::toHeadPitchRoll ( Real &headDegrees , Real &pitchDegrees , Real &rollDegrees ) const
	{

		Real thetaX = std::asin ( m[1].z );
		Real thetaY = static_cast<Real> ( 0 );
		Real thetaZ = static_cast<Real> ( 0 );

		if ( thetaX < Math::kHalfPi )
		{
			if ( thetaX > -Math::kHalfPi )
			{
				thetaZ = std::atan2 (	-m[1].x ,
							m[1].y );
				thetaY = std::atan2 (	-m[0].z ,
							m[2].z );
			}
			else
			{
				// Not a unique solution.
				thetaZ = -std::atan2 ( 	m[2].x ,
							m[0].x );
				thetaY = static_cast<Real> ( 0 );
			}
		}
		else
		{
			// Not a unique solution.
			thetaZ = std::atan2 ( 	m[2].x ,
						m[0].x );
			thetaY = static_cast<Real> ( 0 );
		}

		headDegrees = Math::kRad2Deg * ( thetaY );
		pitchDegrees = Math::kRad2Deg * ( thetaX );
		rollDegrees = Math::kRad2Deg * ( thetaZ );
	}

}

#endif /* MATRIX4X4_INLINE_GRAPHICS_HPP_ */
