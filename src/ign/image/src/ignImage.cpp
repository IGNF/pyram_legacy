/*
 *  ImageIO.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */

#include <iostream>

#include <ignDataString.h>
#include <ignDataDouble.h>
#include <ignException.h>
#include <ignImage.h>
#include <ignBufferImage.h>

using namespace ign::image;

///
///
///
Image::Image():
_defaultValue(0.)
{
}	
		
///
///
///
Image::~Image()
{
}

///
///
///		
size_t	Image::	numCols()const
{
	ImageSize sz = size(1);
	return sz.first;
}
		
///
///
///
size_t	Image::	numLines()const
{
	ImageSize sz = size(1);
	return sz.second;
}
		
///
///
///
bool	Image::isIn(int c, int l, int b)const
{
	if (c<0) return false;
	if (l<0) return false;
	if (b<0) return false;
	if (c>=(int)numCols()) return false;
	if (l>=(int)numLines()) return false;
	if (b>=(int)numBands()) return false;
	return true;
}

ColorSpace Image::colorSpace()const
{
    return Undefined;
}

///\brief	recupere une table des couleurs pour un canal
bool							Image::getColorTable(int bandNum, ColorTable &colorTable)const
{
    return false;
}

///
///
///
void Image::close()
{
}
		
///
///
///
void	Image::		read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType, int nPixelSpace, int nLineSpace, int nBandSpace)const
{
	//si l'image est lisible, on doit passer par une surcharge de cette methode
	IGN_ASSERT(!isReadable());
}
		
///
///
///
void	Image::		write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void *buffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace, int nBandSpace)
{
	//si l'image est writable, on doit passer par une surcharge de cette methode
	IGN_ASSERT(!isWritable());
}
		
