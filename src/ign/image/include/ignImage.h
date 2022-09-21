/*
 *  ImageIO.h
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */
#ifndef __IGN_IMAGE_IMAGE_H__
#define __IGN_IMAGE_IMAGE_H__

#include <string>
#include <iostream>

#include <boost/noncopyable.hpp>

#include <ignCommon.h>
#include <ignDataObject.h>
#include <ignImageType.h>

namespace ign
{
	namespace image
	{
        class ColorTable;
        
		///\brief classe virtuelle d'acces aux images
		///Cette classe a deux filles: ImageIO (pour les acces aux fichiers) et BufferImage (pour les acces en memoire).
		class Image : private boost::noncopyable {
		public:	
			///\brief constructeur par defaut
												Image();
			///\brief destructeur virtuel 
			virtual								~Image();
			
			///\brief Taille de l'image
			virtual ImageSize					size(int aDeZoom = 1)const = 0;
			
			///\brief nombre de canaux
			virtual size_t						numBands()const = 0;
			///\brief nombre de colonnes
			size_t								numCols()const;
			///\brief nombre de lignes
			size_t								numLines()const;
			
			///\brief indique si le pixel est dans l'image
			bool								isIn(int c, int l, int b)const;
			
            virtual ColorSpace colorSpace()const;
            
            ///\brief	recupere une table des couleurs pour un canal
            virtual bool						getColorTable(int bandNum, ColorTable &colorTable)const;
            
			///\brief Type de pixel
			virtual eTypePixel					typePixel(int numBand=0)const = 0;
			///\brief indique si l'image est valide ou non
			virtual bool						isValid()const = 0;
			
			///\brief renvoie les metadonnees attachees a l'image (const)
			virtual const ::ign::data::Object*	metaData()const;
			///\brief renvoie les metadonnees attachees a l'image 
			virtual ::ign::data::Object*        metaData();	
			
			/// \brief verification du type d'une classe  derivee (const)
			template < typename T >
			bool								is() const 
			{	
				return (typeid(*this)==typeid(T));
			}
			/// \brief down cast sur une classe  derivee (const)
			template < typename T >
			T const &							as() const 
			{
				T const * cast = dynamic_cast< T const * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[Image::as] bad cast");	}
				return *cast;
			}
			/// \brief down cast sur une classe  derivee
			template < typename T >
			T  &								as()  
			{	
				T  * cast = dynamic_cast< T * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[Image::as] bad cast");	}
				return *cast;
			}
			
			///\brief	indique si ce type d'image est readable
			virtual bool						isReadable() const = 0;
			///\brief	indique si ce type d'image est writable
			virtual bool						isWritable() const = 0;	
			///\brief	indique si ce type d'image est en memoire ou non
			virtual bool						isInRAM() const = 0;
			///\brief	indique si ce type d'image est geree sous forme de dalle (MultiImage)
			virtual bool						isTiled() const = 0;

			///\brief	pour une image en I/O, ferme le flux, pour une image en RAM, do nothing
			virtual void						close();
			///\brief	lecture dans le buffer en parametre d'une portion de l'image this (si image readable)
			virtual void						read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType, int nPixelSpace, int nLineSpace, int nBandSpace)const;			
			///\brief	ecriture dans un buffer (si image writable)
			virtual void						write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void *buffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace, int nBandSpace);			
			
			///\brief	recopie d'une portion de l'image @param src dans this
			///		\param nXOff offset en colonne dans this
			///		\param nYOff offset en ligne dans this
			///		\param nBandOff offset en bande dans this
			///		\param nXSize nombre de colonnes a coller dans this
			///		\param nYSize nombre de lignes a coller dans this
			///		\param nNBands nombre de bandes a coller dans this
			///		\param src Image source des pixels a coller dans this
			///		\param nXSrc offset en colonne dans src
			///		\param nYSrc offset en ligne dans src
			///		\param nBandSrc offset en bande dans src
			///		\warning: la taille du crop doit etre coherente avec la taille des images, sinon, renvoie assert(false) 
			bool								paste(int nXOff, int nYOff, int nBandOff, int nXSize, int nYSize, int nNBands, const Image* src, int nXSrc, int nYSrc, int nBandSrc);
			
