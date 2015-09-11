#ifndef CELER_QUATERNION_HPP_
#define CELER_QUATERNION_HPP_

#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Matrix3x3.hpp>
#include <Celer/Core/Geometry/Math/Matrix4x4.hpp>
#include <Celer/Core/Geometry/Math/Math.hpp>

namespace Celer{

/*!
*@class Quaternion.
*@brief Class that represent Quaternion.
*@details ...
*@author Felipe Moura.
*@email fmc@cos.ufrj.br
*@version 1.0.
*@date 23 Abril-2008.
*@todo OpenGL interface and a C style array.
*@todo Constructor FromRotMatrix, Get toRotMatrix
*@todo Ver as Classes friends e otimizar
*/

	template <class Real> class Quaternion
	{


	public:

		friend class Vector3<Real>;
		friend class Matrix3x3<Real>;
		friend class Matrix4x4<Real>;

	   	 /*! @name  Defining a Quaternion */
	    //@{
		/*! Default constructor. Value is set to (1,0,0,0). */
		Quaternion();
		Quaternion( const Quaternion<Real>& quat );
		Quaternion( const Real& w, const Real& x, const Real& y, const Real& z );
		Quaternion( const Real& pAngle, const Vector3<Real>& pAxis );

		Quaternion<Real>& 		operator=(const Quaternion<Real>& quat );
  	  	void 					setAngle( const Real& angle );
  	  	void 					setAxis( const Vector3<Real>& axis );
  	  	void 					setAxis( const Real& x, const Real& y, const Real& z );

		/*! @name Accessing the value */
		//@{


  	  	Vector3<Real> 			axis () const;
  	  	Real 					angle() const;

		Real  					operator[]( int index ) const;
		Real& 					operator[]( int index );

		//@}

	    // arithmetic updates

  	  	/*! @name Algebraic computations */
  	  	// @{
		Quaternion<Real>& 		operator*=( const Quaternion<Real>& quat );
	    Quaternion<Real>& 		operator*=( const Real& factor );
	    Quaternion<Real>& 		operator/=( const Real& factor );


	    // arithmetic operations
		template < class T>
	    friend Quaternion<T> 	operator*( const Quaternion<T>& a, const Quaternion<T>& b ) ;

		template < class T>
	    friend Vector3<T>    	operator*( const Quaternion<T>& quat, const Vector3<T>& vector ) ;

	    Quaternion<Real> 		operator*( const Real& factor) const;
	    Quaternion<Real> 		operator/( const Real& factor) const;

		//Negates all the coefficients of the Quaternion.
		Quaternion<Real> 		operator-() const;

	    // functions of a quaternion
	    //Inverses the Quaternion (same rotation angle(), but negated axis()).
	    Quaternion<Real> 		operator~( ) const;
	    //@}

	   	/*!@name Functions */
	    //@{
	    Real 			    	length( void ) const;
	    Quaternion<Real> 		inverse( void ) const;
	    void 					invert( void );
	    Real					norm( void ) const;
	    void	 				normalize( void );
	    Quaternion<Real>		normalized( void );
	    Vector3<Real>        	rotate( const Vector3<Real>& v ) const;
		Real				    dot( const Quaternion<Real>& quat ) const;

		void 					identity();
		void 					to3x3Matrix( Matrix3x3<Real>& rotationMatrix ) const;
		Matrix3x3<Real> 		to3x3Matrix( ) const;
		void 					to4x4Matrix( Matrix4x4<Real>& rotationMatrix ) const;
		void 					to4x4Matrix( Real * rotMatrix) const;
		Matrix4x4<Real>  		to4x4Matrix() const;

		void 					fromRotationMatrix( const Matrix4x4<Real>& rotationMatrix );
		void 					fromAxisAngle( const Vector3<Real>& axis, const Real& degrees );
		void 					fromHeadPitchRoll( const Real& headDegrees, const Real& pitchDegrees, const Real& rollDegrees);

		void 					toAxisAngle( Vector3<Real>& axis, Real& degrees ) const;
		void 					toHeadPitchRoll( const Real& headDegrees, const Real& pitchDegrees, const Real& rollDegrees ) const;
		void 					toRotationArc( Vector3<Real> &u, Vector3<Real> &v );
		//@}

		/*! @name Output stream */
		//@{
		/*! Output stream operator. Enables debugging code like:
    	  \code

    	  std::cout << " Quaternion " << " w = " << q.w() << " x = " << q.x() << " ,y = " << q.y() << " ,z = " << z() << std::endl;
    	  \endcode */
		template <class T>
		friend std::ostream& 	operator<< ( std::ostream & s, const Quaternion<Real>& quat );
		//@}
		virtual ~Quaternion();

	public:

		Real w;
		Real x;
		Real y;
		Real z;

	};


