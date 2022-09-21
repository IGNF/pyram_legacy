#ifndef __SDI_IMAGEIO_IMAGEINPUT_H__
#define __SDI_IMAGEIO_IMAGEINPUT_H__

#include <ignImageIO.h>
#include <ignImageCache.h>

namespace ign 
{
    namespace image 
	{
		class ColorTable;
		
		namespace io
		{
			class ImageInput: public ImageIO
			{				
			public:
				///\brief Constructeur par defaut
												ImageInput();
				///\brief Destructeur
				virtual							~ImageInput();
				
				///\brief Constructeur de copie
												ImageInput(ImageInput const &src, bool const &useExtension=false);
				
				///\brief Constructeur a partir d'un nom de fichier existant. 
				///Cette methode utilise le ImageDriverManager pour trouver le driver capable de lire l'image
												ImageInput(std::string const &fullPathName, bool const &useExtension=false);
				///\brief
                void							open(std::string const &fullPathName);
                ///\brief
				void							initialize(std::string const &fullPathName, bool const &useExtension=false);
				///\brief
				bool							isReadable()const;
				///\brief
				bool							isWritable()const;
				///\brief
				bool							isInRAM()	const;
				///\brief 
				bool							isTiled() const;
					
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
				virtual void					read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType,
													 int nPixelSpace, int nLineSpace, int nBandSpace)const;
				
				///\brief	Lecture d'une zone de l'image dans un buffer 
				///		\param nXOff colonne du premier pixel lu
				///		\param nYOff ligne du premier pixel lu
				///		\param nBand0
				///		\param nXSize nombre de colonne lues (a pleine resolution)
				///		\param nYSize nombre de lignes lues (a pleine resolution)
				///		\param nNBands nombre de canaux
				///		\param nDeZoom pas sous echantillonnage
				///		\param tabBuffer tableau pointeur vers la zone memoire a ecrire (un pointeur par bande)
				///		\param eBufferType type de pixel
				///		\param nPixelSpace saut memoire dans le buffer entre deux pixels
				///		\param nLineSpace saut memoire dans le buffer entre deux lignes	
				virtual  void					read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void **tabBuffer,
													 eTypePixel eBufferType,int nPixelSpace, int nLineSpace)const;
				
				///\brief	Lecture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///		\param L le numero de la ligne
				///		\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				///		\param bufferType type de pixel
				virtual void					readLine(int L,void *buffer,eTypePixel bufferType)const;
				
				///\brief	Lecture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///		\param L le numero de la ligne
				///		\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				void							readLine(int L,unsigned char *buffer)const;
				
				///\brief	Lecture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///		\param L le numero de la ligne
				///		\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				void							readLine(int L,unsigned short *buffer)const;
				
				///\brief	Lecture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///		\param L le numero de la ligne
				///		\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				void							readLine(int L,short *buffer)const;
				
				///\brief	Lecture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///		\param L le numero de la ligne
				///		\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				void							readLine(int L,float *buffer)const;
				
				///\brief	lecture d'un pixel
				///		cet acces simplifie aux images sera optimise a l'aide de cache pour eviter d'avoir a interroger physiquement le fichier pour chaque pixel
				virtual double					operator () (int colonne,int ligne,int canal)const;
				
				///\brief	recuperation d'un pixel de l'image this
				// @warning: attention il y a un probleme avec le transtypage en JP2000
				double							get(int c, int l, int b) const;
				///\brief	mis a jour d'un pixel de l'image this -> throw exception
				void							set(int c, int l, int b, double val);
				
				///\brief	recupere une table des couleurs pour un canal
				bool							getColorTable(int bandNum, ColorTable &colorTable)const;
				
				///\brief renvoie le cache memoire
				detail::ImageCache&				memoryCache();
				///\brief renvoie le cache memoire (CONST)
				const detail::ImageCache&		memoryCache()const;
				
			private:
				///\brief memory cache
				detail::ImageCache				_memoryCache;
			};
		}
	}
}

#endif