			///\brief	recuperation d'une portion de l'image this
			///		\param nXOff offset en colonne dans this
			///		\param nYOff offset en ligne dans this
			///		\param nBandOff offset en bande dans this
			///		\param nXSize nombre de colonnes a copier dans this
			///		\param nYSize nombre de lignes a copier dans this
			///		\param nNBands nombre de bandes a copier dans this
			///		\param dest Image de destination des pixels copiees dans this
			///		\param nXOff offset en colonne dans dest
			///		\param nYOff offset en ligne dans dest
			///		\param nBandOff offset en bande dans dest
			///		\warning: la taille du crop doit etre coherente avec la taille des images, sinon, renvoie assert(false) 
			bool								crop(int nXOff, int nYOff, int nBandOff, int nXSize, int nYSize, int nNBands, Image* dest, int nXDest, int nYDest, int nBandDest) const;

			///\brief	recuperation d'un pixel de l'image this
			virtual double						get(int c, int l, int b) const = 0;
			///\brief	mis a jour d'un pixel de l'image this
			virtual void						set(int c, int l, int b, double val) = 0;

			/// \brief get the size of the image when loaded in RAM
			size_t                              memorySize() const;
			
			
			///\brief met a jour la valeur par defaut
			void								defaultValue(double d);
			
			///\brief renvoie la valeur par defaut
			double								defaultValue() const;
		protected:
			
			///\brief valeur par defaut (utilisee dans les crops partiels)
			double								_defaultValue;

			
		};
    }
}
namespace ign
{
	namespace image
	{
		namespace georef
		{
			typedef enum {PixConvNULL=0, PixConvPOINT, PixConvAREA} PixConv;

			std::string			PixConventionToString(ign::image::georef::PixConv pixconv);
			
			/****
			 
			 |x|		|x0|	|step.first		rot.first  |	|u|
					=		  +
			 |y|		|y0|	|rot.second		step.second|	|v|
			 
			 ****/
			struct Data
			{
				Data(){;}
				Data(double x0, double y0, double dx, double dy, std::string proj, PixConv pixConv)
				{
					_origin = std::make_pair(x0, y0);
					_step = std::make_pair(dx, dy);
                    _rot = std::make_pair(0., 0.);
					_projection = proj;
					_pixConvention = pixConv;
				}
                Data(double geoTransform[6], std::string proj, PixConv pixConv)
				{
					_origin = std::make_pair(geoTransform[0], geoTransform[3]);
					_step = std::make_pair(geoTransform[1], geoTransform[5]);
                    _rot = std::make_pair(geoTransform[2], geoTransform[4]);
					_projection = proj;
					_pixConvention = pixConv;
					
				}
				std::pair<double, double>	_origin;
				std::pair<double, double>	_step;
                std::pair<double, double>   _rot;
				std::string					_projection;
				PixConv                     _pixConvention;
			};
			
			const std::string kStrGeorefX0			= "GRF_X0";
			const std::string kStrGeorefY0			= "GRF_Y0";
			const std::string kStrGeorefDX			= "GRF_DX";
			const std::string kStrGeorefDY			= "GRF_DY";
            const std::string kStrGeorefRX			= "GRF_RX";
			const std::string kStrGeorefRY			= "GRF_RY";
			const std::string kStrGeorefProjection	= "GRF_PROJ";
			const std::string kStrGeorefPixConvention	= "GRF_PIX_CONV";
			
			///\brief regarde si le dictionnaire contient les metadonnees necessaires a son georeferencement
			bool IsGeoref(const data::Object& mtd);
			///\brief indique si la clef correspond a un tag des mtd de georeferencement
			bool IsTagGeoref(const std::string& key);
			///\brief recupere les metadonnees de georeferencement, renvoie une exception si une clef est manquante
			bool GetGeoref(const data::Object& mtd, Data& grf);
			///\brief met a jour Object avec les metadonnees de georeferencement
			///@warning: la projection doit etre mise en convention SD. Pour le georeferencement dans l'entete d'images
			///via GDAL, si le code WKT n'existe pas dans ProjectionList, on peut le passer directement a la place du code SD 
			void SetGeoref(const Data& grf, data::Object& mtd);
		}
	}
}
#endif

