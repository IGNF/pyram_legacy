#ifndef _IGN_TOOLS_STRINGTOOLS_H_
#define _IGN_TOOLS_STRINGTOOLS_H_

#include <ignConfig.h>

#include <sstream>
#include <string>
#include <vector>

namespace ign {
	namespace tools {
		
		///\brief renvoie true si name comporte la pattern
		///pattern utilise les conventions de boost::regex
		class IGN_CORE_API  StringPattern
		{
		public:
			///\brief c
			static bool Search(const std::string& name, const std::string& pattern);
			static bool Match(const std::string& name, const std::string& pattern, bool caseSensitive);
		};

		///
		///
		///
		class IGN_CORE_API  StringManip
		{
		public:

            enum StringCase
            {
                NoCase = 0,
                UpperCase = 1,
                LowerCase = 2,
                CamelCase = 3
            };

			/// \brief manipulations elementaires sur les chaines de caracteres
			///
			/// encapsule des operations STL ou boost
			
			////\brief uppercases the string
			static void			ToUpperCase(std::string& chaine);
			////\brief lowercasing the string
			static void			ToLowerCase(std::string& chaine);
			////\brief
			static std::string	GetExtension(std::string const &nom);
			/**
			 * convert to utf-8 from current code page
			 */
			static std::string	FromUTF8ToCurrentCodePage(const std::string& filename);
			
			/// \brief Return true if sub is actually a substring in string 'chaine', false otherwise
			static bool			FindSubString(const std::string& chaine, const std::string& sub);
			
			///\brief Substitute substring 'sub' by string 'sub2' in string 'chaine' if this string contains it
			static std::string	SubstituteSubString(const std::string& chaine, const std::string& sub, const std::string& sub2);
			
			///\brief Return the number of '0' padded at the left-side of strNum : 0 means no zero-padding
			static size_t		ZeroPadding(const std::string& strNum);
			
			///\brief Returns the number nNum zero-padded with nZeros. nZeros=0 means no 0-padding 
			static std::string	ZeroPadding(const size_t& nNum, const size_t & nZeros);
			
			/// \brief utilty function to split a string into parts, given delimiters.
			/// every character of the delim string is considered as a separator. The result is a vector of strings, which
			/// are the different parts of the input string.
			static void			Split(const std::string& str, const std::string& delim, std::vector<std::string>& parts);
		
			///\brief return true if string is convertible as a number (no matter if it is an integer or a floating number) and returns its value in @param value
			static bool			IsNumeric(const std::string &str, double& value);
			
			///\brief return true if string is convertible as an integer and returns its value in @param value
			static bool			IsInteger(const std::string &str, int& value);
			
			///\brief remove non UTF-8 character from a string
            static void			StripUnicode(std::string &str);

            ///\brief Switch between toUpper and toLower case
            static void         NormalizeStringCase(std::string &str, const StringCase &strCase );

		};
	}
}

///
///
///
namespace ign {
	namespace tools {
		/// \brief [advanced] implode substring to a string
		template < typename Iterator >
		std::string	 Implode( std::string const& glue, Iterator const & begin, Iterator const & end )
		{
			bool isFirst = true ;
			std::ostringstream oss;
			for ( Iterator it = begin; it != end; ++it ){
				if ( isFirst ){
					isFirst = false;
				}else{
					oss << glue ;
				}
				oss << (*it) ;
			}
			return oss.str();
		}
		
		/// \brief implode an iterable container with a glue to a string
		/// \code
		/// std::vector< std::string > letters;
		/// letters.push_back( "A" );
		/// letters.push_back( "B" );
		/// letters.push_back( "C" );
		/// // result is "A<->B<->C"
		/// std::string result = tools::Implode( "<->", letters );
		/// \endcode
		template < typename Container >
		std::string  Implode( std::string const& glue, Container const& container )
		{
			return Implode( glue, container.begin(), container.end() );
		}
	}
}

#endif

