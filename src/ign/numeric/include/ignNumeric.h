#ifndef _IGN_NUMERIC_NUMERIC_H_
#define _IGN_NUMERIC_NUMERIC_H_


#include <limits>
#include <cmath>
#include <boost/math/special_functions/fpclassify.hpp>

#include <ignConfig.h>
#include <ignNumericIsNullImpl.h>


/** disable 6287 = -Wtautological-compare warning ->
 see: http://msdn.microsoft.com/en-us/library/a5b9aa09.aspx
 and http://msdn.microsoft.com/en-us/library/6dhhd6k6.aspx
 //#pragma warning (error : 6287 ) 
 **/

namespace ign {
	namespace numeric{
		/// \brief cette classe abstrait les operations sur les flottants et traite les
		///		valeurs particuliere : nan, epsilon, zero et one.
		///
		/// Elle repose en partie sur std::numeric_limits de la STL.
		template < typename T_NUMERIC >
		class Numeric {
		public:
			/// \brief indique si la parametre vaut NaN
			static bool IsNaN( T_NUMERIC const& value )
			{
				//IEEE standard : true only if isnan
				return boost::math::isnan( value ) ;
			}
			
			/// \brief renvoie une valeur correspondant a Not A Number
			static T_NUMERIC const& NaN()
			{
				static T_NUMERIC nan = std::numeric_limits< T_NUMERIC >::quiet_NaN();
				return nan;
			}
			
			/// \brief renvoie une petite valeur
			static T_NUMERIC const& Epsilon()
			{
				static T_NUMERIC epsilon = std::numeric_limits< T_NUMERIC >::epsilon();
				return epsilon;
			}
			
			/// \brief renvoie un petite valeur
			static T_NUMERIC const& EpsilonWithFactor()
			{
				static T_NUMERIC epsilon = IGN_NUMERIC_LIMIT_EPSILON_FACTOR * Epsilon();
				return epsilon;
			}
			
			/// \brief test d'egalite, avec specialisation sur double et float pour comparaison avec Epsilon
			/// \warning valable sur les flottant uniquement.
			static bool				Equals( T_NUMERIC const& v1, T_NUMERIC const& v2, T_NUMERIC const& eps=Epsilon() )
			{
				if ( v1 < v2 ){
					return (v2 - v1) <= eps;
				}else{
					return (v1 - v2) <= eps;
				}
			}
						
			/// \brief indique si la parametre vaut 0. a epsilon pres
			static bool IsNULL( T_NUMERIC const& value )
			{
				return detail::is_null_impl< T_NUMERIC, std::numeric_limits< T_NUMERIC >::is_exact >::is_null( value );
			}

			/// \brief test if is null with a given epsilon
			static bool IsNULL( T_NUMERIC const& value, T_NUMERIC const& eps )
			{
				return Abs(value) <= eps;
			}


			/// \brief arrondi le double en parametre a la valeur entiere la plus proche
			/// \todo test + s'assurer que c'est sans effet sur int
			static T_NUMERIC round( T_NUMERIC const & r ) {
				return ( (r < static_cast<T_NUMERIC>(0.)) ? ceil(r - static_cast<T_NUMERIC>(.5)) : floor(r + static_cast<T_NUMERIC>(.5)) );
			}

			/// \brief aide pour uniformiser les conversions en double
			static double ToDouble( T_NUMERIC const& r ){
				return static_cast<double>(r);
			}

			/// \brief returns the absolute value of v
			static T_NUMERIC Abs( T_NUMERIC const& v )
			{
				if ( v < static_cast<T_NUMERIC>(0) )
					return -v;
				else
					return v;
			}

			/// \brief renvoie 1 si valeur positive ou nulle, -1 sinon, valable sur les types signes, evidemment
			static int sign(const T_NUMERIC &v) { return v<0?-1:1; }
			
			/// \brief renvoie 1 si valeur positive, 0 si valeur nulle, -1 sinon, valable sur les types signes, evidemment
			static int signOrZero(const T_NUMERIC &v)
			{
				if ( IsNULL(v) ){
					return 0;
				}else if ( v < 0 ){
					return -1;
				}else{
					return 1;
				}
			}
			
			/// \brief renvoie le carre de la valeur
			static T_NUMERIC square(const T_NUMERIC &v) { return v*v; }
		};
		
		bool IsNumeric(const std::string& number);		
		
		bool IsInteger(const std::string& number);	
	}
}

#endif

