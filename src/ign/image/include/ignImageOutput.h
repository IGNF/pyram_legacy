#ifndef __IGN_IMAGE_IO_IMAGEOUTPUT_H__
#define __IGN_IMAGE_IO_IMAGEOUTPUT_H__

#include <ignImageIO.h>

namespace ign {
    namespace image{
			class ColorTable;
	}
}

namespace ign {
    namespace image{
		namespace io {
			class ImageOutput: public ImageIO
			{
				///\brief classe pour l'ecriture d'image 
				/// Cette classe permet d'ecrire par blocs, par ligne ou par pixel.
				/// Certains format (Jpeg, Png, ...) ne supporte pas, ou difficilement des ecritures non sequentielles: 
				/// pour ces formats on s'autorise a creer un cache en Tiff que l'on transforme dans le format desire lorsque l'objet est detruit.
				/// Pour ne pas utiliser de fichier cache il faut utiliser une ImageOutputLineByLine (non implementee pour le moment)
				
			public:
				
				///\brief Constructeur a partir d'un nom de fichier a creer et des caracteristique de l'image
				/// le driver sera automatiquement choisi en fonction de l'extension du nom de fichier
				///		\param fullPathName le chemin complet vers l'image
				///     \param nXSize le nombre de colonnes
				///		\param nYSize le nombre de lignes
				///		\param nNBands le nombre de canaux
				///		\param imageType type de pixel commun a toutes les canaux
				///		\param options un dictionnaire de parametres permettant de specifier des options du format (type de compression, organisation du fichier, ...)
				///		\param metadata un dictionnaire de meta donnees
                ImageOutput(std::string const &fullPathName, int nXSize, int NLines, int NBands, eTypePixel imageType, data::Object const *  options=NULL, data::Object const * metadata=NULL);

				///\brief Constructeur a partir d'un nom de fichier a creer et des caracteristique de l'image
				///		le driver sera automatiquement choisi en fonction de l'extension du nom de fichier
				///		Version permettant de specifier un type de pixel par canal
				///		\param fullPathName le chemin complet vers l'image
				///     \param nXSize le nombre de colonnes
				///     \param nYSize le nombre de lignes
				///     \param nNBands le nombre de canaux
				///		\param tabType un tableau de NBands type de pixel permettant de specifier un type par bande
				///		\param options un dictionnaire de parametres permettant de specifier des options du format (type de compression, organisation du fichier, ...)
				///		\param metadata un dictionnaire de meta donnees
                ImageOutput(std::string const &fullPathName, int nXSize, int nYSize, int nNBands, eTypePixel* tabType, data::Object const * options=NULL, data::Object const * metadata=NULL);
				
				///\brief Constructeur a partir d'un fichier existant. Cette methode utilise le ImageDriverManager pour trouver le driver capable de lire l'image.
				/// Rque: pour construire une nouvelle image, il faut utiliser la methode create du driver correspondant au format choisi.
				ImageOutput(std::string const &fullPathName);

				///\brief                
                virtual			~ImageOutput();
				///\brief 
				bool			isReadable()	const;
				///\brief 
				bool			isWritable()	const;
				///\brief 
				bool			isInRAM()		const;	
				///\brief 
				bool			isTiled() const;
				///\brief 
				void			open (const std::string &fullPathName);	
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
				void			write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void *buffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace, int nBandSpace);
				
				///\brief	ecriture d'une zone de l'image
				/// attention, suivant le type d'image, cette ecriture peut ne pas etre possible, dans ce cas une exception sera generee
				///		\param nXOff colonne du premier pixel ecrit
				///		\param nYOff ligne du premier pixel ecrit
				///		\param nBand0 indice du premier canal ecrit
				///		\param nXSize nombre de colonnes ecrites
				///		\param nYSize nombre de lignes ecrites
				///		\param nNBands nombre de canaux ecrits
				///		\param tabBuffer tableau pointeur vers la zone memoire a recopier (un pointeur par bande)
				///		\param eBufferType type de pixel
				///		\param nPixelSpace saut memoire dans le buffer entre deux pixels
				///		\param nLineSapce saut memoire dans le buffer entre deux lignes
				void			write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void **tabBuffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace);
				
				
				///\brief	Ecriture d'une ligne complete de l'image a plein resolution, sur tous les canaux, en entrelacee
				///\param L le numero de la ligne
				///\param buffer un buffer de (nombre de colonnes)*(nombre de canaux) pixels 
				void			writeLine(int L,void *buffer,eTypePixel bufferType);
				///\brief 
				void			writeLine(int L,unsigned char *buffer);
				///\brief 
				void			writeLine(int L,unsigned short *buffer);
				///\brief 
				void			writeLine(int L,signed short *buffer);
				///\brief 
				void			writeLine(int L,float *buffer);
				
				///\brief lecture d'un pixel
				/// cet acces simplifie aux images sera optimise a l'aide de cache pour eviter d'avoir a interroger physiquement le fichier pour chaque pixel
				void			setPixel (int colonne,int ligne, int band, double val); 
				
				///\brief	recuperation d'un pixel de l'image this -> throw exception
				double			get(int c, int l, int b) const;
				///\brief	mis a jour d'un pixel de l'image this
				void			set(int c, int l, int b, double val);
                				
				///\brief ecriture du georeferencement
				bool			setGeoref(double ULCx, double ULCy, double pasX, double pasY,std::string const &projectio,ign::image::georef::PixConv const & pixConv);
				
				///\brief ecriture du georeferencement
				///		\param adfGeoTransform[0] : top left x
				///		\param adfGeoTransform[1] : w-e pixel resolution 
				///		\param adfGeoTransform[2] : rotation, 0 if image is "north up" 
				///		\param adfGeoTransform[3] : top left y 
				///		\param adfGeoTransform[4] : rotation, 0 if image is "north up" 
				///		\param adfGeoTransform[5] : n-s pixel resolution 
				bool			setGeoref(double geoTransform[6],std::string const &sysProjSD,ign::image::georef::PixConv const & pixConv);
				
				///\brief ajout d'une table de couleur a un canal
				bool			setColorTable(int bandNum, ColorTable const & colorTable);
			};
		}
	}
}
#endif
