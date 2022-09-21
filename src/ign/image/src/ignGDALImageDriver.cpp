#include <ignConfig.h>

#include <bitset>  

#ifdef IGN_WITH_GDAL

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

#include <boost/format.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/string.hpp>
#include <ignLog.h>
#include <ignException.h>
#include <ignStringTools.h>
#include <ignDataAll.h>

#include <ignColorTable.h>
#include <ignBufferImage.h>
#include <ignImageIO.h>
#include <ignImageDriverManager.h>
#include <ignGDALImageDriver.h>
#include <ignProjectionList.h>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_geometry.h>


// SKD: GDALOpen theorically accepts UTF-8 encoding, but the behaviour is BUGGY under Windows!
//		We must then go back to ANSI (machine's codepage), hence loosing the ability to open files with a path out of the range of the current codepage (ex: an arabic character under a Latin-1 machine)
//		Note: convertStringFromUTF8ToCurrentCodePage() does nothing under Linux/Mac

using namespace ign::image;
using namespace ign::image::io::driver;

//namespace ign {
//	namespace image	{
//		namespace  io {
//			namespace driver {

std::string kNomCache="_CACHE_GDAL_.tif";
std::string kWarningGDAL="GDAL_WARNING";


ign::data::Object	GDALImageDriver::_ConfigOptions = ign::data::Object();
bool				GDALImageDriver::_GDALRegistered = false;

///
///
///
void		GDALImageDriver::SetConfigurationOption(ign::data::Object const & configOpt)
{
	_ConfigOptions.clear();
	_ConfigOptions = configOpt;
	
}

///
///
///
ign::data::Object	GDALImageDriver::GetConfigurationOption()
{
	return _ConfigOptions;
}




///
///
///
bool CompareSRS(const OGRSpatialReference& oSRSgenerated, const OGRSpatialReference& oSRS)
{
	if(oSRSgenerated.IsSame(&oSRS) )
	{
		return true;
	}
	
	//sinon, on va essayer de tester les parametres principaux
	std::string oSRSSimplifiedWKT, oSRSgeneratedSimplifiedWKT;
	
	char * ch1Ptr = NULL;	
	char * ch2Ptr = NULL;
	
	oSRSgenerated.exportToPrettyWkt(&ch1Ptr, true);
	oSRS.exportToPrettyWkt(&ch2Ptr, true);
	
	oSRSSimplifiedWKT = std::string(ch1Ptr);
	oSRSgeneratedSimplifiedWKT = std::string(ch2Ptr);
	
	
	CPLFree(ch1Ptr);
	CPLFree(ch2Ptr);
	
	return (oSRSgeneratedSimplifiedWKT == oSRSSimplifiedWKT);
}


///
///
///
/*
 void ImageIO_GDALErrorHandler(CPLErr eErrClass, int err_no, const char *msg)
 {
 
 std::cout<<" error handler"<<std::endl;
 
 if(eErrClass == CE_Warning) std::cout<<" ceci est un warning image"<<std::endl;
 
 if ((eErrClass == CE_Failure) || (eErrClass == CE_Fatal))
 {
 IGN_THROW_EXCEPTION_CODE(err_no,msg);
 }
 if(eErrClass == CE_Warning) std::cout<<" ceci est un warning image"<<std::endl;
 LOG_NOTICE << "GDAL WARNING : "<<msg<<std::endl;
 
 return;
 }
 */

/// comment by RP. The ImageIO_GDALErrorHandler below is a very quick bug fix (quite ugly)
/// to avoid the following problem :
/// in the GDALImageDriver::open method, when GDALOpen is called, an error thrown by GDAL
/// is caught in our error handler (ImageIO_GDALErrorHandler). In this handler,
/// we normally throw an exception when the error is a failure or fatal error.
/// In case the error is a simple warning, the handler does nothing and then crashes when leaving with no explanation
/// for now. That's why I've chosen to throw an exception even if we catch a warning.
/// The exception message begins with the keyword kWarningGDAL (defined above), and is caught in the GDALImageDriver::open
/// method. Then, we are able to know whether the exception stems from a warning thanks to the keyword kWarningGDAL.
/// If so, we turn our handler off, and call GDALOpen again. This time, it works fine.
void ImageIO_GDALErrorHandler(CPLErr eErrClass, int err_no, const char *msg) throw(ign::Exception)
{
	bool verbose = 0;
	if(eErrClass == CE_Warning)
	{
		if (verbose) LOG_NOTICE << "[ImageIO_GDALErrorHandler] GDAL WARNING : "<<msg<<std::endl;
		std::string warnMsg(kWarningGDAL);	warnMsg.append(" ");
		warnMsg.append(std::string(msg));
		// Modif Greg: puisque ce n'est qu'un Warning, on ne fait pas de IGN_THROW_EXCEPTION_CODE
		
		if (verbose) std::cout << "[ImageIO_GDALErrorHandler] GDAL WARNING : "<<msg<<std::endl;
#if defined(WIN32) && !defined(__CYGWIN__)
		//IGN_THROW_EXCEPTION_CODE(err_no,kWarningGDAL.c_str());
		//IGN_THROW_EXCEPTION(kWarningGDAL);
		IGN_THROW_EXCEPTION(warnMsg);
		//LOG_NOTICE << "*GDAL WARNING : "<<msg<<std::endl;
#endif
	}
	
	if ((eErrClass == CE_Failure) || (eErrClass == CE_Fatal))
	{
		if (verbose) LOG_NOTICE << "mauvais positionnement return : "<<std::endl;
		IGN_THROW_EXCEPTION_CODE(err_no,msg);
	}
	/*
	 CE_None = 0,
	 CE_Debug = 1,
	 CE_Warning = 2,
	 CE_Failure = 3,
	 CE_Fatal = 4
	 */
	return;
	
}

///
///
///
void GDALImageDriver::SetNullErrorHandler()
{
	CPLSetErrorHandler(NULL);
	
}

///
///
///
void		GDALImageDriver::SetOwnErrorHandler()
{
	bool verbose = 0;
	if (verbose) std::cout<<"[GDALImageDriver::SetOwnErrorHandler()]"<<std::endl;
	CPLSetErrorHandler((CPLErrorHandler)ImageIO_GDALErrorHandler);
}

///
///
///
GDALDataType TypeIO2TypeGDAL(eTypePixel typeIO);
eTypePixel TypeGDAL2TypeIO(GDALDataType typeGDAL);

///
///
///
GDALImageDriver::GDALImageDriver():ImageDriver(), _poDataset(NULL),_filename(""),_needCopy(false),_useCache(false),_nomCache(""),_driverCache(NULL)
{
	//c'est ici que le driver de lecture des image tiff n'est pas trouve
	bool verbose = 0;
	if (!_GDALRegistered)
	{		
		CPLSetErrorHandler((CPLErrorHandler)ImageIO_GDALErrorHandler);
        CPLSetConfigOption("GDAL_DISABLE_READDIR_ON_OPEN","TRUE");
		        
		GDALAllRegister();
		
		GDALDriver* driverESRI =  GetGDALDriverManager()->GetDriverByName("EHdr");
		if (driverESRI)			
		{
			if(verbose)std::cout << "GDALDeregisterDriver" << " driverESRI " << std::endl;
			GDALDeregisterDriver(driverESRI);
			delete driverESRI;
		}
		
		// added by RP : we want ot use our own ASCII driver
		GDALDriver* driverASCIIGRIDESRI =  GetGDALDriverManager()->GetDriverByName("AAIGrid");
		if (driverASCIIGRIDESRI)			
		{
			if(verbose)std::cout << "GDALDeregisterDriver" << " driverASCIIGRIDESRI " << std::endl;
			GDALDeregisterDriver(driverASCIIGRIDESRI);
			delete driverASCIIGRIDESRI;
		}
		
		_GDALRegistered=true;
	}
	if(verbose)
	{
		size_t k = (size_t) GetGDALDriverManager()->GetDriverCount();
		std::cout << k << " drivers trouves " << std::endl;
		for ( size_t d = 0 ; d < k ; ++d )
		{
			GDALDriver* driver = GetGDALDriverManager()->GetDriver(d);
			std::cout << "Driver #" << d << " " << driver->GetDescription() << std::endl;
		}
	}
	
	
}

///
///
///
GDALImageDriver::~GDALImageDriver()
{
	ImageDriverManager::Instance()->detachDriver(this);
	this->close();
}

///
///
///
ImageDriver* GDALImageDriver::newInstance()const
{
	return (ImageDriver*) new GDALImageDriver;
}

///
///
///
std::string	GDALImageDriver::	DriverName()
{
	return "GDALImageDriver";
}

