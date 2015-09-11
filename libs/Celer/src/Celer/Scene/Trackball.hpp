#ifndef TRACKBALL_HPP_
#define TRACKBALL_HPP_

#ifdef _WIN32 // _WIN32 is defined by many compilers available for the Windows operating system, but not by others.
#include <windows.h>
#endif

#include <GL/gl.h>
#include <Celer/Core/Geometry/Math/Quaternion.hpp>
#include <Celer/Core/Geometry/Math/Matrix4x4.hpp>

#define  INVSQRT2 1.0f/std::sqrt(2.0)

namespace Celer
{
template < class Real>	class Trackball
	{
	private:

	    int width, height;

	    Celer::Quaternion<Real> mOrientation;
	    Celer::Quaternion<Real> startOrientation;

	    Celer::Vector3<Real> startVector;
	    Celer::Vector3<Real> endVector;

	    void MapToSphere(Celer::Vector3<Real> &v);

	  public:
	    Trackball(int w,  int h):width( w ), height( h ) {}

	    Trackball() {}

	    void SetBounds (int pw, int ph)
	    {
	    	width = pw;
	    	height = ph;
	    }

	    void BeginTracking(int x, int y);
	    void Reset () { mOrientation.Identity(); }
	    void Tracking(int x, int y);
	    const Celer::Quaternion<Real>& Orientation() { return mOrientation; }
	    Celer::Matrix4x4<Real> To4x4RotationMatrix();
	    void ToOpenGL();
    Celer::Vector3<Real> getEndVector() const
    {
        return endVector;
    }



};//End Interface


	template < class Real>
	void Trackball<Real>::MapToSphere(Celer::Vector3<Real> &v)
	{
		Real len2;

		v[0] = (2*v.x)/Real(width-1)-1;
		v[1] = 1-(2*v.y)/Real(height-1);
		v[2] = 0;
		if((len2 = v.LengthSqr())<INVSQRT2)
			v[2] = std::sqrt(1.0f-len2); // We are on the sphere
		else
			v[2] = 1.0f/(2*std::sqrt(len2)); // On the hyperbole
	}

	template < class Real>
	void Trackball<Real>::Tracking(int x, int y)
	{

		Celer::Quaternion<Real> q;
		Celer::Vector3<Real> endVector(Real(x), Real(y), 0);

		MapToSphere(endVector);
		q.ToRotationArc(startVector, endVector);
		mOrientation = q * startOrientation;

		//mOrientation.Normalize();
	}

	template < class Real>
	void Trackball<Real>::BeginTracking(int x, int y)
	{
		startVector = Celer::Vector3<Real>(Real(x), Real(y), 0);
		startOrientation = mOrientation;
		MapToSphere(startVector);

	}
	template < class Real>
	Celer::Matrix4x4<Real> Trackball<Real>::To4x4RotationMatrix()
	{
		return mOrientation.To4x4Matrix();
	}


	template < class Real>
	void Trackball<Real>::ToOpenGL()
	{
		Real m[16];


  		//Celer::Quaternion<float> cameraRotation = ~mTrackball.orientation();

        // Transform vectors based on camera's rotation matrix
//      mUp = mOrientation.Rotate(Vector3::UNIT_Y);
//
//      mEyes = mOrientation.Rotate(Vector3::UNIT_Z);
//
//      mEyes =  (mEyes * mZoomRadius) - mPosition;

		Celer::Matrix4x4<Real> m_viewMatrix = mOrientation.To4x4Matrix();

		Celer::Vector3<Real> m_xAxis(m_viewMatrix[0][0],m_viewMatrix[1][0],m_viewMatrix[2][0]);

		Celer::Vector3<Real> m_yAxis(m_viewMatrix[0][1],m_viewMatrix[1][1],m_viewMatrix[2][1]);

		Celer::Vector3<Real> m_zAxis(m_viewMatrix[0][2],m_viewMatrix[1][2],m_viewMatrix[2][2]);

		Celer::Vector3<Real> m_cameraPos = Celer::Vector3<Real>(0.0,0.0,-10.0) + m_zAxis;
		m_viewMatrix[3][0] = -m_xAxis* (m_cameraPos);
		m_viewMatrix[3][1] = -m_yAxis* (m_cameraPos);
		m_viewMatrix[3][2] = -m_zAxis* (m_cameraPos);

		mOrientation.To4x4Matrix(m);
		
		glLoadMatrixf(m_viewMatrix.ToRealPtr());
		glLoadIdentity();

	}

} /* Celer :: NAMESPACE */

#endif /*TRACKBALL_HPP_*/
