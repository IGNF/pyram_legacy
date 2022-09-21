#include <ignTransformSystem.h>
#include <ignImageSystemRadio.h>

using namespace ign::image::radiometry;

///
///
///
SystemRadio::SystemRadio(const std::string& uuid, ign::numeric::unit::SysUnitPtr const& sysUnit):
ign::transform::System(uuid, sysUnit)
{
	
}

///
///
///
std::string SystemRadio::getColorSpace() const
{
	return _colorSpace;
}

///
///
///
void SystemRadio::setColorSpace(const std::string& colorSpace)
{
	_colorSpace = colorSpace;
}

///
///
///
std::string SystemRadio::getTypePixel() const
{
	return _typePixel;
}

///
///
///
void SystemRadio::setTypePixel(const std::string& typePixel)
{
	_typePixel = typePixel;
}

///
///
///
std::string SystemRadio::getSubCode() const
{
	return _subCode;
}

///
///
///
void SystemRadio::setSubCode(const std::string& subCode)
{
	_subCode = subCode;
}

///
///
///
std::string		SystemRadio::toString() const
{
	std::ostringstream oss;
	oss << "[" << this->uuid() << "]";
	return oss.str();
}

///
///
///
SystemRadioPtr  SystemRadio::CreateSystemRadio(const std::string& uuid,
											  const std::string& colorSpace,
											  const std::string& typePixel,
											  const std::string& subCode)
{
	numeric::unit::unit_vector unites(1, numeric::unit::ePixel);
	ign::numeric::unit::SysUnitPtr const& sysUnit = ign::numeric::unit::SysUnitRegistry::Create(ign::numeric::unit::kPixel, unites);

	SystemRadioPtr sys = ign::transform::SystemRegistry::Create<SystemRadio>(uuid, sysUnit);
	sys->_colorSpace = (colorSpace);
	sys->_typePixel = (typePixel);
	sys->_subCode = subCode;
	return sys;
	
}