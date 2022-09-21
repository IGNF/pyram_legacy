#ifndef _IGN_NUMERIC_DETAIL_IS_NULL_IMPL_H_
#define _IGN_NUMERIC_DETAIL_IS_NULL_IMPL_H_

#include <ignCommon.h>

namespace ign {
	namespace numeric {
		namespace detail {

			/// \brief [private]is_null impl, comparison to zero with tolerance for non exact arithmetic types
			template < typename T, bool IsExact >
			struct is_null_impl {
				static bool is_null( T const & v )
				{
					return ( v == (T)0 );
				}
			};

			/// \brief [private]specialization for non exact types
			template < typename T >
			struct is_null_impl< T, false > {

				static bool is_null( T const & v )
				{
					if ( v < 0 ){
						return ( -v < IGN_NUMERIC_LIMIT_EPSILON_FACTOR*std::numeric_limits< T >::epsilon() );
					}else{
						return ( v < IGN_NUMERIC_LIMIT_EPSILON_FACTOR*std::numeric_limits< T >::epsilon() );
					}
				}
			};
		}
	}
}

#endif
