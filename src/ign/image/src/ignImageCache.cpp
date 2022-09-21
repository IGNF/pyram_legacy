/*
 *  ImageIO.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */

#include <iostream>
#include <ignImageCache.h>
#include <ignBufferImage.h>

using namespace ign::image;
using namespace ign::image::detail;

///
///
///
ImageCache::ImageCache(const Image *src):
_src(src),
_cacheSize(0),
_cacheStrategy(CacheStrategyHStrip),
_margin(3),  
minCoefToLoadCache(16),
_offsetX(0), 
_offsetY(0), 
_isValid(false)
{
	_memoryBuffer.reset(NULL);
}	

///
///
///
ImageCache::~ImageCache()
{
}

///
///
///
void	ImageCache::	setParam(size_t memorySize, CacheStrategy strategy, size_t margin) 
{
	bool verbose = 0;
	if(verbose)std::cout<<"[ImageCache::setParam]: START"<<std::endl;
	
	if (memorySize == 0)
	{
		_memoryBuffer.reset(0);
		return;
	}
	eTypePixel type = _src->typePixel();
    //on ne gere pas encore le type bool
    if (type == numeric::eBool) type = numeric::eUnsignedChar;
    
	_isValid = false;
	
	IGN_ASSERT(type != numeric::eOther);
	IGN_ASSERT(_src->numBands()>0);
	_cacheSize = (memorySize);
	_cacheStrategy = strategy;
	_margin = margin;
	
	//compute cache image parameters
	size_t channelSize = memorySize / (_src->numBands() * numeric::TypeSize(type)) ;
	int numColCache(0), numLigCache(0);
	if(verbose)
	{
		std::cout << "[ImageCache::setParam] : channelSize = " << channelSize << std::endl; 
		std::cout << "[ImageCache::setParam] : src dimension = " << _src->numCols() <<" x " << _src->numLines() << " x " << _src->numBands() << std::endl; 
	}
	//on calcule la forme du cache
	switch (_cacheStrategy)
	{
			//si le cache est centre, on fait un cache carre
		case CacheStrategyCenteredBlok:
		case CacheStrategyUpperLeftBlok:
		{
			numColCache = static_cast<int>(std::floor(std::sqrt((double)channelSize)));
			IGN_ASSERT(numColCache>1);
			numLigCache = numColCache;
			break;
		}
			//on prend la largeur de l'image et autant de lignes que possible
		case CacheStrategyHStrip:
		{
			numColCache = static_cast<int>(_src->numCols());
			numLigCache = static_cast<int>(std::floor((double)channelSize / (double)numColCache));
			IGN_ASSERT(numLigCache>1);
			break;
		}
			//on prend la longueur de l'image et autant de lignes que possible
		case CacheStrategyVStrip:
		{
			numLigCache = static_cast<int>(_src->numLines());
			numColCache = static_cast<int>(std::floor((double)channelSize / (double)numLigCache));

			IGN_ASSERT(numColCache>1);
			break;
		}
		default: IGN_ASSERT(false);
	}
	
	//on s'assure que le cache tient dans l'image
	if (numColCache > static_cast<int>(_src->numCols()))
		numColCache = static_cast<int>(_src->numCols());
	if (numLigCache > static_cast<int>(_src->numLines()))
		numLigCache = static_cast<int>(_src->numLines());
    
	if(verbose)
	{
		std::cout << "numColCache " << numColCache << std::endl;
		std::cout << "numLigCache " << numLigCache << std::endl;
	}
    
	int numBands = static_cast<int>(_src->numBands());
	switch(type)
	{
        case numeric::eBool:
        {
            //_memoryBuffer.reset(new BufferImage<bool>(numColCache, numLigCache, numBands));
            _memoryBuffer.reset(new BufferImage<unsigned char>(numColCache, numLigCache, numBands));
            break;
        }
		case numeric::eUnsignedChar:
		{
			_memoryBuffer.reset(new BufferImage<unsigned char>(numColCache, numLigCache, numBands));
			break;
		}
		case numeric::eUnsignedShort:
		{
			_memoryBuffer.reset(new BufferImage<unsigned short>(numColCache, numLigCache, numBands));
			break;
		}
		case numeric::eUnsignedInt:
		{
			_memoryBuffer.reset(new BufferImage<unsigned int>(numColCache, numLigCache, numBands));
			break;
		}
		case numeric::eSignedShort:
		{
			_memoryBuffer.reset(new BufferImage<signed short>(numColCache, numLigCache, numBands));
			break;
		}
		case numeric::eFloat:
		{
			_memoryBuffer.reset(new BufferImage<float>(numColCache, numLigCache, numBands));
			break;
		}
		default:
            IGN_THROW_EXCEPTION("[ImageCache::setParam] type non reconnu: " + ign::image::TypeToString(type));
	}
	if(verbose)std::cout<<"[ImageCache::setParam]: load..."<<std::endl;
	load();
	if(verbose)std::cout<<"[ImageCache::setParam]: END"<<std::endl;
}

