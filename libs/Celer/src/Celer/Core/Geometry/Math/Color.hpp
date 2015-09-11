#ifndef CELER_COLOR_HPP_
#define CELER_COLOR_HPP_

#include <iostream>
#include <cstdlib>

#include <Celer/Core/Geometry/Math/Vector3.hpp>

namespace Celer
{

/*!
*@class Color.
*@brief Class that represent a RGB model of Color.
*@details coming soon , but ... coming soon ... coming soon yet ... wait man !!! ¬¬.
*@author Felipe Moura.
*@version 1.0.
*@date 27-Feb-2008.
*@todo OpenGL interface and a C style array.
*@todo Anather model of color representation like HLS  
*/	

	class Color
	{
		
		public:
									
			Color()
			{
				this->mRed   = 0.0;
				this->mGreen = 0.0;
				this->mBlue  = 0.0;
				this->mAlfa  = 0.0;
			};
			
			Color(const float&pRed,const float&pGreen, const float&pBlue)
				: mRed(pRed), mGreen(pGreen), mBlue(pBlue), mAlfa(0) {};
				
			Color(const float&pRed, const float&pGreen, const float& pBlue, const float& pAlfa)
				: mRed(pRed), mGreen(pGreen), mBlue(pBlue), mAlfa(pAlfa) {};
				
			template < typename C >				
			Color (const Vector3<C>& pVector )
				: mRed(pVector.x), mGreen(pVector.y), mBlue(pVector.z), mAlfa(0) {};
			
			template < typename C >
			inline void setRGB ( const C* pC)
			{
		    	this->mRed   = static_cast< float > ( pC[0] );
		    	this->mGreen = static_cast< float > ( pC[1] );
		    	this->mBlue  = static_cast< float > ( pC[2] );
			};	
			/*!@brief Initialize RGB color.
			*  @param[in] r float.
			*  @param[in] g float.
			*  @param[in] b float. 
			*/		
			inline void setRGB ( const float& pRed, const float& pGreen, const float& pBlue) 
			{
		    	this->mRed   = pRed;
		    	this->mGreen = pGreen;
		    	this->mBlue  = pBlue;
			};
				
			/*!@brief Initialize Red color.
			*  @param[in] r float.
			*/				
			inline void setRed ( const float& pRed) 
			{
		    	this->mRed   = pRed;
			};

			float Red() const
			{
				return this->mRed;
			}

			/*!@brief Initialize Green color.
			*  @param[in] g float.
			*/
			inline void setGreen ( const float& pGreen)
			{
		    	this->mGreen = pGreen;
			};

			float Green() const
			{
				return this->mGreen;
			}
			/*!@brief Initialize Blue color.
			*  @param[in] b float.
			*/		
			inline void setBlue ( const float& pBlue)
			{
		    	this->mBlue  = pBlue;
			};
			
			float Blue() const
			{
				return this->mBlue;
			}

			/*!@brief Initialize Alfa atribute.
			*  @param[in] b float.
			*/		
			inline void setAlfa ( const float& pAlfa)
			{
		    	this->mAlfa  = pAlfa;
			};
			
			float Alfa() const
			{
				return this->mAlfa;
			}

			/*!@brief operator[]
			 * @details array based index. "0" for x, "1" for y, "2" for z.
			 * @ This is ready-only
			 * @return float.
			 */
			inline  float operator [] ( int i)  const
			{
				if ( (i > 2) || ( i < 0))
				{
					std::cerr << "[ERROR] Point3 operator[]"        << std::endl
						      << "        Out of ranger. " 			<< std::endl
						      << "        Accepts, 0 , 1 , 2 only." << std::endl;
					exit(1);
				}
				
			    return (mRGB[i]);
			};
			/*!@brief operator[]
			 * @see operator[]
			 * @note Acept assigement
			 * @return float.
			 */
			inline  float operator [] ( int i)  
		
			{
				if ( (i > 2) || ( i < 0))
				{
					std::cerr << "[ERROR] Point3 operator[]"        << std::endl
						      << "        Out of ranger. " 			<< std::endl
						      << "        Accepts, 0 , 1 , 2 only." << std::endl;
					exit(1);
				}
				  
			    return (mRGB[i]);
			};
				
			/*!@brief operator=
			 * @details Assigment operator
			 * @return Point3
			 */
			inline Color& operator= ( const Color& pColor)
			{
				this->mRed   = pColor.mRed;
				this->mGreen = pColor.mGreen;
				this->mBlue  = pColor.mBlue;
				this->mAlfa  = pColor.mAlfa;
					
				return ( *this );
			};
			
			
			/*!@brief operator==
			 * @details Two points are identical whether your correspondents abscissa are equal.
			 * @return bool.
			 */
			inline bool operator== ( const Color& pColor) const
			{
				return ( ( this->mRed   == pColor.mRed   ) && 
						 ( this->mGreen == pColor.mGreen ) && 
						 ( this->mBlue  == pColor.mBlue  ) );
			};	
			/*!@brief operator!=
			 * @details Opposite to the operator==
			 * @see operator==
			 * @return bool.
			 */
			inline bool operator!= ( const Color& pColor) const
			{
				return  !(*this == pColor) ;
			};	

			
			/*!@brief operator<<
			 * @details For debug
			 */
			friend inline std::ostream& operator<< (std::ostream & s, const Color& pColor)
			{
				s << "Color" << " Red = "    << pColor.mRed 
							 << " ,Green = " << pColor.mGreen 
							 << " ,Blue = "  << pColor.mBlue << std::endl;
				
				
				return ( s );
			};
			


			virtual ~Color(){};
		
		private:
			
			union
			{
				struct
				{	
					float mRed;   /*!< Red color atribute.   */  
					float mGreen; /*!< Green color atribute. */
					float mBlue;  /*!< Blue color atribute.  */
					float mAlfa ; /*!< Alfa color atribute.  */
				};
				
				float mRGB[4];/*!< Array of color atributes.*/
			};
			
	};

}/* Celer::NAMESPACE */

#endif /*COLOR_HPP_*/
