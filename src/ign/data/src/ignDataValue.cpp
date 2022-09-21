#include <ignDataValue.h>

#include <ignCommon.h>

#include <ignDataBoolean.h>
#include <ignDataInteger.h>
#include <ignDataLong.h>
#include <ignDataDouble.h>
#include <ignDataFloat.h>

using namespace ign;
using namespace ign::data;

///
///
///
Value::~Value()
{

}

///
///
///
bool	Value::isNull() const
{
	return false;
}

///
///
///
bool   Value::toBoolean() const
{
	IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), Boolean::TypeName() ) );
}

///
///
///
int	Value::toInteger() const
{
	IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), Integer::TypeName() ) );
}

///
///
///
int64_t	Value::toLong() const
{
	IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), Long::TypeName() ) );
}

///
///
///
double	Value::toDouble() const
{
	IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), Double::TypeName() ) );
}

///
///
///
float    Value::toFloat() const
{
    IGN_THROW_EXCEPTION( Helper::GetMessageBadConvert( getValueTypeName(), Float::TypeName() ) );
}

///
///
///
std::string Value::asJson( int const& numDecimals ) const
{
	std::ostringstream oss ;
	if ( numDecimals < 0 ){
		oss.precision( std::numeric_limits< double >::digits10 + 1 );
	}else{
		oss.precision( numDecimals );
		oss << std::fixed ;
	}
	_asJson( oss ) ;
	return oss.str() ;
}

///
///
///
void Value::asJson( std::ostream & s ) const
{
	_asJson( s ) ;
}

///
///
///
std::string 	Value::GetTypeName( Value::ValueType const& valueType )
{
	switch ( valueType ){
	case VtNull:
		return "Null";
	case VtBoolean:
		return "Boolean";
    case VtInteger:
        return "Integer";
	case VtLong:
		return "Long";
	case VtDouble:
		return "Double";
	case VtString:
		return "String";
	case VtArray:
		return "Array";
	case VtObject:
		return "Object";
	case VtDate:
		return "Date";
	case VtDateTime:
		return "DateTime";
	case VtGeometry:
		return "Geometry";
    case VtPath:
        return "Path";
	case VtFilePath:
		return "FilePath";
	default:
		return "UserDefined";
	}
}

///
///
///
Value::ValueType 	Value::	StringToType( const std::string& str)
{
	if (str == GetTypeName(VtNull)) return VtNull;
	else if (str == GetTypeName(VtBoolean)) return VtBoolean;
	else if (str == GetTypeName(VtInteger)) return VtInteger;
	else if (str == GetTypeName(VtLong)) return VtLong;
	else if (str == GetTypeName(VtDouble)) return VtDouble;
	else if (str == GetTypeName(VtString)) return VtString;
	else if (str == GetTypeName(VtArray)) return VtArray;
	else if (str == GetTypeName(VtObject)) return VtObject;
	else if (str == GetTypeName(VtDate)) return VtDate;
	else if (str == GetTypeName(VtDateTime)) return VtDateTime;
	else if (str == GetTypeName(VtGeometry)) return VtGeometry;
	else if (str == GetTypeName(VtPath)) return VtPath;
	else if (str == GetTypeName(VtFilePath)) return VtFilePath;
	else IGN_ASSERT(false);
	
	return VtNull;
}

///
///
///
Value::Value()
{

}

///
///
///
bool	Value::equals( Value const& other, bool strict ) const
{
	std::ostringstream oss;
	oss << "undefined equals method for " << getValueTypeName() ;
	IGN_THROW_EXCEPTION( oss.str() );
}