///
///
///
void	ImageCache::	load() const
{
	bool verbose = false;
	
	_isValid = false;
	
	//on s'assure que le cache tient dans l'image
	if ((_offsetX + _memoryBuffer->numCols()) > _src->numCols())
		_offsetX = _src->numCols() - _memoryBuffer->numCols();
	if ((_offsetY + _memoryBuffer->numLines()) > _src->numLines())
		_offsetY = _src->numLines() - _memoryBuffer->numLines();
	
	eTypePixel type = _memoryBuffer->typePixel();
    if (type == numeric::eBool) type = numeric::eUnsignedChar;

	void * ptr(0);
	int pixelSpace(0), lineSpace(0), bandSpace(0);
	switch(type)
	{
        case numeric::eBool:
        {
            BufferImage<unsigned char>& buf = _memoryBuffer->as<BufferImage<unsigned char> >();
            ptr = buf.getPtr();
            pixelSpace = buf.getPixelSpace();
            lineSpace = buf.getLineSpace();
            bandSpace = buf.getBandSpace();
            break;
        }
		case numeric::eUnsignedChar:
		{
			BufferImage<unsigned char>& buf = _memoryBuffer->as<BufferImage<unsigned char> >();
			ptr = buf.getPtr();
			pixelSpace = buf.getPixelSpace();
			lineSpace = buf.getLineSpace();
			bandSpace = buf.getBandSpace();
			break;
		}
		case numeric::eUnsignedShort:
		{
			BufferImage<unsigned short>& buf = _memoryBuffer->as<BufferImage<unsigned short> >();
			ptr = buf.getPtr();
			pixelSpace = buf.getPixelSpace();
			lineSpace = buf.getLineSpace();
			bandSpace = buf.getBandSpace();
			break;
		}
		case numeric::eUnsignedInt:
		{
			BufferImage<unsigned int>& buf = _memoryBuffer->as<BufferImage<unsigned int> >();
			ptr = buf.getPtr();
			pixelSpace = buf.getPixelSpace();
			lineSpace = buf.getLineSpace();
			bandSpace = buf.getBandSpace();
			break;
		}
		case numeric::eSignedShort:
		{
			BufferImage<signed short>& buf = _memoryBuffer->as<BufferImage<signed short> >();
			ptr = buf.getPtr();
			pixelSpace = buf.getPixelSpace();
			lineSpace = buf.getLineSpace();
			bandSpace = buf.getBandSpace();
			break;
		}
		case numeric::eFloat:
		{
			BufferImage<float>& buf = _memoryBuffer->as<BufferImage<float> >();
			ptr = buf.getPtr();
			pixelSpace = buf.getPixelSpace();
			lineSpace = buf.getLineSpace();
			bandSpace = buf.getBandSpace();
			break;
		}
		default:
			IGN_ASSERT(false);
	}
	
	//on lit le bloc dans l'image
	if (verbose) std::cout<<"[ImageCache::load] "<<_offsetX<<" x "<<_offsetY<<" - "<<_memoryBuffer->numCols()<<" x "<<_memoryBuffer->numLines()<<" x "<<_memoryBuffer->numBands()<<std::endl;
	
	_src->read(_offsetX, _offsetY, 0, static_cast<int>(_memoryBuffer->numCols()), static_cast<int>(_memoryBuffer->numLines()), static_cast<int>(_memoryBuffer->numBands()), 1, ptr, type, pixelSpace, lineSpace, bandSpace);
	_isValid = true;
}

///
///
///
bool	ImageCache::		isValid() const
{
	return (_memoryBuffer.get() != NULL && _isValid);
}

