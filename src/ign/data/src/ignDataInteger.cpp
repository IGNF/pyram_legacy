#include <ignDataInteger.h>


using namespace ign;
using namespace ign::data;

///
///
///
Integer::Integer():
	_value(0)
{

}

///
///
///
Integer::Integer( Integer const& other ):
	_value(other._value)
{

}

///
///
///
Integer& Integer::operator = ( Integer const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
Integer::~Integer()
{

}

///
///
///
Integer::Integer( int const& value ):
	_value(value)
{

}

///
///
///
Integer& Integer::operator = ( int const& value )
{
	_value = value;
	return *this;
}

///
///
///
std::string Integer::toString() const
{
	return Helper::ToString( _value );
}

///
///
///
bool		Integer::toBoolean() const
{
	return ( _value != 0 );
}

///
///
///
int		Integer::toInteger() const
{
	return _value;
}

///
///
///
int64_t		Integer::toLong() const
{
	return _value;
}

///
///
///
float        Integer::toFloat() const
{
    return static_cast<float>(_value);
}

///
///
///
double		Integer::toDouble() const
{
	return static_cast<double>(_value);
}

///
///
///
Value*	Integer::clone()   const
{
	return new Integer(*this);
}

///
///
///
bool	Integer::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtInteger ){
		return false;
	}
	try {
		return toInteger() == other.toInteger();
	}catch( ... ){
		return false;
	}
}

///
///
///
int const& Integer::value() const
{
	return _value;
}

///
///
///
int & Integer::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	Integer::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Integer::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Integer::Type()
{
	static Value::ValueType type(VtInteger);
	return type;
}

///
///
///
std::string const&	Integer::TypeName()
{
	static std::string name("Integer");
	return name;
}

///
///
///
void Integer::_asJson( std::ostream & s ) const
{
	s << _value ;
}
