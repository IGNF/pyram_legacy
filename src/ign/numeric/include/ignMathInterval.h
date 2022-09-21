#ifndef _IGN_MATH_INTERVAL_H_
#define _IGN_MATH_INTERVAL_H_

#include <cmath>

#include <ignNumeric.h>

namespace ign {
	namespace math {
		/// \brief represente un interval au sens mathematique du terme
		/// \warning T must have NaN concept
		template < typename T >
		class Interval {
		public:
			/// \brief construit un interval "null" (NaN,NaN)
			Interval():
				_min( ign::numeric::Numeric< T >::NaN() ),
				_max( ign::numeric::Numeric< T >::NaN() )
			{
				
			}

			/// \brief construit un interval ponctuel (collapsed) sur la valeur
			Interval( T const& v ):
				_min( v ),
				_max( v )
			{

			}

			/// \brief constructeur a partir de deux valeurs.
			/// si v1 ou v2 est NaN, l'autre est conserve en min et max
			/// si v1 et v2 sont NaN, l'interval est nul
			Interval( T const& v1, T const& v2 )
			{
				bool v1_isNaN = ign::numeric::Numeric< T >::IsNaN(v1);
				bool v2_isNaN = ign::numeric::Numeric< T >::IsNaN(v2);

				if ( v1_isNaN && v2_isNaN ){
					_min = ign::numeric::Numeric< T >::NaN();
					_max = ign::numeric::Numeric< T >::NaN();
					return;
				}
				if ( v1_isNaN ){
					_min = v2;
					_max = v2;
					return;
				}
				if ( v2_isNaN ){
					_min = v1;
					_max = v1;
					return;
				}
				_min = std::min( v1, v2 );
				_max = std::max( v1, v2 );
			}

			/// \brief constructeur par recopie
			Interval( Interval const& other ):
				_min( other._min ),
				_max( other._max )
			{

			}

			/// \brief operateur d'affectation
			Interval& operator = ( Interval const& other )
			{
				_min = other._min ;
				_max = other._max ;
				return *this;
			}
			
			
			/// \brief operateur d'affectation avec cast implicite
			template<class T2>
			Interval& operator = ( Interval<T2> const& other )
			{
				_min = (T2)other.getMin() ;
				_max = (T2)other.getMax() ;
				return *this;
			}

			/// \brief destructeur
			~Interval()
			{

			}

			/// \brief indique si l'interval est nul
			inline bool	isNull() const
			{
				return ( ign::numeric::Numeric< T >::IsNaN(_min) || ign::numeric::Numeric< T >::IsNaN( _max ) );
			}

			/// \brief definit l'interval comme etant nul
			void		setToNull()
			{
				_min = ign::numeric::Numeric< T >::NaN();
				_max = ign::numeric::Numeric< T >::NaN();
			}

			/// \brief indique si l'interval est nul ou correspond a un point
			inline bool isCollapsed( T const& epsilon = numeric::Numeric< T >::Epsilon() ) const
			{
				return ( isNull() || ign::numeric::Numeric< T >::Equals( _min, _max, epsilon ) );
			}

			/// \brief renvoie la borne inferieure
			/// \warning definit le min si ce dernier est NaN
			inline void		setMin( T const& min )
			{
				if ( ign::numeric::Numeric< T >::IsNaN(_max) ){
					setBounds( min, min );
				}else{
					setBounds( min, _max );
				}
			}

			/// \brief renvoie la borne superieur
			/// \warning definit le min si ce dernier est NaN
			inline void		setMax( T const& max )
			{
				if ( ign::numeric::Numeric< T >::IsNaN(_min) ){
					setBounds(  max, max );
				}else{
					setBounds( _min, max );
				}
			}

			/// \brief renvoie la borne inferieure
			inline T const& getMin() const
			{
				return _min;
			}

			/// \brief renvoie la borne superieure
			inline T const& getMax() const
			{
				return _max;
			}

			/// \brief renvoie la largeur de l'interval
			inline T 		width() const
			{
				if ( isNull() ){
					return ign::numeric::Numeric< T >::NaN();
				}
				return ( _max - _min );
			}

			/// \brief renvoie le centre de l'interval, NaN si null
			inline T 		center() const
			{
				if ( isNull() ){
					return ign::numeric::Numeric< T >::NaN();
				}
				return ( _max + _min ) * 0.5;
			}


