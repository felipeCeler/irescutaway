#ifndef LAL_CAMERA_HPP_
#define LAL_CAMERA_HPP_

#include <iostream>

#include <Celer/Core/Geometry/Math/Matrix4x4.hpp>
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Vector4.hpp>
#include <Celer/Core/Geometry/Math/Vector2.hpp>
#include <Celer/Core/Geometry/Math/Quaternion.hpp>

/*!
 *@class Camera
 *@brief Class that represent a dense 4x4 Matrix and Homogeneous Coordinate ..
 *@details coming soon , but ... coming soon  ... wait ¬¬.
 *@author Felipe Moura.
 *@version 0.1.0
 *@todo OpenGL interface and a C style array, Why MakeView ans MakeProjection dont need The Transpose?
 *@todo Não deu certo integrar por que minha nova 3x3 e 4x4 matrix estavam colum major , por isso dava dando bugs ...
 *@por que eu tava armazenando em 16 variáveis e não mais nuam [][]  , e tava ido direto para colum major
 *@ resolvi o reverso depois de por assim (worldAhead * mZoomRadius) - mWorldLookAt ;
 */

// WARNING: makeCurrent must be called by every calling method
namespace Celer
{

	template < class Real >
	class Camera
	{


		public:
			enum CameraBehavior
			{
				FIRST_PERSON, FLIGHT, REVOLVE_AROUND_MODE
			};

		private:

			/// Camera View - Orientation of The Camera
			Celer::Vector3<Real> 		initalPosition_;

			Celer::Quaternion<Real> 	orientation_;
			CameraBehavior 			behavior_;
			Real 				accumPitchDegrees_;
			Real 				pitchDegrees_;
			Real 				headingDegrees_;
			Real 				rollDegrees_;

			/// Camera View - Location of The Camera
			Celer::Vector3<Real> 		target_; 	// Where the camera is focused (Point).
			Celer::Vector3<Real> 		viewDirection_; // Where the camera looked at  (Vector).
			Celer::Vector3<Real> 		position_; 	// Camera position.
			Celer::Vector3<Real> 		up_;		// Camera Orientation.
			Real				offset_;	// The distance from the camera to the target point.
									// Used by CameraBehavior::REVOLVE_AROUND_MODE

			//
			Celer::Vector3<Real> 		xAxis_;
			Celer::Vector3<Real> 		yAxis_;
			Celer::Vector3<Real> 		zAxis_;

			Celer::Matrix4x4<Real> 		viewMatrix_;

			// TODO Take this part to a new Class. Celer::Frustum !!
			// Camera will be only for View !
			// Pespective Projection
			Real 				fieldOfView_;
			Real 				aspectRatio_;
			Real 				perspectiveNearPlane_;
			Real 				perspectiveFarPlane_;
			Celer::Matrix4x4<Real> 		perspectiveProjectionMatrix_;

			// Orthographic
			Real 				left_;
			Real 				right_;
			Real 				bottom_;
			Real 				top_;
			Real 				orthographicNearPlane_;
			Real 				orthographicFarPlane_;
			Celer::Matrix4x4<Real> 		orthographicProjectionMatrix_;



			// User Interaction
			Real 				width_;
			Real 				height_;

			Real 				zoomRadius_;
			Real 				minRadius_;
			Real 				maxRadius_;

			Celer::Vector2<int> 		mouseLockedPosition_;
			Celer::Vector2<int> 		mousePosition_;
			bool 				mouseLocked_;

		public:

						Camera 				( );
			void 			getViewport 			( unsigned int viewport[4] ) const;
			void 			setBehavior 			( enum CameraBehavior p );
			/// View Attributes
			Celer::Vector3<Real> 	viewDirection 			( );
			void 			setViewDirection 		( const Celer::Vector3<Real>& pviewDirection );
			Celer::Vector3<Real> 	position 			( );
			void 			setPosition 			( const Celer::Vector3<Real>& pPosition );
			Celer::Vector3<Real> 	target 				( ) const;
			void 			setTarget 			( const Celer::Vector3<Real>& lookAtThis );
			Celer::Vector3<Real> 	UpVector			( );
			void 			setUpVector			( const Celer::Vector3<Real>& pUp );
			void 			setOffset			( const Real& new_offset );
			template < class OtherReal >
			void 			setOffset			( const OtherReal& new_offset );


