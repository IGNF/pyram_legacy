/*
 *  unit.cpp
 *  ign-all
 *
 *  Created by Nicolas Bellaiche on 29/12/10.
 *  Copyright 2010 Institut Geographique National - France. All rights reserved.
 *
 */
#include <set>
#include <string>
#include <cctype>

#include <boost/algorithm/string.hpp>

#include <ignCommon.h>
#include <ignNumericSysUnit.h>
using namespace ign;
using namespace ign::numeric;
using namespace ign::numeric::unit;

double SysUnit::_AngleUserDefined = 1.;
double SysUnit::_LengthUserDefined = 1.;

SysUnitRegistryPtr SysUnitRegistry::_systemRegistry;
SysUnitPtr	SysUnit::Null = SysUnitPtr();


///
/// 
/// 
SysUnit::SysUnit(eUnit unite)
{
	_units.resize(1);
	_units(0) =(unite);
}

/// 
/// 
/// 
SysUnit::SysUnit(const std::string &id, eUnit unite):
_uuid(id)
{
    if (SysUnitRegistry::Instance().hasSystem(_uuid))
        IGN_THROW_EXCEPTION("SysUnit deja present dans le registre");
	_units.resize(1);
	_units(0) =(unite);
}

/// 
/// 
/// 
SysUnit::SysUnit(const std::string &id, const std::string& strUnite):
_uuid(id)
{
      if (SysUnitRegistry::Instance().hasSystem(_uuid))
        IGN_THROW_EXCEPTION("SysUnit deja present dans le registre");

	eUnit unite = String2Unit(strUnite);
	_units.resize(1);
	_units(0) =(unite);
}

/// 
/// 
/// 
SysUnit::SysUnit(std::string const& id, const unit_vector& unites):
_uuid(id),
_units(unites)
{
	if (SysUnitRegistry::Instance().hasSystem(_uuid))
        IGN_THROW_EXCEPTION("SysUnit deja present dans le registre");
}

/// 
/// 
/// 
const std::string&	SysUnit::uuid()const
{
	return _uuid;
}
///
///
///
SysUnit& SysUnit::operator = ( SysUnit const& other )
{
	_units = other._units;
	return *this;
}

/// 
/// 
/// 
std::string SysUnit::asText(size_t n) const
{
	return ign::numeric::unit::Unit2String(unit(n));
}

/// 
/// 
/// 
SysUnit::~SysUnit()
{
}

