#include <ignDataString.h>

#include <ignDataHelper.h>


using namespace ign;
using namespace ign::data;

///
///
///
String::String():
	_value()
{

}

///
///
///
String::String( String const& other ):
	_value(other._value)
{

}

///
///
///
String& String::operator = ( String const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
String::~String()
{

}

///
///
///
String::String( std::string const& value ):
	_value(value)
{

}

///
///
///
String::String( const char* str )
{
	_value = str;
}

///
///
///
String& String::operator = ( std::string const& value )
{
	_value = value;
	return *this;
}

///
///
///
String& String::operator = ( const char* str )
{
	_value = str;
	return *this;
}

///
///
///
std::string String::toString() const
{
	return _value;
}


///
///
///
bool	String::toBoolean() const
{
	bool out;
	if ( ! Helper::StringTo(_value,out) ){
		IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), "Boolean", toString() ) );
	}
	return out;
}

///
///
///
int		String::toInteger() const
{
	int out;
	if ( ! Helper::StringTo(_value,out) ){
		IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), "Integer", toString() ) );
	}
	return out;
}

///
///
///
int64_t		String::toLong() const
{
	int64_t out;
	if ( ! Helper::StringTo(_value,out) ){
		IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), "Long", toString() ) );
	}
	return out;
}

///
///
///
double		String::toDouble() const
{
	double out;
	if ( ! Helper::StringTo(_value,out) ){
		IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), "Double", toString() ) );
	}
	return out;
}

///
///
///
float        String::toFloat() const
{
    float out;
    if ( ! Helper::StringTo(_value,out) ){
        IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), "Float", toString() ) );
    }
    return out;
}

///
///
///
Value*	String::clone()   const
{
	return new String(*this);
}


///
///
///
bool	String::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtString ){
		return false;
	}
	return toString() == other.toString();
}

///
///
///
std::string const& 	String::value() const
{
	return _value;
}

///
///
///
std::string & String::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	String::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			String::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		String::Type()
{
	static Value::ValueType type(VtString);
	return type;
}

///
///
///
std::string const&	String::TypeName()
{
	static std::string name("String");
	return name;
}

///
///
///
void String::_asJson( std::ostream & s ) const
{
	s << Helper::quote(_value) ;
}