	template <class Real>
	Quaternion<Real>::Quaternion()
	{
	    w = static_cast<Real> (1.0);
	    x = static_cast<Real> (0.0);
	    y = static_cast<Real> (0.0);
	    z = static_cast<Real> (0.0);
	};

	template<class Real>
	Quaternion<Real>::Quaternion( const Quaternion<Real>& quat )
	:
		w( quat.w ),
		x( quat.x ),
		y( quat.y ),
		z( quat.z )
	{};

	template<class Real>
	Quaternion<Real>::Quaternion( const Real& w, const Real& x, const Real& y, const Real& z )
	:
		w(w),
		x(x),
		y(y),
		z(z)
	{};

	template<class Real>
	Quaternion<Real>::Quaternion( const Real& angle ,const Vector3<Real>& axis )
	{
	    const Real lenght = axis.Lenght();

	    if (lenght < 1E-8)
	    {	// Null rotation
	    	w =  static_cast<Real> (1.0);; x =  static_cast<Real> (0.0); y =  static_cast<Real> (0.0); z =  static_cast<Real> (0.0);
	    }
	    else
	    {
	    	const Real sin_half_angle = sin(angle * 0.5);

	    	w = cos(angle * 0.5);
	    	x = sin_half_angle*axis[0]/lenght;
	    	y = sin_half_angle*axis[1]/lenght;
	    	z = sin_half_angle*axis[2]/lenght;

	    }
	};

	//Operator


	template<class Real>
	inline Real Quaternion<Real>::operator[]( int index ) const
	{
		assert( ( index >= 0 ) && ( index < 4 ) );

		return ( ( &w )[ index ] );

	};

	template<class Real>
	inline Real& Quaternion<Real>::operator[]( int index )
	{
		assert( ( index >= 0 ) && ( index < 4 ) );

		return ( ( &w )[ index ] );

	};

	template<class Real>
	inline void Quaternion<Real>::setAngle( const Real& angle )
	{
		w = angle;
	}

	template<class Real>
	inline void Quaternion<Real>::setAxis( const Vector3<Real>& axis )
	{
		x = axis.x;
		y = axis.y;
		z = axis.z;
	};

	template<class Real>
	inline void Quaternion<Real>::setAxis( const Real& x, const Real& y, const Real& z )
	{

		this->x = x;
		this->y = y;
		this->z = z;

	};

	template<class Real>
	inline Vector3<Real> Quaternion<Real>::axis() const
	{
		return ( Vector3<Real> (x,y,z) );
	};

	template<class Real>
	inline Real Quaternion<Real>::angle() const
	{
		return ( w );
	}


	template <class Real>
	inline Quaternion<Real>& Quaternion<Real>::operator=( const Quaternion<Real>& quat )
	{
		w = quat.w;
	    x = quat.x;
	    y = quat.y;
	    z = quat.z;
	    return *this;
	}

	template <class Real>
	inline Quaternion<Real>& Quaternion<Real>::operator*=( const Real& factor )
	{
		w *= factor;
		x *= factor;
		y *= factor;
		z *= factor;

		return *this;
	}

	//------------------------------------------------------------------------------
	//
	template<class Real>
	inline Quaternion<Real>& Quaternion<Real>::operator/=( const Real& factor )
	{
		w /= factor;
		x /= factor;
		y /= factor;
		z /= factor;

		return *this;
	}

	template <class Real>
	inline Quaternion<Real> operator*( const Quaternion<Real>& a, const Quaternion<Real>& b )
	{
	  return Quaternion<Real>( a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
							   a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
							   a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
					           a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w  );

	//  res.w = w*r.w - x*r.x - y*r.y - z*r.z;
	//   res.x = w*r.x + x*r.w + y*r.z - z*r.y;
	//   res.y = w*r.y - x*r.z + y*r.w + z*r.x;
	//   res.z = w*r.z + x*r.y - y*r.x + z*r.w;

	}

	template <class Real>
	inline Vector3<Real> Quaternion<Real>::rotate( const Vector3<Real>& v ) const
	{
	    Quaternion<Real> quat(1.0,v.x, v.y, v.z);

	    quat = (*this) * quat * (~(*this));

	    return Vector3<Real> (quat.x, quat.y, quat.z);
	}