///
///
///
bool GDALImageDriver::open(std::string const &fullPathName)
{
	bool verbose = 0;

	if (verbose) std::cout << "[GDALImageDriver::open] : "<<fullPathName<<std::endl;
	if (_poDataset)
	{
		ImageDriverManager::Instance()->detachDriver(this);
		GDALClose(_poDataset);
		_poDataset = NULL;
	}
	_filename = fullPathName;
	try
	{
		if (verbose) std::cout << " GDALOpen : "<<_filename<<std::endl;//oui
		CPLErrorReset();
		// GDAL convertit les donnees en RGBA a la lecture des images TIFF pour quelques espaces colorimetriques particuliers dont CMYK.
		// Il est possible de desactiver cette conversion afin de lire les donnees brutes.
		// L'option de configuration DISABLE_TIFF_RGBA_CONVERT de ce driver permet de piloter le comportement de la lecture GDAL.
		int optId = _ConfigOptions.getAttributeNumber ("DISABLE_TIFF_RGBA_CONVERT");
		std::string filenameToOpen = (optId < 0) || !_ConfigOptions.attribute(optId).toBoolean() ? _filename : std::string("GTIFF_RAW:") + _filename;
		// SKD: GDALOpen theorically accepts UTF-8 encoding, but the behaviour is BUGGY under Windows! (See note at the begining of the file)
		try
		{
			if (verbose) std::cout << " _filename="<<_filename<<" GA_ReadOnly="<<GA_ReadOnly<<std::endl;//parametres correct
			//SetNullErrorHandler();
			_poDataset = (GDALDataset *)GDALOpen( ign::tools::StringManip::FromUTF8ToCurrentCodePage(filenameToOpen).c_str(), GA_ReadOnly );//
			//SetOwnErrorHandler();
			if (verbose) std::cout << " test try passee "<<std::endl;//non
			//IGN_ASSERT(_poDataset->GetDriver());

			//si on n'a ps reussi a initialiser correctement p0Dataset, on retente avec une gestion d'erreur plus laxiste
			if (!_poDataset) IGN_THROW_EXCEPTION(kWarningGDAL);
     	}
		catch(ign::Exception &e)
		{
			if (verbose) std::cout<<" [GDALImageDriver::open] problem calling GDALOpen"<<std::endl;
			if (verbose) std::cout<<" [GDALImageDriver::open] Exception caught is "<<e.what()<<std::endl;
			
			//std::cout<<" local exception : "<<e.what()<<" -- "<<std::endl;
			std::string ewhat(e.what());
			size_t found = ewhat.find(kWarningGDAL);
			if (found != std::string::npos)
			{

				if(verbose) std::cout<<"gestion warning gdal"<<std::endl;
				SetNullErrorHandler();
				_poDataset = (GDALDataset *)GDALOpen( ign::tools::StringManip::FromUTF8ToCurrentCodePage(filenameToOpen).c_str(), GA_ReadOnly );
				SetOwnErrorHandler();
			}
			else
			{
				if(verbose) std::cout << kWarningGDAL << " non trouve dans "<<ewhat<<std::endl;
				return false;
			}
		}
		catch(boost::exception & e)
		{
			std::cout<<"I AM HERE: "<<boost::diagnostic_information_what(e)<<std::endl;
		}
		catch(...)
		{
			std::cout<<"erreur inconnue GDAL attrapee?"<<std::endl;
		}

		if (verbose)
		{
			if(verbose) std::cout << "OK"<<std::endl;
			if (_poDataset) std::cout << "pointeur non nul"<<std::endl;
		}
	}
	catch (std::exception & e)
	{
		LOG_NOTICE << boost::format("GDALOpen failed opening %1%. Reason: %2%.") % _filename % e.what() << std::endl;
		//CPLErrorReset()
		// RP j
		
		_poDataset = NULL;
		return false;
	}
	catch(...)
	{
		if(verbose) std::cout << "exception attrapee" << std::endl;
	}

	if (!_poDataset) 
	{
		// [Sukender] CPLErrorReset() seems buggy!! Hence the message NOT displayed.
		//if (true) 
		{
			LOG_NOTICE << boost::format("GDALOpen failed opening %1%. Error type=%2%.") % _filename % CPLGetLastErrorType() << std::endl;
		}
        //else
		//{
		//	LOG_NOTICE << boost::format("GDALOpen failed opening %1%. Error type=%2%, message: %3%.") % _filename % CPLGetLastErrorType() % CPLGetLastErrorMsg() << std::endl;
		//}
		CPLErrorReset();
	}
	
	double geoTransform[6];
	std::string sysProjWKT;
	ign::image::georef::PixConv pixConv;

	if (getGeoref(geoTransform,sysProjWKT, pixConv))
	{
		if(verbose)
		{
			std::cout << "GeoRef GDALImageDriver : "<<geoTransform[0]<<" "<<geoTransform[3]<<" "<<sysProjWKT<<std::endl;
			std::cout << "GeoRef GDALImageDriver : "<<ign::image::georef::PixConventionToString(pixConv)<<std::endl;
		}
		////georef::Data geo(geoTransform,sysProjWKT,ign::image::georef::PixConvNULL);
		
		georef::Data geo(geoTransform,sysProjWKT,pixConv);
		georef::SetGeoref(geo, _metaData);
	}
	else if (verbose)
	{
		std::cout << "[GDALImageDriver] no readable GeoRef in image  : "<< _filename  << std::endl;
	}

    // Recuperation des metaDonnees associes a l'image
    if (_poDataset)
    {
        std::vector<std::string> vDomains;
        vDomains.push_back("");
        vDomains.push_back("IMAGE_STRUCTURE");
        vDomains.push_back("GEOLOCATION");
        vDomains.push_back("RPC");
        if (verbose) std::cout << "Lecture des metadonnees : "<<std::endl;
        for(size_t d=0;d<vDomains.size();++d)
        {
            if (verbose) std::cout << "domaine : "<<vDomains[d]<<std::endl;
            char** mtd = _poDataset->GetMetadata(vDomains[d].c_str());
            if (mtd==NULL) continue;
            int i=0;
            while(mtd[i]!=NULL)
            {
                char *pszKey = NULL;
                const char *pszValue = CPLParseNameValue(mtd[i],&pszKey);
                if (verbose) std::cout << "Key : "<<pszKey<<" Value : "<<pszValue<<std::endl;
                _metaData.setAttribute(std::string(pszKey),data::String(pszValue));
                
                CPLFree( pszKey );
                ++i;
            }
        }
    }

    int idAttr = _metaData.getAttributeNumber("SOURCE_COLOR_SPACE");
    if (idAttr!=-1)
    {
        std::string cs = _metaData.attribute("SOURCE_COLOR_SPACE").toString();
        if (cs == std::string("CMYK"))
            _colorSpace=CMYK;
        else if (numBands()==3)
        {
            _colorSpace=RGB;
        }
        else if (numBands()==1)
        {
            _colorSpace=GrayScale;
        }
        else
        {
            _colorSpace=Undefined;
        }
    }
    // Autre possibilite pour recuperer les informations d'espace de couleurs.
    // SOURCE_COLOR_SPACE semble renseigné dans peu de cas.
    else if (_poDataset && (_colorSpace == Undefined)) {
        switch (_poDataset->GetRasterCount()) {
            case 4 : {
                if ((_poDataset->GetRasterBand(1)->GetColorInterpretation() == GCI_CyanBand) &&
                    (_poDataset->GetRasterBand(2)->GetColorInterpretation() == GCI_MagentaBand) &&
                    (_poDataset->GetRasterBand(3)->GetColorInterpretation() == GCI_YellowBand) &&
                    (_poDataset->GetRasterBand(4)->GetColorInterpretation() == GCI_BlackBand)) {
                    _colorSpace=CMYK;
                }
                break;
            }
            case 3 : {
                if ((_poDataset->GetRasterBand(1)->GetColorInterpretation() == GCI_RedBand) &&
                    (_poDataset->GetRasterBand(2)->GetColorInterpretation() == GCI_GreenBand) &&
                    (_poDataset->GetRasterBand(3)->GetColorInterpretation() == GCI_BlueBand)) {
                    _colorSpace=RGB;
                }
                break;
            }
            case 1 : {
                if (_poDataset->GetRasterBand(1)->GetColorInterpretation() == GCI_GrayIndex) {
                    _colorSpace=GrayScale;
                }
                break;
            }
            default : {
            }
        }
    }

	//delete [] geoTransform;
	if (verbose) std::cout << "fin de open gdal" << std::endl;
    
	return (_poDataset!=NULL);
}

