#ifndef __IGN_IMAGE_DETAIL_IMAGECACHE_H__
#define __IGN_IMAGE_DETAIL_IMAGECACHE_H__

#include <string>
#include <iostream>

#include <ignImage.h>

namespace ign
{
	namespace image
	{
		namespace detail
		{
			///\brief classe de gestion de cache memoire
			///
			///
			class ImageCache
			{
				
			public:	

				///\brief strategie de gestion du cache
				/// quand le cache est repositionne, il est place...
				/// - CacheStrategyCenteredBlok: au centre de la nouvelle zone chargee, de forme carree
				/// - CacheStrategyUpperLeftBlok; tel que le premier pixel charge se trouve au coin superieur gauche du cache 
				///		(eventuellement decale d'une marge parametrable), de forme carree	
				/// - CacheStrategyVStrip; tel que la premiere colonne chargee se trouve en haut du cache 
				///		(eventuellement decale d'une marge parametrable), avec un nombre de lignes identique a celui de l'imageIO
                /// - CacheStrategyHStrip; tel que la premiere ligne chargee se trouve en haut du cache
				///		(eventuellement decale d'une marge parametrable), avec un nombre de colonnes identique a celui de l'imageIO
				enum CacheStrategy
				{
					CacheStrategyCenteredBlok = 1,
					CacheStrategyUpperLeftBlok= 2,
					CacheStrategyVStrip = 3,
                    CacheStrategyHStrip = 4
				};
				
				///\brief constructeur
												ImageCache(const Image* img);
				///\brief destructeur virtuel 
				virtual							~ImageCache();
				
				///\brief
				void							setParam(size_t memorySize, CacheStrategy strategy, size_t margin);

				///\brief renvoie le cache avec sa position, le recharge si (col, lig) est en dehors
				/// cache est a NULL si cacheSize est a zero
				void							getCache(double col, double lig, Image*& cache, size_t& offX, size_t& offY)const;

				///\brief indique si le cache est charge
				bool							isValid() const;
				
				///\brief charge le cache a partir de _offsetX, _offsetY
				void							load() const;
				
				///\brief Lecture d'une zone de l'image dans un buffer a partir du cache memoire
				bool							read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType,
															   int nPixelSpace, int nLineSpace, int nBandSpace)const;

				///\brief renvoie la marge du cache
				size_t							margin();

				///\brief renvoie la taille du cache
				size_t							cacheSize();

				///\brief dimension du cache
				void							bufferSize(size_t &numLines, size_t &numCols, size_t &numBands);
				///\brief offsetX
				inline size_t					offsetX()const{return _offsetX;}
				///\brief offsetY
				inline size_t					offsetY()const{return _offsetY;}
				///\brief source
				inline const Image*				src()const{return _src;}
				///\brief buffer memoire
				inline const Image*				memoryBuffer()const{return _memoryBuffer.get();}
			protected:				
				///brief image source
				const Image*				   _src;

				///\brief taille memoire du cache
				size_t							_cacheSize;

				///\brief indique s'il faut recentrer le cache quand on le reinitialise
				/// ou si on on lefait demarrer au pixel a charger
				CacheStrategy					_cacheStrategy;				

				///\brief / si _centerCache est a false
				///charge le cache a partir de (position - marge) pour gerer l'interpolation
				size_t							_margin;

				///\brief rapport de taille minimal entre le cache et la zone a charger
				///si la zone a charger est trop grande par rapport au cache, on ne s'amuse pas a mettre
				//a jour le cache
				size_t							minCoefToLoadCache;
				
				///\brief offset du cache par rapport a l'image
				mutable size_t					_offsetX, _offsetY;			

				///\brief cache memoire
				mutable ign::unique_ptr<Image>	_memoryBuffer;

				///\brief booleen : true si le cache a ete lu (et pas seulement initialise)
				mutable bool					_isValid;
				
			};
		}
	}
}
#endif