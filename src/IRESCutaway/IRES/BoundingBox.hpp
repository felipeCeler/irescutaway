#ifndef BOUNDINGBOX_HPP_
#define BOUNDINGBOX_HPP_

// from Standard Library
#include <vector>
#include <limits>
// Celer Library
#ifdef _WIN32
#include "Eigen\Eigen"  // For math-related stuff.
#else
#include "Eigen/Eigen"
#endif

namespace IRES
{
	/*!
	 *@class BoundingBox.
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

	class BoundingBox
	{
		private:

			Eigen::Vector4f min_;
			Eigen::Vector4f max_;

			/// For the oriented bounding Box;
			Eigen::Vector4f basis_[3];
			float extends[3];

		public:

			BoundingBox ( )
			{
				this->reset();
			}

			BoundingBox ( const BoundingBox& box )
			{
				this->min_ = Eigen::Vector4f ( box.box_min ( ) );
				this->max_ = Eigen::Vector4f ( box.box_max ( ) );
			}

			BoundingBox ( const Eigen::Vector4f& point_min , const Eigen::Vector4f& point_max )
			{
				this->min_ = Eigen::Vector4f ( point_min );
				this->max_ = Eigen::Vector4f ( point_max );
			}

			BoundingBox ( const float& xMin , const float& yMin , const float& zMin , const float& xMax , const float& yMax , const float& zMax )
			{
				this->min_ = Eigen::Vector4f ( xMin , yMin , zMin ,1.0f );
				this->max_ = Eigen::Vector4f ( xMax , yMax , zMax ,1.0f );
			}

			void reset ( )
			{
				// FIXME How to get the real limits of the bounding box using std::numeric_limits
				this->min_ = Eigen::Vector4f (  (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), 1.0f );
				this->max_ = Eigen::Vector4f ( -(std::numeric_limits<float>::max)(),-(std::numeric_limits<float>::max)(),-(std::numeric_limits<float>::max)(), 1.0f );

				basis_[0] = Eigen::Vector4f::UnitX();
				basis_[0] = Eigen::Vector4f::UnitY();
				basis_[0] = Eigen::Vector4f::UnitZ();

				extends[0] = 1.0f;
				extends[1] = 1.0f;
				extends[2] = 1.0f;

			}

			void fromPointCloud(  std::vector<Eigen::Vector4f >::const_iterator new_point_begin,  std::vector<Eigen::Vector4f >::const_iterator new_point_end)
			{

				for (  std::vector<Eigen::Vector4f >::const_iterator new_point = new_point_begin; new_point != new_point_end; new_point++)
				{

					 BoundingBox box( ((std::min)) ( min_[0] , (*new_point)[0] ) ,
								 (std::min) ( min_[1] , (*new_point)[1] ) ,
								 (std::min) ( min_[2] , (*new_point)[2] ) ,
								 (std::max) ( max_[0] , (*new_point)[0] ) ,
								 (std::max) ( max_[1] , (*new_point)[1] ) ,
								 (std::max) ( max_[2] , (*new_point)[2] ) );

//						std::cout << min( );
//						std::cout << max( );
//						std::cout << " new_point " << *new_point;

					 *this = *this + box;
				}

			}


			float diagonal ( ) const
			{
				return ( (this->min_ - this->max_).norm() );
			}

			Eigen::Vector4f center ( ) const
			{
				return Eigen::Vector4f ( (max_ + min_) * static_cast<float>( 0.5 ) );
			}

			inline const Eigen::Vector4f& box_min ( ) const
			{
				return ( this->min_ );
			}

			inline const Eigen::Vector4f& box_max ( ) const
			{
				return ( this->max_ );
			}

			inline bool operator== ( const BoundingBox& box ) const
			{
				return ( box_min ( ) == box.box_min ( ) && box_max ( ) == box.box_max ( ) );
			}

			inline bool operator!= ( const BoundingBox& box ) const
			{
				return ! ( box == *this );
			}

			inline BoundingBox& operator= ( const BoundingBox& box )
			{
				this->min_ = box.box_min ( );
				this->max_ = box.box_max ( );

				return ( *this );
			}

			inline BoundingBox operator+ ( const BoundingBox& box ) const
			{
				return BoundingBox (  (std::min) ( min_[0] , box.box_min ( )[0] ) ,
							     (std::min) ( min_[1] , box.box_min ( )[1] ) ,
							     (std::min) ( min_[2] , box.box_min ( )[2] ) ,
							     (std::max) ( max_[0] , box.box_max ( )[0] ) ,
							     (std::max) ( max_[1] , box.box_max ( )[1] ) ,
							     (std::max) ( max_[2] , box.box_max ( )[2] ) );
			}

			inline BoundingBox operator+ ( const Eigen::Vector4f& new_point ) const
			{
				return BoundingBox (  (std::min) ( min_[0] , new_point[0] ) ,
							     (std::min) ( min_[1] , new_point[1] ) ,
							     (std::min) ( min_[2] , new_point[2] ) ,
							     (std::max) ( max_[0] , new_point[0] ) ,
							     (std::max) ( max_[1] , new_point[1] ) ,
							     (std::max) ( max_[2] , new_point[2] ) );
			}

			bool intersect ( const Eigen::Vector4f& p ) const
			{
				return ( ( p[0]  >= this->min_[0] ) and ( p[0]  < this->max_[0] ) and
		                         ( p[1]  >= this->min_[1] ) and ( p[1]  < this->max_[1] ) and
		                         ( p[2]  >= this->min_[2] ) and ( p[2]  < this->max_[2] ) );
			}

			bool intersect ( const BoundingBox& box ) const
			{

				
				return ( ( box.box_max( )[0] > this->min_[0] ) && ( box.box_min( )[0] < this->max_[0] ) &&
		                 ( box.box_max( )[1] > this->min_[1] ) && ( box.box_min( )[1] < this->max_[1] ) &&
		                 ( box.box_max( )[2] > this->min_[2] ) && ( box.box_min( )[2] < this->max_[2] ) );
			}

			~BoundingBox ( )
			{

			}
	};

}/* IRES :: NAMESPACE */

#endif /*BOUNDINGBOX_HPP_*/


//Celer::BoundingBox<float> box;
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
