#include <ignDataJsonReader.h>

#include <fstream>
#include <sstream>

#include <ignCommon.h>

#include <ignDataVariant.h>
#include <ignDataBoolean.h>
#include <ignDataLong.h>
#include <ignDataDouble.h>
#include <ignDataString.h>
#include <ignDataArray.h>
#include <ignDataObject.h>

namespace ign {
namespace data {
namespace json {


///
///
///
JsonReader::JsonReader()
{

}

///
///
///
JsonReader::~JsonReader()
{

}

///
///
///
void JsonReader::read( std::string const& json, Variant& out )
{
	out.reset(0);

	//lecture de la chaine
	ValuePtr value;
	value.reset(0);
	_lexer.setString(json);

	//ici, au moins une valeur
	readValue(value);
	//si la chaine n'est pas lue totalement
	std::string			 token;
	JsonLexer::TokenType tokenType;
	if ( _lexer.nextToken(token,tokenType) ){
		IGN_THROW_EXCEPTION( "[json parse error]bad format, unexpected token "+token+" of type "+boost::lexical_cast<std::string>((int)tokenType)+" read");
	}

	out.reset( value.release() );
}

///
///
///
void	JsonReader::readValue( ValuePtr& value )
{
	std::string 			token;
	JsonLexer::TokenType   	tokenType;

	value.reset(0);

	if ( ! _lexer.nextToken(token,tokenType) ){
		IGN_THROW_EXCEPTION("[json parse error]");
	}

	switch ( tokenType ){
		case JsonLexer::JNull:
		{
			value.reset( 0 );
			return;
		}
		case JsonLexer::JTrue:
		{
			value.reset(new Boolean(true));
			return;
		}
		case JsonLexer::JFalse:
		{
			value.reset(new Boolean(false));
			return;
		}
		case JsonLexer::JLeftBrace:
		{
			readObject(value);
			return;
		}
		case JsonLexer::JRightBrace:
		{
			IGN_THROW_EXCEPTION("json parse error, <}> unexpected");
		}
		case JsonLexer::JLeftBracket:
		{
			readArray(value);
			return;
		}
		case JsonLexer::JRightBracket:
		{
			IGN_THROW_EXCEPTION("json parse error, <]> unexpected");
		}
		case JsonLexer::JColon:
		{
			IGN_THROW_EXCEPTION("json parse error, <:> unexpected");
		}
		case JsonLexer::JComma:
		{
			IGN_THROW_EXCEPTION("json parse error, <,> unexpected");
		}
		case JsonLexer::JString:
		{
			value.reset( new String( token ) );
			return;
		}
		case JsonLexer::JNumber:
		{
			decodeNumber(token,value);
			return;
		}
		default:
		{
			IGN_THROW_EXCEPTION("[error]cas non pris en compte");
		}
	}
}


///
///
///
void	JsonReader::decodeNumber( std::string const& token, ValuePtr& value )
{
	//test en int64_t
	{
		int64_t i;
		std::istringstream iss(token);
		if ( ( iss >> i ) && iss.eof() ){
			value.reset( new Long(i) );
			return;
		}
	}
	//test en double
	{
		double d;
		std::istringstream iss(token);
		if ( ( iss >> d ) && iss.eof() ){
			value.reset( new Double(d) );
			return;
		}
	}
	IGN_THROW_EXCEPTION("json parse error, cannot parse number");
}



///
///
///
void	JsonReader::readArray( ValuePtr & value )
{
	//[ has been read
	Array * array = new Array();
	value.reset(array);

	//on regarde s'il y a au moins une valeur dans le tableau,
	//	sans quoi la lecture de cette valeur echouera
	size_t pos_saved = _lexer.getPos();
	std::string 			token;
	JsonLexer::TokenType	tokenType;
	if ( ! _lexer.nextToken( token, tokenType ) ){
		IGN_THROW_EXCEPTION("[JsonReader::readArray] json parse error 1");
	}
	if ( tokenType == JsonLexer::JRightBracket )
	{
		return;
	}

	//on revient en arriere, on lit une valeur
	_lexer.setPos( pos_saved );
	for( ;; ){
		//on lit la valeur
		ValuePtr item;
		readValue(item);
		array->addItem( item.release() );

		//on lit le token qui sera soit ] soit ,
		if ( ! _lexer.nextToken( token, tokenType ) ){
			IGN_THROW_EXCEPTION( "JsonReader::readArray] json parse error 2" );
		}

		if ( tokenType == JsonLexer::JRightBracket ){
			return;
		}else if ( tokenType == JsonLexer::JComma ){
			continue;
		}else{
			IGN_THROW_EXCEPTION("json parse error, error reading array <,> or <]> expected");
		}
	}
}

///
///
///
void	JsonReader::readObject( ValuePtr & value )
{
	int verbose = 0;
	if (verbose) std::cout<<"[JsonReader::readObject] START"<<std::endl;
	//{ has been read
	Object * obj = new Object();
	value.reset(obj);

	//on regarde s'il y a au moins une valeur dans l'objet
	size_t pos_saved = _lexer.getPos();
	std::string 			token;
	JsonLexer::TokenType	tokenType;
	if ( ! _lexer.nextToken( token, tokenType ) ){
		IGN_THROW_EXCEPTION("JsonReader::readArray] json parse error");
	}
	//l'objet est vide?
	if ( tokenType == JsonLexer::JRightBrace ){
		return;
	}

	//on revient en arriere, on lit une valeur
	_lexer.setPos( pos_saved );
	for( ;; ){
		std::string key;
		ValuePtr	keyValue;

		//on lit la cle
		if ( ! _lexer.nextToken( token, tokenType ) ){
			IGN_THROW_EXCEPTION("json parse error, key expected");
		}
		if ( tokenType != JsonLexer::JString ){
			IGN_THROW_EXCEPTION("json parse error, key expected");
		}

		key = token;

		//on lit :
		if ( ! _lexer.nextToken( token, tokenType ) ){
			IGN_THROW_EXCEPTION("json parse error, expected :");
		}
		if ( tokenType != JsonLexer::JColon ){
			IGN_THROW_EXCEPTION("json parse error, expected :");
		}

		//on lit la valeur de la cle
		readValue(keyValue);

		//on ajout a l'objet
		if (verbose) std::cout<<"[JsonReader::readObject] add ("<<key<<","<<keyValue->toString()<<")"<<std::endl;
		obj->setAttribute( key, keyValue.release() );


		//on verifie } ou ,
		if ( ! _lexer.nextToken( token, tokenType ) ){
			IGN_THROW_EXCEPTION( "JsonReader::readArray] json parse error 4" );
		}

		if ( tokenType == JsonLexer::JRightBrace ){
			return;
		}else if ( tokenType == JsonLexer::JComma ){
			continue;
		}else{
			IGN_THROW_EXCEPTION("json parse error, error reading object <,> or <}> expected");
		}
	}
}

///
///
///
void	ReadJson( std::string const & json, ign::data::Variant& value )
{
	ign::data::json::JsonReader reader;
	reader.read( json, value );
}

///
///
///
ign::data::Variant		ReadJson( std::string const & json )
{
	ign::data::Variant res;
	ReadJson(json,res);
	return res;
}

///
///
///
void 	ReadJsonFile( std::string const& filename, Variant & value )
{
	value.setToNull();
	std::ifstream ifs(filename.c_str(),std::ios_base::binary);
	if ( ! ifs.good() ){
		ifs.close();
		return ;
	}
	std::ostringstream oss;
    char c;
    while (ifs.get(c)) {
        oss << c;
    }
	ifs.close();

	ReadJson(oss.str(),value);
}


}
}
}