	template <class Real>
	inline Matrix3x3<Real> Quaternion<Real>::to3x3Matrix( ) const
	{
		Matrix3x3<Real> rotationMatrix;

		Real xx = x * x * static_cast<Real> (2.0);
		Real yy = y * y * static_cast<Real> (2.0);
		Real zz = z * z * static_cast<Real> (2.0);

		Real xy = x * y * static_cast<Real> (2.0);
		Real xz = x * z * static_cast<Real> (2.0);
		Real xw = x * w * static_cast<Real> (2.0);

		Real yz = y * z * static_cast<Real> (2.0);
		Real yw = y * w * static_cast<Real> (2.0);

		Real zw = z * w * static_cast<Real> (2.0);

	    rotationMatrix[0].x = static_cast<Real>(1.0) -(yy+zz);
	    rotationMatrix[0].y = xy-zw;
	    rotationMatrix[0].z = xz+yw;

	    rotationMatrix[1].x = xy+zw;
	    rotationMatrix[1].y = static_cast<Real>(1.0) -(xx+zz);
	    rotationMatrix[1].z = yz-xw;

	    rotationMatrix[2].x = xz-yw;
	    rotationMatrix[2].y = yz+xw;
	    rotationMatrix[2].z = static_cast<Real> (1.0) - (xx+yy);

	    return rotationMatrix;
	}

	template <class Real>
	inline void Quaternion<Real>::to3x3Matrix( Matrix3x3<Real>& rotationMatrix ) const
	{

		Real xx = x * x * static_cast<Real> (2.0);
		Real yy = y * y * static_cast<Real> (2.0);
		Real zz = z * z * static_cast<Real> (2.0);

		Real xy = x * y * static_cast<Real> (2.0);
		Real xz = x * z * static_cast<Real> (2.0);
		Real xw = x * w * static_cast<Real> (2.0);

		Real yz = y * z * static_cast<Real> (2.0);
		Real yw = y * w * static_cast<Real> (2.0);

		Real zw = z * w * static_cast<Real> (2.0);

	    rotationMatrix[0].x = static_cast<Real>(1.0) -(yy+zz);
	    rotationMatrix[0].y = xy-zw;
	    rotationMatrix[0].z = xz+yw;

	    rotationMatrix[1].x = xy+zw;
	    rotationMatrix[1].y = static_cast<Real>(1.0) -(xx+zz);
	    rotationMatrix[1].z = yz-xw;

	    rotationMatrix[2].x = xz-yw;
	    rotationMatrix[2].y = yz+xw;
	    rotationMatrix[2].z = static_cast<Real> (1.0) - (xx+yy);
	}

	template <class Real>
	inline void Quaternion<Real>::to4x4Matrix( Matrix4x4<Real>& rotationMatrix ) const
	{
		Real xx = x * x * static_cast<Real> (2.0);
		Real yy = y * y * static_cast<Real> (2.0);
		Real zz = z * z * static_cast<Real> (2.0);

		Real xy = x * y * static_cast<Real> (2.0);
		Real xz = x * z * static_cast<Real> (2.0);
		Real xw = x * w * static_cast<Real> (2.0);

		Real yz = y * z * static_cast<Real> (2.0);
		Real yw = y * w * static_cast<Real> (2.0);

		Real zw = z * w * static_cast<Real> (2.0);

	    rotationMatrix[0].x = static_cast<Real>(1.0) - (yy+zz);
	    rotationMatrix[0].y = xy-zw;
	    rotationMatrix[0].z = xz+yw;

	    rotationMatrix[1].x = xy+zw;
	    rotationMatrix[1].y = static_cast<Real>(1.0) - (xx+zz);
	    rotationMatrix[1].z = yz-xw;

	    rotationMatrix[2].x = xz-yw;
	    rotationMatrix[2].y = yz+xw;
	    rotationMatrix[2].z = static_cast<Real>(1.0) - (xx+yy);

	    rotationMatrix[0].w = rotationMatrix[1].w = rotationMatrix[2].w = static_cast<Real> (0.0);

	    rotationMatrix[3].x = rotationMatrix[3].y = rotationMatrix[3].z = static_cast<Real> (0.0);
	    rotationMatrix[3].w = static_cast<Real>(1.0);
	  }