///
///
///
bool	Image::	paste(int nXOff, int nYOff, int nBandOff, int nXSize, int nYSize, int nNBands, const Image* src, int nXSrc, int nYSrc, int nBandSrc)
{
	bool verbose = 0;
     
	IGN_ASSERT(src);
    IGN_ASSERT(src->isReadable());
	IGN_ASSERT(isWritable());
	// for now, does not work if src is a MultiImage
	//IGN_ASSERT(!src->isTiled());
	IGN_ASSERT(nXSize>0);
	IGN_ASSERT(nYSize>0);

	if (verbose)
	{
		std::cout<<"[Image::paste] START"<<std::endl;
		std::cout<<"src == "<<src->numCols()<<" x "<<src->numLines()<<std::endl;
		std::cout<<"ulc + sz == "<<nXSrc<<" x "<<nYSrc<<" / "<<nXSize<<" x "<<nYSize<<std::endl;
		std::cout << " (nXOff+nXSize) " << (nXOff+nXSize) << " numCols " << (int)numCols() << std::endl;
		std::cout << " (nYOff+nYSize) " << (nYOff+nYSize) << " numLines " << (int)numLines() << std::endl;
	}
	IGN_ASSERT((nXSrc+nXSize) <= static_cast<int>(src->numCols()));
	IGN_ASSERT((nYSrc+nYSize) <= static_cast<int>(src->numLines()));
	if ((nXOff+nXSize) >(int)numCols() || (nYOff+nYSize) > (int)numLines())
	{
		std::ostringstream oss;
		oss<<"[Image::paste] tailles incompatibles: src = ("<<src->numCols()<<" x "<<src->numLines()<<") | OffSrc = ("<<nXSrc<<" x "<<nYSrc<<") | CropSize = ("<<nXSize<<" x "<<nYSize<<") | SzDest = ("<<numCols()<<" x "<<numLines()<<") | OffDest = ("<<nXOff<<" x "<<nYOff<<") ";
		IGN_THROW_EXCEPTION(oss.str());
	}
	//IGN_ASSERT((nXOff+nXSize) <= (int)numCols() && (nYOff+nYSize) <= (int)numLines());
	IGN_ASSERT((nBandOff+nNBands) <= (int)numBands());
	IGN_ASSERT((nBandSrc+nNBands) <= (int)src->numBands());
            
	if (src->isInRAM() && ! src->isTiled())
	{
		switch(src->typePixel())
		{
		case numeric::eFloat:
		{

			BufferImage<float> const & buffer = src->as<BufferImage<float> >();
			int offset = buffer.getLineSpace()*nYSrc+buffer.getPixelSpace()*nXSrc+buffer.getBandSpace()*nBandSrc;
			this->write(nXOff, nYOff, nBandOff , nXSize, nYSize, nNBands, (void*)(buffer.getPtr()+offset),
						src->typePixel(), buffer.getPixelSpace(), buffer.getLineSpace(), buffer.getBandSpace());
			break;
		}
		case numeric::eUnsignedChar:
		{
			BufferImage<unsigned char> const & buffer = src->as<BufferImage<unsigned char> >();
			int offset = buffer.getLineSpace()*nYSrc+buffer.getPixelSpace()*nXSrc+buffer.getBandSpace()*nBandSrc;
			this->write(nXOff, nYOff, nBandOff , nXSize, nYSize, nNBands, (void*)(buffer.getPtr()+offset),
						src->typePixel(), buffer.getPixelSpace(), buffer.getLineSpace(), buffer.getBandSpace());
			break;
		}
		case numeric::eUnsignedShort:
		{

			BufferImage<unsigned short> const & buffer = src->as<BufferImage<unsigned short> >();
			int offset = buffer.getLineSpace()*nYSrc+buffer.getPixelSpace()*nXSrc+buffer.getBandSpace()*nBandSrc;
			this->write(nXOff, nYOff, nBandOff , nXSize, nYSize, nNBands, (void*)(buffer.getPtr()+offset),
						src->typePixel(), buffer.getPixelSpace(), buffer.getLineSpace(), buffer.getBandSpace());
			break;
		}
		case numeric::eSignedShort:
		{
			BufferImage<signed short> const & buffer = src->as<BufferImage<signed short> >();
			int offset = buffer.getLineSpace()*nYSrc+buffer.getPixelSpace()*nXSrc+buffer.getBandSpace()*nBandSrc;
			this->write(nXOff, nYOff, nBandOff , nXSize, nYSize, nNBands, (void*)(buffer.getPtr()+offset),
						src->typePixel(), buffer.getPixelSpace(), buffer.getLineSpace(), buffer.getBandSpace());
			break;
		}
		case numeric::eUnsignedInt:
		{
			BufferImage<unsigned int> const & buffer = src->as<BufferImage<unsigned int> >();
			int offset = buffer.getLineSpace()*nYSrc+buffer.getPixelSpace()*nXSrc+buffer.getBandSpace()*nBandSrc;
			this->write(nXOff, nYOff, nBandOff , nXSize, nYSize, nNBands, (void*)(buffer.getPtr()+offset),
						src->typePixel(), buffer.getPixelSpace(), buffer.getLineSpace(), buffer.getBandSpace());
			break;
		}
		default: IGN_ASSERT(false);
		}
		if (verbose) std::cout<<"[Image::paste] END"<<std::endl;
		return true;
	}
			
	//on cree un bufferImage du type en sortie de la taille du crop
	ign::unique_ptr<Image> bufferPtr;
	void* ptr = 0;
	int nLineSpace=0, nPixelSpace=0, nBandSpace=0;
	for (int b = 0; b < nNBands; ++b)
	{
		switch(typePixel(b + nBandOff))
		{
			case numeric::eFloat:
			{
				BufferImage<float>* buffer = new BufferImage<float>(nXSize, nYSize, 1);
				*buffer = static_cast<float>(_defaultValue);
				bufferPtr.reset(buffer);
				ptr = buffer->getPtr();
				nLineSpace = buffer->getLineSpace();
				nPixelSpace = buffer->getPixelSpace();
				nBandSpace = buffer->getBandSpace();
				break;
			}
			case numeric::eUnsignedChar:
			{
				BufferImage<unsigned char>* buffer = new BufferImage<unsigned char>(nXSize, nYSize, 1);
				*buffer = static_cast<unsigned char>(_defaultValue);
				bufferPtr.reset(buffer);
				ptr = buffer->getPtr();
				nLineSpace = buffer->getLineSpace();
				nPixelSpace = buffer->getPixelSpace();
				nBandSpace = buffer->getBandSpace();
				break;
			}
			case numeric::eUnsignedShort:
			{
				BufferImage<unsigned short>* buffer = new BufferImage<unsigned short>(nXSize, nYSize, 1);
				*buffer = static_cast<unsigned short>(_defaultValue);;
				bufferPtr.reset(buffer);
				ptr = buffer->getPtr();
				nLineSpace = buffer->getLineSpace();
				nPixelSpace = buffer->getPixelSpace();
				nBandSpace = buffer->getBandSpace();
				break;
			}
			case numeric::eSignedShort:
			{
				BufferImage<signed short>* buffer = new BufferImage<signed short>(nXSize, nYSize, 1);
				*buffer = static_cast<signed short>(_defaultValue);
				bufferPtr.reset(buffer);
				ptr = buffer->getPtr();
				nLineSpace = buffer->getLineSpace();
				nPixelSpace = buffer->getPixelSpace();
				nBandSpace = buffer->getBandSpace();
				break;
			}
			case numeric::eUnsignedInt:
			{
				BufferImage<unsigned int>* buffer = new BufferImage<unsigned int>(nXSize, nYSize, 1);
				*buffer = static_cast<unsigned int>(_defaultValue);;
				bufferPtr.reset(buffer);
				ptr = buffer->getPtr();
				nLineSpace = buffer->getLineSpace();
				nPixelSpace = buffer->getPixelSpace();
				nBandSpace = buffer->getBandSpace();
				break;
			}
			default: IGN_ASSERT(false);
		}
		//on lit le crop dans la band b + nBandSrc
		src->read(nXSrc, nYSrc, nBandSrc + b,	nXSize, nYSize, 1, 1, ptr,	typePixel(b + nBandOff), nPixelSpace, nLineSpace, nBandSpace);
				
		//on ecrit dans la bande b + nBandOff this:
		this->write(nXOff, nYOff, nBandOff + b, nXSize, nYSize, 1, ptr,		typePixel(b + nBandOff), nPixelSpace, nLineSpace, nBandSpace);
	}
	if (verbose) std::cout<<"[Image::paste] END"<<std::endl;
	return true;
}

