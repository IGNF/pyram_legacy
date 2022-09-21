/*
 *  ImageDriver.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */

#include <ignImageDriver.h>
#include <ignImageIO.h>
//#include <ign/imageio/Dictionary.h"

namespace ign 
{
	namespace image
	{
		namespace io 
		{
			namespace driver
			{
				
				ImageDriver::ImageDriver():_colorSpace(Undefined)
				{
				}
				
				///
				///
				///
				ImageDriver::~ImageDriver()
				{

				}
				
				/// 
				///
				///
				const ign::data::Object& ImageDriver::metaData()const
				{
					return _metaData;
				}	
				
				///
				///
				///
				ign::data::Object& ImageDriver::metaData()
				{
					return _metaData;
				}
				
				///
				///
				///
				size_t		ImageDriver::numCols()const
				{
					int NC, NL;
					size(NC, NL);
					return (size_t) NC;
				}
				
				///
				///
				///
				size_t		ImageDriver::numLines()const
				{
					int NC, NL;
					size(NC, NL);
					return (size_t) NL;
				}
				
				///
				///
				///
				bool ImageDriver::getGeoref(double /*geoTransform*/[6],std::string &/*sysProjSD*/, ign::image::georef::PixConv & /*pixConv*/)const
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::setGeoref(double /*geomTransfom*/[6],std::string const &/*sysProjSD*/, ign::image::georef::PixConv const & /*pixConv*/)
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::getColorTable(int /*bandNum*/, ColorTable &/*table*/)const
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::setColorTable(int /*bandNum*/, ColorTable const &/*table*/)
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::update(std::string const &/*fullPathName*/)
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::create(std::string const &/*fullPathName*/, int /*NRows*/, int /*NLines*/, int /*NBands*/, eTypePixel* /*tabType*/,data::Object const * /*dico*/,data::Object const * /*dico*/)
				{
					return false;
				}
				
				///
				///
				///
				bool ImageDriver::createCopy(std::string const &/*fullPathNameOut*/, std::string const &/*fullPathNameIn*/, data::Object const * /*dico*/,data::Object const * /*dico*/)const
				{
					return false;
				}
				
				///
				///
				///
				void ImageDriver::write(int /*C0*/,int /*L0*/, int /*band0*/, int /*NBrows*/, int /*NLines*/, int /*NBands*/, void * /*buffer*/, eTypePixel /*bufferType*/,int /*nPixelSpace*/, int /*nLineSpace*/, int /*nBandSpace*/)
				{

				}
				
				///
				///
				///
				void ImageDriver::close()
				{
				}
			}
		}
	}
}