///
///
///
bool GDALImageDriver::update(std::string const &fullPathName)
{
    bool verbose = 0;
	
	if (verbose) std::cout << "GDALImageDriver::Update "<<fullPathName<<std::endl;
	if (_poDataset)
	{
		ImageDriverManager::Instance()->detachDriver(this);
		GDALClose(_poDataset);
		_poDataset = NULL;
	}
	
	_filename = fullPathName;
	
	std::string nomDriverGDAL = GDALImageDriver::GetDriverName(_filename);
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(nomDriverGDAL.c_str());
	if (verbose) std::cout << "Driver GDAL : "<<nomDriverGDAL<<std::endl;
	
	if (!poDriver)
	{
		if (verbose) std::cout << "Driver non trouve"<<std::endl;
		return false;
	}
	
		
	if (verbose)
	{
        char **papszMetadata= poDriver->GetMetadata();
		std::cout << "Info sur le driver : "<<std::endl;
		if (papszMetadata)
		{
			int i=0;
			char * ptr = papszMetadata[i];
			i++;
			while(ptr)
			{
				std::cout << i << " : "<<ptr << std::endl;
				ptr = papszMetadata[i];
				i++;
			}
		}
	}
	
	try
	{
		if(verbose){std::cout << "ouverture : "<<_filename<<" en Update"<<std::endl;}
		// Il y a visiblement une faille dans le driver Kakadu de GDAL
		// lorsqu'on cherche a ouvrir un fichier en update
		if (nomDriverGDAL!="JP2KAK")
		{
			// SKD: GDALOpen theorically accepts UTF-8 encoding, but the behaviour is BUGGY under Windows! (See note at the begining of the file)
			_poDataset = (GDALDataset *) GDALOpen( ign::tools::StringManip::FromUTF8ToCurrentCodePage(_filename).c_str(), GA_Update );
            // Recuperation des metaDonnees associes a l'image si ce n'est pas un cache
            if (_poDataset)
            {
                std::vector<std::string> vDomains;
                vDomains.push_back("");
                vDomains.push_back("IMAGE_STRUCTURE");
                vDomains.push_back("GEOLOCATION");
                vDomains.push_back("RPC");
                if (verbose) std::cout << "Lecture des metadonnees : "<<std::endl;
                for(size_t d=0;d<vDomains.size();++d)
                {
                    if (verbose) std::cout << "domaine : "<<vDomains[d]<<std::endl;
                    char** mtd = _poDataset->GetMetadata(vDomains[d].c_str());
                    if (mtd==NULL) continue;
                    int i=0;
                    while(mtd[i]!=NULL)
                    {
                        char *pszKey = NULL;
                        const char *pszValue = CPLParseNameValue(mtd[i],&pszKey);
                        if (verbose) std::cout << "Key : "<<pszKey<<" Value : "<<pszValue<<std::endl;
                        _metaData.setAttribute(std::string(pszKey),data::String(pszValue));
                        CPLFree( pszKey );
                        ++i;
                    }
                }
                
                if (_metaData.hasAttribute("COMPRESSION"))
                {
                    if(verbose)std::cout << "COMPRESSION : "<< _metaData.attribute("COMPRESSION").toString()<<std::endl;
                    _options.setAttribute("COMPRESS",data::String(_metaData.attribute("COMPRESSION").toString()));
                    if(verbose)std::cout << "donc on utilise un cache"<<std::endl;
                    if (_poDataset)
                        GDALClose(_poDataset);
                    _poDataset = NULL;
                    IGN_THROW_EXCEPTION_CODE(CPLE_NotSupported,"");
                }
            }
		}
		if (_poDataset == NULL)
			IGN_THROW_EXCEPTION_CODE(CPLE_NotSupported,"");
		//throw new Exception(CE_Failure,CPLE_NotSupported,"");
	}
	catch(ign::Exception & e)
	{
		if (verbose) std::cout << " GDALImageDriver::Update -- probleme : "<< boost::diagnostic_information(e) <<std::endl;
		const int * errorCode = boost::get_error_info<boost::errinfo_errno>(e);
		if (errorCode && *errorCode==CPLE_NotSupported)
		{
			if (verbose) std::cout << "On essaye de travailler en cache"<<std::endl;
			// On essaye de l'ouvrir en lecture et de faire un fichier en cache
			GDALDataset *poDatasetOrigine;
			if (verbose) std::cout << "ouverture : "<<_filename<<" en ReadOnly"<<std::endl;
			// SKD: GDALOpen theorically accepts UTF-8 encoding, but the behaviour is BUGGY under Windows! (See note at the begining of the file)
			poDatasetOrigine = (GDALDataset *) GDALOpen( ign::tools::StringManip::FromUTF8ToCurrentCodePage(_filename).c_str(), GA_ReadOnly );
			
			if (!poDatasetOrigine)
			{
				if (verbose) std::cout << "On ne sait meme pas l'ouvrir en lecture"<<std::endl;
				return false;
			}
			if (verbose) std::cout << "On a ouvert le fichier original"<<std::endl;
			_driverCache = GetGDALDriverManager()->GetDriverByName("GTiff");
			
			size_t pos = fullPathName.find_last_of('.');
			if (pos==std::string::npos)
			{
				_nomCache=fullPathName+kNomCache;
			}
			else
			{
				_nomCache=fullPathName.substr(0,pos)+kNomCache;
			}
			if (verbose) std::cout << "Nom du cache : "<<_nomCache<<std::endl;
			
			if (verbose) std::cout << "Creation de la copie en Tiff vers : "<<_nomCache<<std::endl;
			_poDataset = _driverCache->CreateCopy(_nomCache.c_str(),poDatasetOrigine,FALSE, NULL, NULL, NULL );
			if (verbose) std::cout << "Recopie en cache : "<<(_poDataset!=NULL)<<std::endl;
			// Dans le cas d'un update, on a besoin de copier le cache uniquement si on a effectivement ecrit au moins une fois dans l'image
            _needCopy = false;
            _useCache = true;
			GDALClose(poDatasetOrigine);
		}
		else
		{
			throw;
		}
	}
	double geoTransform[6];
	std::string sysProjWKT;
	ign::image::georef::PixConv pixConv;
	
	if (getGeoref(geoTransform,sysProjWKT,pixConv))
	{
		
		if(verbose) std::cout << "GeoRef GDALImageDriver : "<<geoTransform[0]<<" "<<geoTransform[3]<<" "<<sysProjWKT<<std::endl;
		georef::Data geo(geoTransform,sysProjWKT, pixConv);
		georef::SetGeoref(geo, _metaData);
	}
    
    
    
	
	if (verbose)
		std::cout << "Fin du Update : "<<(_poDataset!=NULL)<<std::endl;

    
	return (_poDataset!=NULL);
}

///
///
///
std::string GDALImageDriver::GetDriverName(std::string const &fullPathName)
{
	// A refaire en utilisant plutot la methode GDALIdentifyDriver
	std::string nomDriverGDAL("");
	// Recuperation de l'extension de l'image
	size_t pos = fullPathName.find_last_of('.');
	if (pos==std::string::npos) return nomDriverGDAL;
	std::string extension = fullPathName.substr(pos,std::string::npos);
	std::transform(extension.begin(),extension.end(),extension.begin(),tolower);
	
	
	if ((extension==std::string(".tif"))||(extension==std::string(".tiff")))
		nomDriverGDAL=std::string("GTiff");
	else if (extension== std::string(".bmp"))
		nomDriverGDAL=std::string("BMP");
	else if (extension== std::string(".jp2"))
		nomDriverGDAL=std::string("JP2KAK");
	/*
	 // Comme le Gif ne supporte que les images indexees on verra ca plus tard
	 else if (extension== std::string(".gif"))
	 nomDriverGDAL=std::string("GIF");
	 */
	else if (extension== std::string(".jpg"))
		nomDriverGDAL=std::string("JPEG");
	else if (extension== std::string(".png"))
		nomDriverGDAL=std::string("PNG");
	else if (extension==std::string(".ecw"))
		nomDriverGDAL=std::string("ECW");
	else if (extension==std::string(".bil"))
		nomDriverGDAL=std::string("BILHDR");
	else if (extension==std::string(".vrt"))
		nomDriverGDAL=std::string("VRT");

	return nomDriverGDAL;
}

std::set<std::string> GDALImageDriver::getExtensions()const
{
	std::set<std::string> retour;
	bool verbose = 0;
	int numDriverGDAL = GetGDALDriverManager()->GetDriverCount();
	for (int i = 0; i < numDriverGDAL; ++i)
	{
		GDALDriver* driver = GetGDALDriverManager()->GetDriver(i);
		if (driver)
		{
			std::string driverName = GDALGetDriverShortName(driver);
			if (verbose) std::cout<<"[GDALImageDriver::getExtensions] driverName: "<<driverName<<" -> "<<GDALGetDriverLongName(driver) <<std::endl;
			if (driverName == "GTiff") { retour.insert("TIFF");retour.insert("TIF");}
			if (driverName == "JP2KAK") { retour.insert("JP2");}
			if (driverName == "ECW") { retour.insert("ECW");}
			if (driverName == "BMP") { retour.insert("BMP");}
			if (driverName == "PNG") { retour.insert("PNG");}
			if (driverName == "JPEG") { retour.insert("JPG");}
			if (driverName == "VRT") { retour.insert("VRT");}
      if (driverName == "SRP") { retour.insert("IMG");}
		}
	}
	return retour;
}