/// 
/// 
/// 
double SysUnit::toCanonic(const double& value, eUnit u, size_t n) const
{
	//nota:
	//1 inch = 0.0254 m
	//1 foot = 12 inches
	//1 yard = 3 feet
	//1 mile = 1760 yards
	//j'ai laisse les formules pour raison de lisibilite mais si ca pose un probleme de perforances, il faudra evidement simplifier!
	switch(this->unit(n))
	{
		case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] Canonic unit undefined");
		case ePixel:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: return value;
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}		case eMeter:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value;
				case eCentimeter: return value/100.;
				case eMillimeter: return value/1000.;
				case eKilometer: return value*1e3;
				case eInch: return value*0.0254;
				case eFoot: return value*12*0.0254;
				case eMile:return value*12*3*1760*0.0254;	
				case eLengthUserDefined: return value*_LengthUserDefined;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eCentimeter:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value*100.;
				case eCentimeter: return value;
				case eMillimeter: return value/10.;
				case eKilometer: return value*1e5;
				case eInch: return value*2.54;
				case eFoot: return value*12*2.54;
				case eMile:return value*12*3*1760*2.54;	
				case eLengthUserDefined: return value*_LengthUserDefined * 100;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eMillimeter:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value*1000.;
				case eCentimeter: return value*10.;
				case eMillimeter: return value;
				case eKilometer: return value*1e6;
				case eInch: return value*25.4;
				case eFoot: return value*12*25.4;
				case eMile:return value*12*3*1760*25.4;	
				case eLengthUserDefined: return value*_LengthUserDefined * 1000;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eKilometer:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value/1000.;
				case eCentimeter: return value/1e5;
				case eMillimeter: return value/1e6;
				case eKilometer: return value;
				case eInch: return value*0.0254 / 1000;
				case eFoot: return value*12*0.0254 / 1000;
				case eMile:return value*12*3*1760*0.0254 / 1000;	
				case eLengthUserDefined: return value*_LengthUserDefined / 1000;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eInch:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value / 0.0254;
				case eCentimeter: return value / 2.54;
				case eMillimeter: return value / 25.4;
				case eKilometer: return value / 0.0254 / 1000;
				case eInch: return value;
				case eFoot: return value / 12;
				case eMile:return value / (12*3*1760);	
				case eLengthUserDefined: return value  / 0.0254 *_LengthUserDefined;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eFoot:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value / (12 * 0.0254);
				case eCentimeter: return value / (12 * 2.54);
				case eMillimeter: return value / (12 * 25.4);
				case eKilometer: return value / (12 * 0.0254) / 1000;
				case eInch: return value / 12;
				case eFoot: return value;
				case eMile:return value / (3*1760);	
				case eLengthUserDefined: return value  / (12 * 0.0254) *_LengthUserDefined;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eMile:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value / (1760 * 12 * 3 * 0.0254);
				case eCentimeter: return value / (1760 * 12 * 3 * 2.54);
				case eMillimeter: return value / (1760 * 12 * 3 * 25.4);
				case eKilometer: return value / (1760 * 12 * 3 * 0.0254) / 1000;
				case eInch: return value / 1760 * 12 * 3;
				case eFoot: return value / 1760 * 3;
				case eMile:return value;	
				case eLengthUserDefined: return value  / (1760 * 12 * 3 * 0.0254) *_LengthUserDefined;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eLengthUserDefined:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: return value / _LengthUserDefined;
				case eCentimeter: return value * 100 / _LengthUserDefined;
				case eMillimeter: return value * 1000 / _LengthUserDefined;
				case eKilometer: return value / 1000 / _LengthUserDefined;
				case eInch: return value * 0.0254 / _LengthUserDefined;
				case eFoot: return value * 0.0254 * 12 / _LengthUserDefined;
				case eMile: return value * 0.0254 * 12 / _LengthUserDefined;	
				case eLengthUserDefined: return value  / (1760 * 12 * 3 * 0.0254) *_LengthUserDefined;
				case eDegree: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eRadian: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eGrade: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDMS: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eAngleUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eDegree:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: return value;
				case eRadian: return rad2deg(value);
				case eGrade: return grad2deg(value);
				case eDMS: return dms2deg(value);
				case eAngleUserDefined: return rad2deg(value *_AngleUserDefined);
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eRadian:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: return deg2rad(value);
				case eRadian: return value;
				case eGrade: return grad2rad(value);
				case eDMS: return deg2rad(dms2deg(value));
				case eAngleUserDefined: return value *_AngleUserDefined;
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eGrade:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: return deg2grad(value);
				case eRadian: return rad2grad(value);
				case eGrade: return value;
				case eDMS: return deg2grad(dms2deg(value));
				case eAngleUserDefined: return rad2grad(value * _AngleUserDefined);
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eDMS:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: return deg2dms(value);
				case eRadian: return deg2dms(rad2deg(value));
				case eGrade: return deg2dms(grad2deg(value));
				case eDMS: return value;
				case eAngleUserDefined: return deg2dms(rad2deg(value * _AngleUserDefined));
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
		case eAngleUserDefined:
		{
			switch(u)
			{
				case eUndefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case ePixel: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
				case eMeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eCentimeter: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMillimeter:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eKilometer:IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eInch: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eFoot: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eMile: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eLengthUserDefined: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] incompatible units");
				case eDegree: return deg2rad(value) / _AngleUserDefined;
				case eRadian: return value / _AngleUserDefined;
				case eGrade: return grad2rad(value) / _AngleUserDefined;
				case eDMS: return deg2rad(dms2deg(value)) / _AngleUserDefined;
				case eAngleUserDefined: return value;
			}
			IGN_THROW_EXCEPTION("[SysUnit::toCanonic] conversion unit undefined");
		}
			
		//default not handled to generate compilation warnings
		//default: IGN_THROW_EXCEPTION("[SysUnit::toCanonic] canonic unit undefined");
	}
												  
	IGN_THROW_EXCEPTION("[SysUnit::toCanonic] canonic unit undefined");
}

/// 
/// 
/// 
double SysUnit::fromCanonic(const double& value, eUnit u, size_t n) const
{
	SysUnit sys2(u);
//	std::cout<<"[SysUnit::fromCanonic]: "<<value<<" "<<Unit2String(unit(0))<<" a convertir en "<<Unit2String(u)<<": "<<sys2.toCanonic(value, unit(0))<<std::endl;
	return sys2.toCanonic(value, unit(n), n);
}

/// 
/// 
/// 
void	SysUnit::toCanonic(ign::ublas::vector<double>& values, const SysUnitPtr sysUnit) const
{
	//IGN_ASSERT(sysUnit);
	if(  ! sysUnit ) 
		IGN_THROW_EXCEPTION("[SysUnit::toCanonic] sysUnit is NULL ");
	for (size_t i = 0; i < values.size(); ++i)
	{
		values[i] = toCanonic(values[i], sysUnit->unit(i), i);
	}
}