///
///
///
bool	ImageCache::	read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType,
							 int nPixelSpace, int nLineSpace, int nBandSpace)const
{
	bool verbose = 0;
	//on regarde si le cache est initialise
	//IGN_ASSERT(isValid());
	if(!isValid())
		return false;
	
	//a dezoom != 1 on ne gere pas pour le moment
	if (nDeZoom != 1) 
	{
		if (verbose) std::cout<<"[ImageCache::read] dezoom != 1"<<std::endl;
		return false;
	}
	
	//on regarde si la zone a charger tient dans le cache
	if (nXSize>(int)_memoryBuffer->numCols()) 
	{
		if (verbose) std::cout<<"[ImageCache::read] cache numCols insuffisant: "<<nXSize<<" > "<<_memoryBuffer->numCols()<<std::endl;
		return false;
	}
	if (nYSize>(int)_memoryBuffer->numLines())
	{
		if (verbose) std::cout<<"[ImageCache::read] cache numLines insuffisant "<<nYSize<<" > "<<_memoryBuffer->numLines()<<std::endl;
		return false;
	}
	
	
	//on regarde si le cache contient deja la zone a charger
	bool cacheIsReady = true;
	if (_offsetX>(size_t)nXOff) cacheIsReady =  false;
	else if (_offsetY>(size_t)nYOff) cacheIsReady = false;
	else if ((_offsetX + _memoryBuffer->numCols())<(size_t)(nXOff+nXSize)) cacheIsReady = false;
	else if ((_offsetY + _memoryBuffer->numLines())<(size_t)(nYOff+nYSize)) cacheIsReady = false;
	
	if (!cacheIsReady)
	{
		if (verbose) std::cout<<"[ImageCache::read] cache not ready: "<<_offsetX<<" x "<<_offsetY<<" | "<<nXSize<<" x "<<nYSize<<std::endl;
		
		//si la taille du cache est trop petite par rapport a la taille du crop a charger, 
		//on considere que ca ne vaut pas le coup de recharger
		//(mais on l(utiliser s'il est deja charge, evidemment)
		if (nXSize>(int)(minCoefToLoadCache*_memoryBuffer->numCols())) return false;
		if (nYSize>(int)(minCoefToLoadCache*_memoryBuffer->numLines())) return false;
		
		//on calcule la position du cache
		switch (_cacheStrategy)
		{
			case CacheStrategyCenteredBlok:
			{
				int posX = nXOff + static_cast<int>(nXSize - _memoryBuffer->numCols()) / 2;
				int posY = nYOff + static_cast<int>(nYSize - _memoryBuffer->numLines()) / 2;
				if (posX<0) posX = 0;
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"[ImageCache::read] calcul de l'offset pour un cache centre"<<std::endl;
				break;
			}
			case CacheStrategyUpperLeftBlok:
			{
				int posX = nXOff - static_cast<int>(_margin);
				int posY = nYOff - static_cast<int>(_margin);
				if (posX<0) posX = 0;
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"[ImageCache::read] calcul de l'offset pour un cache ulc"<<std::endl;
				break;
			}
			case CacheStrategyHStrip:
			{
				int posX = 0;
				int posY = nYOff -_margin;
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"[ImageCache::read] calcul de l'offset pour un cache strip: "<<nXOff<<" x "<<nYOff<<" marge: "<<_margin<<std::endl;
				break;
			}
            case CacheStrategyVStrip:
			{
				int posY = 0;
				int posX = nXOff - static_cast<int>(_margin);
				if (posX<0) posX = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"[ImageCache::read] calcul de l'offset pour un cache strip: "<<nXOff<<" x "<<nYOff<<" marge: "<<_margin<<std::endl;
				break;
			}
			default: IGN_ASSERT(false);
		}
		if (verbose) std::cout<<"[ImageCache::read] cache to reload from : "<<_offsetX<<" x "<<_offsetY<<" | "<<nXSize<<" x "<<nYSize<<std::endl;
		load();
	}
	else 
	{
		if (verbose) std::cout<<"[ImageCache::read] cache ready with : "<<_offsetX<<" x "<<_offsetY<<" | "<<nXOff<<" x "<<nYOff<<" and size: "<<nXSize<<" x "<<nYSize<<std::endl;
	}
	
	//maintenant on lit dans le cache
	if (verbose) std::cout<<"[ImageCache::read] readcache: "<<nXOff - _offsetX<<" x "<<nYOff - _offsetY<<" with offset = "<<_offsetX<<" x "<<_offsetY<<std::endl;
	_memoryBuffer->read(nXOff - _offsetX, nYOff - _offsetY, nBand0, nXSize, nYSize, nNBands, nDeZoom, buffer, eBufferType,nPixelSpace, nLineSpace, nBandSpace);
	return true;
}

