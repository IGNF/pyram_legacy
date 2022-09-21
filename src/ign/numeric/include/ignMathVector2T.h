#ifndef _IGN_MATH_VECTOR2T_H_
#define _IGN_MATH_VECTOR2T_H_


#include <ignMathVectorBaseT.h>

namespace ign {
	namespace math {

		/// \brief represente un vecteur d'un espace a deux dimensions
		///
		/// reecrite pour profiter de boost::ublas sur les produits matriciels
		template < typename T >
		class Vector2T :
			public VectorBaseT< T, 2, Vector2T< T > >
		{
		public:
			/// \brief represent base vector type
			typedef	VectorBaseT< T, 2, Vector2T< T > > 		base_vector;

			typedef typename base_vector::size_type 		size_type;
			typedef typename base_vector::difference_type 	difference_type;
			typedef typename base_vector::value_type		value_type;
			typedef typename base_vector::const_reference	const_reference;
			typedef typename base_vector::reference			reference;
			typedef typename base_vector::const_pointer		const_pointer;
			typedef typename base_vector::pointer			pointer;
			typedef typename base_vector::const_iterator	const_iterator;
			typedef typename base_vector::iterator			iterator;

			/// \brief definit la dimension
			const static size_t								dimension = 2 ;


			/// \brief build a vector on the origin
			Vector2T():
				base_vector()
			{
				(*this)[0] = (T)0;
				(*this)[1] = (T)0;
			}

			/// \brief build a vector with x,y coordinates
			Vector2T ( T const& x, T const& y ):
				base_vector()
			{
				(*this)[0] = x;
				(*this)[1] = y;
			}

			/// \brief copy constructor from a uBLAS vector expression
			template <class R>
			Vector2T (const boost::numeric::ublas::vector_expression< R >& r):
				base_vector(r)
			{

			}
			/// \brief assignment from a uBLAS vector expression
			template <class R>
			void operator=(const boost::numeric::ublas::vector_expression<R>& r)
			{
				base_vector::operator=(r);
			}
			/// \brief assignment from an other vector
			template <class R>
			void operator=(const base_vector& r)
			{
				base_vector::operator=(r);
			}

			/// \brief cross product with an other vector
			T	cross( Vector2T const& other ) const
			{
				return ( (*this)[0]*other[1] - (*this)[1]*other[0] );
			}

			/// \brief x const accessor
			inline T const	& x() const { return (*this)[0]; }
			/// \brief y const accessor
			inline T const	& y() const { return (*this)[1]; }

			/// \brief x accessor
			inline T & 		  x() { return (*this)[0]; }
			/// \brief y accessor
			inline T & 		  y() { return (*this)[1]; }

			/// \brief conversion to double vector
			Vector2T< double >	toVectorNd() const
			{
				return Vector2T< double >(
					numeric::Numeric< value_type >::ToDouble( x() ),
					numeric::Numeric< value_type >::ToDouble( y() )
				);
			}

			/// \brief create UX vector
			static Vector2T		UX() { return Vector2T< T >( (T)1, (T)0 ); }
			/// \brief create UY vector
			static Vector2T		UY() { return Vector2T< T >( (T)0, (T)1 ); }

		};

		/// \brief vecteur de double a 3 dimension
		typedef Vector2T<double> 	Vec2d;
		/// \brief vecteur de float a 3 dimension
		typedef Vector2T<float> 	Vec2f;
	}
}

#endif