///
///
///
bool	Image::		crop(int nXOff, int nYOff, int nBandOff, int nXSize, int nYSize, int nNBands, Image* dest, int nXDest, int nYDest, int nBandDest) const
{
	return dest->paste( nXDest, nYDest, nBandDest, nXSize, nYSize, nNBands, this, nXOff, nYOff, nBandOff);
}
		
///
///
///
const ign::data::Object* Image::metaData()const
{
return NULL;
}
		
///
///
///
ign::data::Object* Image::metaData()
{
return NULL;
}

///
///
///
size_t    Image::memorySize() const
{
return (   numCols() * numLines() * numBands() * numeric::TypeSize(typePixel())    );
}



///
///
///
void	Image::		defaultValue(double d)
{
	_defaultValue = d;
}

///
///
///
double		Image::		defaultValue() const
{
	return _defaultValue;
}


///
///
///
namespace ign{namespace image{namespace georef 
{
	///
	///
	///
	std::string			PixConventionToString(ign::image::georef::PixConv pixconv)
	{
		switch (pixconv)
		{
			case PixConvNULL:
				return std::string("PixConvNULL");
			case PixConvPOINT:
				return std::string("PixConvPOINT");
			case PixConvAREA:
				return std::string("PixConvAREA");
			default: 
				IGN_ASSERT(false);
		}
		IGN_ASSERT(false);
		return std::string("");
	}

			
	bool IsGeoref(const ign::data::Object& mtd)
	{
		if (!mtd.hasAttribute(kStrGeorefX0)) return false;
		if (!mtd.hasAttribute(kStrGeorefY0)) return false;
		if (!mtd.hasAttribute(kStrGeorefDX)) return false;
		if (!mtd.hasAttribute(kStrGeorefDY)) return false;
		if (!mtd.hasAttribute(kStrGeorefProjection)) return false;
		if (!mtd.hasAttribute(kStrGeorefPixConvention)) return false;
		return true;
	}
		