	template <class Real>
	inline Matrix4x4<Real> Quaternion<Real>::to4x4Matrix() const
	{

		Matrix4x4<Real> rotationMatrix;

		Real xx = x * x * static_cast<Real> (2.0);
		Real yy = y * y * static_cast<Real> (2.0);
		Real zz = z * z * static_cast<Real> (2.0);

		Real xy = x * y * static_cast<Real> (2.0);
		Real xz = x * z * static_cast<Real> (2.0);
		Real xw = x * w * static_cast<Real> (2.0);

		Real yz = y * z * static_cast<Real> (2.0);
		Real yw = y * w * static_cast<Real> (2.0);

		Real zw = z * w * static_cast<Real> (2.0);

	    rotationMatrix[0].x = static_cast<Real>(1.0) - (yy+zz);
	    rotationMatrix[0].y = xy-zw;
	    rotationMatrix[0].z = xz+yw;

	    rotationMatrix[1].x = xy+zw;
	    rotationMatrix[1].y = static_cast<Real>(1.0) - (xx+zz);
	    rotationMatrix[1].z = yz-xw;

	    rotationMatrix[2].x = xz-yw;
	    rotationMatrix[2].y = yz+xw;
	    rotationMatrix[2].z = static_cast<Real>(1.0) - (xx+yy);

	    rotationMatrix[0].w = rotationMatrix[1].w = rotationMatrix[2].w = static_cast<Real> (0.0);

	    rotationMatrix[3].x = rotationMatrix[3].y = rotationMatrix[3].z = static_cast<Real> (0.0);
	    rotationMatrix[3].w = static_cast<Real>(1.0);

	    return (rotationMatrix);
	  }

	template <class Real>
	inline void Quaternion<Real>::to4x4Matrix( Real * rotationMatrix ) const
	{

		Real xx = x * x * static_cast<Real> (2.0);
		Real yy = y * y * static_cast<Real> (2.0);
		Real zz = z * z * static_cast<Real> (2.0);

		Real xy = x * y * static_cast<Real> (2.0);
		Real xz = x * z * static_cast<Real> (2.0);
		Real xw = x * w * static_cast<Real> (2.0);

		Real yz = y * z * static_cast<Real> (2.0);
		Real yw = y * w * static_cast<Real> (2.0);

		Real zw = z * w * static_cast<Real> (2.0);

	    rotationMatrix[0] = static_cast<Real>(1.0) - (yy+zz);
	    rotationMatrix[1] = xy-zw;
	    rotationMatrix[2] = xz+yw;

	    rotationMatrix[4] = xy+zw;
	    rotationMatrix[5] = static_cast<Real>(1.0) - (xx+zz);
	    rotationMatrix[6] = yz-xw;

	    rotationMatrix[8] = xz-yw;
	    rotationMatrix[9] = yz+xw;
	    rotationMatrix[10] = static_cast<Real>(1.0) - (xx+yy);

	    
	    rotationMatrix[11] = static_cast<Real>(1.0);

	    rotationMatrix[3]  = rotationMatrix[7]  = rotationMatrix[11] = static_cast<Real> (0.0);
	    rotationMatrix[12] = rotationMatrix[13] = rotationMatrix[14] = static_cast<Real> (0.0);

	    rotationMatrix[15] = static_cast<Real> (1.0);

	 }




