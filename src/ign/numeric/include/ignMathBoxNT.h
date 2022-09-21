#ifndef _IGN_MATH_BOXNT_H_
#define _IGN_MATH_BOXNT_H_

#include <ignMathVectorBaseT.h>
#include <ignMathInterval.h>

namespace ign {
	namespace math {

		/// \brief represents a n-dimensional bbox
		template < typename T, size_t N >
		class BoxNT {
		public:
			typedef BoxNT< T, N >				self;
			typedef Interval< T >				interval_type;
			typedef T							value_type;

			const static size_t dimension = N;

			/// \brief assign an other bbox
			/// \return *this
			BoxNT& assign( BoxNT const& other )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					this->_bounds[i] = other._bounds[i];
				}
				return *this;
			}
			
			/// \brief assign an other bbox
			/// \return *this
			template <class T2, size_t N2>
			BoxNT& assign( BoxNT<T2, N2> const& other )
			{
				IGN_ASSERT(N2 == self::dimension);
				for ( size_t i = 0; i < self::dimension; i++ ){
					this->_bounds[i].setBounds((T)other.minN(i), (T)other.maxN(i));
				}
				return *this;
			}

			/// \brief indicates if at least on interval is (NaN,NaN)
			bool	isNull() const
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					if( _bounds[i].isNull() ){
						return true;
					}
				}
				return false;
			}

			/// \brief set box to null
			void	setToNull()
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].setToNull() ;
				}
			}

			inline bool isCollapsed( T const& epsilon = numeric::Numeric< T >::Epsilon() ) const
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					if (_bounds[i].isCollapsed(epsilon)) return true;
				}
				return false;
			}

			/// \brief renvoie
			size_t 		size() const
			{
				return self::dimension;
			}


			/// \brief expands by a distance
			/// \return *this
			BoxNT &  	expandBy( value_type const& value )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].expandBy( value ) ;
				}
				return *this;
			}

			/// \brief expands to include other
			/// \return *this
			BoxNT &  	expandToInclude( BoxNT const& other )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].expandToInclude( other[i] ) ;
				}
				return *this;
			}

			/// \brief expands to include other
			/// \return *this
			template < typename Derived >
			BoxNT &  	expandToInclude( VectorBaseT< T, N, Derived > const& p )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].expandToInclude( p[i] ) ;
				}
				return *this;
			}
			
			/// \brief contract to limit to range defined by other
			/// \return *this
			BoxNT &  	contractToRange( BoxNT const& other )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].contractToRange( other[i] ) ;
				}
				return *this;
			}
			
			/// \brief round coordinates by the given coef
			/// \resulting bbox is:
			/// strategy: 0 nearest multiple of d
			///			  1 resulting interval always equal or larger than input
			///			  2 resulting interval always equal or smaller than input
			/// \return *this
			BoxNT &  	roundBy( value_type const& value , int strategy = 1)
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i].roundBy( value, strategy ) ;
				}
				return *this;
			}
			
			/// \brief test if box intersects an other box
			bool		intersects( BoxNT const& other ) const
			{
				IGN_ASSERT( ! isNull() );
				IGN_ASSERT( ! other.isNull() );

				for ( size_t i = 0; i < self::dimension; i++ ){
					if ( ! _bounds[i].intersects( other._bounds[i] ) ){
						return false;
					}
				}
				return true;
			}

			/// \brief test if box contains an other box
			bool		contains( BoxNT const& other ) const
			{
				IGN_ASSERT( ! isNull() );
				IGN_ASSERT( ! other.isNull() );

				for ( size_t i = 0; i < self::dimension; i++ ){
					if ( ! _bounds[i].covers( other._bounds[i] ) ){
						return false;
					}
				}
				return true;
			}
			
			
			/// \brief test if box contains p
			template < typename Derived >
			bool			contains( VectorBaseT< T, N, Derived > const& p ) const
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					if (!_bounds[i].contains(p[i])) return false;
				}
				return true;
			}


			/// \brief returns center value on the n-th dimension
			value_type/* const&*/		cN( size_t const& n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n].center();
			}

			/// \brief returns min value on the n-th dimension
			value_type const& 		minN( size_t const& n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n].getMin();
			}

			/// \brief returns min value on the n-th dimension
			value_type const&		maxN( size_t const & n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n].getMax();
			}
			
			/// \brief returns width  on the n-th dimension
			value_type		widthN( size_t const & n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n].width();
			}

			/// \brief set bounds for n-th dimension
			void				setBounds(
				size_t const& n,
				value_type const& vmin,
				value_type const& vmax
			)
			{
				IGN_ASSERT( n < self::dimension );
				_bounds[n].setBounds(vmin,vmax);
			}

			/// \brief returns bounds n
			interval_type const& 	bounds( size_t const& n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n];
			}

			/// \brief returns bounds n
			interval_type & 		bounds( size_t const& n )
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n];
			}

			/// \brief returns bounds n
			interval_type const& operator [] ( size_t const& n ) const
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n];
			}
			/// \brief returns bounds n
			interval_type & operator [] ( size_t const& n )
			{
				IGN_ASSERT( n < self::dimension );
				return _bounds[n];
			}
			
			/// \brief produit avec un scalaire
			BoxNT& operator *=( double k )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i] *= k;
				}
				return *this;
			}
			
			/// \brief division avec un scalaire
			BoxNT& operator /=( double k )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i] /= k;
				}
				return *this;
			}
			
		protected:
			interval_type _bounds[self::dimension];


			/// \brief build a null bbox (all intervals are [NaN,NaN])
			BoxNT( )
			{

			}

			/// \brief copy constructor
			BoxNT( BoxNT const& other )
			{
				assign(other);
			}


			/// \brief build a bbox from a vector of same dimension
			template < typename Derived >
			BoxNT( VectorBaseT< T, N, Derived > const& p )
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i] = interval_type(p[i]);
				}
			}
			/// \brief build a bbox from 2 vectors of same dimension
			template < typename Derived >
			BoxNT(
				VectorBaseT< T, N, Derived > const& p1,
				VectorBaseT< T, N, Derived > const& p2
			)
			{
				for ( size_t i = 0; i < self::dimension; i++ ){
					_bounds[i] = interval_type( p1[i], p2[i] );
				}
			}

			
			
			/// \brief operateur d'affectation avec cast implicite
//			template <class T2, size_t N2>
//			BoxNT& operator = ( BoxNT<T2,N2> const& other )
//			{
//				this->assign(other);
//				return *this;
//			}
		};

	}
}


#endif
