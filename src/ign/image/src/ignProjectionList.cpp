#include <map>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ignCommon.h>
#include <ignRuntime.h>

#include <ignStringTools.h>
#include <ignInputStreamReader.h>

#include <ignDataAll.h>
#include <ignDataJsonReader.h>

#include <ignProjectionList.h>

using namespace ign;
using namespace ign::geodesy;

std::string ProjectionList::kStrLocal = "LOCAL";


//std::map<std::pair<ProjectionList::eProjCommon,ProjectionList::eType>, std::string> ProjectionList::_mCodeProjList = std::map<std::pair<ProjectionList::eProjCommon,ProjectionList::eType>, std::string>();
ign::tools::UniqueVector<std::string> ProjectionList::_mInternalCodeProjList = ign::tools::UniqueVector<std::string>();
std::map<std::string, std::map<int, std::string>  >	ProjectionList::_mExternalCodeProjList = std::map<std::string, std::map<int, std::string> >();


///
///
///
int		ProjectionList::	String2InternalCodeProj(const std::string& chaine)
{
	return _mInternalCodeProjList.find(chaine);
}

///
///
///
std::string	ProjectionList	::InternalCodeProj2String(int proj)
{
	return _mInternalCodeProjList.getItem(proj);
}

///
///
///
void			ProjectionList::_InitCodeProjList()
{
	bool verbose = 0;
	if (verbose) std::cout<<">[ProjectionList::_InitCodeProjList]"<<std::endl;
	if (_mInternalCodeProjList.size()!= 0)
	{
		if (verbose) std::cout<<">[ProjectionList::_InitCodeProjList] liste deja initialisee avec "<<_mInternalCodeProjList.size()<<" elements"<<std::endl;
		return;
	}
	_mInternalCodeProjList.clear();
	_mExternalCodeProjList.clear();
	if (verbose) std::cout<<">[ProjectionList::_InitCodeProjList] recherche du DataDirectory"<<std::endl;

	std::string datadir = ign::runtime().getDataDirectory();

	boost::filesystem::path codeListPath(datadir);
	codeListPath/="geodesy";
	codeListPath/="ProjectionList.txt";
	if (verbose) std::cout<<">[ProjectionList::_InitCodeProjList] codeListPath == "<<codeListPath.generic_string()<<std::endl;
	if (!boost::filesystem::exists( codeListPath ))
	{
		std::ostringstream oss;
		oss<<"file "<<codeListPath.generic_string()<<" does not exist. Please setup environment variable \"IGN_DATA\" to specify the root directory to locate $ENV{IGN_DATA}geodesy/ProjectionList.txt";
		if (verbose) std::cout<<oss.str()<<std::endl;
		IGN_THROW_EXCEPTION(oss.str());
	}
	if (verbose) std::cout<<">[ProjectionList::_InitCodeProjList] ReadCodeList"<<std::endl;
	ReadCodeList(codeListPath.generic_string());

	if (verbose) std::cout<<"<[ProjectionList::_InitCodeProjList]"<<std::endl;
}

///
///
///
void ProjectionList::SetCode(const std::string& eproj, const std::string& eformat, const std::string& strCodeProj)
{
	bool verbose = 0;
	//on recherche l'identifiant interne correspondant a eproj
	int idProj = _mInternalCodeProjList.find(eproj);
	if (verbose) std::cout << idProj << " internal identifier for " << eproj << std::endl;
	if (idProj<0)
	{
		std::ostringstream oss;
		oss<<"[ProjectionList::FindExternalCodeProj] internal projection code '"<<eproj<<"' does not exist in ProjectionList";
		IGN_THROW_EXCEPTION(oss.str());
	}

	//on recherche le dico associe
	std::map<std::string, std::map<int, std::string> >::iterator it;
	it = _mExternalCodeProjList.find(eformat);
	if(it == _mExternalCodeProjList.end())
	{
		std::ostringstream oss;
		oss<<"[ProjectionList::FindExternalCodeProj] dictionary '"<<eformat<<"' does not exist in ProjectionList";
		IGN_THROW_EXCEPTION(oss.str());
	}
	std::map<int, std::string>::iterator it2 = it->second.find(idProj);
	if(it2 == it->second.end())
	{
		it->second.insert(std::make_pair(idProj, strCodeProj));
	}
	else
	{
		it2->second = strCodeProj;
	}
}

///
///
///
std::string	ProjectionList::GetCode(int internalId)
{
	return _mInternalCodeProjList.getItem(internalId);
}

///
///
///
size_t	ProjectionList::GetNumCode()
{
	return _mInternalCodeProjList.size();
}