			Celer::Quaternion<Real> orientation                     ( ) const;
			Celer::Matrix4x4<Real> 	normalMatrix 			( );
			Celer::Matrix4x4<Real> 	viewMatrix 			( );
			Celer::Matrix4x4<Real> 	viewMatrix			( Celer::Vector3<Real>& pPosition ,
										  Celer::Vector3<Real>& pFocus ,
										  Celer::Vector3<Real>& pUp );
			void 			computerViewMatrix 		( );

			/// Perspective Projection Attributes
			Real 			fieldOfView 			( );
			void 			setFieldOfView 			( const Real& pFieldOfView );
			Real 			aspectRatio 			( );
			void 			setAspectRatio 			( const Real& pAspectRatio );
			void 			setAspectRatio 			( int width, int height );
			Real 			nearPlane 			( );
			void 			setNearPlane 			( const Real& pNearPlane );
			Real 			farPlane 			( );
			void 			setFarPlane 			( const Real& pFarPlane );
			Celer::Matrix4x4<Real> 	perspectiveProjectionMatrix 	( );
			void 			setPerspectiveProjectionMatrix 	( Real pFieldOfView ,
			     			                    		  Real pAspectRatio ,
			     			                    		  Real pNearPlane ,
			     			                    		  Real pFarPlane );

			Celer::Matrix4x4<Real> 	orthographicProjectionMatrix 	( );
			void 			setOrthographicProjectionMatrix ( const Real& p_left ,
			     			                                  const Real& p_right ,
			     			                                  const Real& p_bottom ,
			     			                                  const Real& p_top ,
			     			                                  const Real& p_near ,
			     			                                  const Real& p_far );

			/// Mouse and Keyboard Modifiers
			void 			zoom 				( Real mouseWheelDelta );
			void 			setWindowSize 			( int width , int height );
			void 			moveForward 			( Real Distance );
			void 			moveUpward 			( Real Distance );
			void 			strafeRight 			( Real Distance );
			void 			setMouseInfo		        ( int x , int y );
			void 			setViewByMouse 			( );
			void 			lockMouse 			( bool value );

			void 		       	rotate 				( Real headingDegrees_ ,
			     		              				  Real pitchDegrees_ ,
			     		              				  Real rollDegrees_ );
			void 		       	rotateOrbit 			( Real headingDegrees_ ,
			     		                   			  Real pitchDegrees_ ,
			     		                   			  Real rollDegrees_ );
			void 		       	rotateFirstPerson 		( Real headingDegrees_ ,
			     		       	                  		  Real pitchDegrees_ );
			void 			lookAt 				( );
			void 			reset 				( );

	};

	template< class Real>
	Camera<Real>::Camera()
	{
		zoomRadius_ = static_cast<Real> (1.0);
		minRadius_ = static_cast<Real> (0.1);
		maxRadius_ = static_cast<Real> (2.0);

		initalPosition_ = Celer::Vector3<Real>(0.0,0.0,0.0);

		orientation_ = Celer::Quaternion<Real>(1.0,0.0,0.0,0.0);
		accumPitchDegrees_ = static_cast<Real> (0.0);

		pitchDegrees_ = static_cast<Real> (0.0);
		headingDegrees_ = static_cast<Real> (0.0);
		rollDegrees_ = static_cast<Real> (0.0);

		mouseLockedPosition_.Set(0,0);
		mousePosition_.Set(0,0);
		mouseLocked_ = 0;

		behavior_ = FIRST_PERSON;

		// Point where the camera is focused
		target_ = Celer::Vector3<Real>(0.0,0.0,0.0);
		viewDirection_ = Celer::Vector3<Real>(0.0,0.0,1.0);
		position_ = Celer::Vector3<Real>(0.0,0.0,0.0);
		up_ = Celer::Vector3<Real>(0.0,1.0,0.0);
		offset_ = 7.0f;

		viewMatrix_.identity();

		fieldOfView_ = static_cast<Real> (60.0);
		aspectRatio_ = static_cast<Real> (1.0);

		// Default model being [-1,+1]
		perspectiveNearPlane_ = static_cast<Real> (0.1);
		perspectiveFarPlane_ = static_cast<Real> (100.0);

		orthographicNearPlane_ = static_cast<Real> (-100.0);
		orthographicFarPlane_  = static_cast<Real> (100.0);

		lookAt();

	}



