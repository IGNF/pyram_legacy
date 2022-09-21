#include <cstdlib>

#include <sstream>

#include <boost/filesystem.hpp>

#include <boost/uuid/uuid.hpp> // generators
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <ignRuntime.h>
#include <ignCommon.h>
#include <ignLog.h>

using namespace ign;


///
///
///
Runtime & 	Runtime::Instance()
{
	static Runtime r;
	return r;
}

///
///
///
std::string Runtime::getDataDirectory() const
{
    std::string dataDir(_dataDirectory);
    if (dataDir.empty())
    {
        boost::optional<std::string> optDataDir = Instance().getEnv("IGN_DATA");
        if (optDataDir.is_initialized())
        {
            boost::filesystem::path myPath(*optDataDir);
            if (myPath.is_relative())
            {
                myPath = boost::filesystem::current_path();
                myPath /= *optDataDir;
            }
            dataDir = myPath.generic_string();
        }
        else
        {
            IGN_THROW_EXCEPTION("[Runtime::getDataDirectory] cannot find data directory");
        }
    }
	return dataDir;
}

///
///
///
void	Runtime::setDataDirectory( std::string const& dataDirectory )
{
	_dataDirectory = dataDirectory;
}


///
///
///
std::string const& 	Runtime::getTestDataDirectory() const
{
	return _testDataDirectory;
}

///
///
///
void	Runtime::setTestDataDirectory( std::string const& dataDirectory )
{
	_dataDirectory = dataDirectory;
}
///
///
///
std::string		Runtime::createTempPath( std::string const& prefix ) const
{
	std::string path = boost::filesystem::temp_directory_path().string();
	path.append("/");
	path.append(prefix);
	path.append(uuid());
	return path;
}

///
///
///
boost::optional< std::string > Runtime::getEnv( std::string const& name ) const
{
	char * value = getenv( name.c_str() );
	if ( ! value ){
		return boost::optional< std::string >();
	}else{
		return boost::optional< std::string >(  std::string(value) );
	}
}

///
///
///
bool Runtime::setEnv( std::string const& key, std::string const& name ) const 
{
#ifndef WIN32
	//setenv(key.c_str(),name.c_str(), 0);
	int stat = setenv(key.c_str(),name.c_str(), 1);
	return(stat==0);
#else
	std::ostringstream oss;
	oss<<key<<"="<<name;
	int stat = putenv(oss.str().c_str());
	return(stat==0);
#endif
}

///
///
///
std::string 		Runtime::getTempDirectory() const
{
	std::string path = getDataDirectory();
	if ( path.empty() ){
		_throwExceptionDataDirectoryNotSet();
	}
	path.append("/tmp/");
	return path;
}

///
///
///
std::string 		Runtime::getTempPath( std::string const& filename ) const
{
	return getTempDirectory().append( filename );
}


///
///
///
std::string		Runtime::uuid() const
{
	std::ostringstream oss;
	boost::uuids::uuid uuid = _uuidGenerator();
	oss << uuid ;
	return oss.str();
}

///
///
///
std::string		Runtime::uuidWithPrefix(const std::string& prefix) const
{
	std::ostringstream oss;
	boost::uuids::uuid uuid = _uuidGenerator();
	oss << prefix<<uuid ;
	return oss.str();
}

///
///
///
std::string		Runtime::uuidWithPostfix(const std::string& postfix) const
{
	std::ostringstream oss;
	boost::uuids::uuid uuid = _uuidGenerator();
	oss <<uuid<<postfix ;
	return oss.str();
}

///
///
///
std::string		Runtime::uuidWithPreAndPostfix(const std::string& prefix, const std::string& postfix) const
{
	std::ostringstream oss;
	boost::uuids::uuid uuid = _uuidGenerator();
	oss << prefix<<uuid<<postfix ;
	return oss.str();
}

///
///
///
Runtime::Runtime():
_uuidGenerator(boost::uuids::random_generator()),
_dataDirectory("")
{
	bool verbose = false;
	boost::optional< std::string > dataDirectory = getEnv( "IGN_DATA" );
	if ( dataDirectory.is_initialized() ){
		_dataDirectory = *dataDirectory;
	}
    else
	{
		if (verbose) std::cout << "IGN_DATA not set" <<std::endl;
	}
	
	boost::optional< std::string > testDataDirectory = getEnv( "IGN_TEST_DATA" );
	if ( testDataDirectory.is_initialized() ){
		_testDataDirectory = *testDataDirectory;
		if (verbose) std::cout<<"IGN_TEST_DATA set to "<<_testDataDirectory<<std::endl;
	}
#ifdef IGN_TEST_DATA
	else
	{
		if (verbose) std::cout<<"IGN_TEST_DATA not set"<<std::endl;
		_testDataDirectory = IGN_TEST_DATA;
	}
#endif
	
}

///
///
///
Runtime::Runtime( Runtime const& other ):
_dataDirectory(other._dataDirectory),
_testDataDirectory(other._testDataDirectory)
{
	
}

///
///
///
Runtime & ign::runtime(){
	return Runtime::Instance();
}

///
///
///
void	Runtime::_throwExceptionDataDirectoryNotSet() const
{
	IGN_THROW_EXCEPTION( "[Runtime error]IGN_DATA is not set and Runtime::setDataDirectory not called" );
}

///
///
///
bool	Runtime::is64b()const
{
	if (sizeof(size_t)==4) return false;
	else if (sizeof(size_t)==8) return true;
	IGN_THROW_EXCEPTION("[Runtime::is64b] Inconsistent word size");
	return false;
}