	//----------------------------------------------------------------------------
	template <class Real>
	inline void  Quaternion<Real>::fromRotationMatrix( const Matrix4x4<Real>& rotationMatrix )
	{
		  const Real onePlusTrace = static_cast<Real> (1.0)  + rotationMatrix[0][0] + rotationMatrix[1][1] + rotationMatrix[2][2];

		  if (onePlusTrace > 1E-5)
		    {
		      // Direct computation
		      const Real s = sqrt(onePlusTrace) * static_cast<Real> (2.0) ;
		      x = (rotationMatrix[2][1] - rotationMatrix[1][2]) / s;
		      y = (rotationMatrix[0][2] - rotationMatrix[2][0]) / s;
		      z = (rotationMatrix[1][0] - rotationMatrix[0][1]) / s;
		      w = static_cast<Real> (0.25)  * s;
		    }
		  else
		    {
		      // Computation depends on major diagonal term
		      if ((rotationMatrix[0][0] > rotationMatrix[1][1])&(rotationMatrix[0][0] > rotationMatrix[2][2]))
			{
			  const Real s = sqrt(1.0 + rotationMatrix[0][0] - rotationMatrix[1][1] - rotationMatrix[2][2]) * static_cast<Real> (2.0) ;
			  x = static_cast<Real> (0.25) * s;
			  y = (rotationMatrix[0][1] + rotationMatrix[1][0]) / s;
			  z = (rotationMatrix[0][2] + rotationMatrix[2][0]) / s;
			  w = (rotationMatrix[1][2] - rotationMatrix[2][1]) / s;
			}
		      else
			if (rotationMatrix[1][1] > rotationMatrix[2][2])
			  {
			    const Real s = sqrt(1.0 + rotationMatrix[1][1] - rotationMatrix[0][0] - rotationMatrix[2][2]) * static_cast<Real> (2.0) ;
			    x = (rotationMatrix[0][1] + rotationMatrix[1][0]) / s;
			    y = static_cast<Real> (0.25) * s;
			    z = (rotationMatrix[1][2] + rotationMatrix[2][1]) / s;
			    w = (rotationMatrix[0][2] - rotationMatrix[2][0]) / s;
			  }
			else
			  {
			    const Real s = sqrt(1.0 + rotationMatrix[2][2] - rotationMatrix[0][0] - rotationMatrix[1][1]) * static_cast<Real> (2.0) ;
			    x = (rotationMatrix[0][2] + rotationMatrix[2][0]) / s;
			    y = (rotationMatrix[1][2] + rotationMatrix[2][1]) / s;
			    z = static_cast<Real> (0.25) * s;
			    w = (rotationMatrix[0][1] - rotationMatrix[1][0]) / s;
			  }
		    }

	 this->normalize();

	}

	template <class Real>
	inline void  Quaternion<Real>::fromAxisAngle( const Vector3<Real> &axis, const Real& degrees )
	{
	    Real halfTheta = static_cast<Real> (Math::kDeg2Rad * (degrees) * 0.5f);

	    Real s = std::sin(halfTheta);

	    w = std::cos(halfTheta);
	    x = axis.x * s;
	    y = axis.y * s;
	    z = axis.z * s;
	}
	template <class Real>
	inline void  Quaternion<Real>::fromHeadPitchRoll( const Real& headDegrees, const Real& pitchDegrees, const Real& rollDegrees)
	{
	    Matrix4x4<Real> m;
	    m.fromHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
	    fromRotationMatrix(m);
	}

	template <class Real>
	inline void  Quaternion<Real>::toAxisAngle( Vector3<Real>& axis, Real& degrees ) const
	{
	    // Converts this quaternion to an axis and an angle.

	    Real sinHalfThetaSq = static_cast< Real >(1) - w * w;

	    // Guard against numerical imprecision and identity quaternions.
	    if (sinHalfThetaSq <= static_cast< Real >(0))
	    {
	        axis[0] = static_cast< Real >(1), axis[1] = axis[2] = static_cast< Real >(0);
	        degrees = static_cast< Real >(0);
	    }
	    else
	    {
	        Real invSinHalfTheta = static_cast< Real >(1) / std::sqrt(sinHalfThetaSq);

	        axis[0] = x * invSinHalfTheta;
	        axis[1] = y * invSinHalfTheta;
	        axis[2] = z * invSinHalfTheta;
	        degrees = Math::kRad2Deg * (2.0f * std::acos(w));
	    }
	}


	// Reference: Stan Melax, Game Programming Gems
	template <class Real>
	inline void  Quaternion<Real>::toRotationArc( Vector3<Real> &u, Vector3<Real> &v )
	{

		u.Normalize();
		v.Normalize();
		Vector3<Real> axis = u ^ v;

		Real d = u*v;
		Real s = std::sqrt((1+d)*2);

		w = s * static_cast<Real> (0.5);
		x = axis.x / s;
		y = axis.y / s;
		z = axis.z / s;

	}


	template <class Real>
	inline  Vector3<Real> operator*( const Quaternion<Real>& quat, const Vector3<Real>& v )
	{
	  return quat.Rotate(v);
	}

	template < class Real>
	inline Quaternion<Real>& Quaternion<Real>::operator*=( const Quaternion<Real>& quat )
	{
	  *this = (*this)*quat;
	  return *this;
	}

	template <class Real>
	inline Quaternion<Real> Quaternion<Real>::operator*( const Real& factor ) const
	{

		return Quaternion<Real>(w * factor, x * factor, y * factor, z * factor);
	}