///
///
///
bool GDALImageDriver::create(std::string const &fullPathName, int NRows, int NLines, int NBands, eTypePixel* tabType, ign::data::Object const *  options, ign::data::Object const *  metadata)
{
    bool verbose = 0;
	
	if (verbose) std::cout << "[GDALImageDriver::create] "<<fullPathName<<" START"<<std::endl;
	if (_poDataset)
	{
		ImageDriverManager::Instance()->detachDriver(this);
		GDALClose(_poDataset);
		_poDataset = NULL;
	}
	// GDAL ne sait pas creer une image avec un type different par bande
	eTypePixel imageType = tabType[0];
	for(int band=1;band<NBands;++band)
	{
		if (tabType[band]!=imageType)
		{
			if (verbose) std::cout<<"[GDALImageDriver::create] Types numeriques differents entrebandes"<<std::endl;
			return false;
		}
	}
	
	_filename = fullPathName;
	std::string nomDriverGDAL = GDALImageDriver::GetDriverName(_filename);
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(nomDriverGDAL.c_str());
	if (verbose) std::cout << "Driver GDAL : "<<nomDriverGDAL<<std::endl;
	
	if (!poDriver)
	{
		if (verbose) std::cout << "[GDALImageDriver::create] Driver de GDAL non trouve"<<std::endl;
		return false;
	}
	
	char **papszMetadata= poDriver->GetMetadata();
	
	// added by RP : sets default behaviour.
    // pixConvAskedByAPI indicates which convention to store within the image file.
	// if no option is specified, we assume it is the "pixel center" convention.
	ign::image::georef::PixConv pixConvAskedByAPI = ign::image::georef::PixConvNULL;
	
	// add RP  pour images binaires
	if(imageType == ign::numeric::eBool)
	{
		_options.setAttribute("NBITS", data::Integer(1));
        
        // ajout pour images binaires
        ColorTable binaryTable;
        // la table possede deux entrees
        ColorEntry entryBlack(0,0,0,0);
        ColorEntry entryWhite(255,255,255,255);
        binaryTable.setColor(0,entryBlack);
        binaryTable.setColor(1,entryWhite);
        setColorTable(0, binaryTable);
	}

	// Gestion des options par defaut
	if(strcmp(nomDriverGDAL.c_str(), "GTiff")==0 && NBands==3)
	{
		_options.setAttribute("INTERLEAVE", data::String("PIXEL"));
		_options.setAttribute("PHOTOMETRIC", data::String("RGB"));
	}
	
	if(strcmp(nomDriverGDAL.c_str(), "JP2KAK")==0)
	{
		_options.setAttribute("QUALITY", data::Integer(100));
		
	}
	// Prise en compte des options demandees par l'API
	if (options)
	{
		for(size_t o=0;o<options->numAttributes();++o)
		{
			std::string key,value;
			key = options->getAttributeName(o);
			value = options->attribute(o).value().toString();
			
			if(key == "GRF_PIX_CONV")
			{
				if(value == "POINT")
				{
					if(verbose) std::cout<<" found PIX_CONV option set to POINT"<<std::endl;
					pixConvAskedByAPI = ign::image::georef::PixConvPOINT;
				}
				else if (value == "AREA")
				{
					if(verbose) std::cout<<" found PIX_CONV option set to AREA"<<std::endl;
					pixConvAskedByAPI = ign::image::georef::PixConvAREA;
				}
				else
				{
					if(verbose) std::cout<<" found PIX_CONV option, but not assigned properly => use of area as default setting"<<std::endl;
				}
				continue;
			}
			_options.setAttribute(key, data::String(value));
		}
	}

	if(strcmp(nomDriverGDAL.c_str(), "ECW")==0)
	{
        // imposed by the ECW format, always interpreted as corner
		if(verbose) std::cout<<" ECW format => force pixConvAskedByAPI to PixConvAREA"<<std::endl;
		pixConvAskedByAPI = ign::image::georef::PixConvAREA;
	}
	
	if(verbose) std::cout<<"pixel convention asked by API is :  "<<ign::image::georef::PixConventionToString(pixConvAskedByAPI)<<std::endl;

	if(remove(fullPathName.c_str())!=0){
		if(verbose) std::cout << "Echec a la suppression du fichier : " << fullPathName << std::endl;
	}
	
	if( !CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
	{
		if (verbose) std::cout << "Le Driver "<<nomDriverGDAL<<" ne sait pas creer d'image"<<std::endl;
		if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
		{
			if (verbose) std::cout << "Le Driver "<<nomDriverGDAL<<" sait creer des images par recopie"<<std::endl;
			
			// On va faire un fichier de cache en Tiff que l'on recopira dans le format souhaite au moment de la destruction
			_needCopy = true;
            _useCache = true;
			
			if (metadata)
				_metaData = *metadata;
			
			size_t pos = fullPathName.find_last_of('.');
			if (pos==std::string::npos)
			{
				_nomCache=fullPathName+kNomCache;
			}
			else
			{
				_nomCache=fullPathName.substr(0,pos)+kNomCache;
			}
			if (verbose) std::cout << "Nom du cache : "<<_nomCache<<std::endl;
			
			_driverCache = GetGDALDriverManager()->GetDriverByName("GTiff");
			if (verbose) std::cout << "Creation du cache"<<std::endl;
			if(remove(_nomCache.c_str())!=0){
				//std::cout << "Echec a la suppression du fichier : " << fullPathName << std::endl;
			}
			
			// Cas TIF 3 canaux => on force les options a RGB
			ign::data::Object optionsCache;
			if(NBands==3)
			{
				optionsCache.setAttribute("INTERLEAVE",data::String("PIXEL"));
				optionsCache.setAttribute("PHOTOMETRIC",data::String("RGB"));
			}
			
			char ** Options = NULL;
			// Traduction du dictionnaire en char ** si necessaire
			if (optionsCache.numAttributes())
			{
				Options = new char*[optionsCache.numAttributes()+1];
				for(size_t o=0;o<optionsCache.numAttributes();++o)
				{
					std::string key,value;
					key = optionsCache.getAttributeName(o);
					value = optionsCache.attribute(o).value().toString();
					std::string optGDAL = key+"="+value;
					if (verbose) std::cout << "Option : "<<optGDAL<<std::endl;
					Options[o] = new char[optGDAL.size() + 1];
					std::copy(optGDAL.begin(), optGDAL.end(), Options[o]);
					Options[o][optGDAL.size()] = '\0';
				}
				Options[optionsCache.numAttributes()]=NULL;
			}
			if (verbose)
			{
				std::cout << "Options de creation " << std::endl;
				if (Options)
				{
					int j=0;
					const char * ptr = Options[j];
					j++;
					while(ptr)
					{
						std::cout << j << " : "<< ptr << std::endl;
						ptr = Options[j];
						j++;
					}
				}else{
					std::cout << "Aucune" << std::endl;
				}
			}
			_poDataset = _driverCache->Create( _nomCache.c_str(), NRows, NLines, NBands, TypeIO2TypeGDAL(imageType),(char **)Options );
			if (verbose) std::cout << "Cache cree"<<std::endl;
			if (_poDataset==NULL)
				std::cout << "Attention probleme de cache"<<std::endl;
            if(Options)
			{
				//fuite memoire corrigee NB 2014/01/21
				for(size_t o=0;o<optionsCache.numAttributes();++o) delete[] Options[o];
				
                delete[] Options;
			}
		}
		else
		{
			if (verbose) std::cout << "[GDALImageDriver::create] else CSLFetchBoolean" << std::endl;
			return false;
		}
	}
	else
	{
		if (verbose) std::cout << "[GDALImageDriver::create] traduction du dictionnaire en char **" << std::endl;
		char ** Options = NULL;
		// Traduction du dictionnaire en char ** si necessaire
		if (_options.numAttributes())
		{
			if (verbose) std::cout << "creation d'une liste d'options et on la parcourt" << std::endl;
			Options = new char*[_options.numAttributes()+1];
			for(size_t o=0;o<_options.numAttributes();++o)
			{
				std::string key,value;
				key = _options.getAttributeName(o);
				value = _options.attribute(o).value().toString();
				std::string optGDAL = key+"="+value;
				if (verbose) std::cout << "Option : "<<optGDAL<<std::endl;
				Options[o] = new char[optGDAL.size() + 1];
				std::copy(optGDAL.begin(), optGDAL.end(), Options[o]);
				Options[o][optGDAL.size()] = '\0';
			}
			Options[_options.numAttributes()]=NULL;
		}
		if (verbose) std::cout << "creation d'un GDALDriver" << std::endl;
		_poDataset = poDriver->Create( fullPathName.c_str(), NRows, NLines, NBands, TypeIO2TypeGDAL(imageType), (char **)Options );
		
		if (verbose) std::cout << "GDALDriver cree" << std::endl;
		if(Options)
		{
			if (verbose) std::cout << "suppression des options" << std::endl;
			
			//fuite memoire corrigee NB 2014/01/21
			for(size_t o=0;o<_options.numAttributes();++o) delete[] Options[o];
			
			delete[] Options;
		}
		if (verbose) std::cout << "fin traduction dictionnaire" << std::endl;
	}
	if (_poDataset==NULL)
	{
		if (verbose) std::cout << "[GDALImageDriver::create] Echec a la creation du DataSet" << std::endl;
		return false;
	}
	if (metadata)
	{
		if (verbose) std::cout << "Metadata"<<std::endl;
		if (verbose) std::cout << "GeoRef : "<<(int)georef::IsGeoref(*metadata)<<std::endl;
		if (georef::IsGeoref(*metadata))
		{
			georef::Data georeferencement;
			georef::GetGeoref(*metadata, georeferencement);
			
			double geoTransform[6];
			geoTransform[0]=georeferencement._origin.first; /* top left x */
			geoTransform[1]=georeferencement._step.first; /* w-e pixel resolution */
			geoTransform[2]=0.; /* rotation, 0 if image is "north up" */
			geoTransform[3]=georeferencement._origin.second; /* top left y */
			geoTransform[4]=0.; /* rotation, 0 if image is "north up" */
			geoTransform[5]=georeferencement._step.second;/* n-s pixel resolution */
			
			// deal with auto georeferenced formats
			
            if (pixConvAskedByAPI==ign::image::georef::PixConvNULL)
                pixConvAskedByAPI = georeferencement._pixConvention;
			else if(georeferencement._pixConvention != pixConvAskedByAPI)
			{// the nwe have to shift the coordinates by half a pixel
				if( (georeferencement._pixConvention == ign::image::georef::PixConvAREA)
				   && (pixConvAskedByAPI == ign::image::georef::PixConvPOINT) )
				{// in this case  we have coordinates of the center, and we want to write the corner
					if(verbose) std::cout<<" have PixConvAREA, but user asked PixConvPOINT"<<std::endl;
					std::cout<<geoTransform[0]<<" , "<<geoTransform[3]<<std::endl;
					geoTransform[0] = geoTransform[0] + 0.5 * fabs(geoTransform[1]);
					geoTransform[3] = geoTransform[3] - 0.5 * fabs(geoTransform[5]);
					
				}
				
				else if( (georeferencement._pixConvention == ign::image::georef::PixConvPOINT)
						&& (pixConvAskedByAPI == ign::image::georef::PixConvAREA) )
				{// in this case  we have coordinates of the corner, and we want to write the center
					if(verbose)std::cout<<" have PixConvPOINT, but user asked PixConvAREA"<<std::endl;
					geoTransform[0] = geoTransform[0] - 0.5 * fabs(geoTransform[1]);
					geoTransform[3] = geoTransform[3] + 0.5 * fabs(geoTransform[5]);
					
				}
				else
				{
					// absolutely abnormal
					std::cout<<"[GDALImageDriver::create] problem encountered with GRF_PIX_CONV option"<<std::endl;
				}
			}
			setGeoref(geoTransform, georeferencement._projection,pixConvAskedByAPI);
            if (verbose) std::cout << "apres le setGeoref"<<std::endl;
		}
        if (verbose) std::cout << "nombre de md : "<<metadata->numAttributes()<<std::endl;
		for(size_t i=0;i<metadata->numAttributes();++i)
		{
			std::string key,value;
			key = metadata->getAttributeName(i);
			if (georef::IsTagGeoref(key))
				continue;
			value = metadata->attribute(i).value().toString();
			if (verbose) std::cout << "Metadata : "<<key<<" : "<<value<<std::endl;
			_poDataset->SetMetadataItem(key.c_str(),value.c_str());
		}
	}
	if( ! _ConfigOptions.isEmpty() )
	{
		for(size_t i=0;i<_ConfigOptions.numAttributes();++i)
		{
			std::string key,value;
			key = _ConfigOptions.getAttributeName(i);
			value = _ConfigOptions.attribute(i).value().toString();
			if (verbose) std::cout << "Configuration option  : "<<key<<" : "<<value<<std::endl;
		    CPLSetConfigOption( key.c_str(),value.c_str() );
		}
	}

	if (verbose) std::cout << "[GDALImageDriver::create] _poDataset: " <<_poDataset<<" END"<< std::endl;
	return (_poDataset!=NULL);
}

///
///
///
bool GDALImageDriver::createCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn, ign::data::Object const *  /*options*/,ign::data::Object const *  /*metadata*/)const
{
	bool verbose = false;
	if (verbose) std::cout << "GDALImageDriver::CreateCopy "<<fullPathNameOut<<std::endl;
	// SKD: GDALOpen theorically accepts UTF-8 encoding, but the behaviour is BUGGY under Windows! (See note at the begining of the file)
	GDALDataset *poSrcDS = (GDALDataset *)GDALOpen( ign::tools::StringManip::FromUTF8ToCurrentCodePage(fullPathNameIn).c_str(), GA_ReadOnly );
	if (!poSrcDS) return false;
	
	std::string nomDriverGDAL = GDALImageDriver::GetDriverName(_filename);
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(nomDriverGDAL.c_str());
	if (verbose) std::cout << "Driver GDAL : "<<nomDriverGDAL<<std::endl;
	
	if (!poDriver)
	{
		if (verbose) std::cout << "Driver non trouve"<<std::endl;
		GDALClose(poSrcDS );
		return false;
	}
	
	char **papszMetadata= poDriver->GetMetadata();
	if( !CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
	{
		if (verbose) std::cout << "Le Driver "<<nomDriverGDAL<<" ne sait pas creer d'image par recopie"<<std::endl;
		GDALClose(poSrcDS );
		return false;
	}
	
	GDALDataset *poDstDS = poDriver->CreateCopy( fullPathNameOut.c_str(), poSrcDS, FALSE, NULL, NULL, NULL );
	
	/* Once we're done, close properly the dataset */
	if( poDstDS != NULL )
		GDALClose(poDstDS );
	GDALClose(poSrcDS );
	return true;
}

/*
///
///
///
bool create(const std::map<int, std::pair<boost::filesystem::path, int> >  &setnom, const std::string &systemId, const ign::geodesy::SystemCoordProjection* srs)
{
    if (setnom.size()==0)
        return false;
    
     GDALDriver *poDriver = (GDALDriver *) GDALGetDriverByName( "VRT" );
     if (poDriver==NULL)
     return false;
     GDALDataset *poVRTDS=NULL;
    std::map<int, std::pair<boost::filesystem::path, int> >::const_iterator it,fin=setnom.end();
    for(it=setnom.begin();it!=fin;++it)
    {
        boost::filesystem::path nomDalle(it->second.first);
        if (verbose) std::cout<<"dalle "<<it->first<<": "<<nomDalle<<std::endl;
        
        if (boost::filesystem::is_symlink(nomDalle))
            nomDalle = boost::filesystem::canonical(nomDalle);
        
        if (!boost::filesystem::exists(nomDalle))
        {
            std::ostringstream oss;
            oss << nomDalle.generic_string() <<" does not exist";
            IGN_THROW_EXCEPTION(oss.str());
        }
        
        if (!boost::filesystem::is_regular_file(nomDalle))
        {
            if (verbose) std::cout << nomDalle.generic_string() <<" is not a regular file"<<std::endl;
            continue;
        }
        
        if (verbose) std::cout << "analyse du fichier : "<< nomDalle.generic_string() <<std::endl;
        
        if (poVRTDS==NULL)
        {
            // On initialise le VRT avec la premiere image
            GDALDataset *poSrcDS=(GDALDataset *) GDALOpenShared( nomDalle.generic_string(), GA_ReadOnly );
            if (poSrcDS==NULL)
            {
                std::ostringstream oss;
                oss << nomDalle.generic_string() <<" is not readable by GDAL";
                IGN_THROW_EXCEPTION(oss.str());
            }
            poVRTDS = poDriver->CreateCopy( "", poSrcDS, FALSE, NULL, NULL, NULL );
            GDALClose((GDALDatasetH) poSrcDS);
        }
        else
        {
            // On ajoute le nouveau fichier au VRT
            
        }
    }
    
    GDALClose((GDALDatasetH) poVRTDS);
    return true;
}
*/

///
///
///
void GDALImageDriver::close()
{
	bool verbose = false;
	
	if (verbose) std::cout << "GDALImageDriver::close"<<std::endl;
	if (verbose) std::cout << "filename : "<<_filename<<std::endl;
	if (verbose) std::cout << "_poDataset : "<<_poDataset<<std::endl;
	if (_poDataset)
	{
		if (verbose) std::cout << "NeedCopy : "<<_needCopy<<std::endl;
        if (verbose) std::cout << "UseCache : "<<_useCache<<std::endl;
		if (_needCopy)
		{
			/*#ifdef WIN32
			 // On detruit le fichier original car sinon la recopie ne fonctionne pas sous Windows
			 // avec un Permission denied?
			 {
			 std::ofstream fic (_filename.c_str());
			 fic << "rien"<<std::endl;
			 }
			 #endif*/
			
			if (verbose) std::cout << "Il faut recopier le cache"<<std::endl;
			std::string nomDriverGDAL = GDALImageDriver::GetDriverName(_filename);
			if (verbose) std::cout << "Nom du driver : "<<nomDriverGDAL<<std::endl;
			GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(nomDriverGDAL.c_str());
			GDALDataset *poDstDS;
			
			
			char ** Options = NULL;
			// gestion des options du format
			if (!_options.isEmpty())
			{
				Options = new char*[_options.numAttributes()+1];
				for(size_t o=0;o<_options.numAttributes();++o)
				{
					std::string key,value;
					key = _options.getAttributeName(o);
					value = _options.attribute(o).value().toString();
					std::string optGDAL = key+"="+value;
					if (verbose) std::cout << "Option : "<<optGDAL<<std::endl;
					Options[o] = new char[optGDAL.size() + 1];
					std::copy(optGDAL.begin(), optGDAL.end(), Options[o]);
					Options[o][optGDAL.size()] = '\0';
				}
				Options[_options.numAttributes()]=NULL;
			}
			
			if (verbose) std::cout << "avant la copie"<<std::endl;
			
			try
			{
				if (verbose) std::cout << "je veux remettre le error handler perso..."<<std::endl;
				if (verbose) std::cout << "remis le error handler perso"<<std::endl;
				poDstDS = poDriver->CreateCopy( _filename.c_str(), _poDataset, FALSE, Options, NULL, NULL );
				if (verbose) std::cout << "apres la copie"<<std::endl;
				
			}
			catch(ign::Exception& e)
			{
				std::string ewhat(e.what());
				if(verbose) std::cout<<"[GDALImageDriver::close] exception attrappee :  "<<e.what()<<std::endl;
				size_t found = ewhat.find(kWarningGDAL);
				if (found!=std::string::npos)
				{
					if(verbose) std::cout<<"[GDALImageDriver::close] Il s'agit en fait d'un simple warning GDAL "<<std::endl;
					SetNullErrorHandler();	//qp 041114 gestion de la lecture des fichiers sortant de photoshop on desactive le warning de gdal
					if(verbose) std::cout<<"[GDALImageDriver::close] error Handler set to NULL , filename "<<_filename<<std::endl;
					poDstDS = poDriver->CreateCopy( _filename.c_str(), _poDataset, FALSE, Options, NULL, NULL );
					SetOwnErrorHandler(); //qp 041114
				}
				else
				{
                    IGN_THROW_EXCEPTION(std::string("[GDALImageDriver::close()] problem calling CreateCopy: ")+e.what());
				}
				
			}
			
			if( poDstDS != NULL )
                GDALClose(poDstDS );
		}
        if (_useCache)
        {
            // Il faut nettoyer le cache
            GDALClose(_poDataset);
            _driverCache->Delete(_nomCache.c_str());
        }
		else
		{
			try {
				GDALClose(_poDataset);
			} catch (...) {
				std::cout << "Pb dans le GDALClose"<<std::endl;
			}
			
		}
		_poDataset = NULL;
		_filename.clear();
		_needCopy = false;
		_nomCache.clear();
		_driverCache = NULL;
	}
	if (verbose) std::cout << "GDALImageDriver::close FIN"<<std::endl;
}

///
///
///
void GDALImageDriver::size(int &NRows,int &NLines)const
{
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	
	if (!_poDataset) return;
	NRows = _poDataset->GetRasterXSize();
	NLines = _poDataset->GetRasterYSize();
}

///
///
///
size_t GDALImageDriver::numBands()const
{
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	
	if (!_poDataset) return 0;
	return _poDataset->GetRasterCount();
}

///
///
///
eTypePixel GDALImageDriver::typePixel(int numBand)const
{
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	
	if (_poDataset)
	{
		GDALRasterBand  *poBand = _poDataset->GetRasterBand( numBand+1 );
		if (poBand)
		{
            // Pour les images 1 bit il faut tester les meta donnes de la bande
            const char * value = poBand->GetMetadataItem("NBITS","IMAGE_STRUCTURE");
			if (value)
            {
                int nb = atoi(value);
                if (nb == 1)
                    return ign::numeric::eBool;
            }
            return TypeGDAL2TypeIO(poBand->GetRasterDataType());
		}
	}
	return ign::numeric::eOther;
}

///
///
///
GDALDataType TypeIO2TypeGDAL(eTypePixel typeIO)
{
	switch (typeIO) {
		case ign::numeric::eOther:
			return GDT_Unknown;
		case ign::numeric::eUnsignedChar:
			return GDT_Byte;
		case ign::numeric::eSignedShort:
			return GDT_Int16;
		case ign::numeric::eUnsignedShort:
			return GDT_UInt16;
		case ign::numeric::eSignedInt:
			return GDT_Int32;
		case ign::numeric::eUnsignedInt:
			return GDT_UInt32;
		case ign::numeric::eFloat:
			return GDT_Float32;
		case ign::numeric::eBool:
			return GDT_Byte;// pour feinter GDAL qui ne connait pas le type booleen
	}
	return GDT_Unknown;
}

///
///
///
eTypePixel TypeGDAL2TypeIO(GDALDataType typeGDAL)
{
	switch (typeGDAL)
	{
		case GDT_Unknown:
			return ign::numeric::eOther;
		case GDT_Byte:
			return ign::numeric::eUnsignedChar;
		case GDT_UInt16:
			return ign::numeric::eUnsignedShort;
		case GDT_Int16:
			return ign::numeric::eSignedShort;
		case GDT_UInt32:
			return ign::numeric::eUnsignedInt;
		case GDT_Int32:
			return ign::numeric::eSignedInt;
		case GDT_Float32:
			return ign::numeric::eFloat;
		case GDT_Float64:
		case GDT_CInt16:
		case GDT_CInt32:
		case GDT_CFloat32:
		case GDT_CFloat64:
		default:
			return ign::numeric::eOther;
	}
	//return eOther;
}

///
///
///
std::string GDALImageDriver::getFileName()const
{
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	return _filename;
}

///
///
///
std::string GDALImageDriver::getDriverName()const
{
	std::string nom = GDALImageDriver::DriverName();
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	if (_poDataset)
	{
		GDALDriver* driver = _poDataset->GetDriver();
		if (driver)
			nom+= std::string(" : ")+std::string(driver->GetDescription());
	}
	return nom;
}

///
///
///
void GDALImageDriver::read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void *buffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)const
{
	bool verbose = 0;
    if (verbose) std::cout << "GDALImageDriver::Read "<<getFileName()<<"\nfrom: C0 = "<<C0<<" L0 = "<<L0<<" Band0 = "<<Band0<<" NRows = "<<NRows<<" NLines = "<<NLines<<" NBands = "<<NBands<<"\ndeZoom = "<<deZoom<<" bufferType ="<<ign::image::TypeToString(bufferType)<<" nPixelSpace = "<<nPixelSpace<<" nLineSpace = "<<nLineSpace<<" nBandSpace = "<<nBandSpace<<std::endl;

	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	
	if (_poDataset)
	{
		if (verbose) std::cout << "_poDataset initialise : "<<_poDataset<<std::endl;
		if (verbose) std::cout << "Utilisation du driver : "<<_poDataset->GetDriver()->GetDescription()<<std::endl;
        
        // GDAL ne sait pas lire directement en bool, il faut utiliser un buffer (en unsigned char)
        if (bufferType==ign::numeric::eBool)
        {
            // Allocation d'un buffer temporaire en unsigned char
            int NCbuffer = NRows/deZoom;
            int NLbuffer = NLines/deZoom;
            ign::unique_ptr<unsigned char> bufferChar(new unsigned char[NCbuffer*NLbuffer*NBands]);
	
            void *bufferPtr = (void*)bufferChar.get();
			eTypePixel bufferTypeIO = ign::numeric::eUnsignedChar;
            
            int *panBandMap = new int[NBands];
            for(int k=0;k<NBands;++k)
                panBandMap[k]=1+Band0+k;
            
            // Lecture dans le buffer temporaire
            CPLErr status = _poDataset->RasterIO(GF_Read, C0, L0, NRows, NLines, bufferPtr, NCbuffer, NLbuffer,TypeIO2TypeGDAL(bufferTypeIO) , NBands, panBandMap, 
                                                 NBands*ign::image::TypeSize(bufferTypeIO),NCbuffer*NBands*ign::image::TypeSize(bufferTypeIO) ,ign::image::TypeSize(bufferTypeIO));
            delete[] panBandMap;
            if (status!=CE_None)
            {
                if (verbose) std::cout<<"[GDALImageDriver] Erreur dans le RasterIO GF_Read"<<std::endl;
                IGN_THROW_EXCEPTION("Erreur dans le RasterIO GF_Read");
            }

            // Recopie dans le buffer definitif
            unsigned char *ptrTmp = bufferChar.get();
            for(int l=0;l<NLbuffer;++l)
            {
                unsigned char* ptrOut = (unsigned char*)buffer + l*nLineSpace/8;
                for(int c=0;c<NCbuffer;++c)
                {
                    for(int k=0;k<NBands;++k)
                    {
                        unsigned char val = (*ptrTmp);
                        ++ptrTmp;
                        int byteOffset = (c*nPixelSpace + k*nBandSpace)/8;
                        int bitOffset = (c*nPixelSpace + k*nBandSpace)%8;
                        unsigned char &mask = *(ptrOut+byteOffset);
                        if (val==0)
                        {
                            mask = mask & ~(1<<bitOffset);
                        }
                        else
                        {
                            mask = mask | (1<<bitOffset);
                        }    
                    }
                }
            }
        }   
        //sinon, on travaille directement avec le buffer donne en entree
        else
        {
            int *panBandMap = new int[NBands];
            for(int k=0;k<NBands;++k)
                panBandMap[k]=1+Band0+k;
			
			if (verbose)
			{
				std::cout<<"C0, L0: "<<C0<<" x "<<L0<<std::endl;
				std::cout<<"NRows, NLines: "<<NRows<<" x "<<NLines<<std::endl;
				std::cout<<"NRows/deZoom, NLines/deZoom: "<<NRows/deZoom<<" x "<<NLines/deZoom<<std::endl;
				std::cout<<"NBands: "<<NBands<<std::endl;
				std::cout<<"nPixelSpace: "<<nPixelSpace<<" x "<<ign::image::TypeSize(bufferType)<<std::endl;
				std::cout<<"nLineSpace: "<<nLineSpace<<" x "<<ign::image::TypeSize(bufferType)<<std::endl;
				std::cout<<"nBandSpace: "<<nBandSpace<<" x "<<ign::image::TypeSize(bufferType)<<std::endl;
				int NC,NL;
				this->size(NC,NL);
				std::cout<<"NC, NL: "<<NC<<" x "<<NL<<" x "<<ign::image::TypeToString(typePixel(0))<< std::endl;
			}

		//	CPLErr status = _poDataset->RasterIO(GF_Read, C0, L0, NRows, NLines, buffer,NRows/deZoom, NLines/deZoom,TypeIO2TypeGDAL(bufferType) , NBands, panBandMap, 
        //                                        NBands*ign::image::TypeSize(bufferType),NRows/deZoom*NBands*ign::image::TypeSize(bufferType) ,ign::image::TypeSize(bufferType));

	
			CPLErr status = _poDataset->RasterIO(GF_Read, C0, L0, NRows, NLines, buffer, NRows/deZoom, NLines/deZoom,TypeIO2TypeGDAL(bufferType) , NBands, panBandMap, 
												nPixelSpace*ign::image::TypeSize(bufferType),nLineSpace*ign::image::TypeSize(bufferType) ,nBandSpace*ign::image::TypeSize(bufferType));

			delete[] panBandMap;
            if (status!=CE_None)
            {
                if (verbose) std::cout<<"[GDALImageDriver] Erreur dans le RasterIO GF_Read"<<std::endl;
                IGN_THROW_EXCEPTION("Erreur dans le RasterIO GF_Read");
            }
        }
    }
    if (verbose) std::cout << "GDALImageDriver::Read end"<<std::endl;
}