///
///
///
void ProjectionList::ReadCodeList(const std::string& nom)
{
    bool verbose = 0;
	bool verbose2 = 0;
	if (verbose) std::cout << "> [ProjectionList::ReadCodeList] " << nom << std::endl;
	_mInternalCodeProjList.clear();
	_mExternalCodeProjList.clear();

	std::ifstream fic(nom.c_str());
	std::string line;
	data::json::JsonReader reader;
	bool readHeader = true;
	std::map<std::string, std::map<int, std::string> >::iterator it;
	std::vector<std::string> dicos;
	int lineCounter = 0;
	while (std::getline(fic, line))
	{
		lineCounter++;
		data::Variant value;
		std::vector<std::string> descr;

		try
		{
			reader.read(line,value);
			data::Array my_array =  value->as<data::Array>();
			for (size_t i = 0; i<my_array.numItems(); ++i)
			{
				if (!my_array.item(i)->isNull())
				{
					if (my_array.item(i)->toString() != "null")
					{
						std::string tmp = ign::tools::StringManip::FromUTF8ToCurrentCodePage(my_array.item(i)->toString());
						descr.push_back(tmp);
					}
					else
						descr.push_back("");
				}
				else
				{
					descr.push_back("");
				}
			}
		}
		catch (ign::Exception& e)
		{
			if (verbose) std::cout<<"line == "<<line<<std::endl;
			std::ostringstream oss;
			oss<<"error in JSon Syntax at line "<<lineCounter<<" of file "<<nom<<": "<<e.what();
			IGN_THROW_EXCEPTION(oss.str());
		}
		if (readHeader)
		{
			dicos  = descr;
			readHeader = false;
			for (size_t i = 0; i<dicos.size(); ++i)
			{
				if (verbose) std::cout<<"dico("<<i<<"): "<<descr[i]<<std::endl;
				_mExternalCodeProjList.insert(std::make_pair(dicos[i], std::map<int, std::string>()));
			}
		}
		else
		{
			IGN_ASSERT(descr.size() == dicos.size());
			int idProj = static_cast<int>(_mInternalCodeProjList.addItem(descr[0]));
			for (size_t i = 0; i<descr.size(); ++i)
			{
				if (descr[i].empty()) continue;
				if (verbose2) std::cout<<"addItem("<<descr[i]<<"): "<<idProj<<std::endl;
				it = _mExternalCodeProjList.find(dicos[i]);
				IGN_ASSERT(it !=_mExternalCodeProjList.end());
				it->second.insert(std::make_pair(idProj, descr[i]));
			}
		}
	}
	if (verbose) std::cout<<"< ProjectionList::ReadCodeList "<<_mInternalCodeProjList.size()<<" codes have been read"<<std::endl;

}

///
///
///
bool ProjectionList::WriteCodeList(const std::string& nom)
{
	//on ecrit la ligne correspondant aux noms des dictionnaires
	std::ofstream fic(nom.c_str());
	if (!fic.good())
	{
		IGN_THROW_EXCEPTION("[ProjectionList::WriteCodeList] can't create file");
	}
	std::map<std::string, std::map<int, std::string> >::const_iterator it;
	fic<<"[\"SD\"";

	std::vector<std::string> dicos;
	for (it = _mExternalCodeProjList.begin(); it != _mExternalCodeProjList.end(); ++it)
	{
		if (it->first != "SD")
		{
			//std::string strCode = ign::tools::SubstituteSubString(it->first, "\"", "\\\"");
			std::string strCode = it->first;
			fic<<",\""<<strCode<<"\"";
			dicos.push_back(it->first);
		}

	}
	fic<<"]\n";

	//on ecrit chaque ligne ayant une entree dans le dico SD
	it = _mExternalCodeProjList.find("SD");

	const std::map<int, std::string>& mProj = it->second;
	std::map<int, std::string>::const_iterator itProj;
	for (itProj = mProj.begin(); itProj != mProj.end(); ++itProj)
	{
		data::Array line;
		line.addItem( data::String( itProj->second ) );

		for (size_t i = 0; i < dicos.size(); ++i)
		{
			try {
				std::string strCode = FindExternalCodeProj(itProj->second, dicos[i]) ;
				line.addItem( data::String( strCode ) );
			}
			catch (...) {
				line.addItem( data::Null() );
			}
		}
		fic << line.toString() << std::endl;
	}
	return true;
}

