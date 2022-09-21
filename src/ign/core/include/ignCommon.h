#ifndef _IGN_COMMON_H_
#define _IGN_COMMON_H_

#include <iostream>
#include <ignConfig.h>
#include <ignException.h>
#include <ignAssertion.h>
#include <ignLog.h>

#define _USE_MATH_DEFINES
#include <math.h>

/// \brief envoie d'une exception
//#define IGN_THROW_EXCEPTION( __message__ )  throw ign::Exception( __message__, __LINE__, __FILE__ );

#if IGN_SAFE_MODE
	#define IGN_SAFE_MODE_ASSERT( _SOMETHING_TO_CHECK_ ) \
		ign::Assertion::Assert( (!!(_SOMETHING_TO_CHECK_)), __LINE__, __FILE__, BOOST_CURRENT_FUNCTION, "IGN_SAFE_MODE_ASSERT(" #_SOMETHING_TO_CHECK_ ")" )
#else
	#define IGN_SAFE_MODE_ASSERT( _SOMETHING_TO_CHECK_ )
#endif

#ifndef DEBUGONLY
	#ifdef _DEBUG 
		#define DEBUGONLY( _SOMETHING_TO_DO_ ) _SOMETHING_TO_DO_
	#else
		#define DEBUGONLY( _SOMETHING_TO_DO_ )
	#endif
#endif

/// \brief helper to remove unused variable warning (null statement)
/// \see http://stackoverflow.com/questions/386220/how-can-i-hide-defined-but-not-used-warnings-in-gcc
#define IGN_UNUSED(x) ((void)(x))

/// Tells the maximum length (in bytes) of an UTF8 character.
/// Source: http://en.wikipedia.org/wiki/UTF-8
#define MAX_UTF8_CHAR_LENGTH 6

// Warning! _MAX_PATH should be avoided since the maximal length of a file path depends on the filesystem the file is, the operating system, the machine, the path encoding...
#ifndef _MAX_PATH
	#ifdef WIN32
		// Windows
		#include <windef.h>
		#ifdef MAX_PATH
			#define _MAX_PATH MAX_PATH
		#endif
	#else
		// Linux
		#include <limits.h>
		#ifdef PATH_MAX
			#define _MAX_PATH PATH_MAX
		#endif
	#endif	// WIN32
#endif

// Default case
#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

#endif



