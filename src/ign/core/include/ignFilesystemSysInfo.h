#ifndef _IGN_FILESYSTEM_SYSTEMINFO_H_
#define _IGN_FILESYSTEM_SYSTEMINFO_H_

#include <ignConfig.h>
#include <boost/filesystem.hpp>


//to-do recuperer l'heure courante, la place sur le disque, le nombre de processeurs...
namespace ign
{
	namespace filesystem
	{
		/**
		 * \brief classe renvoyant des informations sur un disque
		 */
		class IGN_FILESYSTEM_API SystemInfo
		{
			///\brief: encapsulation de boost
		public:
			SystemInfo(const boost::filesystem::path &mypath);
			virtual				~SystemInfo();
			
			///\brief renvoie l'espace disque disponible en byte
			boost::uintmax_t	getFreeSpace() const;
			
			///\brief renvoie la capacite de l'espace disque
			boost::uintmax_t	getCapacity() const;
			
			///\brief renvoie le pourcentage [0; 100] de l'espace disque disponible
			double				getFreeSpaceRatio() const;

			///\brief renvoie la RAM disponible
			static size_t		GetPhysicalAvailableMemory();

			///\brief renvoie le nombre de CPU
			static size_t		GetCPUNumber();
			
		protected: 
			boost::filesystem::space_info _space_info;
		};
	
	
	}
	
	
	
}
#endif
	
	
