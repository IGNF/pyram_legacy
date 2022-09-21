#include <ignDataNull.h>


using namespace ign;
using namespace ign::data;

///
///
///
Null::Null()
{

}

///
///
///
Null::Null( Null const& other )
{

}

///
///
///
Null& Null::operator = ( Null const& other )
{
	return *this ;
}

///
///
///
Null::~Null()
{

}

///
///
///
bool	Null::isNull() const
{
	return true;
}

///
///
///
std::string Null::toString() const
{
	return "null";
}

///
///
///
Value*		Null::clone()   const
{
	return new Null(*this);
}

///
///
///
bool	Null::equals( Value const& other, bool strict ) const
{
	return ( other.getValueType() == data::Value::VtNull );
}

///
///
///
Value::ValueType const& 	Null::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Null::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Null::Type()
{
	static Value::ValueType type(VtNull);
	return type;
}

///
///
///
std::string const&	Null::TypeName()
{
	static std::string name("Null");
	return name;
}

///
///
///
void Null::_asJson( std::ostream & s ) const
{
	s << "null" ;
}

