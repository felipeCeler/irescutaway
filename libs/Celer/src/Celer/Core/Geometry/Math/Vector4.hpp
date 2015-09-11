#ifndef CELER_VECTOR4_HPP_
#define CELER_VECTOR4_HPP_

#include <iostream>

#include <cassert>
#include <cmath>

#include <Celer/Core/Geometry/Math/Vector3.hpp>

namespace Celer
{

	/*!
	 *@class Vector4.
	 *@brief Class that represent a Vector in 3D with w.
	 *@details ...
	 *@author Felipe Moura.
	 *@email fmc@cos.ufrj.br
	 *@version 1.0.
	 *@date 25-Feb-2008.
	 *@todo OpenGL interface and a C style array.
	 */

	template < class Real > class Vector4
	{

		public:

			/*! @name Members  */
			//@{
			union
			{
					struct
					{
							Real x; /*!< x abscissa of space. */
							Real y; /*!< y abscissa of space. */
							Real z; /*!< z abscissa of space. */
							Real w; /*!< w abscissa of space. */
					};

					Real array[4];
			};
			//@}

		public:

			friend class Vector3<Real>;

			/*! @name  Defining a Vector4 */
			//@{
			/*! Default constructor. Value is set to (0,0,0,0). */
			Vector4 ( );
			template < class T >
			Vector4 ( const T* v );
			Vector4 ( const Vector4<Real>& vector );
			Vector4 ( const Vector3<Real>& vector );
			Vector4 ( const Vector3<Real>& vector , const Real& w );
			Vector4 ( const Real& x , const Real& y , const Real& z , const Real& w );
			//@}

			//Operator

			/*! @name Accessing the value */
			//@{

			/*!Conversion operator returning the memory address of the Point.
			 * Useful to pass as a parameter to OpenGL functions:
			 * \code
			 * Celer:Vector3<float> pos, normal;
			 * glNormal3fv(normal);
			 * glVertex3fv(pos);
			 * \endcode */

			Real 		operator[] 	( int index ) const;

			Real& 		operator[] 	( int index );
			//@}

			// Assignment Opertators
			//With Scalar
			/*! @name Algebraic computations */
			// @{

			Vector4<Real> 		operator+ 	( ) const;
			Vector4<Real> 		operator- 	( ) const;

			Vector4<Real>& 		operator+= 	( const Real& factor );
			Vector4<Real>& 		operator-= 	( const Real& factor );
			Vector4<Real>& 		operator*= 	( const Real& factor );
			Vector4<Real>& 		operator/= 	( const Real& factor );

			template < class T >
			friend Vector4<T> 	operator*	( const Vector4<T>& v , const T& factor );
			template < class T >
			friend Vector4<T> 	operator* 	( const T& factor , const Vector4<T>& v );
			template < class T >
			friend Vector4<T> 	operator/ 	( const Vector4<T>& v , const T& factor );
			template < class T >
			friend Vector4<T> 	operator+ 	( const T& factor , const Vector4<T>& v );
			template < class T >
			friend Vector4<T> 	operator+ 	( const Vector4<T>& v , const T& factor );
			template < class T >
			friend Vector4<T> 	operator- 	( const T& factor , const Vector4<T>& v );
			template < class T >
			friend Vector4<T> 	operator-	( const Vector4<T>& v , const T& factor );

			// Assignment with Vector
			Vector4<Real>& 		operator=  	( const Vector4<Real>& v );
			Vector4<Real>& 		operator+= 	( const Vector4<Real>& v );
			Vector4<Real>& 		operator-= 	( const Vector4<Real>& v );
			Vector4<Real>& 		operator/= 	( const Vector4<Real>& v );

			bool 			operator== 	( const Vector4<Real>& v ) const;
			bool 			operator!= 	( const Vector4<Real>& v ) const;

			Vector4<Real> 		operator- 	( const Vector4<Real>& v ) const;
			Vector4<Real> 		operator+ 	( const Vector4<Real>& v ) const;

			Real 			operator* 	( const Vector4<Real>& v ) const;

			//@}
			//@{
			/*! Output stream operator. Enables debugging code like:
			 \code
			 Celer::Vector3<> v(...);
			 std::cout << "Vector4" << " x = " << v.x << " ,y = " << v.y << " ,z = " << v.z << " ,w = " << v.w << std::endl;
			 \endcode */
			template < class T >
			friend std::ostream& operator<< ( std::ostream & s , const Vector4<T>& v );

			/*!@name Functions */
			//@{
			/*!  */
			// AUXILIAR FUNCTIONS
			/// Aqui eu troquei length e lengthSqrt
			Real 		length 		( void );
			Real 		lengthSqrt 	( void );
			void 		normalize 	( void );
			Vector4<Real> 	norm 		( void );
			Vector3<Real>   toVector3 	( void );

			inline operator const Real *	( void ) const;
			inline operator Real * 	( void );
			//@}
			~Vector4 ( );

	};// End Interface


} /* Celer :: NAMESPACE */

#endif

#include <Celer/Core/Geometry/Math/Vector4.inline.hpp>

// CASOS DE TESTE


