#include <ignDataArray.h>
#include <ignDataVariant.h>

using namespace ign;
using namespace ign::data;

///
///
///
Array::Array():
	_values()
{

}

///
///
///
Array::Array( Array const& other )
{
	(*this) = other;
}

///
///
///
Array& Array::operator = ( Array const& other )
{
	_values = other._values;
	return *this;
}

///
///
///
Array::~Array()
{

}

///
///
///
size_t 	Array::numItems() const
{
	return _values.size();
}

///
///
///
Variant const& 	Array::item( size_t const& i ) const
{
	return _values[i];
}

///
///
///
Variant & 	Array::item( size_t const& i )
{
	return _values[i];
}




///
///
///
void	Array::setItem( size_t const& i, Value const& item )
{
	IGN_ASSERT( i < _values.size() );
	_values[i].reset( item.clone() );
}

///
///
///
void	Array::addItem( Value const& value )
{
	_values.push_back( value.clone() );
}

///
///
///
void	Array::addItem( Value* value )
{
	_values.push_back( Variant() );
	_values.back().reset( value );
}

///
///
///
void	Array::removeItem( size_t const& i )
{
	IGN_ASSERT( i < _values.size() );
	_values.erase( _values.begin() + i );
}

///
///
///
void	Array::clear()
{
	_values.clear();
}


///
///
///
Variant &	Array::operator [] ( size_t const& i )
{
	IGN_ASSERT( i < numItems() );
	return _values[i];
}

///
///
///
Variant const &	 Array::operator [] ( size_t const& i ) const
{
	IGN_ASSERT( i < numItems() );
	return _values[i];
}


///
///
///
std::string  Array::toString() const
{
	return asJson();
}


///
///
///
Value*	Array::clone()   const
{
	return new Array(*this);
}

///
///
///
bool	Array::equals( Value const& other, bool strict ) const
{
	if ( other.getValueType() != data::Value::VtArray ){
		return false;
	}
	Array const& otherArray = other.as< Array >();
	if ( numItems() != otherArray.numItems() ){
		return false;
	}
	size_t n = numItems();
	for ( size_t i = 0; i < n; i++ ){
		if ( ! item(i)->equals( *otherArray.item(i), strict ) ){
			return false;
		}
	}
	return true;
}

///
///
///
Value::ValueType const& 	Array::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Array::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Array::Type()
{
	static Value::ValueType type(VtArray);
	return type;
}

///
///
///
std::string const&	Array::TypeName()
{
	static std::string name("Array");
	return name;
}


///
///
///
void Array::_asJson( std::ostream & s ) const
{
	s << "[";
	bool isFirst = true;
	for ( std::vector< Variant >::const_iterator it = _values.begin(); it != _values.end(); ++it )
	{
		if ( isFirst ){
			isFirst = false;
		}else{
			s << ",";
		}
		it->toJson( s ) ;
	}
	s << "]";
}