/// 
/// 
/// 
void	SysUnit::fromCanonic(ign::ublas::vector<double>& values, const SysUnitPtr sysUnit) const
{
//	IGN_ASSERT(sysUnit);
	if(  ! sysUnit ) 
		IGN_THROW_EXCEPTION("[SysUnit::toCanonic] sysUnit is NULL ");
	for (size_t i = 0; i < values.size(); ++i)
	{
		values[i] = fromCanonic(values[i], sysUnit->unit(i), i);
	}
}

///
///
///
unit_vector&		SysUnit::	units() 
{
	return _units;
}

///
///
///
const unit_vector&	SysUnit::	units() const
{
	return _units;
}

///
///
///
void	SysUnit::unit(size_t n, numeric::unit::eUnit u)
{
	if (n >= _units.size())
	{
		size_t oldSize = _units.size();
		_units.resize(n+1);
		for (size_t k = oldSize; k < (n+1) ; ++k)
		{
			_units[k] = u;
			
		}
	}
	else
	{
		_units[n] = u;
	}
}

///
///
///
eUnit		SysUnit::unit(size_t n) const
{
	if (_units.empty()) return eUndefined;
	else if (_units.size() == 1) return _units[0];
	else if (n <_units.size()) return _units[n];
	IGN_ASSERT(false);
	return eUndefined;
}

///
///
///
bool	SysUnit::needConversion(const ign::ublas::vector<double>& values, const SysUnitPtr& values_units) const
{
    bool verbose = false;
	if (verbose) std::cout<<"[SysUnit::needConversion] BEGIN"<<std::endl;
    
    if (values_units.get() == this)
    {
        return false;
    }
	if (values_units.get() == NULL)
	{
		if (verbose) std::cout<<"[SysUnit::needConversion] values_units NULL"<<std::endl;
		return false;
	}

	if( values_units.get() == this )
	{
		std::cout<<" meme systeme je sors de needConversion "<<std::endl;
		return false;
	}

	if (_units.empty()) return false;
	
	//std::cout<<" je teste toutes mes unites c'est la galere "<<std::endl;

	for (size_t k = 0; k < values.size(); ++k)
	{
		if (verbose) std::cout<<"unites de la valeur: "<<k<<std::endl;
		eUnit unitValue = values_units->unit(k);
		if (verbose) std::cout<<"values_units: "<<Unit2String(unitValue)<<std::endl;

		eUnit unitSystem = unit(k);
		if (verbose) std::cout<<"unitSystem: "<<Unit2String(unitSystem)<<std::endl;

		if (unitValue == numeric::unit::eUndefined) continue;
		if (unitSystem == numeric::unit::eUndefined) continue;
		if (unitValue != unitSystem) return true;
	}
	if (verbose) std::cout<<"[SysUnit::needConversion] END"<<std::endl;

	return false;
}


///
///
///
///
///
///
///
const SysUnitPtr	SysUnitRegistry::getSystemById(const std::string& uuid) const
{
	//IGN_ASSERT(uuid != "");
	if(  uuid == "" ) 
		IGN_THROW_EXCEPTION("[SysUnit::getSystemById] uuid is an empty string ");
	system_const_iterator it;
	it = _systemMap.find(uuid);
	if (it != systemEnd())
	{
		return it->second;
	}
//	return NULL;
	IGN_THROW_EXCEPTION("[SysUnit::getSystemById] no SysUnit named " + uuid);

}

///
///
///
void SysUnitRegistry::registerSystem(SysUnitPtr sys)
{
	IGN_ASSERT(sys.get());
	std::string uuid = sys->uuid();
	
	system_const_iterator it;
	it = _systemMap.find(uuid);
	if (it==systemEnd())
	{
		_systemMap.insert(std::make_pair(uuid, sys));
	}
}

///
///
///
SysUnitRegistry :: SysUnitRegistry()
{
	
}

///
///
///
SysUnitRegistry :: SysUnitRegistry(const SysUnitRegistry&)
{
	
}

///
///
///
SysUnitRegistry&		SysUnitRegistry::Instance()
{
	if (SysUnitRegistry::_systemRegistry.get() == NULL)
	{
		_systemRegistry.reset(new SysUnitRegistry);
		//on enregistre quelques systemes standard:
		//- non defini
		{
			SysUnitPtr undefined(new SysUnit(kUndefined, eUndefined));
			_systemRegistry->registerSystem(undefined);
		}
		//- metrique pur (N dimensions):
		{
			SysUnitPtr metric(new SysUnit(kMeter, kMeter));
			_systemRegistry->registerSystem(metric);
		}
		//- pixels (2 dimensions):
		{
			unit_vector  unites(2);
			unites(0) = ePixel;
			unites(1) = ePixel;
			SysUnitPtr pixel(new SysUnit(kPixel, ePixel));
			_systemRegistry->registerSystem(pixel);
		}
		
	}
	return *SysUnitRegistry::_systemRegistry;
}

