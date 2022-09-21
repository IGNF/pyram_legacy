#include <ignTransform.h>
#include <ignTransformSystem.h>
#include <ignRuntime.h>

using namespace ign;
using namespace ign::math;
using namespace ign::transform;

///
///
///
Transform::Transform(SystemPtr const& sys1, SystemPtr const& sys2)
{
	IGN_ASSERT(sys1.get() && sys2.get());
	SystemRegistry::Instance().registerSystem(sys1);
	SystemRegistry::Instance().registerSystem(sys2);
	_system1 = SystemRegistry::Instance().getSystemById(sys1->uuid());
	_system2 = SystemRegistry::Instance().getSystemById(sys2->uuid());
	_verticalSystem = "";
}

///
///
///
Transform::~Transform()
{
}

///
///
///
void	Transform::	direct( const transform::Vector& pt1, transform::Vector& pt2 ) const
{	
    bool verbose = 0;
	if (verbose) std::cout<<"[Transform::direct] BEGIN"<<std::endl;

	const ign::numeric::unit::SysUnitPtr& sysUnit1 = pt1.sysUnit();
    
	if (!_system1->sysUnit()->needConversion(pt1.data(), sysUnit1))
	{
		if (verbose) std::cout<<"[Transform::direct] BEGIN _system1ToSystem2"<<std::endl;
		this->_system1ToSystem2(pt1, pt2);
		if (verbose) std::cout<<"[Transform::direct] END _system1ToSystem2"<<std::endl;
	}
	else
	{
		transform::Vector other(pt1);
		_system1->sysUnit()->toCanonic(other.data(), sysUnit1);
		this->_system1ToSystem2(other, pt2);
	}
	
	const ign::numeric::unit::SysUnitPtr& sysUnit2 = pt2.sysUnit();

	if (_system2->sysUnit()->needConversion(pt2.data(), sysUnit2))
	{
		_system2->sysUnit()->fromCanonic(pt2.data(), sysUnit2);
	}
	else
	{
		pt2.setSysUnit(this->_system2->sysUnit());
	}
	if (verbose) std::cout<<"[Transform::direct] END"<<std::endl;
}

///
///
///
void	Transform::	inverse( const transform::Vector& pt1, transform::Vector& pt2) const
{
	const ign::numeric::unit::SysUnitPtr& sysUnit1 = pt1.sysUnit();
	const ign::numeric::unit::SysUnitPtr& sysUnit2 = pt2.sysUnit();
	if (!_system2->sysUnit()->needConversion(pt1.data(),sysUnit1))
	{
		this->_system2ToSystem1(pt1, pt2);
	}
	else
	{
		transform::Vector other(pt1);
		_system2->sysUnit()->toCanonic(other.data(), sysUnit1);
		this->_system2ToSystem1(other, pt2);
	}
	
	if (_system1->sysUnit()->needConversion(pt2.data(), sysUnit2))
	{
		_system1->sysUnit()->fromCanonic(pt2.data(), sysUnit2);
	}
	else
	{
		pt2.setSysUnit(this->_system1->sysUnit());
	}
}

///
///
///
void Transform::direct(std::vector<transform::Vector*> & points) const
{
	bool verbose = false;
	if (verbose) std::cout<<"[Transform::direct] ";
	for (size_t i = 0; i<points.size(); ++i)
	{
		if (_system1->sysUnit()->needConversion(points[i]->data(), points[i]->sysUnit()))
		{
			_system1->sysUnit()->toCanonic(points[i]->data(), points[i]->sysUnit());
		}
	}
	if (verbose) std::cout<<" [_system1ToSystem2] "<<std::endl;
	_system1ToSystem2(points);

	for (size_t i = 0; i<points.size(); ++i)
	{
		points[i]->setSysUnit(this->_system2->sysUnit());
	}
}

///
///
///
void Transform::inverse(std::vector<transform::Vector*> & points) const
{
	for (size_t i = 0; i<points.size(); ++i)
	{
		if (_system2->sysUnit()->needConversion(points[i]->data(), points[i]->sysUnit()))
		{
			_system2->sysUnit()->toCanonic(points[i]->data(), points[i]->sysUnit());
		}
	}
	
	_system2ToSystem1(points);
	
	for (size_t i = 0; i<points.size(); ++i)
	{
		points[i]->setSysUnit(this->_system1->sysUnit());
	}
}

///
///
///
void Transform::_system1ToSystem2(std::vector<transform::Vector*> & points) const
{
	for (size_t i = 0; i<points.size(); ++i)
	{
		const Vector& tmp = *points[i];
		_system1ToSystem2(tmp, *points[i]);
	}
}

///
///
///
void Transform::_system2ToSystem1(std::vector<transform::Vector*> & points) const
{
	for (size_t i = 0; i<points.size(); ++i)
	{
		const Vector& tmp = *points[i];
		_system2ToSystem1(tmp, *points[i]);
	}
}

///
///
///
Vec3d Transform::localVertical(const Vec3d& p) const
{
	return Vec3d(0., 0., 1);
}

///
///
///
std::string		Transform::	verticalSystem() const
{
	return _verticalSystem;
}
///
///
///
void	Transform::	verticalSystem(const std::string& systemVert) 
{
	_verticalSystem = systemVert;
}

///
///
///
std::string Transform::getSystem1Id() const
{
    return _system1->uuid();
}

///
///
///
std::string Transform::getSystem2Id() const
{
	return _system2->uuid();
}

///
///
///
const SystemPtr &Transform::getSystem1() const
{
	return _system1;
}

///
///
///
const SystemPtr &Transform::getSystem2() const
{
	return _system2;
}

///
///
///
void Transform::	setSystem1(SystemPtr sys1)
{
	_system1 = sys1;
}

///
///
///
void Transform::	setSystem2(SystemPtr sys2)
{
	_system2 = sys2;
}

///
///
///
SystemPtr Transform::getSystem1()
{
	return _system1;
}

///
///
///
SystemPtr Transform::getSystem2()
{
	return _system2;
}

///
///
///
const transform::SystemRegistry &Transform::getSystemRegistry() const
{
	return SystemRegistry::Instance();
}