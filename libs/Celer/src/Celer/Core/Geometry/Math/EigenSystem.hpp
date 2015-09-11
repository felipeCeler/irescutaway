#ifndef CELER_EIGENSYSTEM_HPP_
#define CELER_EIGENSYSTEM_HPP_

#include <cassert>

#include <list>
#include <algorithm>
#include <cmath>

#include <Celer/Core/Geometry/Math/Matrix3x3.hpp>
#include <Celer/Core/Geometry/Math/Vector3.hpp>
#include <Celer/Core/Geometry/Math/Math.hpp>

namespace Celer
{
/*! \brief The EigenSystem class. For now, EigenSystem finds numerical eigenvalues and eigenvectors
 * of a symetric 3x3 matrices
\class EigenSystem EigenSystem.hpp

<h3>Internal representation</h3>

\nosubgrouping */
	template <class Real> class EigenSystem
	{

	public:

		typedef Celer::Vector3<Real> 		Vector3;
		typedef std::list<Vector3>					ListPoint3;
		typedef typename ListPoint3::iterator   	ListPoint3Iterator;


		typedef Celer::Matrix3x3<Real> 	Matrix3x3;

		Real 	  mEigenvalue[3];
		Vector3	  mEigenvector[3];
		Matrix3x3 mCovariance;

		/*! @name Constructors */
		//@{
		/*!  */
		EigenSystem()
		{

		}
		/*!  */
		EigenSystem(const Matrix3x3& rkA)
		{
			mCovariance = rkA;
			EigenDecomposition();
		}
		/*!  */
		EigenSystem(ListPoint3& pPoint3List, const Vector3& pMean)
		{
			CovarianceMatrix (pPoint3List,pMean);
			EigenDecomposition();
			mCurvature = mEigenvalue[0] / (mEigenvalue[0]+mEigenvalue[1]+mEigenvalue[2]);
		}
		//@}
		/*! @name Constructors */
		//@{
		/*!  */
		std::pair<Real,Vector3> MinorAxis()
		{
			return (this->mMinorAxis);
		}

		std::pair<Real,Vector3> MajorAxis()
		{
			return (this->mMajorAxis);
		}

		std::pair<Real,Vector3> Normal()
		{
			return (this->mNormal);
		}

		Real Curvature () const
		{
			return (mCurvature);
		}

		void CovarianceMatrix (ListPoint3& pPoint3List, const Vector3& pMean)
		{

			Real correlationXY = static_cast<Real> (0);
			Real correlationXZ = static_cast<Real> (0);
			Real correlationYZ = static_cast<Real> (0);

			Real correlationXX = static_cast<Real> (0);
			Real correlationYY = static_cast<Real> (0);
			Real correlationZZ = static_cast<Real> (0);

			// Como é 1 e nao 1.0  o resultado é Zero
			//Real N  = static_cast<Real> (1/  pPoint3List.size() );

			Real N  = static_cast<Real> (1.0 / pPoint3List.size());



			for (ListPoint3Iterator it = pPoint3List.begin(); it != pPoint3List.end() ; ++it)
			{
				correlationXX += ( (it->x - pMean.x) * (it->x - pMean.x) );
				correlationYY += ( (it->y - pMean.y) * (it->y - pMean.y) );
				correlationZZ += ( (it->z - pMean.z) * (it->z - pMean.z) );

				correlationXY += ( (it->x - pMean.x) * (it->y - pMean.y) );
				correlationXZ += ( (it->x - pMean.x) * (it->z - pMean.z) );
				correlationYZ += ( (it->y - pMean.y) * (it->z - pMean.z) );

			}


			mCovariance = Matrix3x3(  (N*correlationXX), (N*correlationXY) , (N*correlationXZ),
									  (N*correlationXY), (N*correlationYY) , (N*correlationYZ),
									  (N*correlationXZ), (N*correlationYZ) , (N*correlationZZ) );
		}
		//@}


		/*! @name Eigen decomposition code for symmetric 3x3 matrices */
		//@{
		/*!
		 * Eigen decomposition code for symmetric 3x3 matrices, copied from the public
		 * domain Java Matrix library JAMA.
		 * Symmetric Householder reduction to tridiagonal form.
		 */
		void EigenDecomposition()
		{

			Real e[3];
			Real d[3];

			//  This is derived from the Algol procedures tred2 by
			//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
			//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
			//  Fortran subroutine in EISPACK.

			d[0] = mCovariance[2][0];
			d[1] = mCovariance[2][1];
			d[2] = mCovariance[2][2];

			// Householder reduction to tridiagonal form.

			for (int i = 3-1; i > 0; i--)
			{

				// Scale to avoid under/overflow.

				Real scale = 0.0;
				Real h 	   = 0.0;

				for (int k = 0; k < i; k++)
				{
					scale = scale + fabs(d[k]);
				}

				if (scale == 0.0)
				{

					e[i] = d[i-1];

					for (int j = 0; j < i; j++)
					{
						d[j] = mCovariance[i-1][j];
						mCovariance[i][j] = 0.0;
						mCovariance[j][i] = 0.0;
					}

				}
				else
				{

					// Generate Householder vector.

					for (int k = 0; k < i; k++)
					{
						d[k] /= scale;
						h += d[k] * d[k];
					}

					Real f = d[i-1];
					Real g = sqrt(h);

					if (f > 0)
					{
						g = -g;
					}

					e[i] 	= scale * g;
					h 		= h - f * g;
					d[i-1] 	= f - g;

					for (int j = 0; j < i; j++)
					{
						e[j] = 0.0;
					}

					// Apply similarity transformation to remaining columns.

					for (int j = 0; j < i; j++)
					{

						f 		= d[j];
						mCovariance[j][i] = f;
						g 		= e[j] + mCovariance[j][j] * f;

						for (int k = j+1; k <= i-1; k++)
						{
							g 	 += mCovariance[k][j] * d[k];
							e[k] += mCovariance[k][j] * f;
						}

						e[j] = g;
					}

					f = 0.0;
					for (int j = 0; j < i; j++)
					{
						e[j] /= h;
						f 	 += e[j] * d[j];
					}
					Real hh = f / (h + h);
					for (int j = 0; j < i; j++)
					{
						e[j] -= hh * d[j];
					}
					for (int j = 0; j < i; j++)
					{
						f = d[j];
						g = e[j];
						for (int k = j; k <= i-1; k++)
						{
							mCovariance[k][j] -= (f * e[k] + g * d[k]);
						}
						d[j] 	= mCovariance[i-1][j];
						mCovariance[i][j] = 0.0;
					}
				}

				d[i] = h;
			}

			// Accumulate transformations.

			for (int i = 0; i < 3-1; i++)
			{
				mCovariance[3-1][i] = mCovariance[i][i];
				mCovariance[i][i] = 1.0;
				Real h = d[i+1];
				if (h != 0.0) {
					for (int k = 0; k <= i; k++) {
						d[k] = mCovariance[k][i+1] / h;
					}
					for (int j = 0; j <= i; j++) {
						Real g = 0.0;
						for (int k = 0; k <= i; k++) {
							g += mCovariance[k][i+1] * mCovariance[k][j];
						}
						for (int k = 0; k <= i; k++) {
							mCovariance[k][j] -= g * d[k];
						}
					}
				}
				for (int k = 0; k <= i; k++) {
					mCovariance[k][i+1] = 0.0;
				}
			}

			for (int j = 0; j < 3; j++)
			{
				d[j] = mCovariance[3-1][j];
				mCovariance[3-1][j] = 0.0;
			}

			mCovariance[3-1][3-1] = 1.0;
			e[0] = 0.0;


			// Symmetric tridiagonal QL algorithm.
			// Start QL algorithm
			//static void tql2(Real V[3][3], Real d[3], Real e[3])

			//  This is derived from the Algol procedures tql2, by
			//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
			//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
			//  Fortran subroutine in EISPACK.

			for (int i = 1; i < 3; i++)
			{
				e[i-1] = e[i];
			}
			e[3-1] = 0.0;

			Real f = 0.0;
			Real tst1 = 0.0;
			Real eps = 0.00000001;
			for (int l = 0; l < 3; l++) {

				// Find small subdiagonal element
				Real diag = fabs( d[l]) + fabs(e[l] );
				tst1 = std::max( tst1, diag );
				int m = l;
				while (m < 3)
				{
					if (fabs(e[m]) <= eps*tst1)
					{
						break;
					}
					m++;
				}

				// If m == l, d[l] is an eigenvalue,
				// otherwise, iterate.

				if (m > l) {
					int iter = 0;
					do
					{
						iter = iter + 1;  // (Could check iteration count here.)

						// Compute implicit shift

						Real g = d[l];
						Real p = (d[l+1] - g) / (2.0 * e[l]);
						Real r = Math::Hypotenuse(p,1.0f);
						if (p < 0) {
							r = -r;
						}
						d[l] = e[l] / (p + r);
						d[l+1] = e[l] * (p + r);
						Real dl1 = d[l+1];
						Real h = g - d[l];
						for (int i = l+2; i < 3; i++) {
							d[i] -= h;
						}
						f = f + h;

						// Implicit QL transformation.

						p = d[m];
						Real c 	 = 1.0;
						Real c2  = c;
						Real c3  = c;
						Real el1 = e[l+1];
						Real s 	 = 0.0;
						Real s2  = 0.0;

						for (int i = m-1; i >= l; i--)
						{
							c3 = c2;
							c2 = c;
							s2 = s;
							g = c * e[i];
							h = c * p;
							r = Math::Hypotenuse(p,e[i]);
							e[i+1] = s * r;
							s = e[i] / r;
							c = p / r;
							p = c * d[i] - s * g;
							d[i+1] = h + s * (c * g + s * d[i]);

							// Accumulate transformation.

							for (int k = 0; k < 3; k++)
							{
								h = mCovariance[k][i+1];
								mCovariance[k][i+1] = s * mCovariance[k][i] + c * h;
								mCovariance[k][i] = c * mCovariance[k][i] - s * h;
							}
						}
						p = -s * s2 * c3 * el1 * e[l] / dl1;
						e[l] = s * p;
						d[l] = c * p;

						// Check for convergence.

					} while (fabs(e[l]) > eps*tst1);
				}
				d[l] = d[l] + f;
				e[l] = 0.0;
			}

			// Sort eigenvalues and corresponding vectors.

			for (int i = 0; i < 3-1; i++)
			{
				int k = i;
				Real p = d[i];
				for (int j = i+1; j < 3; j++)
				{
					if (d[j] < p)
					{
						k = j;
						p = d[j];
					}
				}
				if (k != i)
				{
					d[k] = d[i];
					d[i] = p;
					for (int j = 0; j < 3; j++)
					{
						p = mCovariance[j][i];
						mCovariance[j][i] = mCovariance[j][k];
						mCovariance[j][k] = p;
					}
				}
			}
			mEigenvalue[0] = d[0];
			mEigenvalue[1] = d[1];
			mEigenvalue[2] = d[2];


			mEigenvector[0] = mCovariance.Column(0);
			mEigenvector[1] = mCovariance.Column(1);
			mEigenvector[2] = mCovariance.Column(2);

			mNormal.first  = mEigenvalue[0];
			mNormal.second = mEigenvector[0];

			mMinorAxis.first  = mEigenvalue[1];
			mMinorAxis.second = mEigenvector[1];

			mMajorAxis.first  = mEigenvalue[2];
			mMajorAxis.second = mEigenvector[2];

		}

	private:

		std::pair<Real, Vector3> mMinorAxis;
		std::pair<Real, Vector3> mMajorAxis;
		std::pair<Real, Vector3> mNormal;
		Real 					mCurvature;


	};

}/* Celer :: NAMESPACE */

#endif /*POLYNOMIAL_HPP_*/

// 2x3   - 4x2   - 22x + 24 = 0  3 raizes diferentes
// 3x3   - 10x2   + 14x + 27 = 0 1 raiz real
// x3   + 6x2   + 12x + 8 = 0 3 raizes reais e iguaiss
