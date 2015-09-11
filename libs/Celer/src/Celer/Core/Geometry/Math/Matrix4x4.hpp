#ifndef CELER_MATRIX4X4_HPP_
#define CELER_MATRIX4X4_HPP_

// [Header files]
#include <iostream>
// [Project Includes]
#include <Celer/Core/Geometry/Math/Math.hpp>
#include <Celer/Core/Geometry/Math/Vector4.hpp>
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Matrix3x3.hpp>



namespace Celer
{

	/*! \brief The Matrix4x4 class .

	 *@class Matrix4x4.
	 *@brief Class that represent a dense 4x4 Matrix and Homogeneous Coordinate ..
	 *@details ...
	 *@author Felipe Moura.
	 *@email fmc@cos.ufrj.br
	 *@version 1.0.
	 *@todo OpenGL interface and a C style array,
	 *@todo Keep supporting Homogeneous Coordinates ?

	 \nosubgrouping */

	template < class Real > class Matrix4x4
	{

		private:

			Vector4<Real> m[4];
		public:

			Matrix4x4 							( );
			Matrix4x4							( const Real other[4][4]);
			Matrix4x4 							( const Matrix4x4<Real>& other);
			template < class Type >
			Matrix4x4 							( const Matrix4x4<Type>& other );
			template < class Type >
			Matrix4x4 							( const Type other[4][4] );
			Matrix4x4 							( const Vector4<Real>& first ,
			          							  const Vector4<Real>& second ,
			          							  const Vector4<Real>& third ,
			          							  const Vector4<Real>& forth );

			Matrix4x4 							( const Vector3<Real>& first ,
			          							  const Vector3<Real>& second ,
			          							  const Vector3<Real>& third );

			Matrix4x4 							( const Real other00 ,
			          							  const Real other01 ,
			          							  const Real other02 ,
			          							  const Real other03 ,
			          							  const Real other10 ,
			          							  const Real other11 ,
			          							  const Real other12 ,
			          							  const Real other13 ,
			          							  const Real other20 ,
			          							  const Real other21 ,
			          							  const Real other22 ,
			          							  const Real other23 ,
			          							  const Real other30 ,
			          							  const Real other31 ,
			          							  const Real other32 ,
			          							  const Real other33 );
			// Existence of virtual function(s) will add 4 bytes of virtual table pointer in the class, which will be added to size of class
			// @link http://www.cprogramming.com/tutorial/size_of_class_object.html
			~Matrix4x4 ( );

			Vector4<Real> 		column 					( int index ) const;
			Vector4<Real> 		row 					( int index ) const;

			Matrix4x4<Real> 	operator~ 				( ) const;
			Matrix4x4<Real> 	operator- 				( ) const;
			Matrix4x4<Real> 	operator+ 				( ) const;
			Matrix4x4<Real>& 	operator/= 				( const Real& factor );
			Matrix4x4<Real>& 	operator*= 				( const Real& factor );
			Matrix4x4<Real>& 	operator+= 				( const Real& factor );
			Matrix4x4<Real>& 	operator-= 				( const Real& factor );

			Matrix4x4<Real> 	makeTranslate 				( const Vector3<Real>& delta ) const;
			Matrix4x4<Real>		makeTranslate 				( const Vector4<Real>& delta ) const;
			Matrix4x4<Real> 	makeScalar 				( const Vector3<Real>& delta ) const;

			static Matrix4x4<Real>	makeViewMatrix				( const Vector3<Real>& eyes ,
			                                     			  	  const Vector3<Real>& position ,
						  	  	  	  	  	  const Vector3<Real>& up );

			static Matrix4x4<Real>	makePerspectiveProjectionMatrix 	( const Real& fov ,
			                      	                               	  	  const Real& aspectRation ,
			                      	                               	  	  const Real& near ,
			                      	                               	  	  const Real& far );

			static Matrix4x4<Real>	makeOrthographicProjectionMatrix 	( const Real& left ,
			                      	                                 	  const Real& right,
			                                                        	  const Real& bottom ,
			                                                        	  const Real& top ,
			                                                        	  const Real& near ,
			                                                        	  const Real& far );

			void 			fromHeadPitchRoll 			( Real headDegrees ,
			     			                  			  Real pitchDegrees ,
			     			                  			  Real rollDegrees );

			void		 	toHeadPitchRoll 			( Real& headDegrees ,
			    		 	                			  Real& pitchDegrees ,
			    		 	                			  Real& rollDegrees ) const;

			void 			rotate 					( const Vector3<Real>& axis , Real& degrees );

			void			identity 				( );
			Matrix4x4<Real> 	makeIdentity 				( );
			Real 			determinant 				( ) const;
			Matrix4x4<Real> 	inverse 				( ) const;
			bool 			isSymetric 				( );

			inline Matrix4x4<Real>& operator= ( const Matrix3x3<Real>& other );
			inline Matrix4x4<Real>& operator= ( const Matrix4x4<Real>& other );
			inline			operator const Real* 			( ) const;
			inline 			operator Real* 			( );

			const Vector4<Real>& 	operator[] 				( int rowIndex ) const;
			Vector4<Real>& 		operator[] 				( int rowIndex );

			Real 			operator( ) 				( int i , int j ) const;
			Real& 			operator( ) 				( int i , int j );
			template < class Type >
			friend Matrix4x4<Type> operator* 				( const Type& factor , const Matrix4x4<Type>& a );
			template < class Type >
			friend Matrix4x4<Type> operator* 				( const Matrix4x4<Type>& matrix4x4 , const Type& factor );
			template < class Type >
			friend Matrix4x4<Type> operator/ 				( const Matrix4x4<Type>& matrix4x4 , const Type& factor );
			template < class Type >
			friend Matrix4x4<Type> operator+ 				( const Matrix4x4<Type>& matrix4x4 , const Matrix4x4<Type>& other4x4 );
			template < class Type >
			friend Matrix4x4<Type> operator- 				( const Matrix4x4<Type>& matrix4x4 , const Matrix4x4<Type>& other4x4 );
			template < class Type >
			friend Matrix4x4<Type> operator* 				( const Matrix4x4<Type>& matrix4x4 , const Matrix4x4<Type>& other4x4 );
			template < class Type >
			friend Vector4<Type> 	operator* 				( const Matrix4x4<Type>& matrix4x4 , const Vector4<Type>& vector4 );
			template < class Type >
			friend Vector3<Type> 	operator* 				( const Matrix4x4<Type>& matrix4x4 , const Vector3<Type>& vector3 );
			template < class Type >
			friend std::ostream& operator<< ( std::ostream & s , const Matrix4x4<Type>& matrix );

			friend class Vector3<Real> ;
			friend class Vector4<Real> ;
			friend class Matrix3x3<Real> ;

	}; // End Interface


}/* Celer :: NAMESPACE */