	///
	///
	///	
	bool IsTagGeoref(const std::string& key)
	{
		if (kStrGeorefX0 == key) return true;
		if (kStrGeorefY0 == key) return true;
		if (kStrGeorefDX == key) return true;
		if (kStrGeorefDY == key) return true;
		if (kStrGeorefRX == key) return true;
		if (kStrGeorefRY == key) return true;
		if (kStrGeorefProjection == key) return true;
		if (kStrGeorefPixConvention == key) return true;
		return false;
	}
		
	///
	///
	///
	bool GetGeoref(const data::Object& mtd, Data& grf)
	{
		if (!IsGeoref(mtd)) return false;
		grf._origin.first	=		mtd.attribute(kStrGeorefX0).value().toDouble();
		grf._origin.second	=		mtd.attribute(kStrGeorefY0).value().toDouble();
		grf._step.first		=		mtd.attribute(kStrGeorefDX).value().toDouble();
		grf._step.second	=		mtd.attribute(kStrGeorefDY).value().toDouble();
		grf._rot.first		=		0.;
		grf._rot.second		=		0.;
		if (!mtd.hasAttribute(kStrGeorefRX))
		{
			grf._rot.first = mtd.attribute(kStrGeorefRX).value().toDouble();
		}
		if (!mtd.hasAttribute(kStrGeorefRY))
		{
			grf._rot.second = mtd.attribute(kStrGeorefRY).value().toDouble();
		}
		grf._projection = mtd.attribute(kStrGeorefProjection).value().toString();

		std::string regConv(mtd.attribute(kStrGeorefPixConvention).value().toString());
		if(regConv == ign::image::georef::PixConventionToString(PixConvAREA))
			grf._pixConvention   =    ign::image::georef::PixConvAREA;
		else if (regConv == ign::image::georef::PixConventionToString(PixConvPOINT))
			grf._pixConvention   =    ign::image::georef::PixConvPOINT;
		else
			grf._pixConvention   =   ign::image::georef::PixConvNULL;

		return true;
	}
			
	///
	///
	///
	void SetGeoref(const Data& grf, data::Object& mtd)
	{
		mtd.setAttribute(kStrGeorefX0,			data::Double(grf._origin.first));
		mtd.setAttribute(kStrGeorefY0,			data::Double(grf._origin.second));
		mtd.setAttribute(kStrGeorefDX,			data::Double(grf._step.first));
		mtd.setAttribute(kStrGeorefDY,			data::Double(grf._step.second));
		mtd.setAttribute(kStrGeorefRX,			data::Double(grf._rot.first));
		mtd.setAttribute(kStrGeorefRY,			data::Double(grf._rot.second));
		mtd.setAttribute(kStrGeorefProjection,	data::String(grf._projection));
				
		std::string strRegConvention;
		if(grf._pixConvention == ign::image::georef::PixConvAREA)
			strRegConvention   =    ign::image::georef::PixConventionToString(PixConvAREA);
		else if (grf._pixConvention == ign::image::georef::PixConvPOINT)
			strRegConvention   =    ign::image::georef::PixConventionToString(PixConvPOINT);
		else
		{
			strRegConvention   =    ign::image::georef::PixConventionToString(PixConvNULL);
		}
		mtd.setAttribute(kStrGeorefPixConvention,	data::String(strRegConvention));
	}
}}}
