#ifndef CELER_BOUNDINGBOX3_HPP_
#define CELER_BOUNDINGBOX3_HPP_

// from Standard Library
#include <vector>
#include <limits>
// Celer Library
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Vector4.hpp>

namespace Celer
{

	/*!
	 *@class BoundingBox3.
	 *@brief Class that represent a Box in 3D.
	 *@details coming soon , but ... coming soon  ... wait ¬¬.
	 *@author Felipe Moura.
	 *@version 1.0.
	 *@date 25-Feb-2008.
	 *@todo Give a model, polygon, ellipse ... and so one, with its coords point creat a Box.
	 */

	// Box ///////////////////////////////////////////////////////////////////////
	//    * ------*
	//   /|      /|
	//  *-----max
	//  | |     | |
	//  | min ----*
	//  |/      |/
	//  * ------*

	template < class Real >
	class BoundingBox3
	{
		private:

			Celer::Vector3<Real> min_;
			Celer::Vector3<Real> max_;

			/// For the oriented bounding Box;
			Celer::Vector3<Real> basis_[3];
			Real extends;

		public:

			BoundingBox3 ( )
			{
				this->reset();
			}

			BoundingBox3 ( const BoundingBox3<Real>& box )
			{
				this->min_ = Celer::Vector3<Real> ( box.box_min ( ) );
				this->max_ = Celer::Vector3<Real> ( box.box_max ( ) );
			}

			BoundingBox3 ( const Celer::Vector3<Real>& point_min , const Celer::Vector3<Real>& point_max )
			{
				this->min_ = Celer::Vector3<Real> ( point_min );
				this->max_ = Celer::Vector3<Real> ( point_max );
			}

			BoundingBox3 ( const Real& xMin , const Real& yMin , const Real& zMin , const Real& xMax , const Real& yMax , const Real& zMax )
			{
				this->min_ = Celer::Vector3<Real> ( xMin , yMin , zMin );
				this->max_ = Celer::Vector3<Real> ( xMax , yMax , zMax );
			}

			void reset ( )
			{
				// FIXME How to get the real limits of the bounding box using std::numeric_limits
				this->min_ = Celer::Vector3<Real> (  std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max() );
				this->max_ = Celer::Vector3<Real> ( -std::numeric_limits<Real>::max(),-std::numeric_limits<Real>::max(),-std::numeric_limits<Real>::max() );

				basis_[0] = Celer::Vector3<Real>::UNIT_X;
				basis_[0] = Celer::Vector3<Real>::UNIT_Y;
				basis_[0] = Celer::Vector3<Real>::UNIT_Z;

			}

			void fromPointCloud( typename std::vector<Celer::Vector3<Real> >::const_iterator new_point_begin, typename std::vector<Celer::Vector3<Real> >::const_iterator new_point_end)
			{

				for ( typename std::vector<Celer::Vector3<Real> >::const_iterator new_point = new_point_begin; new_point != new_point_end; new_point++)
				{

					 BoundingBox3<Real> box( std::min ( min_.x , new_point->x ) ,
								 std::min ( min_.y , new_point->y ) ,
								 std::min ( min_.z , new_point->z ) ,
								 std::max ( max_.x , new_point->x ) ,
								 std::max ( max_.y , new_point->y ) ,
								 std::max ( max_.z , new_point->z ) );

//						std::cout << min( );
//						std::cout << max( );
//						std::cout << " new_point " << *new_point;

					 *this = *this + box;
				}

			}

			void fromPointCloud( typename std::vector<Celer::Vector4<Real> >::const_iterator new_point_begin, typename std::vector<Celer::Vector4<Real> >::const_iterator new_point_end)
			{

				for ( typename std::vector<Celer::Vector4<Real> >::const_iterator new_point = new_point_begin; new_point != new_point_end; new_point++)
				{

					 BoundingBox3<Real> box( std::min ( min_.x , new_point->x ) ,
								 std::min ( min_.y , new_point->y ) ,
								 std::min ( min_.z , new_point->z ) ,
								 std::max ( max_.x , new_point->x ) ,
								 std::max ( max_.y , new_point->y ) ,
								 std::max ( max_.z , new_point->z ) );

//						std::cout << min( );
//						std::cout << max( );
//						std::cout << " new_point " << *new_point;

					 *this = *this + box;
				}

			}

			void fromPointCloud( const typename std::vector<Celer::Vector4<Real> >& points,
			                     const Celer::Vector3<Real>& first_basis,
			                     const Celer::Vector3<Real>& second_basis,
			                     const Celer::Vector3<Real>& third_basis )
			{
				// Reference :
				// Geometric Tools, LLC
				// Copyright (c) 1998-2012
				// Distributed under the Boost Software License, Version 1.0.
				// Wm5ContBox3.cpp

				basis_[0] = first_basis;
				basis_[1] = second_basis;
				basis_[2] = third_basis;


				for ( typename std::vector<Celer::Vector4<Real> >::const_iterator new_point = points.begin(); new_point != points.end(); new_point++)
				{

					 BoundingBox3<Real> box( std::min ( min_.x , new_point->x ) ,
								 std::min ( min_.y , new_point->y ) ,
								 std::min ( min_.z , new_point->z ) ,
								 std::max ( max_.x , new_point->x ) ,
								 std::max ( max_.y , new_point->y ) ,
								 std::max ( max_.z , new_point->z ) );

//						std::cout << min( );
//						std::cout << max( );
//						std::cout << " new_point " << *new_point;

					 *this = *this + box;
				}

				Celer::Vector3<Real> diff = points[0].toVector3() - this->center();
				Celer::Vector3<Real> pmin ( ( diff * basis_[0] ) , ( diff * basis_[1] ) , (diff * basis_[2]) );
				Celer::Vector3<Real> pmax = pmin;


				for ( typename std::vector<Celer::Vector4<Real> >::const_iterator new_point = points.begin(); new_point != points.end(); new_point++)
				{
//					diff = points[i] - box.Center;
//					for ( int j = 0; j < 3; ++j )
//					{
//						Real dot = diff.Dot ( box.Axis[j] );
//						if ( dot < pmin[j] )
//						{
//							pmin[j] = dot;
//						}
//						else if ( dot > pmax[j] )
//						{
//							pmax[j] = dot;
//						}
//					}

				}


//				for ( int i = 1; i < numPoints; ++i )
//				{
//				}

//				min_ += ( ( (Real) 0.5 ) * ( pmin[0] + pmax[0] ) ) * box.Axis[0] + ( ( (Real) 0.5 ) * ( pmin[1] + pmax[1] ) ) * box.Axis[1] + ( ( (Real) 0.5 ) * ( pmin[2] + pmax[2] ) ) * box.Axis[2];

			}