	//------------------------------------------------------------------------------
	//
	template <class Real>
	inline Quaternion<Real> Quaternion<Real>::operator/( const Real& factor ) const
	{

		return Quaternion<Real>(w / factor, x / factor, y / factor, z / factor);

	}

	template <class Real>
	inline Quaternion<Real>  Quaternion<Real>::operator-() const
	{
		return Quaternion<Real> ( -w , -x, -y, -z );

	}
	// Conjugate
	template <class Real>
	inline Quaternion<Real> Quaternion<Real>::operator~() const
	{
		return Quaternion<Real> (w , -x, -y, -z );
	}

	template <class Real>
	inline Real Quaternion<Real>::length() const
	{
	    return std::sqrt(  x*x +  y*y +  z*z + w*w );
	}

	template <class Real>
	inline Real Quaternion<Real>::norm() const
	{
	    return   ( x*x + y*y +  z*z + w*w ) ;
	}


	template <class Real>
	inline void Quaternion<Real>::normalize()
	{
	    Real lLength = sqrt(  x*x +  y*y +  z*z + w*w );

	    if (lLength > 1e-06f)
	    {
	        Real lInvLength =  static_cast<Real> (1.0) / lLength;

	        w *= lInvLength;
	        x *= lInvLength;
	        y *= lInvLength;
	        z *= lInvLength;
	    }
	    else
	    {
	        w = static_cast<Real> (1.0);
	        x = static_cast<Real> (0.0);
	        y = static_cast<Real> (0.0);
	        z = static_cast<Real> (0.0);
	    }

	}

	template <class Real>
	inline void Quaternion<Real>::identity()
	{
		this->w = static_cast<Real> (1);
		this->x = static_cast<Real> (0);
		this->y = static_cast<Real> (0);
		this->z = static_cast<Real> (0);

	}


	template <class Real>
	inline Quaternion<Real> Quaternion<Real>::normalized()
	{
	    Real lLength = sqrt(  x*x +  y*y +  z*z + w*w );

	    if (lLength > 1e-06f)
	    {
	        Real lInvLength =  static_cast<Real> (1.0) / lLength;

	        return Quaternion<Real> (w * lInvLength, x * lInvLength, y * lInvLength, z * lInvLength );

	    }
	    else
	    {
	    	return Quaternion<Real> ( 1.0, 0.0, 0.0, 0.0);
	    }

	}



	template <class Real>
	inline Quaternion<Real> Quaternion<Real>::inverse() const
	{
		Real lNorm =   x*x +  y*y +  z*z + w*w ;

	    if (lNorm > static_cast<Real> (0.0))
	    {
	        Real lInvNorm = static_cast<Real> (1.0) / lNorm;

	        return Quaternion<Real> (w * lInvNorm, -x * lInvNorm, -y * lInvNorm, -z * lInvNorm );
	    }
	    else
	    {	// Return as a flag error !!
	    	return Quaternion<Real> (1.0, 0.0, 0.0, 0.0);
	    }

	}

	template <class Real>
	inline void  Quaternion<Real>::invert()
	{
		Real lNorm =   x*x +  y*y +  z*z + w*w ;

	    if (lNorm > static_cast<Real> (0.0))
	    {
	        Real lInvNorm = static_cast<Real> (1.0) / lNorm;

	        w *= lInvNorm;
	       -x *= lInvNorm;
	       -y *= lInvNorm;
	       -z *= lInvNorm;

	    }
	    else
	    {
	        w = static_cast<Real> (1.0);
	        x = static_cast<Real> (0.0);
	        y = static_cast<Real> (0.0);
	        z = static_cast<Real> (0.0);
	    }

	}

	template <class Real>
	inline Real Quaternion<Real>::dot ( const Quaternion<Real>& quat ) const
	{
	    Real lScalar = static_cast<Real> (0.0);

	    lScalar +=  w * quat.w +
	    			x * quat.x +
	    			y * quat.y +
	    			z * quat.z ;

	    return lScalar;
	}
	//--

	template<class Real>
	inline std::ostream& operator<<( std::ostream & s, const Quaternion<Real>& quat )
	{
		s << " Quaternion " << " w = " << quat.w << " x = " << quat.x << " ,y = " << quat.y << " ,z = " << quat.z << std::endl;

		return s;
	};

	template <class Real>
	Quaternion<Real>::~Quaternion()
	{}


} /* Celer :: NAMESPACE */

#endif /*QUATERNION_HPP_*/
