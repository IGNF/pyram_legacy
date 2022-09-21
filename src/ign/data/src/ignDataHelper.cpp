#include <ignDataHelper.h>

#include <sstream>
#include <algorithm>

using namespace ign;
using namespace ign::data;

///
///
///
std::string Helper::quote( std::string const& str )
{
	std::ostringstream oss;
	oss << '"';
	
	for ( size_t i = 0; i < str.length(); ++i ){
		char c = str[i];
//	for ( std::string::const_iterator it = str.begin(); it != str.end(); ++it ){
//		char c = *it;
		switch ( c ){
			case '"':{
				oss << "\\\"";
				break;
			}
			case '\\':{
				oss << "\\\\";
				break;
			}
			case '/':{
				oss << "\\/";
				break;
			}
			case '\b':{
				oss << "\\b";
				break;
			}
			case '\f':{
				oss << "\\f";
				break;
			}
			case '\n':{
				oss << "\\n";
				break;
			}
			case '\r':{
				oss << "\\r";
				break;
			}
			case '\t':{
				oss << "\\t";
				break;
			}
			default:
			{
				oss << c;
				break;
			}
		}
	}
	oss << '"';
	return oss.str();
}

///
///
///
std::string Helper::unquote( std::string const& str  )
{
	if ( str.empty() ){
		return str;
	}
	if ( str[0] != '"' || str[ str.size() - 1 ] != '"' ){
		return str;
	}

	std::ostringstream oss;
	
	for ( size_t i = 1; i < str.size()-1; i++ ){
		char c = str[i];
		if ( c != '\\' ){
			oss << c;
			continue;
		}

		//ici, le caractere suivant est echappe
		if ( i + 1 == str.size() ){
			break;
		}

		//on interprete le suivant
		i++;
		c = str[i];
		switch ( c ){
			case '"':{
				oss << "\"";
				break;
			}
			case '\\':{
				oss << "\\";
				break;
			}
			case '/':{
				oss << "/";
				break;
			}
			case 'b':{
				oss << "\b";
				break;
			}
			case 'f':{
				oss << "\f";
				break;
			}
			case 'n':{
				oss << "\n";
				break;
			}
			case 'r':{
				oss << "\r";
				break;
			}
			case 't':{
				oss << "\t";
				break;
			}
			default:
			{
				//on ignore, ya comme une...
				//oss << *it;
				break;
			}
		}
	}
	return oss.str();
}

///
///
///
std::string Helper::GetMessageBadCast( std::string const& from, std::string const& to )
{
	std::ostringstream oss;
	oss << "[downcast error] bad cast from '" << from << "' to '" << to << "'";
	return oss.str();
}


///
///
///
std::string Helper::GetMessageBadConvert( std::string const& from, std::string const & to, std::string const& value )
{
	std::ostringstream oss;
	oss << "[convert error] bad conversion from '" << from << "' to '" << to << "'";
	if ( ! value.empty() ){
		oss << " with value [" << value << "]";
	}
	return oss.str();
}


///
///
///
std::string Helper::GetMessageNoAttribute( std::string const& attributeName )
{
	std::ostringstream oss;
	oss << "[attribute name error] no attribute '" << attributeName << "'";
	return oss.str();
}


///
///
///
std::string Helper::GetMessageAttributeAlreadyExists( std::string const& attributeName )
{
	std::ostringstream oss;
	oss << "[attribute already exists]attribute : '" << attributeName << "'";
	return oss.str();
}


///
///
///
template <>
bool Helper::StringTo( std::string const& value, bool& out )
{
	out = false;
	std::string copy(value);
	std::transform(copy.begin(),copy.end(),copy.begin(),tolower);
	if ( copy == "true" || copy == "1" || copy == "t" ){
		out = true;
		return true;
	}else if ( copy == "false" || copy == "0" || copy == "f" ){
		out = false;
		return true;
	}else{
		return false;
	}
}

template <>
std::string Helper::ToString( bool const& value )
{
	std::ostringstream oss;
	oss << ( value ? "true":"false" ) ;
	return oss.str();
}