	template < class Real >
	void Camera<Real>::getViewport ( unsigned int viewport[4] ) const
	{
		viewport[0] = 0;
		viewport[1] = 0;
		viewport[2] = width_;
		viewport[3] = height_;
	}

	template < class Real >
	void Camera<Real>::setBehavior ( enum CameraBehavior newBehavior )
	{
		behavior_ = newBehavior;
	}

	template < class Real >
	Celer::Vector3<Real> Camera<Real>::viewDirection ( )
	{
		return viewDirection_;
	}

	template < class Real >
	void Camera<Real>::setViewDirection ( const Celer::Vector3<Real>& newViewDirection )
	{
		viewDirection_ = newViewDirection;
	}

	template < class Real >
	Celer::Vector3<Real> Camera<Real>::position ( )
	{
		return position_;

	}

	template < class Real >
	Celer::Vector3<Real> Camera<Real>::target ( ) const
	{
		return target_;
	}

	template < class Real >
	void Camera<Real>::setTarget ( const Celer::Vector3<Real>& lookAtThis )
	{
		target_ = lookAtThis;
	}

	template < class Real >
	void Camera<Real>::setPosition ( const Celer::Vector3<Real>& newPosition )
	{
		position_ = newPosition;
	}

	template < class Real >
	Celer::Vector3<Real> Camera<Real>::UpVector ( )
	{
		return up_;
	}

	template < class Real >
	void Camera<Real>::setUpVector ( const Celer::Vector3<Real>& newUpVector )
	{
		up_ = newUpVector;
	}

	template < class Real >
	void Camera<Real>::setOffset ( const Real& new_offset )
	{
		offset_ = new_offset;
	}


	template < class Real >
	template < class OtherReal >
	void Camera<Real>::setOffset ( const OtherReal& new_offset )
	{
		offset_ = static_cast<Real> (new_offset);
	}


	template < class Real >
	Real Camera<Real>::fieldOfView ( )
	{
		return fieldOfView_;
	}

	template < class Real >
	void Camera<Real>::setFieldOfView ( const Real& newFieldOfView )
	{
		fieldOfView_ = newFieldOfView;
		setPerspectiveProjectionMatrix ( newFieldOfView , aspectRatio_ , perspectiveNearPlane_ , perspectiveFarPlane_ );
	}

	template < class Real >
	Real Camera<Real>::aspectRatio ( )
	{
		return aspectRatio_;
	}

	template < class Real >
	void Camera<Real>::setAspectRatio ( const Real& newAspectRatio )
	{
		aspectRatio_ = newAspectRatio;
		setPerspectiveProjectionMatrix ( fieldOfView_ , newAspectRatio , perspectiveNearPlane_ , perspectiveFarPlane_ );
	}

	template < class Real >
	void Camera<Real>::setAspectRatio ( int width, int height )
	{
			aspectRatio_ = static_cast<Real>(width)/static_cast<Real>(height);
			setPerspectiveProjectionMatrix ( fieldOfView_ , aspectRatio_ , perspectiveNearPlane_ , perspectiveFarPlane_ );
	}

	template < class Real >
	Real Camera<Real>::nearPlane ( )
	{
		return perspectiveNearPlane_;
	}