///
///
///
void GDALImageDriver::write(int C0, int L0, int Band0, int NRows, int NLines, int NBands, void *buffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)
{
    bool verbose = 0;
    if ((_useCache)&&(!_needCopy))
        _needCopy=true;
	if(verbose) std::cout<<"[GDALImageDriver::write] START"<<std::endl;
	if(verbose) std::cout<<"GDALImageDriver::write: C0:"<<C0<<" L0:"<<L0<<" Band0:"<<Band0<<" NRows:"<<NRows<<" NLines:"<<NLines<<" NBands:"<<NBands<<std::endl;
	if (_poDataset)
	{
        int *panBandMap = new int[NBands];
		for(int k=0;k<NBands;++k)
			panBandMap[k]=1+Band0+k;
		
		CPLErr status;
		
		if(bufferType != ign::numeric::eBool)
		{
			if(verbose) std::cout<<"[GDALImageDriver::write] Not a boolean image"<<std::endl;
			
			status = _poDataset->RasterIO(GF_Write, C0, L0, NRows, NLines, (void*)buffer, NRows, NLines,
										  TypeIO2TypeGDAL(bufferType) , 
										  NBands, panBandMap, 
										  nPixelSpace*ign::image::TypeSize(bufferType),
										  nLineSpace*ign::image::TypeSize(bufferType) ,
										  nBandSpace*ign::image::TypeSize(bufferType));
			if(verbose) std::cout<<"[GDALImageDriver::write] Done: status = "<<status<<std::endl;
		}
		else
		{
            // L'ajout de la table de couleur est fait dans le create
            /*
			 // ajout pour images binaires
			 ColorTable binaryTable;
			 // la table possede deux entrees
			 ColorEntry entryBlack(0,0,0,0);
			 ColorEntry entryWhite(255,255,255,255);
			 binaryTable.setColor(0,entryBlack);
			 binaryTable.setColor(1,entryWhite);
			 setColorTable(0, binaryTable);
			 */
			
            // GDAL ne sait pas manipuler directement le tableau de bits, il faut creer un buffer un unsigned char
            // allocation du buffer temporaire
            ign::unique_ptr<unsigned char> bufferChar(new unsigned char[NRows*NLines*NBands]);
            // recopie des donnees dans ce buffer
            unsigned char *ptrTmp = bufferChar.get();
            for(int l=0;l<NLines;++l)
            {
                unsigned char* ptrIn = (unsigned char*)buffer + l*nLineSpace/8;
                for(int c=0;c<NRows;++c)
                {
                    for(int k=0;k<NBands;++k)
                    {
                        unsigned char val = 0;
                        int byteOffset = (c*nPixelSpace + k*nBandSpace)/8;
                        int bitOffset = (c*nPixelSpace + k*nBandSpace)%8;
                        unsigned char &mask = *(ptrIn+byteOffset);
                        if (mask & (1<<bitOffset))
                        {
                            val = 255;
                        }
                        (*ptrTmp) = val;
                        ++ptrTmp;
                    }
                }
            }
             
			status = _poDataset->RasterIO(GF_Write, C0, L0, NRows, NLines, (void*)bufferChar.get(), NRows, NLines,
										  TypeIO2TypeGDAL(ign::numeric::eUnsignedChar) , 
										  NBands, panBandMap, 
										  NBands,
										  NRows*NBands ,
										  1);
		}
		if (status!=CPLE_None)
		{
			IGN_THROW_EXCEPTION("Erreur dans le RasterIO GF_Write");
		}
		delete[] panBandMap;
	}
	if(verbose) std::cout<<"[GDALImageDriver::write] END"<<std::endl;
}

