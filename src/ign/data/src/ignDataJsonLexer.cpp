#include <ignDataJsonLexer.h>

#include <ignCommon.h>

using namespace ign;
using namespace ign::data;
using namespace ign::data::json;

///
///
///
JsonLexer::JsonLexer()
{
	_pos 	= 0 ;
	_length = 0 ;
	_json	= "";
}

///
///
///
JsonLexer::~JsonLexer()
{

}

///
///
///
void	JsonLexer::setString( std::string const& jsonString )
{
	_pos 	= 0 ;
	_length = jsonString.size() ;
	_json	= jsonString;
}

///
///
///
size_t	JsonLexer::getPos() const
{
	return _pos;
}

///
///
///
void	JsonLexer::setPos( size_t const& pos )
{
	_pos = pos;
}

///
///
///
size_t	JsonLexer::getLength() const
{
	return _length;
}



///
///
///
bool	JsonLexer::nextToken( std::string& token, TokenType & tokenType )
{
	token.clear();
	while ( _pos < _length ){
		const char c = _json[ _pos ];
		//si caractere ignore, suivant
		if ( _isblank(c) || c == '\r' || c == '\n' ){
			_pos++;
			continue;
		}
		//si {
		if ( c == '{' ){
			token 		= "{";
			tokenType 	= JLeftBrace;
			_pos++;
			return true;
		}
		//si }
		if ( c == '}' ){
			token 		= "}";
			tokenType 	= JRightBrace;
			_pos++;
			return true;
		}
		//si :
		if ( c == ':' ){
			token = ":";
			tokenType = JColon;
			_pos++;
			return true;
		}
		//si [
		if ( c == '[' ){
			token 		= "[";
			tokenType 	= JLeftBracket;
			_pos++;
			return true;
		}
		//si ]
		if ( c == ']' ){
			token 		= "]";
			tokenType 	= JRightBracket;
			_pos++;
			return true;
		}
		if ( c == ',' ){
			token		= ",";
			tokenType	= JComma;
			_pos++;
			return true;
		}
		//si commence par n, comme null
		if ( c == 'n' ){
			token 		= "null";
			tokenType 	= JNull;
			if ( ! checkIsOnString(token) ){
				IGN_THROW_EXCEPTION("json parse error [1]");
			}
			_pos += token.size();
			return true;
		}
		//si commence par f, comme false
		if ( c == 'f' ){
			token 		= "false";
			tokenType 	= JFalse;
			if ( ! checkIsOnString(token) ){
				IGN_THROW_EXCEPTION("json parse error [2]");
			}
			_pos += token.size();
			return true;
		}
		//si commence par t, comme true
		if ( c == 't' ){
			token 		= "true";
			tokenType 	= JTrue;
			if ( ! checkIsOnString(token) ){
				IGN_THROW_EXCEPTION("json parse error [3]");
			}
			_pos += token.size();
			return true;
		}
		//si ressemble a un nombre
		if ( isdigit(c) || c == '.' || c == 'E' || c == 'e' || c == '+' || c == '-' ){
			readNumber(token);
			tokenType 	= JNumber;
			return true;
		}
		//si c'est une string "
		if ( c == '"' ){
			tokenType = JString;
			if ( ! readString(token) ){
				IGN_THROW_EXCEPTION("json parse error [4]");
			}
			return true;
		}
		
		IGN_THROW_EXCEPTION("json parse error [5]: unexpected character found: " + boost::lexical_cast<std::string>(c)+" in " + _json + " at position " + boost::lexical_cast<std::string>(_pos));
	}
	return false;
}

///
///
///
bool   JsonLexer::_isblank( char c )
{
	static std::string blanks("\r\n \t");
	return ( blanks.find(c) != std::string::npos );
}

///
///
///
void   JsonLexer::readNumber( std::string& number ){
	while ( _pos < _length ){
		const char c = _json[_pos];
		if ( isdigit(c) || c == '.' || c == 'E' || c == 'e' || c == '+' || c == '-' ){
			number += c;
			_pos++;
			continue;
		}
		return;
	}
}

///
///
///
bool   JsonLexer::readString( std::string& str ){
	_pos++;
	while ( _pos < _length ){
		char c = _json[_pos];
		//si caractere d'echappement
		if ( c == '"' ){
			_pos++;
			return true;
		}
		// si caractere d'echappement
		if ( c == '\\' ){
			//on verifie que l'on echappe pas le vide
			if ( _pos + 1 >= _length ){
				return false;
			}
			_pos++;
			char escapedC = _json[_pos];
			//on interprete
			switch ( escapedC ){
				case '"':
				{
					str.append("\"");
					break;
				}
				case '\\':
				{
					str.append("\\");
					break;
				}
				case '/':
				{
					str.append("/");
					break;
				}
				case 'b':
				{
					str.append("\b");
					break;
				}
				case 'f':
				{
					str.append("\f");
					break;
				}
				case 'n':
				{
					str.append("\n");
					break;
				}
				case 'r':
				{
					str.append("\r");
					break;
				}
				case 't':
				{
					str.append("\t");
					break;
				}
				case 'u':
				{
					IGN_THROW_EXCEPTION("json parse error (\\uxxxx 16 bits unicode characters are not supported)");
				}
				default:
					return false;
			}
			_pos++;
			continue;
		}
		//sinon, caractere normal
		str += c;
		_pos++;
	}
	return false;
}


///
///
///
bool  JsonLexer::checkIsOnString( std::string const& str )
{
	if ( _pos + str.size() - 1 >= _length ){
		return false;
	}
	std::string sub = _json.substr( _pos, str.size() );
	if ( sub != str ){
		return false;
	}
	return true;
}



