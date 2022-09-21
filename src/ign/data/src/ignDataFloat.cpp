#include <ignDataFloat.h>


using namespace ign;
using namespace ign::data;
using namespace ign::numeric;
///
///
///
Float::Float():
	_value(0.0f)
{

}

///
///
///
Float::Float( Float const& other ):
	_value(other._value)
{

}

///
///
///
Float& Float::operator = ( Float const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
Float::~Float()
{

}

///
///
///
Float::Float( float const& value ):
	_value(value)
{

}

///
///
///
Float& Float::operator = ( float const& value )
{
	_value = value;
	return *this;
}

///
///
///
std::string Float::toString() const
{
	return Helper::ToString(_value);
}

///
///
///
int64_t	Float::toLong() const
{
	return (int64_t)Numeric< float >::round(_value);
}

///
///
///
double	Float::toDouble() const
{
	return static_cast<double>(_value);
}

///
///
///
float    Float::toFloat() const
{
    return _value;
}

///
///
///
Value*	Float::clone()   const
{
	return new Float(*this);
}

///
///
///
bool	Float::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtFloat ){
		return false;
	}
	try {
		return toDouble() == other.toDouble();
	}catch( ... ){
		return false;
	}
}


///
///
///
float const& Float::value() const
{
	return _value;
}

///
///
///
float & 	Float::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	Float::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Float::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Float::Type()
{
	static Value::ValueType type(VtFloat);
	return type;
}

///
///
///
std::string const&	Float::TypeName()
{
	static std::string name("Float");
	return name;
}

///
///
///
void Float::_asJson( std::ostream & s ) const
{
	s << _value ;
}
