#ifndef _IGN_MATH_VECTORBASET_H_
#define _IGN_MATH_VECTORBASET_H_

#include <iostream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <ignNumeric.h>
#include <ignMathFixedSizeArray.h>

namespace ign {
	namespace math {

		/// \brief [private]classe de base representant un vecteur mathematique
		template < typename T, size_t N, typename Derived >
		class VectorBaseT :
			public boost::numeric::ublas::vector<T, ign::math::detail::fixed_size_array<T,N> >
		{
		public:
			/// \brief represent base vector type
			typedef boost::numeric::ublas::vector<
				T, ign::math::detail::fixed_size_array<T,N>
			> base_vector;

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
			static  const size_t							dimension = N ;

		protected:
			/// \brief constructeur par defaut
			VectorBaseT():
				base_vector( dimension, (T)0 )
			{

			}
			/// \brief constructeur par recopie
			VectorBaseT( VectorBaseT const& other )
			{
				base_vector::assign(other);
			}

			/// \brief constructeur par recopie d'un vecteur de meme value_type et N
			template < typename OtherDerived >
			VectorBaseT( VectorBaseT<T,N,OtherDerived> const& other )
			{
				base_vector::assign(other);
			}

			/// \brief copy constructor from a uBLAS vector expression
			template <class R>
			VectorBaseT (const boost::numeric::ublas::vector_expression< R >& r):
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

		public:

			/// \brief renvoie le carre de la norme euclidienne
			T		norm2() const
			{
				T res = (T)0;
				for ( size_t i = 0; i < N; i++ ){
					res += (*this)[i]*(*this)[i];
				}
				return res;
			}

			/// \brief renvoie la norme euclidienne
			double 	norm() const
			{
				return sqrt( norm2() );
			}
			
			/// \brief divise le vecteur par sa norme si celle-ci est non nulle
			void 	normalize()
			{
				double normTmp(norm());
				if (normTmp>0) *this /= normTmp;
			}

			/// \brief retourne vrai si le vecteur est strictement null
			bool 	isNull() const
			{
				for ( size_t i = 0; i < N; i++ ){
					if ( (*this)[i] != (T)0 )
						return false;
				}
				return true;
			}
			/// \brief retourne vrai si le vecteur est null en prenant en compte une tolerance (<=epsilon)
			bool 	isNull( T const& epsilon ) const
			{
				for ( size_t i = 0; i < N; i++ ){
					if ( numeric::Numeric< T >::Abs( (*this)[i] ) <= epsilon)
						continue;
					return false;
				}
				return true;
			}

			/// \brief renvoie la distance a un autre vecteur au carre
			template < typename OtherDerived >
			T		distance2( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				return boost::numeric::ublas::inner_prod( (*this - other), (*this - other) );
			}

			/// \brief renvoie la distance a un autre vecteur
			template < typename OtherDerived >
			double	distance( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				return sqrt( distance2(other) );
			}

			/// \brief produit scalaire avec un autre vecteur
			template < typename OtherDerived >
			T		dot( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				return boost::numeric::ublas::inner_prod( *this, other );
			}

			/// \brief comparaison avec un autre vecteur
			template < typename OtherDerived >
			bool	operator < ( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				for ( size_t i = 0; i < N; i++ ){
					if ( (*this)[i] < other[i] ){
						return true;
					}
					if ( other[i] < (*this)[i] ){
						return false;
					}
				}
				return false;
			}
			/// \brief comparaison avec un autre vecteur
			template < typename OtherDerived >
			bool	operator != ( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				for ( size_t i = 0; i < N; i++ ){
					if ( (*this)[i] < other[i] ){
						return true;
					}
					if ( other[i] < (*this)[i]  ){
						return true;
					}
				}
				return false;
			}
			/// \brief comparaison avec un autre vecteur
			template < typename OtherDerived >
			bool	operator == ( VectorBaseT<T,N,OtherDerived> const& other ) const
			{
				return ! ( (*this) != other );
			}

		};

	}
}


#endif