			/// \brief definit les bornes
			void			setBounds( T const& min, T const& max )
			{
				_min = std::min( min, max );
				_max = std::max( min, max );
			}

			/// \brief agrandit l'interval ( min - d, min + d )
			inline void			expandBy( T const& d )
			{
				if ( isNull() )
					return ;
				_min -= ign::numeric::Numeric< T >::Abs(d); //std::fabs(d);
				_max += ign::numeric::Numeric< T >::Abs(d); //std::fabs(d);
			}

			/// \brief etend pour inclure deux bornes
			void			expandToInclude( T const& v )
			{
				if ( isNull() )
				{
					_min = v;
					_max = v;
					return ;
				}
				_min = std::min( _min, v );
				_max = std::max( _max, v );
			}

			/// \brief etend pour inclure un autre interval
			void			expandToInclude( Interval const& other )
			{
				expandToInclude(other._min);
				expandToInclude(other._max);
			}

			/// \brief contracte pour se limiter un autre interval
			void			contractToRange( Interval const& other )
			{
				if ( isNull() )
				{
					_min = other._min;
					_max = other._max;
					return ;
				}

				_min = std::max( _min, other._min );
				_max = std::min( _max, other._max );
			}
			
			
			/// \brief arrondit l'interval au coef pres
			/// strategy: 0 nearest multiple of d
			///			  1 resulting interval always equal or larger than input
			///			  2 resulting interval always equal or smaller than input
			
			inline void			roundBy( T const& d, int strategy = 1 )
			{
				if ( isNull() )
					return ;
				if (d == 0) 
					return;
				double dd = (double)d;
				switch( strategy)
				{
					case 0:
					{	
						_min = static_cast<T>(std::floor((double)(_min/dd+0.5))*dd); 
						_max = static_cast<T>(std::ceil ((double)(_max/dd-0.5))*dd);
						break;
					}
					case 1:
					{	
						_min = static_cast<T>(std::floor((double)(_min/dd))*dd); 
						_max = static_cast<T>(std::ceil ((double)(_max/dd))*dd);
						break;
					}
					case 2:
					{	
						_min = static_cast<T>(std::ceil ((double)(_min/dd))*dd); 
						_max = static_cast<T>(std::floor((double)(_max/dd))*dd);
						break;
					}
					default: IGN_ASSERT(false);
				}
			}

			/// \brief test d'egalite entre deux intervals
			bool			equals( Interval const& other ) const
			{
				if ( isNull() || other.isNull() )
					return false;

				if ( ! ign::numeric::Numeric< T >::Equals( _min, other._min ) )
					return false;
				if ( ! ign::numeric::Numeric< T >::Equals( _max, other._max ) )
					return false;

				return true;
			}


			/// \brief test d'appartenance d'un point a l'interval
			bool			contains( T const& v ) const
			{
				return intersects( v );
			}

			/// \brief test d'appartenance d'un point a l'interval
			bool			intersects( T const& v ) const
			{
				if ( isNull() || ign::numeric::Numeric< T >::IsNaN(v) )
					return false;

				if ( v > ( _max + ign::numeric::Numeric< T >::Epsilon() ) )
					return false;
				if ( v < ( _min - ign::numeric::Numeric< T >::Epsilon() ) )
					return false;

				return true;
			}


			/// \brief test d'intersection avec un autre interval
			bool			intersects( Interval const& other ) const
			{
				if ( isNull() || other.isNull() )
					return false;

				if ( other._min > ( _max + ign::numeric::Numeric< T >::Epsilon() ) )
					return false;
				if ( other._max < ( _min - ign::numeric::Numeric< T >::Epsilon() ) )
					return false;

				return true;
			}

			/// \brief test d'intersection avec un autre interval
			bool			covers( Interval const& other ) const
			{
                return (
					_min <= other._min + ign::numeric::Numeric< T >::Epsilon() &&
					_max >= other._max - ign::numeric::Numeric< T >::Epsilon()
				);
			}
			
			/// \brief produit avec un scalaire
			Interval & operator *=( double k )
			{
				_min = static_cast<T>(_min * k);
				_max = static_cast<T>(_max * k);
				return *this;
			}
			
			/// \brief division avec un scalaire
			Interval & operator /=( double k )
			{
				_min = static_cast<T>(_min / k);
				_max = static_cast<T>(_max / k);
				return *this;
			}
			
		private:
			/// \brief la borne inferieure
			T 	_min;
			/// \brief la borne superieure
			T 	_max;
		};
	}
}

#endif

