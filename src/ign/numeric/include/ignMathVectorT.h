#ifndef _IGN_MATH_VECTORT_H_
#define _IGN_MATH_VECTORT_H_

#include <ignMathVectorBaseT.h>

namespace ign {
	namespace math {

		/// \brief represente un vecteur d'un espace a deux dimensions
		///
		/// reecrite pour profiter de boost::ublas sur les produits matriciels
		template < typename T, size_t N >
		class VectorT : public VectorBaseT< T, N, VectorT< T, N > >
		{
		public:
			/// \brief represent base vector type
			typedef VectorBaseT< T, N, VectorT< T, N > > 	base_vector;

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
			const static size_t								dimension = N ;


			/// \brief build a vector on the origin
			VectorT():
				base_vector()
			{

			}

			/// \brief copy constructor from a uBLAS vector expression
			template <class R>
			VectorT( const boost::numeric::ublas::vector_expression< R >& r ):
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

			/// \brief conversion to double vector
			VectorT< double, N >	toVectorNd() const
			{
				VectorT< double, N > res;
				for ( size_t i = 0; i < N; i++ ){
					res[i] = numeric::Numeric< value_type >::ToDouble( (*this)[i] );
				}
			}

		};

	}
}

#endif