	template < class Real >
	void Camera<Real>::setNearPlane ( const Real& pNearPlane )
	{
		setPerspectiveProjectionMatrix ( fieldOfView_ , aspectRatio_ , pNearPlane , perspectiveFarPlane_ );
	}

	template < class Real >
	Real Camera<Real>::farPlane ( )
	{
		return perspectiveFarPlane_;
	}

	template < class Real >
	void Camera<Real>::setFarPlane ( const Real& pFarPlane )
	{
		setPerspectiveProjectionMatrix ( fieldOfView_ , aspectRatio_ , perspectiveNearPlane_ , pFarPlane );
	}

	template < class Real >
	void Camera<Real>::setPerspectiveProjectionMatrix ( Real pFieldOfView , Real pAspectRatio , Real pNearPlane , Real pFarPlane )
	{
		fieldOfView_ = pFieldOfView;

		aspectRatio_ = pAspectRatio;
		perspectiveNearPlane_ = pNearPlane;
		perspectiveFarPlane_ = pFarPlane;

		perspectiveProjectionMatrix_ = Celer::Matrix4x4<Real>::makePerspectiveProjectionMatrix ( fieldOfView_ * zoomRadius_ , aspectRatio_ , perspectiveNearPlane_ , perspectiveFarPlane_ );
	}

//	template < class Real >
//	void Camera<Real>::ComputeProjectionMatrix ( )
//	{
//		perspectiveProjectionMatrix_ = Celer::Matrix4x4<Real>::MakePespectiveProjectionMatrix ( fieldOfView_ * zoomRadius_ , aspectRatio_ , perspectiveNearPlane_ , perspectiveFarPlane_ );
//	}

	template < class Real >
	void Camera<Real>::setOrthographicProjectionMatrix ( const Real& p_left , const Real& p_right , const Real& p_bottom , const Real& p_top , const Real& p_near , const Real& p_far )
	{
		left_ = p_left;
		right_ = p_right;
		bottom_ = p_bottom;
		top_ = p_top;

		orthographicNearPlane_ = p_near;
		orthographicFarPlane_ = p_far;

		orthographicProjectionMatrix_ = Celer::Matrix4x4<Real>::makeOrthographicProjectionMatrix ( left_ , right_ , bottom_ , top_ , orthographicNearPlane_ , orthographicFarPlane_);
	}

//	template < class Real >
//	void Camera<Real>::ComputeOrthographicProjectionMatrix ( )
//	{
//		orthographicProjectionMatrix_ = Celer::Matrix4x4<Real>::MakeOrthographicProjectionMatrix ( left_ , right_ , bottom_ , top_ , orthographicNearPlane_ , orthographicFarPlane_);
//	}

	template < class Real >
	void Camera<Real>::moveForward ( Real Distance )
	{
		position_ += ( viewDirection_ * Distance );
	}

	template < class Real >
	void Camera<Real>::moveUpward ( Real Distance )
	{
		position_ += ( up_ * Distance );
	}

	template < class Real >
	void Camera<Real>::strafeRight ( Real Distance )
	{
		position_ += ( ( viewDirection_ ) ^ up_ ) * Distance;
	}

	template < class Real >
	void Camera<Real>::setMouseInfo ( int x , int y )
	{
		mousePosition_.Set ( x , y );
	}

	template < class Real >
	void Camera<Real>::lockMouse ( bool value )
	{
		if ( value )
		{
			mouseLockedPosition_ = mousePosition_;
		}

		mouseLocked_ = value;
	}

	template < class Real >
	void Camera<Real>::computerViewMatrix ( )
	{

		viewMatrix_ = orientation_.to4x4Matrix ( );

		xAxis_.set ( viewMatrix_[0][0] , viewMatrix_[0][1] , viewMatrix_[0][2] );
		yAxis_.set ( viewMatrix_[1][0] , viewMatrix_[1][1] , viewMatrix_[1][2] );
		zAxis_.set ( viewMatrix_[2][0] , viewMatrix_[2][1] , viewMatrix_[2][2] );

		viewDirection_ = -zAxis_;

		if ( behavior_ == REVOLVE_AROUND_MODE )
		{
			// Calculate the new camera position based on the current
			// orientation. The camera must always maintain the same
			// distance from the target. Use the current offset vector
			// to determine the correct distance from the target.

			position_ = target_ + ( zAxis_ * offset_ );
		}

		viewMatrix_[0][3] -= ( xAxis_ * position_ );
		viewMatrix_[1][3] -= ( yAxis_ * position_ );
		viewMatrix_[2][3] -= ( zAxis_ * position_ );

	}

