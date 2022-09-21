#include <ignRuntime.h>
#include <ignCommon.h>
#include <ignTransformSystem.h>

using namespace ign;
using namespace ign::transform;

ign::shared_ptr<transform::System>	System::NullPtr					= ign::shared_ptr<transform::System>();

ign::shared_ptr<SystemUnknown>		SystemUnknown::_systemUnknown	= ign::shared_ptr<SystemUnknown> ();
std::string							SystemUnknown::_UUID			= "UNKNOWN";

ign::shared_ptr<SystemRegistry>		SystemRegistry::_systemRegistry = ign::shared_ptr<SystemRegistry> ();

///
///
///
System::System()
{
	_uuid = ign::runtime().uuid();
}

///
///
///
System::System(const System& other):
_uuid(other._uuid),
_sysUnit(other._sysUnit)
{
}

///
///
///
System::System(const std::string&  uuid, numeric::unit::SysUnitPtr sysUnit):
_uuid(uuid),
_sysUnit(sysUnit)
{
    if (_sysUnit.get() == NULL)
        IGN_THROW_EXCEPTION("System utilisation d'un sysUnit NULL");
	//IGN_ASSERT(sysUnit);
}

///
///
///
System::~System()
{
	
}

///
///
///
const std::string& System::uuid()const
{
	return _uuid;
}

///
///
///
std::string	System::toString() const
{
	return "";
}

///
///
///
bool System::equals(const System& other) const
{
	return (uuid()==other.uuid());
}

///
///
///
numeric::unit::SysUnitPtr		System::		sysUnit()
{
    if (_sysUnit.get() == NULL)
        IGN_THROW_EXCEPTION("System utilisation d'un sysUnit NULL");
	return _sysUnit;
}

///
///
///
const numeric::unit::SysUnitPtr	System::	sysUnit() const
{
	if (_sysUnit.get() == NULL)
        IGN_THROW_EXCEPTION("System utilisation d'un sysUnit NULL");
	return _sysUnit;
}

///
///
///
SystemUnknown::SystemUnknown():
System(_UUID, numeric::unit::SysUnitRegistry::Instance().getSystemById(numeric::unit::kUndefined))
{
}

///
///
///
SystemUnknown::~SystemUnknown()
{	
}


///
///
///
SystemUnknown :: SystemUnknown(const SystemUnknown& other): System(_UUID, other.sysUnit())
{
	
}

///
///
///
SystemPtr		SystemUnknown::Instance()
{
	if (SystemUnknown::_systemUnknown.get() == NULL)
	{
		_systemUnknown.reset(new SystemUnknown());
	}
	return ign::static_pointer_cast<System>(_systemUnknown);
}

///
///
///
// provisoire / devra renvoyer une chaine unique
std::string SystemUnknown::toString() const
{
	return UUID();
}

///
///
///
 std::string	SystemUnknown::UUID()
{
	return _UUID;
}

///
///
///
const transform::SystemPtr	SystemRegistry::getSystemById(const std::string& uuid) const
{
	system_const_iterator it;
	it = _systemMap.find(uuid);
	if (it != systemEnd())
	{
		return it->second;
	}
	
	if (uuid == SystemUnknown::UUID())
		return SystemUnknown::Instance();
	
	std::ostringstream oss;
	oss<<"[SystemRegistry] system "<<uuid<<" doesn't exist";
	IGN_THROW_EXCEPTION(oss.str());
}

///
///
///
void SystemRegistry::registerSystem(SystemPtr const& sys)
{
	bool verbose = 0;
	if (verbose) std::cout<<"[SystemRegistry::registerSystem] system "<<std::endl;
	IGN_ASSERT(sys.get());
	if (verbose) std::cout<<"[SystemRegistry::registerSystem] system not NULL "<<std::endl;
	std::string systemName = sys->uuid();
	
	//on n'enregistre pas les classes de ce type
	if (sys->is<SystemUnknown>())
    {
        if(verbose)    std::cout<<"[SystemRegistry::registerSystem] System Unknown"<<std::endl;
        return;
    }
    
	if (!hasSystem(sys->uuid()))
	{
		std::string uuid = sys->uuid();
		_systemMap.insert(std::make_pair(uuid, sys));		
	}
	else 
	{
		if (verbose) std::cout<<"[SystemRegistry::registerSystem] system "<<sys->uuid()<<" / "<<sys<<" already registered"<<std::endl;
		//modif NB 2014/02/06 si ce n'est pas le meme pointeur, on envoie une exception (fuite memoire sinon)
		system_const_iterator it;
		it = _systemMap.find(systemName);
		if (sys.get() != it->second.get())
		{
			std::ostringstream oss;
			oss<<"[SystemRegistry] system "<<sys->uuid()<<" already exist at adress "<<it->second.get();
			IGN_THROW_EXCEPTION(oss.str());
		}
	}
	if (verbose) std::cout<<"[SystemRegistry::registerSystem] system "<<sys->uuid()<<" properly registered"<<std::endl;
}

///
///
///
SystemRegistry::SystemRegistry()
{
}

///
///
///
SystemRegistry::SystemRegistry(const SystemRegistry&)
{
	
}

///
///
///
SystemRegistry::~SystemRegistry()
{
    
}

///
///
///
SystemRegistry&		SystemRegistry::Instance()
{
	if (SystemRegistry::_systemRegistry.get() == NULL)
	{
		_systemRegistry.reset(new SystemRegistry());
	}
	return *SystemRegistry::_systemRegistry;
}

///
///
///
bool SystemRegistry::hasSystem(const std::string& uuid) const
{
	return (_systemMap.end() != _systemMap.find(uuid));
}

///
///
///