///
///
///
std::string GetSystemNameSD(GDALDataset *poDataset)
{
	bool verbose = 0;

	if (verbose)  std::cout << " [GetSystemNameSD] START"<< std::endl;

	std::string sysProjWKT, sysProj4, sysProjSD;
	try
	{
		if (poDataset->GetProjectionRef() == NULL)
		{
			return ign::geodesy::ProjectionList::kStrLocal;
		}
        std::string proj (poDataset->GetProjectionRef());
        if (proj.empty()) // pas de projection, pas besoin d'aller plus loin
        {
            return ign::geodesy::ProjectionList::kStrLocal;
        }
	}
	catch (...)
	{
		return ign::geodesy::ProjectionList::kStrLocal;
	}

	ign::shared_ptr<OGRSpatialReference> oSRSPtr;
	try
	{
		oSRSPtr.reset(new OGRSpatialReference(poDataset->GetProjectionRef()));
		if (verbose)  std::cout << " projection trouvee : " << std::string(poDataset->GetProjectionRef()) << std::endl;
	}
	catch (std::exception& e)
	{
		if (verbose) std::cout << "[GetSystemNameSD] GDAL exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		if (verbose) std::cout << "[GetSystemNameSD] other exception: " <<std::endl;
	}

	if (oSRSPtr.get() != 0)
	{
		try
		{
			char * pszSRS_WKT = NULL;
			if (oSRSPtr->exportToWkt(&pszSRS_WKT) == OGRERR_NONE)
			{
				sysProjWKT = std::string(pszSRS_WKT);
				//delete[] pszSRS_WKT; //remis NB 2014/01/24 -> fuite memoire sinon sous MacOSX
				CPLFree(pszSRS_WKT);
			}
		}
		catch (...)
		{
		}
		try
		{
			char * pszSRS_PJ4(NULL);

			if (oSRSPtr->exportToProj4(&pszSRS_PJ4) == OGRERR_NONE)
			{
				sysProj4 = std::string(pszSRS_PJ4);
				if (verbose) std::cout << pszSRS_PJ4 << std::endl;
				CPLFree(pszSRS_PJ4);
				pszSRS_PJ4 = 0;
			}
		}
		catch (...)
		{
		}
	}
	

	bool projFound(false);
	if (!sysProjWKT.empty())
	{
		try
		{
			//on regarde si on a la chaine WKT dans le dictionnaire de projection
			sysProjSD = ign::geodesy::ProjectionList::FindInternalCodeProj(sysProjWKT, "WKT");
			projFound = true;
			if (verbose) std::cout << "[GDALImageDriver::getGeoref] conversion en code SD reussie == " << sysProjSD << std::endl;
		}
		catch (...)
		{
		}
	}
	if (!sysProj4.empty() && !projFound)
	{
		try
		{
			//en desespoir de cause on recupere la chaine PROJ4 retransformee par OGR
			//sysProjSD = sysProj4;
			//if (verbose) std::cout<<"[GDALImageDriver::getGeoref] on renvoie la chaine PROJ4 car on n'a pas reussi a la convertir en code SD"<<std::endl;

			if (verbose) std::cout << "[GDALImageDriver::getGeoref] conversion en PROJ4 via GDAL:   == " << sysProj4 << std::endl;
			sysProjSD = ign::geodesy::ProjectionList::FindInternalCodeProj(sysProj4, "PROJ4");
			if (verbose) std::cout << "[GDALImageDriver::getGeoref] conversion en code SD reussie == " << sysProjSD << std::endl;
		}
		catch (...)
		{
		}
	}
		

	if (!projFound)
	{
		try
		{
			// other solution : we fetch the list of internal codes; for each code, we generate the corresponding WKT code.
			// Then we compare the latter with sysProjWKT. This is very slow, but is useful in case GDAL writes a different WKT code than
			// ours.

			// comparer avec oSRS, qui possede operateur d'egalite
			bool found = false;
			size_t nbCodes = ign::geodesy::ProjectionList::GetNumCode();
			for (size_t i = 0; i < nbCodes; i++)
			{
				std::string internalCurrentCode = ign::geodesy::ProjectionList::GetCode(static_cast<int>(i));

				// on recree le code WKT correspondant
				std::string WKTCurrentCode;
				try
				{
					WKTCurrentCode = ign::geodesy::ProjectionList::FindExternalCodeProj(internalCurrentCode, "WKT");
				}
				catch (...)
				{
					continue;
				}
				// on compare avec le code WKT recupere via oSRS
				OGRSpatialReference oSRSgenerated;
				try
				{
					oSRSgenerated.importFromWkt(WKTCurrentCode.c_str());

					if (CompareSRS(oSRSgenerated, *oSRSPtr.get()))
					{
						found = true;
						sysProjSD = internalCurrentCode;
						break;
					}
				}
				catch (...)
				{
                    if (verbose) std::cout << "impossible de charger un SRS GDal a partir de " << WKTCurrentCode << std::endl;
				}
			}
		}
		catch (...)
		{
			sysProjSD = ign::geodesy::ProjectionList::kStrLocal;
		}
	}


	if (verbose)  std::cout << " [GetSystemNameSD] END: " << sysProjSD << std::endl;
	return sysProjSD;
}

