#include <Celer/Core/Geometry/Math/Vector3.hpp>

namespace Celer
{
	// FIXME : Specialized template, how to do it ?
	template < > const Vector3<float> Vector3<float>::ZERO ( 0.0f, 0.0f , 0.0f );

	template < > const Vector3<float> Vector3<float>::UNIT_X ( 1.0f , 0.0f , 0.0f );
	template < > const Vector3<float> Vector3<float>::UNIT_Y ( 0.0f , 1.0f , 0.0f );
	template < > const Vector3<float> Vector3<float>::UNIT_Z ( 0.0f , 0.0f , 1.0f );

	template < > const Vector3<float> Vector3<float>::UNIT ( 1.0f , 1.0f , 1.0f );

	// double

	template < > const Vector3<double> Vector3<double>::ZERO ( 0.0f, 0.0f , 0.0f );

	template < > const Vector3<double> Vector3<double>::UNIT_X ( 1.0f , 0.0f , 0.0f );
	template < > const Vector3<double> Vector3<double>::UNIT_Y ( 0.0f , 1.0f , 0.0f );
	template < > const Vector3<double> Vector3<double>::UNIT_Z ( 0.0f , 0.0f , 1.0f );

	template < > const Vector3<double> Vector3<double>::UNIT ( 1.0f , 1.0f , 1.0f );
}
