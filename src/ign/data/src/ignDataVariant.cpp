#include <ignCommon.h>

#include <ignDataVariant.h>
#include <ignDataValue.h>
#include <ignDataNull.h>

#include <ignDataJsonReader.h>


using namespace ign;
using namespace ign::data;


///
///
///
Variant::Variant():
	_value(new Null())
{

}

///
///
///
Variant::Variant( Variant const& other ):
	_value(other._value->clone())
{

}

///
///
///
Variant& Variant::operator = ( Variant const& other )
{
	reset( other._value->clone() );
	return *this;
}

///
///
///
Variant::~Variant()
{
	delete _value;
	_value = 0 ;
}

///
///
///
Variant::Variant( Value const& value ):
	_value( value.clone() )
{

}

///
///
///
Variant::Variant( Value* value ):
	_value( value?value : new Null() )
{

}

///
///
///
Variant& Variant::operator = ( Value const& value )
{
	reset( value.clone() );
	return *this;
}

///
///
///
bool	Variant::isNull() const
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return _value->isNull();
}

///
///
///
void	Variant::setToNull()
{
	reset(0);
}

///
///
///
void		Variant::setValue( Value const& value )
{
	reset( value.clone() );
}

///
///
///
void	Variant::setValue( Value* value )
{
	reset( value );
}

///
///
///
Value const &  Variant::value() const
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return *_value;
}

///
///
///
Value & 	Variant::value()
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return *_value;
}

///
///
///
void 	Variant::reset( Value* value )
{
	delete _value;
	if ( value ){
		_value = value ;
	}else{
		_value = new Null();
	}
}

///
///
///
Value*	Variant::release()
{
	Value* tmp = _value;
	_value = new Null() ;
	return tmp;
}

///
///
///
Value*	Variant::get()
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return _value;
}

///
///
///
Value const*	Variant::get() const
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return _value;
}

///
///
///
Value* 	Variant::operator -> ()
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return _value ;
}



///
///
///
Value const* 	Variant::operator -> () const
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return _value ;
}

///
///
///
Value& 	Variant::operator * ()
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return *_value ;
}

///
///
///
Value const& 	Variant::operator * () const
{
	IGN_SAFE_MODE_ASSERT( _value != 0 );
	return *_value ;
}


///
///
///
std::string Variant::toJson( int const& numDecimals ) const
{
	return _value->asJson( numDecimals );
}

///
///
///
void	Variant::toJson( std::ostream & s ) const
{
	_value->asJson( s );
}

///
///
///
void	Variant::fromJson( std::string const& json )
{
	json::JsonReader reader;
	reader.read(json,*this);
}
