/*
 *  ImageDriverManager.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */
#include <iostream>
#include <set>

#include <boost/algorithm/string/case_conv.hpp>
#include <ignConfig.h>
#include <ignCommon.h>

#include <ignStringTools.h>

#include <ignImageDriverManager.h>
#include <ignMPDImageDriver.h>

#ifdef IGN_WITH_GDAL
#include <ignGDALImageDriver.h>
#endif
#ifdef IGN_WITH_DMR
#include <ign/image/io/driver/DMRImageDriver.h>
#endif
#ifdef IGN_WITH_KAKADU
#include <ign/image/io/driver/KakaduImageDriver.h>
#endif


using namespace ign::image;
using namespace ign::image::io::driver;


#if __IDM_WITH_MUTEX__
bool ImageDriverManager::is_destructed = (ImageDriverManager::GetMutex(), false);
#endif

///
///
///
ImageDriverManager* ImageDriverManager::_instance=NULL;

///
///
///
ImageDriverManager::ImageDriverManager():_useExtension(false)
#if __IDM_WITH_MUTEX__
, _maxOpenDrivers(160)
, _skipAwakeForDriver(NULL)
#endif
{
}

///
///
///
ImageDriverManager* ImageDriverManager::Instance()
{
#if __IDM_WITH_MUTEX__
	//crash violent constate sous MacOSX 27/03/2015
	// double check : pas sur malgre tout !
	
	IGN_ASSERT(!is_destructed);
	(void)is_destructed; // prevent removing is_destructed in Release configuration
	
	if (!_instance)
	{
		boost::mutex::scoped_lock lock(GetMutex());
		if( !_instance)
		{
			_instance = new ImageDriverManager;
			_instance->registerAll();
		}
	}
	return _instance;
#else
	if (!_instance)
	{
		_instance = new ImageDriverManager;
		_instance->registerAll();
	}
	return _instance;
#endif
	/*
	 IGN_ASSERT(!is_destructed);
	 (void)is_destructed; // prevent removing is_destructed in Release configuration
	 
	 // lock a chaque fois, tant pis !
	 boost::mutex::scoped_lock lock(GetMutex());
	 
	 if( !_instance)
	 {
	 _instance = new ImageDriverManager;
	 }
	 return _instance;*/
}

///
///
///
void ImageDriverManager::registerAll()
{
	bool verbose = 0;
	if (verbose) std::cout<<"[ImageDriverManager::registerAll] START"<<std::endl;
	if (verbose) std::cout<<"[ImageDriverManager::registerAll] MPDImageDriver"<<std::endl;
	if (!getDriver(MPDImageDriver::DriverName())) _drivers.push_back((ImageDriver*)new MPDImageDriver);

#ifdef IGN_WITH_GDAL
	if (verbose) std::cout<<"[ImageDriverManager::registerAll] GDALImageDriver"<<std::endl;
	if (!getDriver(GDALImageDriver::DriverName()))_drivers.push_back((ImageDriver*)new GDALImageDriver);
#else
	LOG_WARN << "[ImageDriverManager] Without GDAL " << std::endl;
#endif
	
#ifdef IGN_WITH_DMR
	if (verbose) std::cout<<"[ImageDriverManager::registerAll] DMRImageDriver"<<std::endl;
	if (!getDriver(DMRImageDriver::DriverName()))_drivers.push_back((ImageDriver*)new DMRImageDriver);
#else
	LOG_WARN << "[ImageDriverManager] Without DMR " << std::endl;
#endif
	
	if (verbose) std::cout<<"[ImageDriverManager::registerAll] END"<<std::endl;

}

///
///
///
size_t ImageDriverManager::numDrivers()const
{
	return _drivers.size();
}

///
///
///
ImageDriver* ImageDriverManager::getDriver(size_t i)const
{
	if (i>=_drivers.size())
		return NULL;
	return _drivers[i];
}

///
///
///
ImageDriver* ImageDriverManager::getDriver(std::string const &driverName)const
{
	std::vector<ImageDriver*>::const_iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;++it)
	{		
		if ((*it)&&((*it)->getDriverName()==driverName))
			return (*it);
	}
	return NULL;
}

///
///
///
void ImageDriverManager::registerDriver(ImageDriver* driver)
{
	if (!driver) return;
	std::vector<ImageDriver*>::iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;)
	{
		if ((*it)&&((*it)->getDriverName()==driver->getDriverName()))
		{
			delete (*it);
			it=_drivers.erase(it);
		}
		else
		{
			++it;
		}
	}
	_drivers.push_back(driver);
}