///
///
///
bool SysUnitRegistry::hasSystem(const std::string& uuid) const
{
	return (_systemMap.find(uuid) != _systemMap.end());
}

///
///
///
SysUnitPtr	SysUnitRegistry::Create(const std::string& uuid, const unit_vector& unites)
{				
	if (!SysUnitRegistry::Instance().hasSystem(uuid))
	{
		SysUnitPtr system(new SysUnit(uuid, unites));
		SysUnitRegistry::Instance().registerSystem(system);
	}
	SysUnitPtr const& system = SysUnitRegistry::Instance().getSystemById(uuid);
	return system;
}

/// 
/// 
/// 
namespace ign{namespace numeric{ namespace unit{
	
	std::string Unit2String(const eUnit& unite)
	{
		switch(unite)
		{
			case eUndefined: return kUndefined;
			case ePixel: return kPixel;
			case eMeter: return kMeter;
			case eCentimeter: return kCentimeter;
			case eMillimeter: return kMillimeter;
			case eKilometer: return kKilometer;
			case eInch: return kInch;
			case eFoot: return kFoot;
			case eMile: return kMile;
			case eDegree: return kDegree;
			case eRadian: return kRadian;
			case eGrade: return kGrade;
			case eDMS: return kDMS;
			case eLengthUserDefined: return kLengthUserDefined;
			case eAngleUserDefined: return kAngleUserDefined;
			//no default handled so as to generate a warning...
		}
		IGN_THROW_EXCEPTION("[SysUnit::asText()] unknown unit???");
	}
	
	eUnit String2Unit(std::string const& unite)
	{
		std::string chaine = unite;
		//on passe la chaine en lowercase
		//std::transform(chaine.begin(), chaine.end(), chaine.begin(),
		//			   (int(*)(int)) std::tolower);
		boost::algorithm::to_lower(chaine);
		//indefini
		{
			std::set<std::string> sChaine;
			sChaine.insert("undefined");
			sChaine.insert("indefini");
			sChaine.insert("");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eUndefined; 
			}
		}
		//pixel
		{
			std::set<std::string> sChaine;
			sChaine.insert("p");
			sChaine.insert("pix");
			sChaine.insert("p.");
			sChaine.insert("pix.");
			sChaine.insert("pixel");
			sChaine.insert("pixels");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return ePixel; 
			}
		}
		//metres
		{
			std::set<std::string> sChaine;
			sChaine.insert("m");
			sChaine.insert("m.");
			sChaine.insert("metres");
			sChaine.insert("metre");
			sChaine.insert("meter");
			sChaine.insert("meters");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eMeter; 
			}
		}
		//centimetres
		{
			std::set<std::string> sChaine;
			sChaine.insert("cm");
			sChaine.insert("cm.");
			sChaine.insert("cms");
			sChaine.insert("centimetres");
			sChaine.insert("centimetre");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eCentimeter;
			}
		}
		//millimetres
		{
			std::set<std::string> sChaine;
			sChaine.insert("mm");
			sChaine.insert("mm.");
			sChaine.insert("mms");
			sChaine.insert("millimetres");
			sChaine.insert("millimetre");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eMillimeter;
			}
		}
		//kilometres
		{
			std::set<std::string> sChaine;
			sChaine.insert("km");
			sChaine.insert("km.");
			sChaine.insert("kms");
			sChaine.insert("kilometres");
			sChaine.insert("kilometre");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eKilometer;
			}
		}
		//inch
		{
			std::set<std::string> sChaine;
			sChaine.insert("inch");
			sChaine.insert("inches");
			sChaine.insert("pouce");
			sChaine.insert("pouces");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eInch;
			}
		}
		//degre
		{
			std::set<std::string> sChaine;
			sChaine.insert("deg");
			sChaine.insert("dg");
			sChaine.insert("dd");
			sChaine.insert("degre");
			sChaine.insert("degres");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eDegree;
			}
		}
		//radians
		{
			std::set<std::string> sChaine;
			sChaine.insert("rd");
			sChaine.insert("rad");
			sChaine.insert("rad.");
			sChaine.insert("radian");
			sChaine.insert("radians");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eRadian;
			}
		}
		//grades
		{
			std::set<std::string> sChaine;
			sChaine.insert("grad");
			sChaine.insert("grades");
			sChaine.insert("grad.");
			if(sChaine.find(chaine)!=sChaine.end()) 
			{
				return eGrade;
			}
		}
		std::string exc = "[numeric::unit::String2Unit] unable to find unit from string: ";
		
		IGN_THROW_EXCEPTION(exc + unite);
		
		return eUndefined;
	}
}}}
