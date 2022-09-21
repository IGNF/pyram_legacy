#ifndef _PYRAM_EXCEPTION_H_
#define _PYRAM_EXCEPTION_H_

// #include <ign/ign_config.h>
#include <boost/exception/all.hpp>
#include <boost/program_options.hpp>

#include <string>


/// Convenience macro that throws a boost exception and initialises the reason with a string.

//#define IGN_THROW_EXCEPTION(x) BOOST_THROW_EXCEPTION( std::runtime_error( std::string(x).c_str() ) )		// A bit ugly if x is already a string, but overhead is only in case of exceptions :)
///#define IGN_THROW_EXCEPTION(x) BOOST_THROW_EXCEPTION( ign::Exception(std::string(x)) )

/// Convenience macro that throws a boost exception and initialises the reason with a string and an error code.
/* #define IGN_THROW_EXCEPTION_CODE(code, x) {                 \
boost::throw_exception(                                 \
boost::exception_detail::set_info( boost::exception_detail::set_info( boost::exception_detail::set_info( boost::exception_detail::set_info(             \
boost::enable_error_info( ign::Exception(std::string(x)) ), \
boost::throw_function(BOOST_CURRENT_FUNCTION)), \
boost::throw_file(__FILE__)),                   \
boost::throw_line(__LINE__)),                   \
boost::errinfo_errno(code))                     \
); }   */                                                  \
/**/

/// "Standardization" macro (same prefix as above) that throws a boost exception. To be used when throwing a class derived from std::exception.
/// #define IGN_THROW_EXCEPTION_TYPE(x)       BOOST_THROW_EXCEPTION( x )

/// full diagnostic information that dump infos from an exception
#define PYRAM_DUMP_EXCEPTION(e) boost::diagnostic_information(e)

/// catch all types of exception sending message in STREAM
#define PYRAM_CATCH_ALL_EXCEPTIONS(STREAM, caught, SHORT_MESSAGE)\
catch(Exception & e)\
{\
if (SHORT_MESSAGE) STREAM<<e.what()<<std::endl;\
else STREAM<<"Ign Exception: "<<PYRAM_DUMP_EXCEPTION(e)<<std::endl;\
caught=true;\
}\
catch (const  boost::program_options::error &ex)\
{\
	STREAM << ex.what() << std::endl;\
	caught=true;\
}\
catch(boost::exception & e)\
{\
if (SHORT_MESSAGE) STREAM<<boost::diagnostic_information_what(e)<<std::endl;\
else STREAM<<"Boost Exception: "<<boost::diagnostic_information(e)<<std::endl;\
caught=true;\
}\
catch(std::exception & e)\
{\
STREAM<<"STL Exception: "<<e.what()<<std::endl;\
caught=true;\
}\
catch(...)\
{\
STREAM<<"Unknown Exception"<<std::endl;\
caught=true;\
}

	
class Exception : public virtual boost::exception, public virtual  std::exception {

public:
	Exception() throw() : _message("Unknown exception") {}
	Exception( std::string const& message ) throw() : _message(message) {}
	virtual ~Exception() throw() {}
		
	/// \brief renvoie le message 
	virtual const char* what() const throw() { return _message.c_str(); }
		
	/// \brief renvoie le message de diagnostic (riche)
	std::string diagnostic() const throw() { return boost::diagnostic_information( *this ); }
		
protected:
	/// \brief le message de l'exception
	std::string _message;
};

#endif