///
///
///
void ImageDriverManager::unregisterDriver(size_t i)
{
	
	if (i>=_drivers.size())
		return;
	ImageDriver* & driver = _drivers[i];
	if (driver) delete driver;
	_drivers.erase(_drivers.begin()+i);
}

///
///
///
void ImageDriverManager::unregisterDriver(std::string const &nom)
{
	std::vector<ImageDriver*>::iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;++it)
	{
		if ((*it)&&((*it)->getDriverName()==nom))
		{
			delete (*it);
			_drivers.erase(it);
			return;
		}
	}
}

///
///
///
bool ImageDriverManager::accept(const  ImageDriver* driver, const std::string filename, bool useExt)const
{
	bool verbose = 0;
	if (!useExt)
    {
        if (verbose) std::cout<<"[ImageDriverManager::accept] all extensions allowed, all files accepted"<<std::endl;
        return true;
    }
	
	// extension du fichier en entree :
	boost::filesystem::path filePath(filename);
	std::string ext = filePath.extension().generic_string();
	boost::algorithm::to_upper(ext);
	//on supprime le point
	if (!ext.empty())	ext=ext.substr(1, ext.size()-1);
	if (verbose) std::cout<<"[ImageDriverManager::accept] extension read in ImageDriverManager::accept : "<<ext<<std::endl;
	
	std::set<std::string> sExtension;
	sExtension = driver->getExtensions();
	if (verbose) std::cout << "[ImageDriverManager::accept] Test du driver "<<driver->getDriverName()<<std::endl;
	std::set<std::string>::iterator itext;
	if(verbose)
	{
		std::cout<<"[ImageDriverManager::accept] Liste des extensions du driver : ";
		for(itext=sExtension.begin(); itext!= sExtension.end(); ++itext)
			std::cout<<(*itext)<<" ";
		std::cout<<"\n" ;
	}
	
	if (sExtension.find(ext)!=sExtension.end())
	{
		if(verbose) std::cout<<"[ImageDriverManager::accept] Extension trouvee dans la liste\n" ;
		return true;
	}
	return false;
}

///
///
///
ImageDriver* ImageDriverManager::open(std::string const &fullPathName, bool useExtension)
{
	bool verbose = 0;
	if (verbose)	std::cout << "[ImageDriverManager::open] "<<fullPathName<<", utilisation en priorite de l'extension du fichier img : "<<(int)(useExtension || _useExtension)<<std::endl;
	std::vector<ImageDriver*>::const_iterator it,fin=_drivers.end();
	
	for(it=_drivers.begin();it!=fin;++it)
	{
		if (verbose) std::cout << "[ImageDriverManager::open] Utilisation du driver "<<(*it)->getDriverName()<<std::endl;

		ign::unique_ptr<ImageDriver> ptrDriver((*it)->newInstance());
		
		if( !ptrDriver.get())		IGN_THROW_EXCEPTION("[ImageDriverManager::open] driver non valide");

		if (verbose) std::cout<<"[ImageDriverManager::open] ok driver VALIDE " <<std::endl;

		if (accept(ptrDriver.get(), fullPathName, (useExtension || _useExtension)))
		{
            if(verbose) std::cout<< "[ImageDriverManager::open] le driver " <<ptrDriver->getDriverName() << " a accepte le format" <<std::endl;
#if __IDM_WITH_MUTEX__
			// On bloque l'acces aux donnees de gestion des drivers ouverts aux autres threads.
			boost::mutex::scoped_lock lock(this->_openDriversMutex);
			
			// On s'assure de pouvoir ouvrir le fichier.
			this->suspendLessUsedDrivers();
			
			// On bloque la re-entrance dans awakeDriver pour ce driver, pendant le open.
			this->_skipAwakeForDriver = ptrDriver.get();
#endif
			try {
				if (ptrDriver->open(fullPathName))
				{
#if __IDM_WITH_MUTEX__
					// On a pu ouvrir le driver, on libere la re-entrance dans awakeDriver et on reference le driver parmis les drivers ouverts si necessaire.
					this->_skipAwakeForDriver = NULL;
					if (ptrDriver->isClosable()) {
						this->_openDrivers.push_front (ptrDriver.get());
					}
#endif
                    if(verbose) std::cout<< "[ImageDriverManager::open] le driver " <<ptrDriver->getDriverName() << " a reussi a ouvrir image" <<std::endl;
					return ptrDriver.release();
				}
				else
				{
					if(verbose) std::cout<< "[ImageDriverManager::open] le driver " <<ptrDriver->getDriverName() << " n'a pas reussi a ouvrir image " <<std::endl;
#if __IDM_WITH_MUTEX__
					// On libere la re-entrance dans awakeDriver.
					this->_skipAwakeForDriver = NULL;
#endif
				}
			}
			catch (...) {
#if __IDM_WITH_MUTEX__
				// On libere la re-entrance dans awakeDriver.
				this->_skipAwakeForDriver = NULL;
#endif
                if(verbose) std::cout<< "[ImageDriverManager::open] renvoi de l'exception vers l'appelant" <<std::endl;
				throw;
			}
		}
        else
        {
            if(verbose) std::cout<< "[ImageDriverManager::open] this driver doesn't accept this file" <<std::endl;
        }
	}
    if(verbose) std::cout<< "[ImageDriverManager::open] END: No driver found" <<std::endl;
	return NULL;
}

