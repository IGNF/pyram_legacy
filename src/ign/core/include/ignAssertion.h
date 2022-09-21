#ifndef _IGN_ASSERTION_H_
#define _IGN_ASSERTION_H_
#include <boost/current_function.hpp>

#include <ignConfig.h>

namespace ign {
	/// \brief
	class IGN_CORE_API Assertion {
	public:
		/// \brief effectue une verification sur le premier parametre et renvoie une exception exploitable en Debug sinon
		/// \see IGN_ASSERT et IGN_SAFE_MODE_ASSERT
		static void Assert( bool expectedTrue, int line, const char* filename, const char* function_name, const char* code = "" );
	};
}

/// \brief assertion toujours active
#define IGN_ASSERT( _SOMETHING_TO_CHECK_ ) \
	ign::Assertion::Assert( (!!(_SOMETHING_TO_CHECK_)), __LINE__, __FILE__, BOOST_CURRENT_FUNCTION, "IGN_ASSERT(" #_SOMETHING_TO_CHECK_ ")" )


#endif

