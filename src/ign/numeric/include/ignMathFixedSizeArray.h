#ifndef _IGN_MATH_DETAIL_FIXED_SIZE_ARRAY_H_
#define _IGN_MATH_DETAIL_FIXED_SIZE_ARRAY_H_

#include <boost/numeric/ublas/vector.hpp>

#include <algorithm>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/collection_size_type.hpp>

#include <ignCommon.h>

namespace ign {
	namespace math {
		namespace detail {
			// Sukender: should remove "using namespace" in ".h".
			//using namespace boost::numeric::ublas;

			/// \brief a null initialized fixed size array for boost::numeric::blas vector storage
			/// \warning no resize operation (to a different size)
			///
			/// adaptee a partir de boost::numeric::ublas::bounded_array qui permettait les resize. Si
			/// une classe equivalente arrive dans boost, supprimer celle-ci
			/// Sukender: est-ce equivalent a boost::array<> ?
			template<class T, std::size_t N >
			class fixed_size_array:
				public boost::numeric::ublas::storage_array< fixed_size_array< T, N > > {

				typedef fixed_size_array<T, N > self_type;
			public:
				typedef size_t 						size_type;
				typedef size_t 						difference_type;
				typedef T 							value_type;
				typedef const T &					const_reference;
				typedef T &							reference;
				typedef const T *					const_pointer;
				typedef T *							pointer;
				typedef const_pointer 				const_iterator;
				typedef pointer 					iterator;

				// Construction and destruction
				fixed_size_array ()
				{

				}
				explicit BOOST_UBLAS_INLINE
				fixed_size_array (size_type size)
				{
					IGN_SAFE_MODE_ASSERT( size == N );
					for ( size_type i = 0; i < N; i++ ){
						data_[i] = (T)0;
					}
				}

				BOOST_UBLAS_INLINE
				fixed_size_array (size_type size, const value_type &init)
				{
					IGN_SAFE_MODE_ASSERT( size == N );
					for ( size_type i = 0; i < N; i++ ){
						data_[i] = init;
					}
				}

				BOOST_UBLAS_INLINE
				fixed_size_array (const fixed_size_array &c)
				{
					IGN_SAFE_MODE_ASSERT( c.size() == N );
					for ( size_type i = 0; i < N; i++ ){
						data_[i] = c.data_[i];
					}
				}

				// Resizing
				BOOST_UBLAS_INLINE
				void resize (size_type size) {
					IGN_SAFE_MODE_ASSERT( size == N );
				}
				BOOST_UBLAS_INLINE
				void resize (size_type size, value_type init) {
					IGN_SAFE_MODE_ASSERT( size == N );
					//always keeps
				}

				// Random Access Container
				BOOST_UBLAS_INLINE
				size_type max_size () const {
					return N;
				}

				BOOST_UBLAS_INLINE
				bool empty () const {
					//never empty
					return false;
				}

				BOOST_UBLAS_INLINE
				size_type size () const {
					return N;
				}

				// Element access
				BOOST_UBLAS_INLINE
				const_reference operator [] (size_type i) const {
					IGN_SAFE_MODE_ASSERT( i < N );
					return data_[i];
				}
				BOOST_UBLAS_INLINE
				reference operator [] (size_type i) {
					IGN_SAFE_MODE_ASSERT( i < N );
					return data_[i];
				}

				// Assignment
				BOOST_UBLAS_INLINE
				fixed_size_array &operator = (const fixed_size_array &a) {
					if (this != &a) {
						IGN_SAFE_MODE_ASSERT( a.size() == N );
						for ( size_type i = 0; i < N; i++ ){
							data_[i] = a.data_[i];
						}
					}
					return *this;
				}
				BOOST_UBLAS_INLINE
				fixed_size_array &assign_temporary (fixed_size_array &a) {
					*this = a;
					return *this;
				}

				// Swapping
				BOOST_UBLAS_INLINE
				void swap (fixed_size_array &a) {
					if (this != &a) {
						std::swap_ranges (data_, data_ + N, a.data_);
					}
				}
				BOOST_UBLAS_INLINE
				friend void swap (fixed_size_array &a1, fixed_size_array &a2) {
					a1.swap (a2);
				}

				BOOST_UBLAS_INLINE
				const_iterator begin () const {
					return data_;
				}
				BOOST_UBLAS_INLINE
				const_iterator end () const {
					return data_ + N;
				}

				BOOST_UBLAS_INLINE
				iterator begin () {
					return data_;
				}
				BOOST_UBLAS_INLINE
				iterator end () {
					return data_ + N;
				}

				// Reverse iterators
				typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
				typedef std::reverse_iterator<iterator> reverse_iterator;

				BOOST_UBLAS_INLINE
				const_reverse_iterator rbegin () const {
					return const_reverse_iterator (end ());
				}
				BOOST_UBLAS_INLINE
				const_reverse_iterator rend () const {
					return const_reverse_iterator (begin ());
				}
				BOOST_UBLAS_INLINE
				reverse_iterator rbegin () {
					return reverse_iterator (end ());
				}
				BOOST_UBLAS_INLINE
				reverse_iterator rend () {
					return reverse_iterator (begin ());
				}

			private:
				// Serialization
				friend class boost::serialization::access;

				template<class Archive>
				void serialize(Archive & ar, const unsigned int version)
				{
					boost::serialization::collection_size_type s(N);
					ar & boost::serialization::make_nvp("size", s);
					if ( Archive::is_loading::value ) {
						if (s > N) {
							boost::numeric::ublas::bad_size("too large size in bounded_array::load()\n").raise();
						}
					}
					ar & boost::serialization::make_array(data_, s);
				}

			private:
				BOOST_UBLAS_BOUNDED_ARRAY_ALIGN value_type data_ [N];
			};
		}
	}
}

#endif

