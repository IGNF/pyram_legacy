#include <sstream>
#include <boost/throw_exception.hpp>
#include <ignAssertion.h>
#include <ignException.h>

using namespace ign;

void Assertion::Assert( bool expectedTrue, int line, const char* filename, const char* function_name, const char* code )
{
	if ( ! expectedTrue )
	{
		std::ostringstream oss;
		#ifdef _MSC_VER
			oss << filename<<"("<<line<<"): fatal error in " << code <<std::endl;
		#else
			oss << filename << ":" << line << ": error:" << code << std::endl;
		#endif
		//throw Exception(oss.str(),line,filename);
#if BOOST_VERSION < 107300
		::boost::exception_detail::throw_exception_(ign::Exception(oss.str()),function_name,filename,line);
#else
        ::boost::source_location  loc(filename, line, function_name);
        ::boost::throw_exception(ign::Exception(oss.str()),loc);
#endif
	}
}