#endif /*MATRIX4X4_HPP_*/

/// Base matrix operations.
#include <Celer/Core/Geometry/Math/Matrix4x4.inline.hpp>
/// Affine transformation.
#include <Celer/Core/Geometry/Math/Matrix4x4.Graphics.hpp>

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


//Celer::Matrix4x4<float> m2 (5.0,1.0,6.0,0.0,
//						   9.0,7.0,8.0,0.0,
//						   4,5.0,6.0,0.0,
//						   0.0,0.0,0.0,0.0);
//Celer::Matrix4x4<float> m3;
//
//
//std::cout << m3;
//
//m3 = m1 * m2;
//
//std::cout << "#m3 = m1*m2 = # "
//"\n  m[1][1] = 184 ,m[1][2] = 88 ,m[1][3] = 214 "
//"\n  m[2][1] = 105 ,m[2][2] = 92 ,m[2][3] = 112 "
//"\n  m[3][1] = 111 ,m[3][2] = 87 ,m[3][3] = 126 \n"  << m3 << std::endl;
//
//m3 = -m1 + m2;
//std::cout << "#m3 = -m1+m2 = # "
//"\n  m[1][1] = 28 ,m[1][2] = 6 ,m[1][3] = 12 "
//"\n  m[2][1] = 10 ,m[2][2] = 15 ,m[2][3] = 15 "
//"\n  m[3][1] = 9 ,m[3][2] = 11,m[3][3] = 14 \n"  << m3 << std::endl;
//
//m3 = (-m2) / (-2.0f);
//std::cout << "#m3 = 2.0* (-m2) = # "
//"\n m[1][1] = -10 ,m[1][2] = -2 ,m[1][3] = -12 "
//"\n m[2][1] =  -18 , m[2][2] = -14 ,m[2][3]  = -16 "
//"\n m[3][1] =  -8  ,m[3][2] = -10 ,m[3][3] =  -12 \n"  << m3 << std::endl;
//
//
//std::cout << m3;
