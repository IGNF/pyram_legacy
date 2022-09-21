#ifndef __IGN_IMAGE_IO_DRIVER_IMAGEDRIVER__
#define __IGN_IMAGE_IO_DRIVER_IMAGEDRIVER__

#include <string>
#include <set>

#include <ignDataObject.h>
#include <ignImage.h>


namespace ign
{
    namespace image
	{
		class ColorTable;
		namespace io
		{
			namespace driver 
			{
				class ImageDriver
				{
					///\brief classe virtuelle a surcharger pour implementer les differents formats d'image
				public:
					///\brief constructeur par defaut
														ImageDriver();
					///\brief destructeur
					virtual								~ImageDriver();
					///\brief lecture d'un bloc
					virtual void						read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void *buffer, ::ign::image::eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)const=0;
					///\brief ecriture d'un bloc
					virtual void						write(int C0,int L0, int band0, int NBrows, int NLines, int NBands, void *buffer, ::ign::image::eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace);
					///\brief renvoie le georeferencement
					virtual bool						getGeoref(double geoTransfom[6], std::string &sysProjSD, ign::image::georef::PixConv & pixConv)const;
					///\brief met a jour le georeferencement
					virtual bool						setGeoref(double geoTransfom[6], std::string const &sysProjSD, ign::image::georef::PixConv const & pixConv);
					///\brief recuperation de la table de couleur
					virtual bool						getColorTable(int bandNum, ColorTable &table)const;
					///\brief mise a jour de la table de couleur
					virtual bool						setColorTable(int bandNum, ColorTable const &table);
					///\brief nouvelle instance de la classe
					virtual ImageDriver*				newInstance()const=0;
					
					///\brief Ouverture en lecture seule
					virtual bool						open(std::string const &fullPathName)=0;
					///\brief Ouverture en mise a jour
					virtual bool						update(std::string const &fullPathName);
					///\brief Ouverture en ecriture
					virtual bool						create(std::string const &fullPathName, int NRows, int NLines, int NBands, ::ign::image::eTypePixel* tabType, ::ign::data::Object const *  options,::ign::data::Object const *  metadata);
					
					///\brief indique si le driver peut etre ferme entre un open et sa destruction.
					virtual bool isClosable() const { return false; }
					///\brief Fermeture du driver.
					virtual void close();
					
					///\brief Methode de creation utilisee pour les formats ne pouvant etre ecrit que sequentiellement
					virtual bool						createCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn, ::ign::data::Object const *  options,::ign::data::Object const *  metadata)const;
					///\brief renvoie la taille de l'image
					virtual void						size(int &NRows,int &NLines)const=0;
					///\brief renvoie le nombre de colonnes
					size_t								numCols()const;
					///\brief renvoie le nombre de lignes
					size_t								numLines()const;
					///\brief renvoie le nombre de canaux
					virtual size_t						numBands()const=0;
					///\brief renvoie le type de l'image pour le canal @numBand
					virtual ::ign::image::eTypePixel	typePixel(int numBand)const=0;
					///\brief renvoie le nom du fichier en lecture / ecriture
					virtual std::string					getFileName()const=0;
					///\brief renvoie le nom du driver
					virtual std::string					getDriverName()const=0;
					///\brief renvoie les extensions gerees par le driver
					virtual std::set<std::string>		getExtensions()const=0;
					
					///\brief renvoie les metadonnees attachees a l'image (const)
					const ::ign::data::Object&			metaData()const;
					///\brief renvoie les metadonnees attachees a l'image 
					::ign::data::Object&				metaData();
					
                    
					ColorSpace							colorSpace()const {  return _colorSpace; }
					
				protected:
					///\brief les metadonnees attachees a l'image 
					data::Object						_metaData;
					ColorSpace							_colorSpace;
					
				};
			}
		}
	}
}

#endif
