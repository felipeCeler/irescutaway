#ifndef CELER_BOUNDINGBOX3_HPP_
#define CELER_BOUNDINGBOX3_HPP_

#include <Celer/Core/Geometry/Math/Point3.hpp>

namespace Celer
{

/*!
*@class BoundingBox3.
*@brief Class that represent a Box in 3D.
*@details coming soon , but ... coming soon  ... wait ¬¬.
*@author Felipe Moura.
*@version 1.0.
*@date 25-Feb-2008.
*@todo Give a model, polygon, ellipse ... and so one, with it coords point creat a Box.
*/

template < class Real>
class BoundingBox3
{
private:

	Point3<Real> mMin;
	Point3<Real> mMax;

public:


	BoundingBox3 ( )
	{
		this->mMin = Point3<Real> ();
		this->mMax = Point3<Real> ();
	};

	BoundingBox3 ( const BoundingBox3<Real>& box )
	{
		this->mMin = Point3<Real> ( box.Min() );
		this->mMax = Point3<Real> ( box.Max() );
	};

	BoundingBox3 ( const Point3<Real>& pointMin, const Point3<Real>& pointMax )
	{
		this->mMin = Point3<Real> ( pointMin );
		this->mMax = Point3<Real> ( pointMax );
	};

	BoundingBox3 ( const Real& xMin, const Real& yMin, const Real& zMin,
				   const Real& xMax, const Real& yMax, const Real& zMax )
	{
		this->mMin = Point3<Real> ( xMin,yMin,zMin );
		this->mMax = Point3<Real> ( xMax,yMax,zMax );
	};

	Real Center() const
	{
		Vector3<Real>( 0.5*(mMax.x - mMin.x) , 0.5*(mMax.y - mMin.y), 0.5*(mMax.z - mMin.z)  );
	}

	inline const Real& xMin() const
	{
		return ( this->mMin.x );
	};

	inline const Real& yMin() const
	{
		return ( this->mMin.y );
	};

	inline const Real& zMin() const
	{
		return ( this->mMin.z );
	};

	inline const Real& xMax() const
	{
		return ( this->mMax.x );
	};

	inline const Real& yMax() const
	{
		return ( this->mMax.y );
	};

	inline const Real& zMax() const
	{
		return ( this->mMax.z );
	};

	inline const Point3<Real>& Min() const
	{
		return ( this->mMin);
	};

	inline const Point3<Real>& Max() const
	{
		return ( this->mMax );
	};


	inline bool operator==(const BoundingBox3<Real>& box) const
	{
	  return ( xMin() == box.xMin() and xMax() == box.xMax() and
	      	   yMin() == box.yMin() and yMax() == box.yMax() and
	           zMin() == box.zMin() and zMax() == box.zMax() );
	};

	inline bool operator!=(const BoundingBox3<Real>& box) const
	{
	  return  ! (box == *this);
	};

	inline BoundingBox3<Real>& operator= (const BoundingBox3<Real>& box)
	{
		this->mMin = box.Min();
		this->mMax = box.Max();

		return ( *this );
	};

	inline BoundingBox3<Real> operator+(const BoundingBox3<Real>& box) const
	{
	  return BoundingBox3<Real>((std::min)(xMin(), box.xMin()),
	                	  		(std::min)(yMin(), box.yMin()),
	                	  		(std::min)(zMin(), box.zMin()),
	                	  		(std::max)(xMax(), box.xMax()),
	                	  		(std::max)(yMax(), box.yMax()),
	                	  		(std::max)(zMax(), box.zMax()));
	};


	bool Intersect (const Point3<Real>& p) const
	{
	    return ( ( p.x() >= this->xMin() ) and ( p.x() < this->xMax() ) and
	             ( p.y() >= this->yMin() ) and ( p.y() < this->yMax() ) and
	             ( p.z() >= this->zMin() ) and ( p.z() < this->zMax() )   );
	}



	virtual ~BoundingBox3(){};
};

}/* Celer :: NAMESPACE */

#endif /*BOUNDINGBOX3_HPP_*/