///
///
///
std::string		ProjectionList::FindExternalCodeProj(const std::string& eproj, const std::string& eformat)
{
	bool verbose = 0;
	if(verbose) std::cout << "[ProjectionList::FindExternalCodeProj] Find " << eformat << " projection code for " << eproj << std::endl;
	std::string strCodeProj("");
	_InitCodeProjList();

	//on recherche l'identifiant interne correspondant a eproj
	int idProj = _mInternalCodeProjList.find(eproj);
	if (verbose) std::cout << " internal identifier for " << eproj <<" is "<< idProj<<std::endl;
	if (idProj<0)
	{
		std::ostringstream oss;
		oss<<"[ProjectionList::FindExternalCodeProj] internal projection code '"<<eproj<<"' does not exist in ProjectionList";
		IGN_THROW_EXCEPTION(oss.str());
	}

	//on recherche le dico associe
	std::map<std::string, std::map<int, std::string> >::const_iterator it;
	it = _mExternalCodeProjList.find(eformat);
	if(it == _mExternalCodeProjList.end())
	{
		if (verbose)
		{
			for (it = _mExternalCodeProjList.begin(); it != _mExternalCodeProjList.end(); ++it)
			{
				std::cout<<"dico: "<<it->first<<" / "<<it->first.size()<<" / "<<eformat.size()<<std::endl;
			}
		}
		std::ostringstream oss;
		oss<<"[ProjectionList::FindExternalCodeProj] dictionary '"<<eformat<<"' does not exist in ProjectionList";
		IGN_THROW_EXCEPTION(oss.str());
	}
	if(verbose) std::cout << "existing projection codes for " <<  it->first << " format : " << std::endl;
	std::map<int, std::string>::const_iterator it2 = it->second.find(idProj);
	if(it2 == it->second.end())
	{
		for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				if (verbose) std::cout << it2->first << "\t" << it2->second << std::endl;
			}
		std::ostringstream oss;
		oss << "No external code found for internal code "<< eproj << " and dictionary " << eformat ;
		IGN_THROW_EXCEPTION(oss.str());
	}
	strCodeProj = it2->second;

	if (verbose) std::cout<<"[ProjectionList::FindExternalCodeProj]: code " << eproj << " and type " << eformat << " -> " << strCodeProj << std::endl;
	return strCodeProj;
}

///
///
///
std::string ProjectionList::FindInternalCodeProj(const std::string& chaine, const std::string& eformat)
{
	bool verbose = 0;
	if (verbose) std::cout << "[ProjectionList::FindInternalCodeProj] '"<<chaine<<"' | "<<eformat<<std::endl;

	//std::string strCodeProj("");
	_InitCodeProjList();

	//on recherche le dico associe
	if (verbose) std::cout << "[ProjectionList::FindInternalCodeProj] recherche du dictionnaire " << eformat << std::endl;
	std::map<std::string, std::map<int, std::string>  >::const_iterator it;
	it = _mExternalCodeProjList.find(eformat);
	if(it == _mExternalCodeProjList.end())
	{
		std::ostringstream oss;
		oss<<"[ProjectionList::FindInternalCodeProj] dictionary '"<<eformat<<"' does not exist in ProjectionList";
		if (verbose) std::cout<<oss.str()<<std::endl;
		IGN_THROW_EXCEPTION(oss.str());
	}
	if (verbose) std::cout<<"[ProjectionList::FindInternalCodeProj] dictionary "<<eformat<<" ("<<it->first<<", "<<it->second.size()<<") found!";

	//on cherche l'identifiant interne
	std::map<int, std::string>::const_iterator it2;
	int idProj = -1;
	for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	{
		if (verbose) std::cout<<"test with: "<<it2->first<<", "<<it2->second<<std::endl;
		if (it2->second == chaine)
		{
			idProj = it2->first;
			break;
		}
	}
	if (verbose) std::cout<<"idProj == "<<idProj<<std::endl;
	if(idProj == -1)
	{
		std::ostringstream oss;
		oss<<"[ProjectionList::FindInternalCodeProj] external code '"<<chaine<<"' does not exist in ProjectionList for dictionary "<<eformat;
		if (verbose) std::cout<<oss.str()<<std::endl;
		IGN_THROW_EXCEPTION(oss.str());
	}

	if (verbose) std::cout<<"[ProjectionList::FindInternalCodeProj] internal idProj '"<<idProj<<"' found!";

	//on renvoie la chaine interne correspondante:
	std::string codeSD = _mInternalCodeProjList.getItem(idProj);
	if (verbose) std::cout<<"[ProjectionList::FindInternalCodeProj] internal code for ("<<chaine<<", "<<eformat<<") is '"<<codeSD<<"'"<<std::endl;
	return codeSD;
}

///
///
///
std::string ProjectionList::GetFormatProjection(const std::string& eproj)
{
    bool verbose (0);
    if (verbose) std::cout << "[ProjectionList::GetFormatProjection] "<<eproj<<std::endl;
    
    _InitCodeProjList();
    
    std::map<std::string, std::map<int, std::string>  >::const_iterator it;
    std::map<int, std::string>::const_iterator it2;
    
    for (it = _mExternalCodeProjList.begin(); it != _mExternalCodeProjList.end(); it++)
    {
        if (verbose) std::cout << "[ProjectionList::GetFormatProjection] test du format " << it->first << std::endl;
        const std::map<int, std::string>& map = it->second;
        for (it2 = map.begin(); it2 != map.end(); it2++)
        {
            if (verbose) std::cout << "[ProjectionList::GetFormatProjection] test de la projection " << it2->second << std::endl;
            if (it2->second == eproj)
                return  it->first;
        }
    }
    
    return "";
}
