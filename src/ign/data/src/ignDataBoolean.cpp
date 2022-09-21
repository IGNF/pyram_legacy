#include <ignDataBoolean.h>


using namespace ign;
using namespace ign::data;

///
///
///
Boolean::Boolean():
	_value(false)
{

}

///
///
///
Boolean::Boolean( Boolean const& other ):
	_value(other._value)
{

}

///
///
///
Boolean& Boolean::operator = ( Boolean const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
Boolean::~Boolean()
{

}

///
///
///
Boolean::Boolean( bool const& value ):
	_value(value)
{

}

///
///
///
Boolean& Boolean::operator = ( bool const& value )
{
	_value = value;
	return *this;
}


///
///
///
std::string Boolean::toString() const
{
	return Helper::ToString(_value);
}

///
///
///
bool		Boolean::toBoolean() const
{
	return _value;
}

///
///
///
int64_t		Boolean::toLong() const
{
	return ( _value ?1:0 );
}


///
///
///
Value*	Boolean::clone()   const
{
	return new Boolean(*this);
}


///
///
///
bool	Boolean::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtBoolean ){
		return false;
	}
	try {
		return toBoolean() == other.toBoolean();
	}catch( ... ){
		return false;
	}
}

///
///
///
bool const& Boolean::value() const
{
	return _value;
}

///
///
///
bool & 		Boolean::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	Boolean::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Boolean::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Boolean::Type()
{
	static Value::ValueType type(VtBoolean);
	return type;
}


///
///
///
std::string const&	Boolean::TypeName()
{
	static std::string name("Boolean");
	return name;
}



///
///
///
void Boolean::_asJson( std::ostream & s ) const
{
	s << ( _value ? "true" : "false" ) ;
}