	template < class Real >
	Celer::Matrix4x4<Real> Camera<Real>::viewMatrix ( )
	{
		return viewMatrix_;
	}

	template < class Real >
	void Camera<Real>::setViewByMouse ( )
	{
		Real pitch = 0.0f, heading = 0.0f;

		if ( ( mousePosition_ == mouseLockedPosition_ ) || !mouseLocked_ )
		{
			return;
		}


		rotate ( -heading , -pitch , 0.0f );

		if ( !mouseLocked_ )
		{
			mouseLockedPosition_ = mousePosition_;
		}


	}
	template < class Real >
	Celer::Quaternion<Real> Camera<Real>::orientation ( ) const
	{
			return orientation_;
	}

	template < class Real >
	Celer::Matrix4x4<Real> Camera<Real>::normalMatrix ( )
	{

		return Celer::Matrix4x4<Real>::makeViewMatrix ( position_ , viewDirection_ , up_ );

	}

	template < class Real >
	Celer::Matrix4x4<Real> Camera<Real>::viewMatrix ( Celer::Vector3<Real>& pPosition , Celer::Vector3<Real>& pviewDirection , Celer::Vector3<Real>& pUp )
	{
		return Celer::Matrix4x4<Real>::makeViewMatrix ( pPosition , pviewDirection , pUp );
	}


	template < class Real >
	Celer::Matrix4x4<Real> Camera<Real>::perspectiveProjectionMatrix ( )
	{
		return perspectiveProjectionMatrix_;
	}

	template < class Real >
	Celer::Matrix4x4<Real> Camera<Real>::orthographicProjectionMatrix ( )
	{
		return orthographicProjectionMatrix_;
	}

	template < class Real >
	void Camera<Real>::rotate ( Real headingDegrees_ , Real pitchDegrees_ , Real rollDegrees_ )
	{
		// Rotates the camera based on its current behavior.
		// Note that not all behaviors support rolling.

		pitchDegrees_ = -pitchDegrees_;
		headingDegrees_ = -headingDegrees_;
		rollDegrees_ = -rollDegrees_;

		switch ( behavior_ )
		{
			case FIRST_PERSON:
				rotateFirstPerson ( headingDegrees_ , pitchDegrees_ );
				break;

			case REVOLVE_AROUND_MODE:
				rotateOrbit ( headingDegrees_ , pitchDegrees_ , rollDegrees_ );
				break;
			default:
				break;
		}

	}

	template < class Real >
	void Camera<Real>::rotateOrbit ( Real headingDegrees_ , Real pitchDegrees_ , Real rollDegrees_ )
	{
		// Implements the rotation logic for the orbit style camera behavior.
		// Roll is ignored for target Y axis orbiting.
		//
		// Briefly here's how this orbit camera implementation works. Switching to
		// the orbit camera behavior via the setBehavior() method will set the
		// camera's orientation to match the orbit target's orientation. Calls to
		// rotateOrbit() will rotate this orientation. To turn this into a third
		// person style view the updateViewMatrix() method will move the camera
		// position back 'm_orbitOffsetDistance' world units along the camera's
		// local z axis from the orbit target's world position.

		Celer::Quaternion<Real> rot;

		if ( headingDegrees_ != 0.0f )
		{
			rot.fromAxisAngle ( Celer::Vector3<Real>::UNIT_Y , headingDegrees_ );
			orientation_ = orientation_ * rot;
		}

		if ( pitchDegrees_ != 0.0f )
		{
			rot.fromAxisAngle ( Celer::Vector3<Real>::UNIT_X , pitchDegrees_ );
			orientation_ = rot * orientation_;
		}
		orientation_.normalize ( );
	}

