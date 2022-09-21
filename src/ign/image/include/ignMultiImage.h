#ifndef __IGN_IMAGE_MULTIIMAGE_H__
#define __IGN_IMAGE_MULTIIMAGE_H__
// created by RP

#define _IGN_AUTO_PTR_ 0

#include <string>
#include <complex>
#include <vector>
#include <iostream>
#include <map>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include <ignImage.h>
#include <ignImageCache.h>

namespace ign 
	{
    namespace image	
		{
		
			class MultiImage: public Image
		{			
			///\brief classe pour la gestion d'une liste d'image dallee
			/// Elle permet de realiser des operations d'une taille donnee sur une collection
			/// d'objets Image, de facon transparente pour l'utilisateur 
			///(principe d'une image virtuelle)
		public:
			// constructeur par defaut
													MultiImage();
			
			// destructeur 
			virtual									~MultiImage();
			
			///\brief taille de l'image
			ImageSize								size(int aDezoom = 1)const;
			
			/** Nombre de canaux*/
			size_t									numBands()const;
			
			/** Type de pixel*/
			eTypePixel								typePixel(int numBand=0)const;
			
			/// \brief ajout d'une image de @param nBands, a partir de la @param band0 dans le MultiImage
			/// la tile est pris en charge par la classe
			void									addTile(const ign::shared_ptr<Image>& tile,  std::pair<int, int> const & position, size_t band0, size_t nBands, std::string tileName);
			/// \brief ajout d'une image a partir de ses parametres sans verification de la validite de l'image
			void									addTileNoCheck(const ign::shared_ptr<Image>& tile, ImageSize const & size, std::pair<int, int> const & position, ign::image::eTypePixel typePixel, size_t band0, size_t nBands, std::string tileName);
			
			///\brief met a jour le type de l'image
			void									setTypePixel(const eTypePixel &t, int numBand = 0);

			///\brief met a jour le nombre de canaux
			void									setNumBands(int nb);
			
			/// \brief translation de (x, y) des positions des objets du _data.vLoader
			void									translate(int x, int y);
			
			/// \brief renvoie l'origine de la MultiImage (ie upperleft corner des tuiles)
			std::pair<int, int>						origin()const;
			
			/// \brief nettoyage .. (to-do: a utiliser avec des boost::ptr_container?)
			void									clear();
			
			/// \brief nombre de tuiles dans le multiImageInput
			size_t									numTiles()const;
			
			/// \brief renvoie true si le crop intersecte au moins une tuile**/
			bool									isIn(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands) const;

			///\brief verifie que l'image a un driver
			bool									hasDriver() const;
			
			bool									isValid() const;
			
			void									close();
			
			///\brief	indique si ce type d'image est readable
			bool									isReadable() const;

			///\brief	indique si ce type d'image est writable
			bool									isWritable() const;	

			///\brief	indique si ce type d'image est en memoire ou non
			bool									isInRAM() const;

			///\brief 
			bool									isTiled() const;
			
			
			/** Lecture d'une zone de l'image dans un buffer 
			 \param nXOff colonne du premier pixel lu
			 \param nYOff ligne du premier pixel lu
			 \param nBand0
			 \param nXSize nombre de colonne lues (a pleine resolution)
			 \param nYSize nombre de lignes lues (a pleine resolution)
			 \param nNBands nombre de canaux
			 \param nDeZoom pas sous echantillonnage
			 \param buffer pointeur vers la zone memoire a ecrire
			 \param eBufferType type de pixel
			 \param nPixelSpace deplacement dans le buffer entre deux pixels (typiquement NBands pour une image entrelacee RVBRVB...)
			 \param nLineSpace saut memoire dans le buffer entre deux lignes (typiquement NBands*NRows pour une image entrelacee RVBRVB...)
			 \param nBandSpace saut memoire dans le buffer entre deux canaux (typiquement 1 pour une image entrelacee RVBRVB...)
			 
			 */
			void									read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom,
														 void *buffer, eTypePixel eBufferType,
														 int nPixelSpace, int nLineSpace, int nBandSpace)const;
			
			///\brief	ecriture dans un buffer (si image writable)
			void									write(	int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, 
															void *buffer, eTypePixel eBufferType,
															int nPixelSpace, int nLineSpace, int nBandSpace);			
			
			///\brief	recuperation d'un pixel de l'image this
			double									get(int c, int l, int b) const;

			///\brief	mis a jour d'un pixel de l'image this
			void									set(int c, int l, int b, double val);
			
            size_t									maxOpenTiles()const;            
            
            void									maxOpenTiles(size_t max); 

			/// brief
			const ign::shared_ptr<Image>&           imageN(size_t const & n) const;

			/// brief version non const
			ign::shared_ptr<Image>& 				imageN(size_t const & n); 
			
			/// \brief renvoie le numero de la premiere dalle sous le point; -1 sinon
			int										numTile(const double &col, const double& lig,  size_t canal) const;
			
			/// \brief 
			const std::pair<int, int>				getTilePosition(size_t const & i) const;

			/// \brief 
			const std::pair<size_t, size_t>			getTileSize(size_t const & i) const;

			/// \brief renvoie le nom de dalle i 
			const std::string						getTileName(size_t const & i) const;

			/// \brief renvoie le nom de dalle i 
			std::string								getTileName(size_t const & i);
			
			///\brief renvoie le cache memoire
			detail::ImageCache&						memoryCache();

			///\brief renvoie le cache memoire (CONST)
			const detail::ImageCache&				memoryCache()const;

			///\brief met a jour le booleen indiquant si les methodes get renvoient une exception ou une valeur par defaut
			void									throwExc(bool thr);

			///\brief renvoie le booleen indiquant si les methodes get renvoient une exception ou une valeur par defaut
			bool									throwExc() const;

			///\brief met a jour la verbosite
			void									verbose(int d);

			///\brief renvoie la verbosite
			int										verbose() const;	
			
		protected :
			
			///\brief memory cache
			detail::ImageCache						_memoryCache;
			
			///\brief type pixel
			eTypePixel								_typePixel; 
			
			///\brief number of bands
			size_t									_numBands;
			
			///\brief	nombre max. de dalles ouvertes, si atteint, on ferme les drivers des tuiles et on ne les rouvrent que pour faire du read/write				
			size_t									_maxOpenTiles;
			
			///\brief	nombre courant de dalles ouvertes, si atteint, on ferme toutes les tuiles et 
			////on ne les rouvrent au fur et a mesure que pour faire du read/write jusqu'a ce que _maxTiles soit atteint	
			mutable size_t							_numOpenTiles;

			/// \brief renvoie true si la dalle numTile est bien sous le point dans ce canal
			bool									_isInTile(size_t numTile, const double &xIma, const double& yIma,  size_t canal) const;
			
			/// \brief renvoie true si la dalle numTile est bien sous le crop dans ce canal**/
			bool									_isInTile(size_t numTile, const double &xIma, const double& yIma, const ImageSize& sz, size_t canal) const;
						
			/// \brief 
			void									_box(size_t i, std::pair<int, int> &position, std::pair<size_t, size_t>& taille) const;
			
			/// \brief
			std::string								_name() const {return "MultiImage";}

			/// \brief
			ign::data::Object          				_metadata;
			
			///\brief
			bool									_throwExc;
			
			
			///\brief
			int										_verbose;

            ///
			///
			///
			///\brief small structure describing an element in the MultiImage
			struct TileData
			{
				//TileData();
				/// \brief constructeur par recopie
				TileData(TileData const & other);
				/// \brief constructeur
				TileData(const ign::shared_ptr<Image>& img,std::pair<int, int> const& position, const ImageSize& sz, size_t band0, size_t nBands, std::string name="");
				
				/// \brief destructeur par defaut
				virtual						~TileData();
				
				/// \brief le ImageInput a croper
				ign::shared_ptr<Image>	_image;
				///\brief origine de la tuile dans la MultiImage
				std::pair<int, int>			_position;
				///\brief de la tuile (cache sur _image->size(1) ) pour eviter d'ouvrir les images inutilement
				ImageSize					_size;
				
				///\brief numero de la premiere bande a charger 
				size_t						_band0;
				///\brief nombre de bandes a charger 
				size_t						_nBands;
				
				/// \brief Pour preserver la memoire des noms de fichier
				std::string					_name;
				
			};
			
			typedef	ign::shared_ptr<TileData>			TileDataPtr;
			
			/// \brief  the map containing the descriptions of the elements in the MultiImageInput
			std::map<size_t, TileDataPtr>				_data;
			
			///\brief si _numOpenTiles depasse _maxOpenTiles, ferme tous les fichiers sauf le fichier num 
			void										_checkClose(const TileDataPtr& tile) const;
			///\brief si l'image num n'est pas ouverte, l'ouvre / indifferent pour un BufferImage
			void										_openTile(TileDataPtr& tile) const;
		};
	}
}
#endif