///
///
///
bool GDALImageDriver::getGeoref(double geoTransform[6],std::string &sysProjSD, ign::image::georef::PixConv & pixConv)const
{
	bool verbose = 0;
	
	try
	{
		if (verbose) std::cout<<">[GDALImageDriver::getGeoref]"<<std::endl;
		
		// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
		ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
		
		if (_poDataset)
		{
			//NB plante sous MacOsX sinon...
			//ign::unique_ptr<const char> metaAeraPointPtr(_poDataset->GetMetadataItem(GDALMD_AREA_OR_POINT));
            const char* metaAeraPoint = _poDataset->GetMetadataItem(GDALMD_AREA_OR_POINT);
			
			if(metaAeraPoint)
			{
				std::string strMetaAreaPoint(metaAeraPoint);
				// if(verbose)	std::cout<<"recp butale : "<<metaAeraPoint<<std::endl;
				//delete[] metaAeraPoint; => plante sous MacOsX
				
				boost::algorithm::to_upper(strMetaAreaPoint);
				if(strMetaAreaPoint == "POINT")
				{
					if(verbose)std::cout<<" AREA_OR_POINT : POINT "<<std::endl;
					pixConv = ign::image::georef::PixConvPOINT;
				}
				else if(strMetaAreaPoint == "AREA")
				{
					if(verbose)std::cout<<" AREA_OR_POINT : AREA "<<std::endl;
					pixConv = ign::image::georef::PixConvAREA;
				}
				else
				{
					if(verbose)std::cout<<" AREA_OR_POINT : not found, so assumed it is AREA (same interpretation as GDAL itself) "<<std::endl;
					pixConv = ign::image::georef::PixConvAREA;
				}
			}
			else
			{
				if(verbose)std::cout<<" AREA_OR_POINT : not found, so assumed it is AREA (same interpretation as GDAL itself) "<<std::endl;
				pixConv = ign::image::georef::PixConvAREA;
			}
			
			if (verbose) std::cout<<"[GDALImageDriver::getGeoref] GetGeoTransform?"<<std::endl;
			
			if( _poDataset->GetGeoTransform( geoTransform ) == CE_None )
			{
				if (verbose) std::cout<<"[GDALImageDriver::getGeoref] GetGeoTransform found"<<std::endl;
				
				sysProjSD = GetSystemNameSD(_poDataset);
				
				//ajout NB 2015/06/09
				//La doc de la méthode GDALDataset::GetGeoTransform (http://www.gdal.org/classGDALDataset.html#af9593cc241e7d140f5f3c4798a43a668) indique que la partie translation de la transformation correspond toujours au coin haut gauche du pixel haut gauche, et que la metadonnee est une indication sur le georeferencement d'ORIGINE
				//donc pour revenir au grf d'origine, on redecale l'origine d'1/2 pixel
				if (pixConv == ign::image::georef::PixConvPOINT)
				{
					
					if(verbose)std::cout<<"avant: "<<geoTransform[0]<<" x "<<geoTransform[3]<<std::endl;
					geoTransform[0] += geoTransform[1]/2.;
					geoTransform[3] += geoTransform[5]/2.;
					if(verbose)std::cout<<"res: "<<geoTransform[1]<<" x "<<geoTransform[5]<<std::endl;
					if(verbose)std::cout<<"apres: "<<geoTransform[0]<<" x "<<geoTransform[3]<<std::endl;

				}
				
				if (verbose) std::cout<<"avant return true"<<std::endl;
				
				return true;
			}
			else
			{
				if (verbose) std::cout<<"[GDALImageDriver::getGeoref] GetGeoTransform not found"<<std::endl;
			}
		} 
		else
		{
			if (verbose) std::cout<<"[GDALImageDriver::getGeoref] _poDataset not found"<<std::endl;
		}
	}
	
	catch(std::exception& e)
	{
		if (verbose) std::cout<<"[GDALImageDriver::getGeoref] GDAL exception: "<<e.what()<<std::endl;
	}
	catch(...)
	{
		if (verbose) std::cout<<"[GDALImageDriver::getGeoref] unknown GDAL exception"<<std::endl;
	}
	
	if (verbose) std::cout<<"<[GDALImageDriver::getGeoref] end (with return false)"<<std::endl;
	return false;
}