///
///
///
ImageDriver* ImageDriverManager::update(std::string const &fullPathName)const
{
	ImageDriver* driver=NULL;
	std::vector<ImageDriver*>::const_iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;++it)
	{
		driver=(*it)->newInstance();
		if (accept(driver,fullPathName, _useExtension))
		{
			if (driver->update(fullPathName))
			{
				return driver;
			}
			else
			{
				delete driver;
				driver=NULL;
			}
		}
	}
	return driver;
}

///
///
///
ImageDriver* ImageDriverManager::create(std::string const &fullPathName, int NRows, int NLines, int NBands, eTypePixel* tabType, ign::data::Object const *  options, ign::data::Object const *  metadata)const
{
	bool verbose = 0;
	if (verbose) std::cout<<"[ImageDriverManager::create] START"<<std::endl;
	ImageDriver* driver=NULL;
	
	std::vector<ImageDriver*>::const_iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;++it)
	{
		if(verbose) std::cout << "Tentative de creation du fichier "<<fullPathName<<" avec le driver : "<<(*it)->getDriverName()<<std::endl;
		driver=(*it)->newInstance();
		
		if (verbose) std::cout << "check if driver is ok" << std::endl;
		if (accept(driver,fullPathName, _useExtension))
		{
			if (verbose) std::cout << "driver accepte" << std::endl;
			//if (verbose) std::cout << "|||||" << driver->create(fullPathName, NRows, NLines, NBands, tabType,options,metadata) << "|||||" << std::endl;
			if (driver->create(fullPathName, NRows, NLines, NBands, tabType,options,metadata))
			{
				if (verbose) std::cout<<"Driver trouve: "<<driver->getDriverName()<<std::endl;
				return driver;
			}
			else
			{
				if (verbose) std::cout << "driver delete" << std::endl;
				delete driver;
				driver=NULL;
			}
			if (verbose) std::cout << "fin condition driver accepte" << std::endl;
		}
		else
		{
            delete driver;
            //driver = NULL;
		}
		if (verbose) std::cout << "driver ok" << std::endl;
	}
	if (driver==NULL)
	{
		if (verbose) std::cout << "pas de driver pour creer le fichier" << std::endl;
		IGN_THROW_EXCEPTION(std::string("Erreur : pas de driver pour la creation du fichier ")+fullPathName);
	}
	if (verbose) std::cout<<"[ImageDriverManager::create] END"<<std::endl;
	return driver;
}

///
///
///
bool ImageDriverManager::createCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn, ign::data::Object const *  options, ign::data::Object const *  metadata)const
{
	bool verbose = 0;
	if (verbose) std::cout<<"[ImageDriverManager::createCopy] START"<<std::endl;
	std::vector<ImageDriver*>::const_iterator it,fin=_drivers.end();
	for(it=_drivers.begin();it!=fin;++it)
	{
		ImageDriver* driver=(*it)->newInstance();
		bool resultat = driver->createCopy(fullPathNameOut, fullPathNameIn, options,metadata);
		delete driver;
		if (resultat) return true;
	}
	if (verbose) std::cout<<"[ImageDriverManager::createCopy] END"<<std::endl;
	return false;
}

///
///
///
bool ImageDriverManager::useExtension()const
{
	return _useExtension;
}

///
///
///
void	ImageDriverManager::useExtension(bool b)
{
	_useExtension = b;
}



