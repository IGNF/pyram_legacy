#include <ignTransform.h>
#include <ignTransformSystem.h>
#include <ignRuntime.h>

using namespace ign;
using namespace ign::math;
using namespace ign::transform;

#ifdef _IGN_USING_TRANSFORM_UNIT_

///
///
///
Vector::Vector(size_t n, const numeric::unit::SysUnitPtr &sysUnit) :
_sysUnit(sysUnit)
{
	_data.resize(n);
}

///
///
///
Vector::Vector(const Vector &other):
_data(other._data),
_sysUnit(other._sysUnit)
{
} 


///
///
///
Vector::Vector(double x, double y):
_sysUnit(NULL)
{
	_data.resize(2);
	_data[0] = x;
	_data[1] = y;
}	

///
///
///
Vector::Vector(double x, double y, double z):
_sysUnit(NULL)
{
	_data.resize(3);
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;
}

///
///
///
//to-do: gerer la conversion d'unites
Vector &	Vector::operator = (const Vector &other)
{
	(*this)._data = other._data;
	(*this)._sysUnit = other._sysUnit;
	return (*this);
}

///
///
///
//to-do: gerer la conversion d'unites
Vector &	Vector::operator -= (const Vector &other)
{
    if (_data.size() != other._data.size())
        IGN_THROW_EXCEPTION("Vector operator -= sur des Vector de tailles differentes");
	//IGN_ASSERT(_data.size() == other._data.size());
	for (size_t i = 0; i < _data.size(); ++i)
	{
		_data[i] -= other._data[i];
	}
	return (*this);
}

///
///
///
//to-do: gerer la conversion d'unites
Vector &	Vector::operator += (const Vector &other)
{
    if (_data.size() != other._data.size())
        IGN_THROW_EXCEPTION("Vector operator += sur des Vector de tailles differentes");
	//IGN_ASSERT(_data.size() == other._data.size());
	for (size_t i = 0; i < _data.size(); ++i)
	{
		_data[i] += other._data[i];
	}
	return (*this);
}

///
///
///
//to-do: gerer la conversion d'unites
Vector	Vector::	operator +( Vector const & other ) const
{
    if (_data.size() != other._data.size())
        IGN_THROW_EXCEPTION("Vector operator + sur des Vector de tailles differentes");
	//IGN_ASSERT(_data.size() == other._data.size());
	Vector v(*this);
	for (size_t i = 0; i < _data.size(); ++i)
	{
		v[i] += other._data[i];
	}
	return v;
}

///
///
///
//to-do: gerer la conversion d'unites
Vector	Vector::	operator -( Vector const & other ) const
{
    if (_data.size() != other._data.size())
        IGN_THROW_EXCEPTION("Vector operator - sur des Vector de tailles differentes");
	//IGN_ASSERT(_data.size() == other._data.size());
	Vector v(*this);
	for (size_t i = 0; i < _data.size(); ++i)
	{
		v[i] -= other._data[i];
	}
	return v;
}

///
///
///
//to-do: gerer la conversion d'unites
bool    Vector::   operator == (const Vector& other) const
{
	//on ne teste pas les unites ici...
	if (_data.size() != other._data.size()) return false;
	for (size_t i = 0; i < _data.size(); ++i)
	{
		if (_data[i] != other._data[i]) return false;
	}
	return true;
}

///
///
///
Vector::~Vector()
{
}

///
///
///
ign::ublas::vector<double>&	Vector::	data()
{
	return _data;
}

///
///
///
const ign::ublas::vector<double>&	Vector::	data() const
{
	return _data;
}

///
///
///
numeric::unit::SysUnitPtr		Vector::	sysUnit()
{
	return _sysUnit;
}

///
///
///
const numeric::unit::SysUnitPtr	Vector::	sysUnit() const
{
	return _sysUnit;
}	

///
///
///
void	Vector::	setSysUnit(const numeric::unit::SysUnitPtr& other)
{
	_sysUnit = other;
}

///
///
///
namespace ign
{
	namespace transform
	{
		std::ostream& operator << ( std::ostream& s, ign::transform::Vector const& v )
		{
			std::ostringstream oss;
			oss<<"(";
			for ( size_t i = 0; i < v.size(); ++i ){
				oss << v[i]<<" ";
			}
			oss<<")";
			s << oss.str();
			return s;
		}
	}
}
#endif