			Real diagonal ( ) const
			{
				return ( this->min_.length ( this->max_ ) );
			}

			Celer::Vector3<Real> center ( ) const
			{
				return Celer::Vector3<Real> ( (max_ + min_) * static_cast<Real>( 0.5 ) );
			}

			inline const Celer::Vector3<Real>& box_min ( ) const
			{
				return ( this->min_ );
			}

			inline const Celer::Vector3<Real>& box_max ( ) const
			{
				return ( this->max_ );
			}

			inline bool operator== ( const BoundingBox3<Real>& box ) const
			{
				return ( box_min ( ) == box.box_min ( ) and box_max ( ) == box.box_max ( ) );
			}

			inline bool operator!= ( const BoundingBox3<Real>& box ) const
			{
				return ! ( box == *this );
			}

			inline BoundingBox3<Real>& operator= ( const BoundingBox3<Real>& box )
			{
				this->min_ = box.box_min ( );
				this->max_ = box.box_max ( );

				return ( *this );
			}

			inline BoundingBox3<Real> operator+ ( const BoundingBox3<Real>& box ) const
			{
				return BoundingBox3<Real> (  std::min ( min_.x , box.box_min ( ).x ) ,
							     std::min ( min_.y , box.box_min ( ).y ) ,
							     std::min ( min_.z , box.box_min ( ).z ) ,
							     std::max ( max_.x , box.box_max ( ).x ) ,
							     std::max ( max_.y , box.box_max ( ).y ) ,
							     std::max ( max_.z , box.box_max ( ).z ) );
			}

			inline BoundingBox3<Real> operator+ ( const Celer::Vector3<Real>& new_point ) const
			{
				return BoundingBox3<Real> (  std::min ( min_.x , new_point.x ) ,
							     std::min ( min_.y , new_point.y ) ,
							     std::min ( min_.z , new_point.z ) ,
							     std::max ( max_.x , new_point.x ) ,
							     std::max ( max_.y , new_point.y ) ,
							     std::max ( max_.z , new_point.z ) );
			}

			inline BoundingBox3<Real> operator+ ( const Celer::Vector4<Real>& new_point ) const
			{
				return BoundingBox3<Real> (  std::min ( min_.x , new_point.x ) ,
							     std::min ( min_.y , new_point.y ) ,
							     std::min ( min_.z , new_point.z ) ,
							     std::max ( max_.x , new_point.x ) ,
							     std::max ( max_.y , new_point.y ) ,
							     std::max ( max_.z , new_point.z ) );
			}

			bool intersect ( const Celer::Vector3<Real>& p ) const
			{
				return ( ( p.x ( ) >= this->min_.x ) and ( p.x ( ) < this->max_.x ) and
		                         ( p.y ( ) >= this->min_.y ) and ( p.y ( ) < this->max_.y ) and
		                         ( p.z ( ) >= this->min_.z ) and ( p.z ( ) < this->max_.z ) );
			}

			bool intersect ( const Celer::BoundingBox3<Real>& box ) const
			{

				
				return ( ( box.box_max( ).x > this->min_.x ) && ( box.box_min( ).x < this->max_.x ) &&
		                 ( box.box_max( ).y > this->min_.y ) && ( box.box_min( ).y < this->max_.y ) &&
		                 ( box.box_max( ).z > this->min_.z ) && ( box.box_min( ).z < this->max_.z ) );
			}


			~BoundingBox3 ( )
			{

			}
	};

}/* Celer :: NAMESPACE */

#endif /*BOUNDINGBOX3_HPP_*/


//Celer::BoundingBox3<float> box;
//
//Celer::Vector3< float > v[9];
//
//v[0] = Celer::Vector3<float> ( 1.f , 0.f , -1.f );
//v[1] = Celer::Vector3<float> ( 1.f , 0.f , 1.f );
//v[2] = Celer::Vector3<float> ( -1.f , 0.f , 1.f );
//v[3] = Celer::Vector3<float> ( -1.f , 0.f , -1.f );
//
//v[4] = Celer::Vector3<float> ( 1.f , 1.f , -1.f );
//v[5] = Celer::Vector3<float> ( 1.f , 1.f , 1.f );
//v[6] = Celer::Vector3<float> ( -1.f , 1.f , 1.f );
//v[7] = Celer::Vector3<float> ( -1.f , 1.f , -1.f );
//
//v[8] = Celer::Vector3<float> ( 0.f , 2.f , 0.f );
//
//for (int var = 0; var < 9 ; ++var)
//{
//	box = box + v[var];
//}
//
//std::cout << "box center : " << box.center( ) << std::endl;
//
//std::cout << "box min : " << box.min( ) << std::endl;
//std::cout << "box max : " << box.max( ) << std::endl;