///
///
///
void ImageDriverManager::awakeDriver(ImageDriver* driver)
{
#if __IDM_WITH_MUTEX__
	if (driver && driver->isClosable() && (driver != this->_skipAwakeForDriver)) {

		// On bloque l'acces aux donnees de gestion des drivers ouverts aux autres threads.
		boost::mutex::scoped_lock lock(this->_openDriversMutex);
		// On commence par chercher si le driver est actuellement ouvert, auquel cas on le remet en tete de liste et c'est fini.
		for (std::list<ImageDriver*>::iterator itOpen = this->_openDrivers.begin(); itOpen != this->_openDrivers.end(); ++itOpen) {
			if (*itOpen == driver) {
				if (itOpen != this->_openDrivers.begin()) {
					this->_openDrivers.erase (itOpen);
					this->_openDrivers.push_front (driver);
				}
				return;
			}
		}
		
		// Sinon on le cherche parmi les drivers suspendus ou il est alors sense se trouver.
		std::map<ImageDriver*,std::string>::iterator itSuspended = this->_suspendedDrivers.find (driver);
		if (itSuspended != this->_suspendedDrivers.end()) {
			
			// On suspend les drivers les plus anciennement utilises si necessaire.
			this->suspendLessUsedDrivers();
			
			// On reouvre le driver.
			try {
				this->_skipAwakeForDriver = driver;
				IGN_ASSERT (driver->open (itSuspended->second));
				this->_skipAwakeForDriver = NULL;
			}
			catch (...) {
				this->_skipAwakeForDriver = NULL;
				throw;
			}
			
			// On le transfert des drivers suspendus aux drivers ouverts.
			this->_suspendedDrivers.erase (itSuspended);
			this->_openDrivers.push_front (driver);
		}
	}
#endif
}

///
///
///
void ImageDriverManager::detachDriver(ImageDriver* driver)
{
#if __IDM_WITH_MUTEX__
	if (driver && driver->isClosable()) {
		
		// On bloque l'acces aux donnees de gestion des drivers ouverts aux autres threads.
		boost::mutex::scoped_lock lock(this->_openDriversMutex);
		
		// On commence par chercher parmi les drivers actuellement ouverts.
		for (std::list<ImageDriver*>::iterator itOpen = this->_openDrivers.begin(); itOpen != this->_openDrivers.end(); ++itOpen) {
			if (*itOpen == driver) {
				this->_openDrivers.erase (itOpen);
				return;
			}
		}
		
		// Si on ne l'a pas trouve, on cherche parmi les drivers suspendus.
		std::map<ImageDriver*,std::string>::iterator itSuspended = this->_suspendedDrivers.find (driver);
		if (itSuspended != this->_suspendedDrivers.end()) {
			this->_suspendedDrivers.erase (itSuspended);
		}
	}
#endif
}

///
///
///
#if __IDM_WITH_MUTEX__
uint16_t ImageDriverManager::getMaxOpenDrivers() const
{
	return this->_maxOpenDrivers;
}

///
///
///
void ImageDriverManager::setMaxOpenDrivers(uint16_t maxOpenDrivers)
{
	
	// On bloque l'acces aux donnees de gestion des drivers ouverts aux autres threads.
	boost::mutex::scoped_lock lock(this->_openDriversMutex);
	
	this->_maxOpenDrivers = maxOpenDrivers;
	if (this->_maxOpenDrivers < 50) {
		this->_maxOpenDrivers = 50;
	}
	else if (this->_maxOpenDrivers > 200) {
		this->_maxOpenDrivers = 200;
	}
	
	// On suspend les drivers les moins utilises si necessaire.
	this->suspendLessUsedDrivers();
}

///
///
///
void ImageDriverManager::suspendLessUsedDrivers()
{
	while (this->_openDrivers.size() >= this->_maxOpenDrivers) {
		
		// Recuperation du driver le moins utilise.
		ImageDriver* lessUsedDriver = this->_openDrivers.back();
		
		// Recuperation du chemin de son fichier.
		// On prend soin de bloquer les awake sur ce driver, pour eviter un verrou mortel.
		this->_skipAwakeForDriver = lessUsedDriver;
		std::string lessUsedPath = lessUsedDriver->getFileName();
		this->_skipAwakeForDriver = NULL;
		
		// Suppression de la liste des drivers ouverts.
		this->_openDrivers.pop_back();
		
		// Fermeture du driver.
		lessUsedDriver->close();
		
		// Placement du driver dans la map des drivers suspendus.
		this->_suspendedDrivers[lessUsedDriver] = lessUsedPath;
	}
}
#endif

