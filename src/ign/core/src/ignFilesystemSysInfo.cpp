#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>

#include <ignConfig.h>
#include <ignCommon.h>
#include <ignFilesystemSysInfo.h>
#include <ignStringTools.h>

#ifdef WIN32
#include <windows.h>
#endif

using namespace ign;
using namespace ign::filesystem;

///
///
///
SystemInfo::SystemInfo(const boost::filesystem::path &mypath)
{
	try
	{
		_space_info = boost::filesystem::space(mypath);
		
	}
	catch (boost::filesystem::filesystem_error &)
	{
		std::ostringstream oss;
		oss<<"Impossible de calculer l'espace disque sur "<<mypath.generic_string();
		IGN_THROW_EXCEPTION(oss.str());
	}
}

///
///
///
SystemInfo:: ~SystemInfo()
{
}

///
///
///
boost::uintmax_t SystemInfo::getFreeSpace() const
{
	return _space_info.available;
}

///
///
///
boost::uintmax_t SystemInfo::getCapacity() const
{
	return _space_info.capacity;
}

///
///
///
double SystemInfo::getFreeSpaceRatio() const
{
	return (getFreeSpace()*100.)/(double)getCapacity();
}

///
///
///
size_t SystemInfo::GetPhysicalAvailableMemory()
{
size_t dispo =0;

#ifdef WIN32
	MEMORYSTATUS  memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memInfo);
	dispo = memInfo.dwAvailPhys;
	return dispo;
#else
	IGN_THROW_EXCEPTION("[SystemInfo::GetPhysicalAvailableMemory] OS non gere ");
    return dispo;
#endif
}


///
///
///
size_t  SystemInfo::GetCPUNumber()
{
	//return (size_t) boost::thread::hardware_concurrency();
	
	int NbTh(0);
	int verbose = 0;
	//http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
#ifdef WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	NbTh = sysinfo.dwNumberOfProcessors;
	if (verbose) std::cout << "[SystemInfo::GetCPUNumber] Nombre Thread detecte sous windows : " << NbTh << std::endl;
#elif __APPLE__ || __MACH__ || __linux__
	NbTh = sysconf(_SC_NPROCESSORS_ONLN);
	if(verbose) std::cout << "[SystemInfo::GetCPUNumber] Nombre Thread detecte sous linux/mac : " << NbTh << std::endl;
#else
	IGN_THROW_EXCEPTION("[SystemInfo::GetCPUNumber] OS non gere ");
#endif
	return NbTh;
	
}
