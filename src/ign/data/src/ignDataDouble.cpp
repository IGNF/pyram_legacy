#include <ignDataDouble.h>


using namespace ign;
using namespace ign::data;
using namespace ign::numeric;
///
///
///
Double::Double():
	_value(0.0)
{

}

///
///
///
Double::Double( Double const& other ):
	_value(other._value)
{

}

///
///
///
Double& Double::operator = ( Double const& other )
{
	_value = other._value;
	return *this;
}

///
///
///
Double::~Double()
{

}

///
///
///
Double::Double( double const& value ):
	_value(value)
{

}

///
///
///
Double& Double::operator = ( double const& value )
{
	_value = value;
	return *this;
}

///
///
///
std::string Double::toString() const
{
	return Helper::ToString(_value);
}

///
///
///
int64_t	Double::toLong() const
{
	return (int64_t)Numeric< double >::round(_value);
}

///
///
///
double	Double::toDouble() const
{
	return _value;
}

///
///
///
float    Double::toFloat() const
{
    return static_cast<float>(_value);
}

///
///
///
Value*	Double::clone()   const
{
	return new Double(*this);
}

///
///
///
bool	Double::equals( Value const& other, bool strict ) const
{
	if ( strict && other.getValueType() != data::Value::VtDouble ){
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
double const& Double::value() const
{
	return _value;
}

///
///
///
double & 	Double::value()
{
	return _value;
}


///
///
///
Value::ValueType const& 	Double::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Double::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Double::Type()
{
	static Value::ValueType type(VtDouble);
	return type;
}

///
///
///
std::string const&	Double::TypeName()
{
	static std::string name("Double");
	return name;
}

///
///
///
void Double::_asJson( std::ostream & s ) const
{
	s << _value ;
}
