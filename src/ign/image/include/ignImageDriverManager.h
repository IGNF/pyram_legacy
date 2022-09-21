#ifndef __IGN_IMAGE_IO_DRIVER_IMAGEDRIVERMANAGER_H__
#define __IGN_IMAGE_IO_DRIVER_IMAGEDRIVERMANAGER_H__

#ifndef __IDM_WITH_MUTEX__
#define __IDM_WITH_MUTEX__ 0
#endif

#include <vector>
#include <string>
#include <list>
#include <boost/thread/mutex.hpp>

#include <ignImageIO.h>
namespace ign {
    namespace image {
		namespace	io {
			namespace	driver {
				class ImageDriver;
			}
		}
	}
}

namespace ign {
    namespace image {
		namespace io {
			namespace driver {
				
				class ImageDriverManager
				{
					///\brief Classe Singleton qui connait l'ensemble des DriverImage disponibles
				public:
					
					///\brief Permet de recuperer l'instance unique de cette classe
					static ImageDriverManager*	Instance();
					
					///\brief Chargement de tous les drivers par defaut
					void						registerAll();
					
					///\brief Donne le nombre de Drivers
					size_t						numDrivers()const;
					
					///\brief Recuperer un Driver
					///\param i indice du driver
					ImageDriver*				getDriver(size_t i)const;
					
					////\brief  Recuperer un Driver a partir de son nom
					///\param driverName le nom du driver
					///\return retourne un pointeur NULL si aucun driver ne porte ce nom
					ImageDriver*				getDriver(std::string const &driverName)const;
					
					///\brief Inscrit un nouveau driver dans le manager 
					/// \param driver pointeur du driver a ajouter (le Manager prend en charge le pointeur)
					/// Attention, si un driver de meme nom est present, il sera remplacer
					void						registerDriver(ImageDriver* driver);
					
					///\brief  Desincription du driver d'indice i
					void						unregisterDriver(size_t i);
					
					///\brief Desinscription du driver a partir de son nom
					void						unregisterDriver(std::string const &nom);
					
					///\brief  Ouverture d'un fichier en lecture, par defaut, le comportement precedent
					ImageDriver*				open(std::string const &fullPathName, bool useExtension=false);
					
					///\brief Ouverture d'un fichier existant en ecriture
					ImageDriver*				update(std::string const &fullPathName)const;
					
					///\brief Creation d'un fichier
					/// on utilise le premier driver disponible capable de creer l'image
					ImageDriver*				create(std::string const &fullPathName, int NRows, int NLines, int NBands, image::eTypePixel *tabType, data::Object const *  options, data::Object const *  metadata)const;
					
					///\brief Methode de creation utilisee pour les formats ne pouvant etre ecrit que sequentiellement
					bool						createCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn, data::Object const *  options, data::Object const *  metadata)const;
					
					///\brief renvoie @_useExtension
					bool						useExtension()const;
					///\brief met a jour @_useExtension
					void						useExtension(bool b);
					
					bool						accept(const  ImageDriver*, const std::string filename, bool useExt)const;
					
					///\brief Lecture du nombre de drivers pouvant etre ouverts simultanement (en lecture - avec open).
					uint16_t					getMaxOpenDrivers() const;
					
					///\brief Modification du nombre de drivers pouvant etre ouverts simultanement (en lecture - avec open).
					void						setMaxOpenDrivers (uint16_t maxOpenDrivers);
					
					///\brief Verification qu'un driver reference par le manager est ouvert et reouverture si necessaire.
					void						awakeDriver(ImageDriver* driver);
					
					///\brief De-referencement d'un driver ouvert.
					void						detachDriver(ImageDriver* driver);
					
				protected:
					///\brief  Le constructeur n'est pas public pour garantir l'unicite de l'instance
					ImageDriverManager();					
				private:
					///\brief Pointeur de l'instance unique
					static ImageDriverManager*	_instance;
					
					///\brief tableau des drivers images
					std::vector<ImageDriver*>	_drivers;
					
					///\brief booleen indiquant si l'on doit gerer l'extension de fichier avant de tenter son ouverture avec un driver
					///ce booleen n'est utilise que si le parametre @useExtension est a false
					bool						_useExtension;

#if __IDM_WITH_MUTEX__
					///
					static bool					is_destructed;

					///
					static boost::mutex&		GetMutex()
					{
						static boost::mutex mutex;
						return mutex;
					}
					
					// Gestion de l'ouverture en lecture d'un nombre virtuellement illimite de drivers.
					
					boost::mutex				_openDriversMutex;
					
					///\brief liste des drivers actuellement ouverts parmi ceux references par le manager.
					std::list<ImageDriver*>		_openDrivers;
					
					///\brief nombre maximum de drivers actuellement ouverts parmis ceux references par le manager.
					uint16_t					_maxOpenDrivers;
					
					///\brief driver en cours de manipulation, sur lequel il ne faut pas faire d'awakeDriver.
					ImageDriver*				_skipAwakeForDriver;
					
					///\brief map des drivers momentanement fermes parmis ceux references par le manager (pointeur du driver et chemin du fichier a reouvrir).
					std::map<ImageDriver*,std::string> _suspendedDrivers;
					
					///\brief suspension des drivers ouverts les moins utilises.
					void						suspendLessUsedDrivers();
#endif
				};
			}
		}
	}
}


#endif