///
///
///
bool GDALImageDriver::setGeoref(double geoTransform[6],std::string const &sysProjSD, ign::image::georef::PixConv const &pixConv)
{
	bool verbose = 0;
	if (verbose) std::cout<<"[GDALImageDriver::setGeoref] START"<<std::endl;
	
    if ((_useCache)&&(!_needCopy))
        _needCopy=true;
	
	if (_poDataset)
	{
		//correction GM le 30/08/2017: il faut faire la meme correction que celle faite par NB le 2015/06/09 sur le getGeoref
                //La doc de la méthode GDALDataset::SetGeoTransform (https://gdal.org/doxygen/classGDALDataset.html#a5101119705f5fa2bc1344ab26f66fd1d) indique que la partie translation de la transformation correspond toujours au coin haut gauche du pixel haut gauche, et que la metadonnee est une indication sur le georeferencement d'ORIGINE
                //donc pour revenir au grf d'origine, on redecale l'origine d'1/2 pixel
                double modifGeoTransform[6];
                for(int i=0;i<6;++i) modifGeoTransform[i] = geoTransform[i];
                if (pixConv == ign::image::georef::PixConvPOINT)
                {
                    if(verbose) std::cout << "[GDALImageDriver::setGeoref] gestion de la convention CORNER"<<std::endl;

                        modifGeoTransform[0] -= modifGeoTransform[1]/2.;
                        modifGeoTransform[3] -= modifGeoTransform[5]/2.;
                }
                if(verbose)
                {
                        std::cout << "[GDALImageDriver::setGeoref] Apres la prise en compte de pixConv"<<std::endl;
                        std::cout << "[GDALImageDriver::setGeoref]  -- "<<modifGeoTransform[0]<<" "<<modifGeoTransform[1]<<" "<<modifGeoTransform[2]<<std::endl;
                        std::cout << "[GDALImageDriver::setGeoref]  -- "<<modifGeoTransform[3]<<" "<<modifGeoTransform[4]<<" "<<modifGeoTransform[5]<<std::endl;
                }
                if (_poDataset->SetGeoTransform(modifGeoTransform)!= CE_None)
                {
                        //std::cout << "Erreur dans SetGeoref" << std::endl;
                        //return false;
                        IGN_THROW_EXCEPTION("[GDALImageDriver::setGeoref] SetGeoTransform failed");
                }
		/*if (_poDataset->SetGeoTransform(geoTransform)!= CE_None)
		{
			//std::cout << "Erreur dans SetGeoref" << std::endl;
			//return false;
			IGN_THROW_EXCEPTION("[GDALImageDriver::setGeoref] SetGeoTransform failed");
		}*/
		
		std::string sysProjWKT;
		try {
			//on regarde si on a la chaine WKT dans le dictionnaire de projection
			if(verbose)	std::cout << "[GDALImageDriver::setGeoref] on recherche la chaine WKT correspondant a " <<sysProjSD<< std::endl;
			sysProjWKT = ign:: geodesy::ProjectionList::FindExternalCodeProj(sysProjSD, "WKT");
		}
		catch(...)
		{
			//sinon on regarde si on a la chaine PROJ4 dans le dictionnaire de projection
			//et on essaie de la transformer via OGR
			//attention il faut une chaine proj4 normalisee dans le dictionnaire, OGR ne reconnait pas les +proj=init:IGNF...
			try {
				if(verbose)	std::cout << "[GDALImageDriver::setGeoref] on essaie de passer par les conversions PROJ4 <-> WKT de OGR" << std::endl;
                std::string sysProj4 = sysProjSD;
				char *pszSRS_WKT = NULL;
				OGRSpatialReference oSRS;
				oSRS.importFromProj4(sysProj4.c_str());
				oSRS.exportToWkt( &pszSRS_WKT );
				sysProjWKT = std::string(pszSRS_WKT);
				CPLFree( pszSRS_WKT );
				
				if (!sysProjWKT.empty())
				{
					if(verbose)	std::cout << "[GDALImageDriver::setGeoref] projWKT trouvee par conversion du code PROJ4 dans OGR : " << sysProjWKT << std::endl;
				}
				else
				{
					//on suppose qu'on a passe directement une chaine PROJ4 normalisee
					oSRS.importFromProj4(sysProjSD.c_str());
					oSRS.exportToWkt( &pszSRS_WKT );
					sysProjWKT = std::string(pszSRS_WKT);
					CPLFree( pszSRS_WKT );
					if (!sysProjWKT.empty())
					{
						if(verbose)	std::cout << "[GDALImageDriver::setGeoref] projWKT trouvee par conversion directe du code PROJ4 dans OGR : " << sysProjWKT << std::endl;
					}
					else
					{
						//en desespoir de cause on recupere directement sysProjSD
						//en esperant que ce soit du WKT
						if(verbose) std::cout << "[GDALImageDriver::setGeoref] ... en desespoir de cause on recupere directement sysProjSD et on espere que c'est du WKT..." << std::endl;
						if (sysProjSD != ign:: geodesy::ProjectionList::kStrLocal) sysProjWKT = sysProjSD;
					}
				}
			}
			catch (...)
			{
				//en desespoir de cause on recupere directement sysProjSD
				//en esperant que ce soit du WKT
				if(verbose) std::cout << "[GDALImageDriver::setGeoref] ... en desespoir de cause on recupere directement sysProjSD et on espere que c'est du WKT..." << std::endl;
				if (sysProjSD != ign:: geodesy::ProjectionList::kStrLocal) sysProjWKT = sysProjSD;
			}
			
		}
		if (sysProjWKT.empty())
		{
			if(verbose) std::cout << "[GDALImageDriver::setGeoref] ProjWKT vide, on lui attribue le code SD" << std::endl;
			if (sysProjSD != ign::geodesy::ProjectionList::kStrLocal) sysProjWKT = sysProjSD;
		}
				
		if (!sysProjWKT.empty())
		{
			if(verbose) std::cout << "[GDALImageDriver::setGeoref] SetGeoref -- Projection au format WKT : "<<sysProjWKT<<std::endl;
			_poDataset->SetProjection(sysProjWKT.c_str());
		}
		
		// added by RP
		switch(pixConv)
		{
			case ign::image::georef::PixConvAREA:
			{
				// default behavior (according to F. WARMERDAM)
				//_poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_AREA);
				break;
			}
			case ign::image::georef::PixConvPOINT:
			{
				// to be discussed : with ECW, we can afford to skip this action,
			    // otherwise an extra xml file wille be created
				if (verbose) std::cout<<"[GDALImageDriver::setGeoref] On fait un set METADATA AREAORPOINT : POINT"<<std::endl;
				
				if( sysProjWKT.empty() )IGN_THROW_EXCEPTION("[GDALImageDriver::setGeoref] code projection WKT vide, impossible d'affecter correctement la metadonnee AREA_OR_POINT = POINT");
				
			    CPLErr cple = _poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_POINT);
				
				if (cple == CE_None) 
				{
					if(verbose) std::cout<<"[GDALImageDriver::setGeoref] SUCCESS calling _poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_POINT) "<<std::endl;
				}
				
				if (cple == CE_Debug) 
				{
					if(verbose) std::cout<<"[GDALImageDriver::setGeoref] DEBUG calling _poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_POINT) "<<std::endl;
				}
				
				if (cple == CE_Warning) 
				{
					std::cout<<"[GDALImageDriver::setGeoref] WARNING calling _poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_POINT) "<<std::endl;
				}
				
				if (  (cple == CE_Failure)|| (cple == CE_Fatal))
				{
					std::cout<<"[GDALImageDriver::setGeoref] FAILURE/FATAL ERROR calling _poDataset->SetMetadataItem(GDALMD_AREA_OR_POINT, GDALMD_AOP_POINT)"<<std::endl;
				}
				break;
			}
			case ign::image::georef::PixConvNULL:
			{
				if(verbose) std::cout<<"[GDALImageDriver::setGeoref]  no convention written in image"<<std::endl;
				break;
			}
		}
		return true;
	}
	
	if (verbose) std::cout<<"<[GDALImageDriver::setGeoref] END"<<std::endl;
	return false;
}

///
///
///
bool GDALImageDriver::getColorTable(int bandNum, ColorTable &table)const
{
	bool verbose= false;
	
	// Le driver peut avoir ete ferme par le manager, on s'assure qu'il soit ouvert le cas echeant.
	ImageDriverManager::Instance()->awakeDriver((ImageDriver*)this);
	
	if (_poDataset)
	{
		GDALColorTable *colorTable = _poDataset->GetRasterBand(bandNum+1)->GetColorTable();
		if (colorTable)
		{
			int nbColor = colorTable->GetColorEntryCount();
			for(int i=0;i<nbColor;++i)
			{
				const GDALColorEntry* colorGDAL = colorTable->GetColorEntry(i);
				if (colorGDAL)
				{
					if (verbose) std::cout << "Couleur : "<<i<<" : "<<colorGDAL->c1<<" "<<colorGDAL->c2<<" "<<colorGDAL->c3<<" "<<colorGDAL->c4<<std::endl;
					ColorEntry color(colorGDAL->c1,colorGDAL->c2,colorGDAL->c3,colorGDAL->c4);
					table.setColor(i,color);
				}
			}
			return true;
		}
		else
		{
			if (verbose) std::cout << "no colorTable for band "<<bandNum<<std::endl;
		}
	}
	return false;
}

///
///
///
bool GDALImageDriver::setColorTable(int bandNum, ColorTable const & table)
{
	bool verbose = 0;
	if(verbose)std::cout<<" GDALImageDriver::setColorTable "<<std::endl;
    
    if ((_useCache)&&(!_needCopy))
        _needCopy=true;
    
	if (_poDataset)
	{
		if(verbose)std::cout<<" GDALImageDriver::setColorTable _poDataset ok"<<std::endl;
		size_t nbColor =table.numEntries();
		if(verbose)std::cout<<" num entries in table : "<<nbColor<<std::endl;
		if (nbColor != 0)
		{
			ign::unique_ptr<GDALColorTable> colorTable (new GDALColorTable(GPI_RGB));
            for(size_t i = 0 ; i < table.numEntries() ; i++)
			{
                ign::unique_ptr<GDALColorEntry> colorEntry (new GDALColorEntry());
				ColorEntry val;
				table.getColor(static_cast<int>(i),val);
				// c1 : gray, red, cyan or hue
				colorEntry->c1 = val.c1;
				// c2 : green, magenta, or lightness
				colorEntry->c2 = val.c2;
				// c3 : blue, yellow, or saturation
				colorEntry->c3 = val.c3;
				// alpha or blackband;
				colorEntry->c4 = val.c4;
				colorTable->SetColorEntry (static_cast<int>(i),colorEntry.get());
			}
			_poDataset->GetRasterBand(bandNum+1)->SetColorTable(colorTable.get());
			return true;
		}
		return false;
	}
	return false;
}

#endif //IGN_WITH_GDAL