	template < class Real >
	void Camera<Real>::rotateFirstPerson ( Real headingDegrees_ , Real pitchDegrees_ )
	{
		// Implements the rotation logic for the first person style and
		// spectator style camera behaviors. Roll is ignored.

		accumPitchDegrees_ += pitchDegrees_;

		if ( accumPitchDegrees_ > 90.0f )
		{
			pitchDegrees_ = 90.0f - ( accumPitchDegrees_ - pitchDegrees_ );
			accumPitchDegrees_ = 90.0f;
		}

		if ( accumPitchDegrees_ < -90.0f )
		{
			pitchDegrees_ = -90.0f - ( accumPitchDegrees_ - pitchDegrees_ );
			accumPitchDegrees_ = -90.0f;
		}

		Celer::Quaternion<Real> rot;

		// Rotate camera about the world y axis.
		// Note the order the quaternions are multiplied. That is important!
		if ( headingDegrees_ != 0.0f )
		{
			rot.fromAxisAngle ( Celer::Vector3<Real>::UNIT_Y , headingDegrees_ );
			orientation_ = orientation_ * rot;
		}

		// Rotate camera about its local x axis.
		// Note the order the quaternions are multiplied. That is important!
		if ( pitchDegrees_ != 0.0f )
		{
			rot.fromAxisAngle ( Celer::Vector3<Real>::UNIT_X , pitchDegrees_ );
			orientation_ = rot * orientation_;
		}
		orientation_.normalize ( );
	}

	template < class Real >
	void Camera<Real>::lookAt ( )
	{

		zAxis_ = viewDirection_;
		zAxis_.normalize ( );

		xAxis_ = ( up_ ^ zAxis_ );
		xAxis_.normalize ( );

		yAxis_ = ( zAxis_ ^ xAxis_ );
		yAxis_.normalize ( );

		viewMatrix_[0][0] = xAxis_.x;
		viewMatrix_[0][1] = xAxis_.y;
		viewMatrix_[0][2] = xAxis_.z;
		viewMatrix_[0][3] = - ( xAxis_ * position_ );

		viewMatrix_[1][0] = yAxis_.x;
		viewMatrix_[1][1] = yAxis_.y;
		viewMatrix_[1][2] = yAxis_.z;
		viewMatrix_[1][3] = - ( yAxis_ * position_ );

		viewMatrix_[2][0] = zAxis_.x;
		viewMatrix_[2][1] = zAxis_.y;
		viewMatrix_[2][2] = zAxis_.z;
		viewMatrix_[2][3] = - ( zAxis_ * position_ );

		// Extract the pitch angle from the view matrix.
		accumPitchDegrees_ = Math::kRad2Deg * ( std::asin ( viewMatrix_[2][1] ) );

		orientation_.fromRotationMatrix ( viewMatrix_ );
	}

	template < class Real >
	void Camera<Real>::zoom ( Real mouseWheelDelta )
	{
	     // Change the radius from the camera to the model based on wheel scrolling
	     // zoomRadius_ -= mouseWheelDelta * 0.1f;

	     // std::cout << "Wheel "<< mouseWheelDelta << " zoom  "<< zoomRadius_ <<  std::endl;
	     // zoomRadius_  = std::min<Real> ( maxRadius_, zoomRadius_ );
	     // zoomRadius_  = std::max<Real> ( minRadius_, zoomRadius_ );

	     viewDirection_.z -= 0.2;
	}

	template < class Real >
	void Camera<Real>::setWindowSize ( int width , int height )
	{
		width_ = width;
		height_ = height;

	}

	template < class Real >
	void Camera<Real>::reset ( )
	{
		position_ = initalPosition_;
		orientation_.identity ( );

	}

}/* Celer :: NAMESPACE */

#endif /*CAMERA_HPP_*/
