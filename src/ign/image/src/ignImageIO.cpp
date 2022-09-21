/*
 *  ImageIO.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */

#include <iostream>

#include <ignException.h>

#include <ignImageIO.h>
#include <ignImageDriver.h>
#include <ignImageDriverManager.h>

namespace ign 
{
	namespace image
	{
		namespace io 
		{
			///
			///
			///
			ImageIO::ImageIO():Image(), _driver(NULL)
			{
			}

			///
			///
			///
			ImageSize ImageIO::size(int aDeZoom)const
			{
				int NRows=0,NLines=0;
				if (hasDriver()) _driver->size(NRows,NLines);
				return std::make_pair((size_t)(NRows/aDeZoom),(size_t)(NLines/aDeZoom));
			}
			
			///
			///
			///
			size_t ImageIO::numBands()const
			{
				if (hasDriver()) return _driver->numBands();
				return 0;
			}
            
            
            ColorSpace ImageIO::colorSpace()const
            {
                if (hasDriver()) return _driver->colorSpace();
                return Undefined;
            }
			
			///
			///
			///
			ign::image::eTypePixel ImageIO::typePixel(int numBand)const
			{
				if (hasDriver()) return _driver->typePixel(numBand);
				return ign::numeric::eOther;
			}
			
			///
			///
			///
			std::string ImageIO::getFileName()const
			{
				if (hasDriver()) return _driver->getFileName();
				return std::string("");
			}
			
			///
			///
			///
			std::string ImageIO::getDriverName()const
			{
				if (hasDriver()) return _driver->getDriverName();
				return std::string("");
			}
			
			///
			///
			///
			bool ImageIO::hasDriver() const
			{
				return (_driver!=0);
			}
			
			///
			///
			///
			ImageIO::~ImageIO()
			{
				close();
			}
            
			///
			///
			///
            void ImageIO::close()
            {
				if (hasDriver()) {
					delete _driver;
					_driver = NULL;
				}
            }
			
			///
			///
			///
			void ImageIO::open(std::string const &fullPathName)
			{
				IGN_THROW_EXCEPTION("[ImageIO::open()] virtual method not allowed by root class ImageIO...");
			}

			
			///
			///
			///
			bool ImageIO::isValid()const
			{
				return (hasDriver());
			}
			
			///
			///
			///			
			bool ImageIO::getGeoref(double &ULCx, double &ULCy, double &pasX, double &pasY,std::string &sysProjSD, ign::image::georef::PixConv & pixConv)const
			{
				if (!hasDriver()) 
					return false;
				
				double geoTransform[6];
				bool res = _driver->getGeoref(geoTransform,sysProjSD,pixConv);
				if (res)
				{
					ULCx = geoTransform[0];
					pasX = geoTransform[1];
					ULCy = geoTransform[3];
					pasY = geoTransform[5];
					return true;
				}
				return false;
			}
			
			///
			///
			///
			bool ImageIO::getGeoref(double geoTransform[6],std::string &sysProjSD, ign::image::georef::PixConv & pixConv)const
			{
				if (hasDriver())
					return _driver->getGeoref(geoTransform,sysProjSD,pixConv);
				
				return false;
			}
			
			///
			///
			///
			const ign::data::Object* ImageIO::metaData()const
			{
				if (hasDriver())
					return &_driver->metaData();
				return NULL;
			}
			
			///
			///
			///
			ign::data::Object* ImageIO::metaData()
			{
				if (hasDriver())
					return &_driver->metaData();
				return NULL;
			}
		}
	}
	
}
	
	
	
