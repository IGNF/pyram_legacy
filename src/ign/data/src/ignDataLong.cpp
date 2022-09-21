#include <ignDataLong.h>

using namespace ign;
using namespace ign::data;

///
///
///
Long::Long():
	_value(0)
{

}

///
///
///
Long::Long( Long const& other ):
	_value(other._value)
{

}

///
///
///
Long& Long::operator = ( Long const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
Long::~Long()
{

}

///
///
///
Long::Long( int64_t const& value ):
	_value(value)
{

}

///
///
///
Long& Long::operator = ( int64_t const& value )
{
	_value = value;
	return *this;
}

///
///
///
std::string Long::toString() const
{
	return Helper::ToString( _value );
}

///
///
///
bool		Long::toBoolean() const
{
	return ( _value != 0 );
}

///
///
///
int   		Long::toInteger() const
{
	//check overflow
	if ( _value < std::numeric_limits< int >::min() || _value > std::numeric_limits< int >::max() ){
		std::ostringstream oss;
		oss << "integer overflow (conversion from Long to Integer with value '" << _value << "')";
		IGN_THROW_EXCEPTION( oss.str() );
	}
	return (int)_value ;
}

///
///
///
int64_t		Long::toLong() const
{
	return _value;
}

///
///
///
double		Long::toDouble() const
{
	return static_cast<double>(_value);
}

///
///
///
float        Long::toFloat() const
{
    return static_cast<float>(_value);
}

///
///
///
Value*	Long::clone()   const
{
	return new Long(*this);
}

///
///
///
bool	Long::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtLong ){
		return false;
	}
	try {
		return toLong() == other.toLong();
	}catch( ... ){
		return false;
	}
}

///
///
///
int64_t const& Long::value() const
{
	return _value;
}

///
///
///
int64_t & Long::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	Long::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Long::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Long::Type()
{
	static Value::ValueType type(VtLong);
	return type;
}

///
///
///
std::string const&	Long::TypeName()
{
	static std::string name("Long");
	return name;
}

///
///
///
void Long::_asJson( std::ostream & s ) const
{
	s << _value ;
}

