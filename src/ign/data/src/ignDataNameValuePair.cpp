#include <ignDataNameValuePair.h>

#include <ignDataVariant.h>

using namespace ign;
using namespace ign::data;

///
///
///
NameValuePair::NameValuePair():
		_name(),
		_value(0)
{

}

///
///
///
NameValuePair::NameValuePair( std::string const& name, Value const& value ):
	_name(name),
	_value(value.clone())
{

}

///
///
///
NameValuePair::NameValuePair( std::string const& name, Value* value ):
	_name(name),
	_value(value)
{

}


///
///
///
NameValuePair::NameValuePair( NameValuePair const & other ):
	_name(other._name),
	_value(other._value)
{

}


///
///
///
NameValuePair& NameValuePair::operator = ( NameValuePair const& other )
{
	_name  = other._name;
	_value = other._value;
	return *this;
}

///
///
///
NameValuePair::~NameValuePair()
{

}

///
///
///
bool	NameValuePair::isNull() const
{
	return _value.isNull() ;
}

///
///
///
void	NameValuePair::setToNull()
{
	_value.reset( 0 );
}

///
///
///
std::string const& 	NameValuePair::name() const
{
	return _name;
}

///
///
///
std::string & 	NameValuePair::name()
{
	return _name;
}

///
///
///
Variant const& 	NameValuePair::value() const
{
	return _value;
}

///
///
///
Variant & 	NameValuePair::value()
{
	return _value;
}

///
///
///
void		NameValuePair::setValue( Value const& value )
{
	_value.reset( value.clone() );
}

///
///
///
void	NameValuePair::setValue( Value* value )
{
	_value.reset( value );
}
