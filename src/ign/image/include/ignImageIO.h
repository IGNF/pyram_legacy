/*
 *  ImageIO.h
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */
#ifndef __SDI_IMAGEIO_H__
#define __SDI_IMAGEIO_H__

#include <string>
#include <complex>
#include <iostream>

#include <ignImage.h>
#include <ignImageDriver.h>


namespace ign{
    namespace image{
		namespace io {
			class ImageIO: public  Image{
				///\brief classe d'acces aux images sans se poser de question de format.
				/// Cette classe a deux filles: ImageInput (pour les acces en lecture) et ImageOutput (pour les acces en ecriture).
				/// A priori on ne prevoit pas la possibilite de faire simultanement des lectures et des ecritures sur le même fichier.
				///
				
			public:	
				///\brief constructeur par defaut
											ImageIO();
				
				///\brief destructeur virtuel 
				virtual						~ImageIO();
                
                ColorSpace colorSpace()const;

///METHODS HERITEES DE IMAGE				
				///\brief Taille de l'image
				ImageSize					size(int aDeZoom = 1)const;
				///\brief indique si l'image est valide ou non
				bool						isValid()const;
				///\brief Type de pixel
				eTypePixel					typePixel(int numBand=0)const;
				///\brief nombre de canaux
				size_t						numBands()const;
				///\brief renvoie les metadonnees attachees a l'image (const)
				const data::Object*			metaData()const;
				///\brief renvoie les metadonnees attachees a l'image 
				data::Object*				metaData();	
				///\brief ferme le flux
				void						close();
				///\brief ouvre le flux (en mode update si ImageOutput)

				virtual void				open(std::string const &fullPathName);

///METHODS PROPRES A LA CLASSE					

				///\brief Le nom du fichier associe
				std::string					getFileName()const;

				///\brief Le nom du driver associe
				std::string					getDriverName()const;
				
				//\brief verifie que le driver est bien initialise
				/// virtual est necessaire pour gerer correctement la classe MultiImageInput
				virtual bool				hasDriver() const;
				///\brief recupere les metadonnees de georeferencement. 
				/// renvoie false si inexistantes
				bool						getGeoref(double &ULCx, double &ULCy, double &pasX, double &pasY,std::string &sysProjWKT, ign::image::georef::PixConv & pixConv)const;
				///\brief recupere les metadonnees de georeferencement. 
				/// renvoie false si inexistantes
				bool						getGeoref(double geoTransform[6],std::string &sysProjWKT, ign::image::georef::PixConv & pixConv)const;
				
				static bool					CreateCopy(std::string const &fullPathNameOut, std::string const &fullPathNameIn);

			protected:
				///\brief le driver de lecture/ecriture associe a l'image
				driver::ImageDriver*		_driver;
			};
		}
	}
}
	
#endif
	
