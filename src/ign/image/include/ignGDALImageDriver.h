#ifndef __IGN_IMAGE_IO_DRIVER_GDALIMAGEDRIVER__
#define __IGN_IMAGE_IO_DRIVER_GDALIMAGEDRIVER__

#include <ignConfig.h>

//#ifdef IGN_WITH_GDAL
#if 1

#include <string> 
#include <ignDataObject.h>
#include <ignImageDriver.h>

class GDALDataset;
class GDALDriver;


namespace ign {
    namespace image{
		namespace io {
			namespace driver{
				/** Utilisation de la lib GDAL
				 * Pour le moment seul les formats standards sont compiles (donc sans Kakadu et ECW en particulier).
				 * Pour la compilation sous Windows, attention pour que la lib fonctionne avec un exe en debug, 
				 * il faut compiler la lib en debug apres avoir editer le fichier nmake.opt pour remplacer /MD par /MDd 
				 * dans la config debug
				 */
				class GDALImageDriver:public ImageDriver
				{
				public:
					///\brief constructeur
											GDALImageDriver();
					///\brief destructeur
					virtual					~GDALImageDriver();
					
					static std::string		DriverName();

					///\brief renvoie une nouvelle instance du driver avec le constructeur par defaut
					ImageDriver*			newInstance()const;
					
					///\brief Ouverture en lecture d'un fichier existant.
					/// Teste sur le TIF
					bool					open(std::string const &fullPathName);
					
					///\brief  Ouverture d'un fichier existant pour modification 
					bool					update(std::string const &fullPathName);
					
					///\brief Creation d'une nouvelle image
					///\warning GDAL ne supporte pas la creation d'une image avec des types differents par canaux
					bool					create(std::string const &fullPathName, int NRows, int NLines, int NBands, eTypePixel* tabType, data::Object const *  options,ign::data::Object const *  metadata);
					
					///\brief Creation par recopie d'une image
					bool					createCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn, data::Object const *  options,ign::data::Object const *  metadata)const;
					
					///\brief Indique si le driver peut etre ferme entre un open et sa destruction.
					bool					isClosable() const { return true; }
					
					///\brief Fermeture.
					void					close();
					
					///\brief renvoie la taille de l'image
					void					size(int &NRows,int &NLines)const;
					///\brief renvoie le nombre de canaux de l'image
					size_t					numBands()const;
					///\brief renvoie le type numerique de l'image
					eTypePixel				typePixel(int numBand)const;
					///\brief renvoie le nom du fichier a ouvrir
					std::string				getFileName()const;
					///\brief renvoie le nom du driver
					std::string				getDriverName()const;
					///\brief renvoie les extensions gerees par le driver
                    std::set<std::string>	getExtensions()const;
					
					///\brief Lecture d'une zone de l'image dans un buffer 
					///		\param nXOff colonne du premier pixel lu
					///		\param nYOff ligne du premier pixel lu
					///		\param nBand0
					///		\param nXSize nombre de colonne lues (a pleine resolution)
					///		\param nYSize nombre de lignes lues (a pleine resolution)
					///		\param nNBands nombre de canaux
					///		\param nDeZoom pas sous echantillonnage
					///		\param buffer pointeur vers la zone memoire a ecrire
					///		\param eBufferType type de pixel
					///		\param nPixelSpace deplacement dans le buffer entre deux pixels (typiquement NBands pour une image entrelacee RVBRVB...)
					///		\param nLineSpace saut memoire dans le buffer entre deux lignes (typiquement NBands*NRows pour une image entrelacee RVBRVB...)
					///		\param nBandSpace saut memoire dans le buffer entre deux canaux (typiquement 1 pour une image entrelacee RVBRVB...)					
					void			read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, 
										 int deZoom, void *buffer, eTypePixel bufferType,int nPixelSpace, 
										 int nLineSpace, int nBandSpace)const;
					
					///\brief ecriture d'une zone de l'image dans le fichier
					/// attention, suivant le type d'image, cette ecriture peut ne pas etre possible, dans ce cas une exception sera generee
					///		\param nXOff colonne du premier pixel ecrit
					///		\param nYOff ligne du premier pixel ecrit
					///		\param nBand0 indice du premier canal ecrit
					///		\param nXSize nombre de colonnes ecrites
					///		\param nYSize nombre de lignes ecrites
					///		\param nNBands nombre de canaux ecrits
					///		\param Buffer pointeur vers la zone memoire a recopier (un pointeur par bande)
					///		\param eBufferType type de pixel
					///		\param nPixelSpace saut memoire dans le buffer entre deux pixels
					///		\param nLineSapce saut memoire dans le buffer entre deux lignes
					///		\param nBandSpace saut memoire dans le buffer entre deux canaux
					void			write(int C0, int L0, int Band0, int NRows, int NLines, 
										  int NBands, void *buffer, eTypePixel bufferType,
										  int nPixelSpace, int nLineSpace, int nBandSpace);
					
                    
					///\brief lit le georeferencement de l'entete de l'image si elle existe
					///@warning recupere desormais une chaine SD ou proj4 si conversion via ProjectionList impossible
					//(les conversions OGR ne gerent pas les geoidgrids donc code proj4 eventuellement incomplet)
					bool			getGeoref(double geoTransform[6],std::string &sysProjSD, ign::image::georef::PixConv & pixConv)const;
					///\brief ecrit le georeferencement dans l'entete de l'image
					///@warning: la projection doit etre mise en convention SD. Pour le georeferencement dans l'entete d'images
					///via GDAL, si le code WKT n'existe pas dans ProjectionList, on peut le passer directement a la place du code SD 
					bool			setGeoref(double geoTransform[6] ,std::string const &sysProjSD, ign::image::georef::PixConv const &pixConv);
                    
                    
					///\brief renvoie la table de couleur
					bool			getColorTable(int bandNum, ColorTable &table)const;
					///\brief met a jour la table de couleur
					bool			setColorTable(int bandNum, ColorTable const & table);

					/// \brief
					static void		SetConfigurationOption(ign::data::Object const & configOpt);
                    /// \brief
					static ign::data::Object	GetConfigurationOption();
                    

					static			void SetNullErrorHandler();
					static void		SetOwnErrorHandler();

				private:

					static std::string	GetDriverName(std::string const &fullPathName);
					static ign::data::Object	_ConfigOptions;	

					GDALDataset*		_poDataset;
					std::string			_filename;
					bool				_needCopy;
                    bool                _useCache;
					std::string			_nomCache;
					GDALDriver*			_driverCache;
					ign::data::Object	_options;	

					static bool			_GDALRegistered;

				};
			}
		}
	}
}	

#endif //IGN_WITH_GDAL
		
#endif