//// Teste operator+ e operator- with vectors
//Celer::Vector4<double> v1(1.0,1.0,1.0,0.0);
//Celer::Vector4<double> v2(-1.0,-1.0,-1.0,0.0);
//Celer::Vector4<double> v3(0.0,0.0,0.0,0.0);
//
//v3 = v1 + v2;
//std::cout << "#v1+v2# = 0,0,0 " << v3 << std::endl;
//v3 = v1 - v2;
//std::cout << "#v1+v2# = 2,2,2 " << v3 << std::endl;
//v3 = -v1 - v2;
//std::cout << "#-v1-v2# = 0,0,0 "<< v3 << std::endl;
//v3 = -v1 + v2;
//std::cout << "#-v1+v2# = -2,-2,-2 "<< v3 << std::endl;
//v3 = + v1 - v2;
//std::cout << "#+v1-v2# = 2,2,2 "<< v3 << std::endl;
//v3 = + v1 + v2;
//std::cout << "#+v1+v2 = 0,0,0 # "<< v3 << std::endl;
//
//// Teste operator+ e operator- with scalar
//
//v3 = v1 + 4.0;
//std::cout << "#v1+4# = 4,4,4 " << v3 << std::endl;
//v3 = 9.0 - v2;
//std::cout << "#9-v2# = 10,10,10 " << v3 << std::endl;
//v3 = -v1 / -8.0;
//std::cout << "#-v1-8# = 0.125,0.125,0.125 "<< v3 << std::endl;
//v3 = -v1 * 3.0;
//std::cout << "#-v1+3# = -3,-3,-3 "<< v3 << std::endl;
//
//
////Teste operator* with scalar
//v1 = Celer::Vector4<double>(1.0,1.0,1.0,0.0);
//v3 = Celer::Vector4<double>(0.0,0.0,0.0,0.0);
//
//v3 = 3.0 * v1;
//std::cout << "#3*v1# = 3,3,3 "<<v3 << std::endl;
//v3 = 3.0 * (-v1);
//std::cout << "#3*(-v1)# = -3,-3,-3 " << v3 << std::endl;
//v3 = 3.0 * (+v1);
//std::cout << "#3*(+v1)# = 3,3,3 "<< v3 << std::endl;
//v3 = (-v1) * 3.0;
//std::cout << "#(-v1)*3# = -3,-3,-3 " << v3 << std::endl;
//v3 = (+v1) * 3.0;
//std::cout << "#(+v1)*3# = 3,3,3 "<< v3 << std::endl;
//v3 = (+v1) * 0.0;
//std::cout << "#(+v1)*0.0# = 0,0,0"<< v3 << std::endl;
//v3 = (+v1) * v2[3];
//std::cout << "#(+v1)*0.0# = 0,0,0" << v3 << std::endl;
//
//Celer::Vector4<double> v4(1.0,1.0,1.0,1.0);
//Celer::Vector4<double> v5(-1.0,-1.0,-1.0,-1.0);
//Celer::Vector4<double> v6(2.0,2.0,2.0,4.0);
//double r = 1.0;
//
//v3 += v4;
//std::cout << "#v3+=v4# = 1,1,1 "<<v3 << std::endl;
//v3 = Celer::Vector4<double>(0.0,0.0,0.0,0.0);
//v3 += -3.0*v4;
//std::cout << "#v3+=-3*v4# = -3,-3,-3 "<<v3 << std::endl;
//v3 = Celer::Vector4<double>(0.0,0.0,0.0,0.0);
//v3 += -v5+v4;
//std::cout << "#v3+=-v5+v4# = 2,2,2 "<<v3 << std::endl;
//
//v3 -= v4;
//std::cout << "#v3-=v4# = -1,-1,-1 "<<v3 << std::endl;
//v3 = Celer::Vector4<double>(0.0,0.0,0.0,0.0);
//v3 -= -3.0*v4;
//std::cout << "#v3-=-3*v4# = 3,3,3 "<<v3 << std::endl;
//v3 = Celer::Vector4<double>(0.0,0.0,0.0,0.0);
//v3 -= -v5+v4;
//std::cout << "#v3-=-v5+v4# = -2,-2,-2 "<<v3 << std::endl;
//
//r = v4 * v6;
//std::cout << "#v4*v64# = 10 "<< r << std::endl;
//r = v4[1] * v6[3];
//std::cout << "#v4[1]*v6[3]# = 4 "<< r << std::endl;
//
//v6 /= v4;
//std::cout << "#v6/=v4# = 2,2,2,4 "<<v6 << std::endl;
//v6 = Celer::Vector4<double>(2.0,2.0,2.0,4.0);
//v6 /= -3.0*v4;
//std::cout << "#v6/=-3*v4# = -0.666,-0.666,-0.666 "<<v6 << std::endl;
//v6 = Celer::Vector4<double>(2.0,2.0,2.0,4.0);
//v6 /= -v5+v4;
//std::cout << "#v6/=-v5+v4# = 1,1,1,2 "<<v6 << std::endl;
//
//std::vector<double> vec(static_cast< double * >(v4),static_cast< double * >(v4)+4);
//std::ostream_iterator<double> output( std::cout, " ");
//std::cout << "vec" << std::endl;
//
//std::copy(vec.begin(),vec.end(),output);
//
//solucão para não ter warnning no retorno de uma referencia
//inline Vector3<T>&  operator- ( const Vector3<T>& u) const
//{
//	Vector3<T> w;
//
//	w.x = this->x - u.x;
//	w.y = this->y - u.y;
//	w.z = this->z - u.z;
//
//	Vector3<T>& h = w;
//
//	return ( h );
//};

