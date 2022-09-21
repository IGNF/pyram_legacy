#include <sstream>

#include <ignDataObject.h>
#include <ignDataHelper.h>
#include <ignDataNameValuePair.h>


using namespace ign;
using namespace ign::data;


int Object::_IndentJson = 0;
///
///
///
Object::Object():
	_attributes(),_index()
{

}

///
///
///
Object::Object( Object const& other ):
	_attributes( other._attributes ),_index()
{
	_computeIndex();
}

///
///
///
Object& Object::operator = ( Object const& other )
{
	_attributes = other._attributes;
	_computeIndex();
	return *this;
}

///
///
///
Object::~Object()
{

}

void Object::append(Object const& other)
{
	_attributes.insert(_attributes.end(),other._attributes.begin(),other._attributes.end());
	_computeIndex();
}

///
///
///
bool	Object::hasAttribute( std::string const& name ) const
{
	return ( getAttributeNumber(name) >= 0 );
}

///
///
///
std::string const&	 Object::getAttributeName( size_t const& i ) const
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].name();
}

///
///
///
int 	Object::getAttributeNumber( std::string const& name ) const
{
	if ( _index.empty() ){
		return -1;
	}

	std::map< std::string, int >::const_iterator it = _index.find(name);
	if ( it == _index.end() ){
		return -1;
	}else{
		return it->second;
	}
}

///
///
///
bool	Object::isNullAttribute( std::string const& name ) const
{
	int id = getAttributeNumber(name);
	if (id!=-1)
		return isNullAttribute( id );

	IGN_THROW_EXCEPTION( Helper::GetMessageNoAttribute( name ) );
}

///
///
///
bool	Object::isNullAttribute( size_t const& i ) const
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].isNull();
}


///
///
///
Value::ValueType	Object::getAttributeType( size_t const& i ) const
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].value()->getValueType();
}

///
///
///
Value::ValueType	Object::getAttributeType( std::string const& name ) const
{
	int i = getAttributeNumber(name);
	if ( i < 0 ){
		IGN_THROW_EXCEPTION( Helper::GetMessageNoAttribute( name ) );
	}
	return getAttributeType(i);
}


///
///
///
size_t 	Object::numAttributes() const
{
	return _attributes.size();
}

///
///
///
bool	Object::isEmpty() const
{
	return _attributes.empty();
}

///
///
///
void Object::setAttribute( std::string const& name, Value* value )
{
	int i = getAttributeNumber(name);
	if ( i < 0 ){
		_index.insert( std::make_pair( name, _attributes.size() ) );
		//definition de l'attribut avant appel du constructeur par recopie
		_attributes.push_back( NameValuePair(name,0) );
		_attributes.back().setValue(value);
	}else{
		_attributes[i].setValue( value );
	}
}

///
///
///
void	Object::setAttribute( std::string const& name, Value const& value )
{
	setAttribute(name,value.clone());
}

///
///
///
Variant const& 	Object::attribute( std::string const& name ) const
{
	int i = getAttributeNumber(name);
	if (i < 0)
	{
		std::ostringstream oss;
		oss<<"[Object::attribute] no attribute '"<<name<<"' in Object";
		IGN_THROW_EXCEPTION(oss.str());
	}
	//IGN_ASSERT( i >= 0 );
	return _attributes[i].value();
}

///
///
///
Variant & 	Object::attribute( std::string const& name )
{
	int i = getAttributeNumber(name);
	if (i < 0)
	{
		std::ostringstream oss;
		oss<<"[Object::attribute] no attribute '"<<name<<"' in Object";
		IGN_THROW_EXCEPTION(oss.str());
	}
	//IGN_ASSERT( i >= 0 );
	return _attributes[i].value();
}

///
///
///
Variant const& 	Object::attribute( size_t const& i ) const
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].value();
}

///
///
///
Variant &  Object::attribute( size_t const& i )
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].value();
}

///
///
///
Variant const & Object::operator[] ( size_t const& i ) const
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].value();
}

///
///
///
Variant & Object::operator[] ( size_t const& i )
{
	IGN_ASSERT( i < _attributes.size() );
	return _attributes[i].value();
}

///
///
///
Variant const & Object::operator[] ( std::string const& name ) const
{
	return attribute(name);
}

///
///
///
Variant & 	Object::operator[] ( std::string const& name )
{
	return attribute(name);
}

///
///
///
void	Object::removeAttribute( size_t const& i )
{
	IGN_ASSERT( i < _attributes.size() );
	_attributes.erase( _attributes.begin() + i );
	_computeIndex();
}

///
///
///
void	Object::removeAttribute( std::string const& name, bool throwIfNotExists )
{
	int i = getAttributeNumber( name );
	if ( i < 0 ){
		if ( throwIfNotExists ){
			IGN_THROW_EXCEPTION( Helper::GetMessageNoAttribute(name) );
		}
		return;
	}
	removeAttribute(i);
}

///
///
///
void	Object::clear()
{
	_attributes.clear();
	_computeIndex();
}

///
///
///
std::string 	Object::toString() const
{
	return asJson();
}


///
///
///
Value*	Object::clone()   const
{
	return new Object(*this);
}


///
///
///
bool	Object::equals( Value const& other, bool strict ) const
{
	if ( other.getValueType() != data::Value::VtObject ){
		return false;
	}
	Object const& otherObject = other.as< Object >();

	for ( size_t i = 0; i < numAttributes(); i++ ){
		std::string const& name = getAttributeName(i);
		if ( ! otherObject.hasAttribute(name) ){
			return false;
		}
		if ( ! attribute(i)->equals( *otherObject.attribute(name), strict ) ){
			return false;
		}
	}
	for ( size_t i = 0; i < otherObject.numAttributes(); i++ ){
		if ( ! hasAttribute( otherObject.getAttributeName(i) ) ){
			return false;
		}
	}
	return true;
}

///
///
///
void	Object::_computeIndex()
{
	_index.clear();
	int ct = 0;
	for ( std::vector< NameValuePair >::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it,++ct ){
		_index.insert( std::make_pair( it->name(), ct ) );
	}
}


///
///
///
Value::ValueType const& 	Object::getValueType() const
{
	return Type();
}

///
///
///
std::string const& 			Object::getValueTypeName() const
{
	return TypeName();
}

///
///
///
Value::ValueType const&		Object::Type()
{
	static Value::ValueType type(VtObject);
	return type;
}

///
///
///
std::string const&	Object::TypeName()
{
	static std::string name("Object");
	return name;
}

///
///
///
void Object::_asJson( std::ostream & s ) const
{
    const std::string space=" ";
    const std::string carriage="\n";
    IGN_ASSERT(_IndentJson>=0);
    for (size_t k = 0; k < _IndentJson; ++k) s<<space;
	s << "{" << carriage;
    _IndentJson+=4;
	bool isFirst = true;
	for ( std::vector< NameValuePair >::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it ){
		if ( isFirst ){
			isFirst = false;
		}else{
			s << ","<<carriage;
		}
        
        for (size_t k = 0; k < _IndentJson; ++k) s<<space;
        
		s << Helper::quote(it->name()) << ":" ;
		if ( it->isNull() ){
			s << "null";
		}else{
			it->value().toJson( s ) ;
		}
	}
     _IndentJson-=4;
    for (size_t k = 0; k < _IndentJson; ++k) s<<space;
	s << carriage<<"}" ;
}