///
///
///
void	ImageCache::getCache(double col, double lig, Image*& cache, size_t& offX, size_t& offY)const
{
	bool verbose = 0;
	//on verifie que le cache existe
	if (_memoryBuffer.get() == NULL) 
	{
		cache = NULL;
		offX = 0; offY = 0;
		if (verbose) std::cout<<"[ImageCache::getCache(] No Cache"<<std::endl;
		return;
	}	
	//on verifie qu'on peut recuperer le cache sous (col, lig)
	if (col<0. || col >= _src->numCols() || lig<0. || lig >= _src->numLines())
	{
		cache = NULL;
		offX = 0; offY = 0;
		if (verbose) std::cout<<"[ImageCache::getCache(] inconsistent size: "<<col<<" x "<<lig<<" | "<<_src->numCols()<<" x "<<_src->numLines()<<std::endl;
		return;
	}
	
	//on regarde si le cache contient deja la zone a charger
	bool cacheIsReady = true;
	if (_offsetX>(size_t)(col - _margin) && _offsetX>0) cacheIsReady =  false;
	else if (_offsetY>(size_t)(lig - _margin) && _offsetY>0) cacheIsReady = false;
	else if ((_offsetX + _memoryBuffer->numCols())<(size_t)(col + _margin) && _memoryBuffer->numCols() < _src->numCols()) cacheIsReady = false;
	else if ((_offsetY + _memoryBuffer->numLines())<(size_t)(lig + _margin) && _memoryBuffer->numLines() < _src->numLines()) cacheIsReady = false;
	
	if (verbose && !cacheIsReady) std::cout<<"cache to reload: ("<<col<<", "<<lig<<") outside: "<<_offsetX<<", "<<_offsetY<<std::endl;
	if (!cacheIsReady)
	{						
		//on calcule la position du cache
		switch (_cacheStrategy)
		{
			case CacheStrategyCenteredBlok:
			{
				int posX = static_cast<int>(col)-static_cast<int>(_memoryBuffer->numCols()) / 2;
				int posY = static_cast<int>(lig)-static_cast<int>(_memoryBuffer->numLines()) / 2;
				if (posX<0) posX = 0;
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"new centered cache reloaded from "<<_offsetX<<", "<<_offsetY<<std::endl;
				break;
			}
			case CacheStrategyUpperLeftBlok:
			{
				int posX = static_cast<int>(col-_margin);
				int posY = static_cast<int>(lig-_margin);
				if (posX<0) posX = 0;
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"new upper corner cache reloaded from "<<_offsetX<<", "<<_offsetY<<std::endl;
				break;
			}
            case CacheStrategyHStrip:
			{
				int posX = 0;
				int posY = static_cast<int>(lig-_margin);
				if (posY<0) posY = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
                if (verbose) std::cout<<"new strip H cache reloaded from "<<_offsetX<<", "<<_offsetY<<std::endl;
				break;
			}
            case CacheStrategyVStrip:
			{
				int posY = 0;
				int posX = static_cast<int>(col-_margin);
				if (posX<0) posX = 0;
				_offsetX = (size_t) posX;
				_offsetY = (size_t) posY;
				if (verbose) std::cout<<"new strip V cache reloaded from "<<_offsetX<<", "<<_offsetY<<std::endl;
				break;
			}
            default: IGN_ASSERT(false);
		}
		
		load();
	}
	
	cache = _memoryBuffer.get();
	offX = _offsetX;
	offY = _offsetY;
}

///
///
///
size_t	ImageCache::	margin()
{
	return _margin;
}

///
///
///
size_t	ImageCache::	cacheSize()
{
	return _cacheSize;
}

///
///
///
void	ImageCache::	bufferSize(size_t &numLines, size_t &numCols, size_t &numBands)
{
	numLines = 0;
	numCols = 0;
	numBands = 0;
	if(isValid())
	{
		numLines = _memoryBuffer->numLines();
		numCols = _memoryBuffer->numCols();
		numBands = _memoryBuffer->numBands();
		return;
	}
}
