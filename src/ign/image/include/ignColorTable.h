#ifndef __SDI_IMAGEIO_COLORTABLE_H__
#define __SDI_IMAGEIO_COLORTABLE_H__

#include <map>

namespace ign 
{
    namespace image
	{
		
		class ColorEntry
		{
			///\brief Classe pour la gestion des couleurs dans les tables
			/// Classe calquee sur GDALColorEntry
		public:
			///\brief constructeur par defaut
						ColorEntry();
			///\brief destructeur
			virtual		~ColorEntry();
			///\brief constructeur
						ColorEntry(short aC1,short aC2, short aC3, short aC4);
		public:
			short	c1;
			short	c2;
			short	c3;
			short	c4;
		};
		
		
		/** Classe pour la gestion des tables de couleurs
		 */
		class ColorTable
		{
		public:
			///\brief constructeur par defaut
			ColorTable();
			///\brief destructeur
			virtual									~ColorTable();
			///\brief renvoie le nombre d'entree dans la table
			size_t									numEntries() const;
			///\brief renvoie la Neme entree dans la table
			std::pair<const int, ColorEntry>&		entryN(int n);
			///\brief renvoie la Neme entree dans la table (const)
			std::pair<const int, ColorEntry>const&	entryN(int n)const;
			///\brief recupere dans 'val' la valeur de la Neme entree dans la table
			void									getColor(int index,ColorEntry &val)const;
			///\brief met a jour la Neme entree dans la table avec la couleur 'val'
			void									setColor(int index, ColorEntry const &val);
			
		private:
			///\brief table des couleurs
			std::map<int, ColorEntry>				_table;
		};
		
	}
}

#endif
