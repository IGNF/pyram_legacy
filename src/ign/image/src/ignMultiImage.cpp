#include <fstream>
#include <set>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/progress.hpp>
// #include <boost/timer/progress_display.hpp> /* if boost > 1.72

#include <ignCommon.h>
#include <ignStringTools.h>

#include <ignMultiImage.h>
#include <ignBufferImage.h>
#include <ignImageDriverManager.h>

using namespace ign::image;


// constructeur par recopie
MultiImage::TileData::TileData(TileData const & other)
{
	IGN_THROW_EXCEPTION(" constructeur de copie de TileData non gere, car contenant un pointeur");
}

///
///
///
MultiImage::TileData::~TileData()
{
//	if (_image) delete (_image);
}

///
///
///
MultiImage::TileData::TileData(const ign::shared_ptr<Image>& img,std::pair<int, int> const& position, const ImageSize& sz, size_t band0, size_t nBands, std::string name):
_image(img),
_position(position),
_size(sz),
_band0(band0),
_nBands(nBands),
_name(name)
{
}

///
///
///
MultiImage::MultiImage():
_memoryCache(this),
_typePixel(ign::numeric::eOther),
_numBands(0),
_maxOpenTiles(20),
_numOpenTiles(0),
_throwExc(true),
_verbose(0)
{
}

///
///
///
MultiImage::~MultiImage()
{
	clear();
}

///
///
///
size_t MultiImage::numBands()const
{
	return _numBands;
}

///
///
///
eTypePixel MultiImage::typePixel(int numBand)const
{
	return _typePixel;
}

///
///
///
void MultiImage::setTypePixel(const eTypePixel &t, int numBand)
{
	if (numBand != 0) 
	{
		std::ostringstream oss;
		oss<<"on ne gere qu'un unique type numerique";
		IGN_THROW_EXCEPTION(oss.str());
	}
	_typePixel = t;
}

///
///
///
void MultiImage::setNumBands(int nb)
{
	_numBands = nb;
}

///
///
///
bool MultiImage::hasDriver() const
{
	return true;
}

///
///
///
bool MultiImage::isValid() const
{
	if(_data.size() == 0 ) return false;
	
	return true;
#if 0
	bool invalidTileFound = false;

	std::map<size_t, TileDataPtr>::const_iterator it;

	for(it = _data.begin() ; it != _data.end() ;++it)
	{
		if(!it->second->_image->isValid())
		{
			if (numTiles() < _maxOpenTiles)
			{
				invalidTileFound = true;
				break;
			}
		}
	}

	return !invalidTileFound;
#endif
}

///
///
///
void MultiImage::close()
{
	std::map<size_t, TileDataPtr>::const_iterator it;

	for(it = _data.begin() ; it != _data.end() ;++it)
	{
		it->second->_image->close();
	}
    _numOpenTiles = 0;
}

///
///
///		
bool	MultiImage::	isReadable() const
{
	if (_data.empty()) return true;
	std::map<size_t, TileDataPtr>::const_iterator it = _data.begin() ;
	return it->second->_image->isReadable();
}

///
///
///
bool	MultiImage::	isWritable() const
{
	if (_data.empty()) return true;
	std::map<size_t, TileDataPtr>::const_iterator it = _data.begin() ;
	return it->second->_image->isWritable();
}

///
///
///
bool	MultiImage::	isInRAM() const
{
	if (_data.empty()) return true;
	std::map<size_t, TileDataPtr>::const_iterator it = _data.begin() ;
	return it->second->_image->isInRAM();
}

///
///
///
bool	MultiImage::	isTiled() const
{
	return true;
}

///
///
///
void MultiImage::read (	int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType,
						   int nPixelSpace, int nLineSpace, int nBandSpace)const
{
	bool verbose  = 0;
    bool verbose2 = 0;
	
	if (verbose)
	{
		std::cout << "[MultiImage::read] "<<std::endl;
		std::cout << "nXOff nYOff nBand0: "<<nXOff<<" x "<<nYOff<<" x "<<nBand0<<std::endl;
		std::cout << "nXSize nYSize nNBands: "<<nXSize<<" x "<<nYSize<<" x "<<nNBands<<std::endl;	
	}
	

	//tentative de lecture a travers le cache
	if (_memoryCache.isValid())
	{
		if (_memoryCache.read(nXOff,nYOff,nBand0,nXSize,nYSize,nNBands,nDeZoom,buffer,eBufferType,nPixelSpace,nLineSpace,nBandSpace))
		{
			return;
		}
	}
#if 1
	//on commence par remplir buffer avec la valeur de MultiImage par defaut pour s'assurer que les pixels non remplis par les tuiles
	//auront bien cette valeur
	switch(eBufferType)
	{
		case numeric::eUnsignedChar:
		{
			ign::image::BufferImage<unsigned char> buf((unsigned char*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<unsigned char>(_defaultValue);
			break;
		}
		case numeric::eUnsignedShort:
		{
			ign::image::BufferImage<unsigned short> buf((unsigned short*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<unsigned short>(_defaultValue);
			break;
		}
		case numeric::eUnsignedInt:
		{
			ign::image::BufferImage<unsigned int> buf((unsigned int*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<unsigned int>(_defaultValue);
			break;
		}
		case numeric::eBool: //NB: pas sur que ca marche en bool...
		{
			ign::image::BufferImage<bool> buf((bool*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<bool>(_defaultValue);
			break;
		}
		case numeric::eSignedShort:
		{
			ign::image::BufferImage<signed short> buf((signed short*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<signed short>(_defaultValue);
			break;
		}
		case numeric::eSignedInt:
		{
			ign::image::BufferImage<signed int> buf((signed int*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<signed int>(_defaultValue);
			break;
		}
		case numeric::eFloat:
		{
			ign::image::BufferImage<float> buf((float*)buffer, ImageSize(nXSize/nDeZoom, nYSize/nDeZoom), nNBands, nPixelSpace,nLineSpace,nBandSpace, false);
			buf = static_cast<float>(_defaultValue);
			break;
		}
		default:;
	}
#endif
	
	if ((nXOff + nXSize) >= (int)numCols()) nXSize = static_cast<int>(numCols()) - nXOff;
	if ((nYOff + nYSize) >= (int)numLines()) nYSize = static_cast<int>(numLines()) - nYOff;
	
	
    if ((nBand0 + nNBands) > (int)numBands())
    {
        std::ostringstream oss;
        oss << "[MultiImage::read] ==== Nbre de bandes superieur au nombre de bandes de l'image : "<<nBand0 << " + "<<nNBands<<" "<<numBands();
        IGN_THROW_EXCEPTION(oss.str());
    }

	std::vector<bool> vCanalRempli(numBands(), false);

	std::map<size_t, TileDataPtr>::const_iterator it;
	
	if (verbose) std::cout<<"numTiles == "<<numTiles()<<std::endl;
	
	ign::unique_ptr<boost::progress_display> progressPtr;
	if (_verbose>0)
	{
		std::ostringstream oss;
		oss<<"reading a multiImage with "<<_data.size()<<" tiles...\n";
		progressPtr.reset(new boost::progress_display( _data.size(), std::cout, oss.str()));
	}
	for (it = _data.begin(); it != _data.end() ; ++it)
	{
		if (_verbose>0)++(*progressPtr.get());
		const TileDataPtr& tile = it->second;
		if ((verbose)||(verbose2)) std::cout<<"tile == "<<it->first<<" | band0, numBands == "<<tile->_band0<<" x "<<tile->_nBands<<" | nb open ports = "<<_numOpenTiles<<" / "<<_maxOpenTiles<<std::endl;
		if (! (tile->_image)) 
		{ 
				std::cout<<"[MultiImage::read] ==== tile NULL pour image "<<it->first<<"!!!"<<std::endl; continue;
		}
		
		ImageSize szIma= tile->_size;
		
		if ((verbose)||(verbose2)) std::cout<<"tile -> size: "<<szIma.first<<" x "<<szIma.second<<"; position "<<tile->_position.first<<" x "<<tile->_position.second<<" "<<tile->_name<<std::endl;
		
		// position du crop dans la tile courante
		std::pair<int, int> posIni(	(int) std::max(0, std::min(nXOff-tile->_position.first, (int)szIma.first)),
									(int) std::max(0, std::min(nYOff-tile->_position.second, (int)szIma.second))    );
		
		std::pair<int, int>  posFin(	(int)std::min(nXSize+nXOff-tile->_position.first, (int)szIma.first), 
									(int)std::min(nYSize+nYOff-tile->_position.second, (int)szIma.second)  ) 	;
		
		if ((verbose)||(verbose2)) std::cout<<"tile "<<it->first<<" -> position crop: "<<posIni.first<<" x "<<posIni.second<<" -> "<<posFin.first<<" x "<<posFin.second<<std::endl;
		if (posFin.second<=posIni.second || posFin.first<=posIni.first) continue;
		
		///si on a trop de tuiles dans la multiImage, on verifie que l'image n'a pas ete fermee pour limiter le nombre de ports ouverts
		TileDataPtr tileDataPtr = it->second;
		_openTile(tileDataPtr);
		
		if (verbose) std::cout<<"MultiImage::Read ==== crop a charger dans la dalle "<<it->first<<" de l'image virtuelle: "<<posIni.first<<" x "<<posIni.second<<" -> "<<posFin.first<<" x "<<posFin.second<<std::endl;
		
		int NTileCol = posFin.first-posIni.first;
		int NTileLin = posFin.second-posIni.second;	
			
		//shift canal
		int nCan0 = nBand0  - (int)tile->_band0;
		
		if (nCan0 < 0) nCan0 = 0;
		//nb de bandes a lire
		int numCan = (nNBands + nBand0) - (int)tile->_band0;
		if (numCan > nNBands) numCan = nNBands;
		if (numCan <= 0 || nCan0 >= (int)tile->_nBands)
		{
			//rien a lire dans cette image
			if (verbose) std::cout<<"pas de canal a lire dans cette tuile: nBand0 = "<<nBand0<<"  | tile->(band0 x nBands) == "<<tile->_band0<<" x "<<tile->_nBands<<std::endl;
			continue;
		}
		
		if (numCan >(int)(tile->_nBands  - nCan0))
		{ 
			//nb de canaux a lire dans cette image
			IGN_ASSERT(tile->_nBands>0);
			numCan = (int)tile->_nBands  - nCan0;
		}
		
		//on place le pointeur du buffer au premier pixel a lire
		int lineOffset  =	(posIni.second + tile->_position.second - nYOff)/nDeZoom;
		int pixelOffset =	(posIni.first + tile->_position.first - nXOff)/nDeZoom;
		
		// corr NB / RP
		int bdeOffset	=	(int)(tile->_band0 - nBand0);
		if (bdeOffset < 0) bdeOffset = 0;

		if (verbose) 
			{
			std::cout<<"nCan0 == "<<nCan0<<std::endl;
			std::cout<<"numCan == "<<numCan<<std::endl;
			std::cout<<"lineOffset == "<<lineOffset<<std::endl;
			std::cout<<"pixelOffset == "<<pixelOffset<<std::endl;
			std::cout<<"bdeOffset == "<<bdeOffset<<std::endl;
			std::cout<<"NTileCol == "<<NTileCol<<std::endl;
			std::cout<<"NTileLin == "<<NTileLin<<std::endl;
			}

		if (numCan<=0) 
		{
			continue;
		}

		size_t shift = (lineOffset * nLineSpace + nPixelSpace * pixelOffset + bdeOffset * nBandSpace)*numeric::TypeSize(eBufferType);
		if (verbose) std::cout<<"on decale le pointeur de "<<shift<<" bits"<<std::endl;
		
		char* charPtr = (char*)(buffer);
		void* ptr = (void*)(charPtr + shift);
			
		if (verbose) std::cout<<"on lit "<<numCan<<" bandes de la tile "<<it->first<<" a partir du pixel "<<posIni.first<<" x "<<posIni.second<<" et du canal "<<nCan0<<" avec Sz = "<<NTileCol<<" "<<NTileLin<<std::endl;
		if (verbose) std::cout<<"buffer = "<<buffer<<" | ptr == "<<ptr<<std::endl;
			
		//on lit le crop dans le buffer
		try
		{
			tile->_image->read(	posIni.first, posIni.second,nCan0, NTileCol, NTileLin, numCan, nDeZoom,ptr, eBufferType, nPixelSpace, nLineSpace, nBandSpace);
		}
		catch(...)
		{
			std::ostringstream oss;
			oss<<"[MultiImage::read] unable to read tile "<<tile->_name;
			IGN_THROW_EXCEPTION(oss.str());
		}
		if (verbose) std::cout<<"crop in tile  "<<it->first <<" done!" <<std::endl;
	}
	
	if (verbose) 		std::cout<<"[MultiImage::read] done "<< std::endl << std::endl;
}

///
///
///
void MultiImage::_checkClose(const TileDataPtr &currentTile) const
{
	bool verbose = false;
	if (verbose) std::cout<<"[MultiImage::_checkClose] "<<currentTile->_name<<std::endl;
	if (_numOpenTiles < _maxOpenTiles) return;
	
	_numOpenTiles = 0;
	std::map<size_t, TileDataPtr>::const_iterator it;
	for (it = _data.begin(); it != _data.end() ; ++it)
	{
		const TileDataPtr& tile = it->second;
		if (tile == currentTile) 
		{
			if (verbose) std::cout<<"[MultiImage::_checkClose] keeping open: "<<tile->_name<<std::endl;
			_numOpenTiles++;
			continue;
		}
		if (! (tile->_image)) 
		{ 
			if (verbose) std::cout<<"[MultiImage::_checkClose] warning: "<<tile->_name<<" has no valid image"<<std::endl;
			continue;
		}
		///si on a trop de tuiles ouvertes dans la multiImage, on verifie que l'image n'a pas ete ferme pour limiter le nombre de ports ouverts
		if (! tile->_image->isInRAM() && numTiles()>=_maxOpenTiles)
		{
			io::ImageIO& imageIO = const_cast<io::ImageIO&>(tile->_image->as<io::ImageIO>());
			if(imageIO.hasDriver())
			{
				if (verbose) std::cout<<"[MultiImage::_checkClose] closing: "<<tile->_name<<std::endl;
				imageIO.close();
			}
		}
	}
}

///
///
///
void	MultiImage::	_openTile(TileDataPtr& tile) const
{
	bool verbose = false;
    if (verbose) std::cout << "[MultiImage::_openTile] : "<<tile->_name<<std::endl;
	
	if (!tile->_image->isInRAM())
	{
		io::ImageIO& imageIO = const_cast<io::ImageIO&>(tile->_image->as<io::ImageIO>());
		if(!imageIO.hasDriver())
		{
			if (verbose) std::cout<<"[MultiImage::_openTile] opening: "<<tile->_name<<std::endl;
            bool useExtBefore = io::driver::ImageDriverManager::Instance()->useExtension();
            io::driver::ImageDriverManager::Instance()->useExtension(true);
			imageIO.open(tile->_name);
            if (!useExtBefore)io::driver::ImageDriverManager::Instance()->useExtension(false);
			_numOpenTiles++;
			
			///si on a trop de tuiles ouvertes dans la multiImage, on les ferment toutes sauf l'image courante
			_checkClose(tile);
		}
	}
}

///
///
///
void MultiImage::write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, 
								  void *buffer, eTypePixel eBufferType,
								  int nPixelSpace, int nLineSpace, int nBandSpace)
{
	bool verbose = false;
	
	std::vector<bool> vCanalRempli(numBands(), false);
	
	if (verbose)
	{
		std::cout<<"MultiImage ::Write: "<<std::endl;
		std::cout<<"nXOff nYOff: "<<nXOff<<" x "<<nYOff<<std::endl;
		std::cout<<"nXSize nYSize: "<<nXSize<<" x "<<nYSize<<std::endl;		
	}
	
	std::map<size_t, TileDataPtr>::const_iterator it;
	
	if (verbose) std::cout<<"nbTile == "<<_data.size()<<std::endl;
	ign::unique_ptr<boost::progress_display> progressPtr;
	if (_verbose>0)
	{
		std::ostringstream oss;
		oss<<"writing a multiImage with "<<_data.size()<<" tiles...\n";
		progressPtr.reset(new boost::progress_display(static_cast<unsigned long>(_data.size()), std::cout, oss.str()));
	}
	for (it = _data.begin(); it != _data.end() ; ++it)
	{
		if (_verbose>0)++(*progressPtr.get());
		TileDataPtr tile = it->second;
		if (verbose) std::cout<<"tile == "<<it->first<<" / "<<tile->_nBands<<std::endl;
		if (! (tile->_image)) 
		{ 
			std::cout<<"MultiImage::write ==== image NULL pour tile "<<it->first<<"!!!"<<std::endl; continue;
		}
		
		//ImageSize szIma= tile->_image->size(1);
		ImageSize szIma= tile->_size;
		
		if (verbose) std::cout<<"tile -> taille: "<<szIma.first<<" x "<<szIma.second<<"; position "<<tile->_position.first<<" x "<<tile->_position.second<<std::endl;
		
		// position du crop dans la tile courante
		
		std::pair<int, int> posIni(	(int) std::max(0, std::min(nXOff-tile->_position.first, (int)szIma.first)),
								   (int) std::max(0, std::min(nYOff-tile->_position.second, (int)szIma.second))    );
		
		std::pair<int, int>  posFin(	(int)std::min(nXSize+nXOff-tile->_position.first, (int)szIma.first), 
									(int)std::min(nYSize+nYOff-tile->_position.second, (int)szIma.second)  ) 	;
		
		if (verbose) std::cout<<"tile "<<it->first<<" -> position crop: "<<posIni.first<<" x "<<posIni.second<<" -> "<<posFin.first<<" x "<<posFin.second<<std::endl;
		if (posFin.second<=posIni.second || posFin.first<=posIni.first) continue;
		
		_openTile(tile);
		
		if (verbose) std::cout<<"MultiImage::write ==== crop a mettre a jour dans la dalle "<<it->first<<" (band0: "<<tile->_band0<<", nbands:"<<tile->_nBands<<") de l'image virtuelle: "<<posIni.first<<" "<<posFin.first<<"; "<<posIni.second<<" "<<posFin.second<<std::endl;
		
		int NTileBands = static_cast<int>(tile->_image->numBands());
		int NTileCol = static_cast<int>(posFin.first - posIni.first);
		int NTileLin = static_cast<int>(posFin.second - posIni.second);
		
		if (verbose) std::cout<<"NTileBands == "<<NTileBands<<std::endl;
		//on ecrit canal par canal
		
		///ex: _vBands = {2, 1} -> bde 2 de la tile a placer dans le canal 0 de l'image
		///						-> bde 1 de la tile a placer dans le canal 1 de l'image
		///						-> bde 0 de la tile est ignore
		
		for (size_t iCan = 0; iCan < tile->_nBands; ++iCan)
		{
			int nCanal = tile->_nBands + tile->_band0 - 1;			
			if (verbose) std::cout<<"nCanal == "<<nCanal<<std::endl;
			
			if (nCanal<0 || nCanal>(NTileBands-1)) continue;
			if (nCanal<nBand0) continue;
			if (nCanal>(nBand0+nNBands-1)) continue;
			
			//on place le pointeur du buffer au premier pixel a ecrire
			int lineOffset  =	posIni.second + tile->_position.second - nYOff;
			int pixelOffset =	posIni.first + tile->_position.first - nXOff;
			int bdeOffset	=	nCanal-nBand0;

			if (verbose) std::cout<<"lineOffset == "<<lineOffset<<std::endl;
			if (verbose) std::cout<<"pixelOffset == "<<pixelOffset<<std::endl;
			if (verbose) std::cout<<"bdeOffset == "<<bdeOffset<<std::endl;
			if (verbose) std::cout<<"NTileCol == "<<NTileCol<<std::endl;
			if (verbose) std::cout<<"NTileLin == "<<NTileLin<<std::endl;
			
			size_t shift = (lineOffset * nLineSpace + nPixelSpace * pixelOffset + bdeOffset * nBandSpace)*numeric::TypeSize(eBufferType);
			if (verbose) std::cout<<"on decale le pointeur de "<<shift<<" bits"<<std::endl;
			
			
			char* charPtr = (char*)(buffer);
			void* ptr = (void*)(charPtr + shift);
			
			if (verbose) std::cout<<"on ecrit dans le canal "<<nCanal<<" de la tile "<<it->first<<" a partir du pixel "<<posIni.first<<" x "<<posIni.second<<" avec Sz = "<<NTileCol<<" "<<NTileLin<<std::endl;
			if (verbose) std::cout<<"buffer = "<<buffer<<" | ptr == "<<ptr<<std::endl;
			
			//on ecrit le crop dans le buffer
			tile->_image->write(posIni.first, posIni.second, 
								nCanal, NTileCol, NTileLin, 1,
								ptr, eBufferType, nPixelSpace, nLineSpace, nBandSpace);
			
			if (verbose) std::cout<<"paste in tile  "<<it->first <<" done!" <<std::endl;
		}
	}
}

///
///
///
ImageSize MultiImage::size(int aDezoom)const
{
	bool verbose = false;
	
	//IGN_ASSERT (numTiles()!=0);
	int Cmin(0), Lmin(0), Cmax(0), Lmax(0);

	if (numTiles() == 0)
	{	
		return std::make_pair((size_t)(Cmax-Cmin), (size_t)(Lmax-Lmin));
	}
	
	if (verbose) std::cout<<"Nombre de dalles : "<<numTiles()<<std::endl;
	
	std::map<size_t, TileDataPtr>::const_iterator it;
	
	for (it = _data.begin(); it != _data.end() ; ++it)
	{
		if (verbose) std::cout<<"position of tile "<<it->second->_name<<": "<<it->second->_position.first<<" x "<<it->second->_position.second<<std::endl;
		
		int cmin, lmin, cmax, lmax;		
		cmin = static_cast<int>(it->second->_position.first);
		cmax = cmin + it->second->_size.first;
		lmin = it->second->_position.second;
		lmax = lmin + static_cast<int>(it->second->_size.second);
		
		if (it==_data.begin())
		{
			Cmin=cmin;
			Cmax=cmax;
			Lmin=lmin;
			Lmax=lmax;
		}
		else
		{
			Cmin=std::min(Cmin, cmin);
			Cmax=std::max(Cmax, cmax);
			Lmin=std::min(Lmin, lmin);
			Lmax=std::max(Lmax, lmax);			
		}
		if (verbose) std::cout<<"bbox cur: "<<Cmin<<" "<<Cmax<<" x "<<Lmin<<" "<<Lmax<<std::endl;
	}	
	return std::make_pair((size_t)(Cmax-Cmin), (size_t)(Lmax-Lmin));
}

///
///
///
void MultiImage::addTile(const ign::shared_ptr<Image>& img,  std::pair<int, int> const & position, size_t band0, size_t nBands, std::string tileName)
{
    bool verbose = false;
	if(verbose) std::cout << "[MultiImage::addTile]" << std::endl;
	if (!img->isReadable() && isReadable() &&  numTiles() > 0)
	{
		IGN_THROW_EXCEPTION("[MultiImage::addTile] ajout d'une tuile non accessible en lecture dans un MultiImage en mode lecture");
	}
	if (!img->isWritable() && isWritable() &&  numTiles() > 0)
	{
		IGN_THROW_EXCEPTION("[MultiImage::addTile] ajout d'une tuile non accessible en ecriture dans un MultiImage en mode ecriture");
	}
	if (img->isInRAM() != isInRAM() &&  numTiles() > 0)
	{
		IGN_THROW_EXCEPTION("[MultiImage::addTile] ajout d'une tuile dans un type RAM/file incompatible avec le MultiImage");
	}
	ImageSize sz = img->size(1);		
	if(verbose) std::cout << "Pos image " << position.first << " " << position.second << " "<<band0<<" "<<nBands<<std::endl;
	if(verbose) std::cout << "Taille image " << sz.first << " " << sz.second << std::endl;
	TileDataPtr tileToAdd(new TileData(img, position, sz, band0, nBands, tileName));
	size_t indice = numTiles();
	
	_numOpenTiles++;

	_numBands = std::max(nBands+band0,_numBands);
	_data.insert(std::make_pair(indice,tileToAdd));
	if(verbose) std::cout << "sz MultiImage " << _numBands << " apres insert tile Id = " << indice<<std::endl;

    // typePixel is defined with the type of the first tile
    if (_data.size()==1)
        _typePixel=img->typePixel();
    
	if (_numOpenTiles>_maxOpenTiles)
    {
        if (verbose) std::cout << "_numOpenTiles : "<<_numOpenTiles<<" | "<<_maxOpenTiles<<std::endl;
        this->close();
    }
}

///
///
///
void	MultiImage::addTileNoCheck(const ign::shared_ptr<Image>& img, ImageSize const & sz, std::pair<int, int> const & position, ign::image::eTypePixel typePixel,  size_t band0, size_t nBands, std::string tileName)
{
	bool verbose = false;
	if(verbose) std::cout << "[MultiImage::addTile]" << std::endl;
	
	if(verbose) std::cout << "Pos image " << position.first << " " << position.second << " "<<band0<<" "<<nBands<<std::endl;
	if(verbose) std::cout << "Taille image " << sz.first << " " << sz.second << std::endl;
	//TileData * tileToAdd = new TileData(img, position, sz, band0, nBands, tileName);
	TileDataPtr tileToAdd(new TileData(img, position, sz, band0, nBands, tileName));

	size_t indice = numTiles();
	
	_numOpenTiles++;
	
	_numBands = std::max(nBands+band0,_numBands);
	_data.insert(std::make_pair(indice,tileToAdd));
	if(verbose) std::cout << "sz MultiImage " << _numBands << " apres insert tile Id = " << indice<<std::endl;
	
	// typePixel is defined with the type of the first tile
	if (_data.size()==1)	_typePixel=typePixel;
}


///
///
///
size_t MultiImage::numTiles()const
{
	return _data.size();
}

///
///
///
void MultiImage::_box(size_t i, std::pair<int, int> &position, ImageSize& taille) const
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
	position= it->second->_position;
	taille = it->second->_size;
}

///
///
///
void MultiImage::translate(int x, int y)
{
	bool verbose = false;
	if (verbose) std::cout<<" translate: "<<x<<" "<<y<<std::endl;
    for (size_t i=0; i<numTiles(); i++)
	{
		TileDataPtr tile = _data[i];
		if (verbose) std::cout<<"pos ini de la dalle "<<tile->_name<<": "<< tile->_position.first<<" "<<tile->_position.second<<std::endl;
		tile->_position=std::make_pair(tile->_position.first+x, tile->_position.second+y);
		if (verbose) std::cout<<"pos fin de la dalle "<<tile->_name<<": "<<tile->_position.first<<" "<<tile->_position.second<<std::endl;
	}
}

///
///
///
std::pair<int, int>	MultiImage:: origin()const
{
	int posX = std::numeric_limits<int>::max();
	int posY = std::numeric_limits<int>::max();
	
	for (std::map<size_t, TileDataPtr>::const_iterator it = _data.begin(); 	it != _data.end(); ++it)
	{
		const TileDataPtr& tile = it->second;
		
		if (tile->_position.first < posX) posX = tile->_position.first;
		if (tile->_position.second < posY) posY = tile->_position.second;
	}
	
	return std::make_pair(posX, posY);
}

///
///
///
void MultiImage::clear()
{   
	_data.clear();
}

///
///
///
bool MultiImage::_isInTile(size_t numTile, const double &xIma, const double& yIma, size_t canal) const
{
	if(numTile >= numTiles()) return false;
	
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(numTile);
	IGN_ASSERT(it != _data.end());
	
	if (it->second->_position.first>xIma) return false;
	if (it->second->_position.second>yIma) return false;
	if (canal<it->second->_band0) return false;
	if (canal>=(it->second->_band0 + it->second->_nBands)) return false;

	
	const ImageSize &sz = it->second->_size;
	if (xIma>=(it->second->_position.first+sz.first)) return false;
	if (yIma>=(it->second->_position.second+sz.second))return false;


	return true;
	
}

///
///
///
bool MultiImage::_isInTile(size_t numTile, const double &xIma, const double& yIma, const ImageSize& sz, size_t canal) const
{
	if (!_isInTile(numTile, xIma, yIma, canal)) return false;
	if (!_isInTile(numTile, xIma+sz.first, yIma+sz.second, canal)) return false;
	return true;
}

///
///
///
bool	MultiImage::isIn(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands) const
{
	bool verbose = false;
	if (verbose) std::cout<<"isIn: "<<nXOff<<" x "<<nYOff<<" x "<<nBand0<<" | "<<nXSize<<" x" <<nYSize<<" x "<<nNBands<<std::endl;
	
	for (size_t i = 0; i <numTiles(); ++i)
	{
		for (size_t can = nBand0; can <(size_t)(nNBands+nBand0); ++can)
		{
			//crop intersects or inside tile i, check the 4 corners of crop
			if (_isInTile(i, nXOff, nYOff, can))
			{
				return true;
			}
			else if (_isInTile(i, nXOff +nXSize, nYOff + nYSize, can))
			{
				return true;
			}
			else if (_isInTile(i, nXOff, nYOff + nYSize, can))
			{
				return true;
			}
			else if (_isInTile(i, nXOff +nXSize, nYOff, can))
			{
				return true;
			}
			//tile i inside crop (and csqly : tile::position inside crop)
			else 
			{
				std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
				IGN_ASSERT(it != _data.end());
				//verbose = (it->second->_name == "/DATA/CORSE/ECW/IMAGES/20-2007-LA2E-1/20-2007-1170-1620-LA2E-C07.ecw");
				if (verbose) std::cout<<"nXOff x nYOff == "<< nXOff<<" x " <<nYOff<<" | "<<nXSize<<" x "<<nYSize<<std::endl;
				if (verbose) std::cout<<"tile "<<i<<" == "<< it->second->_name<<":" <<it->second->_position.first<<" x "<<it->second->_position.second<<std::endl;
				if (it->second->_band0!=can) continue;				
				if (it->second->_position.first<nXOff) continue;
				if (it->second->_position.second<nYOff) continue;
				if (it->second->_position.first>=(nXOff+nXSize)) continue;
				if (it->second->_position.second>=(nYOff+nYSize)) continue;
				return true;
			}
		}
	}
	return false;
}

///
///
///
int MultiImage::numTile(const double &xIma, const double& yIma, size_t canal) const
{
     
	std::map<size_t,TileDataPtr>::const_iterator it;
	for (it = _data.begin() ; it !=_data.end() ; ++it)
	{		
		if (it->second->_position.first>xIma) continue;
		if (it->second->_position.second>yIma) continue;
		if (canal<it->second->_band0) continue;
		if (canal>=(it->second->_band0 + it->second->_nBands))  continue;		

		const ImageSize &sz = it->second->_size;
		if (xIma>=it->second->_position.first+sz.first) continue;
		if (yIma>=it->second->_position.second+sz.second) continue;
		return  static_cast<int>(it->first);
	}
	return -1;
}

///
///
///
double	MultiImage::get(int c, int l, int b) const
{
    //si on a un cache, on fait directement un read
    if (this->memoryCache().isValid())
    {
        float val=0.;
        read(c,l,b,1,1,1,1,(void*)(&val),ign::numeric::eFloat,1,0,0);
        return (double)val;
    }
    int idTile = numTile(c, l, b);
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(idTile);
	if (it == _data.end())
	{
		if (_throwExc)
		{
			std::ostringstream oss;
			oss<<"[MultiImage::get] No data under coordinates ("<<c<<", "<<l<<", "<<b<<")";
			IGN_THROW_EXCEPTION(oss.str());
		}
		else
			return _defaultValue;
	}
	TileDataPtr tileDataPtr = it->second;
	_openTile(tileDataPtr);

	return it->second->_image->get(c-it->second->_position.first, l-it->second->_position.second, b-it->second->_band0);
}

///
///
///
void	MultiImage::set(int c, int l, int b, double val)
{
	int idTile = numTile(c, l, b);
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(idTile);
	if (it == _data.end())
	{
		if (_throwExc) IGN_THROW_EXCEPTION("[MultiImage::set] No data under coordinates");
		else return;
	}
	///si on a trop de tuiles dans la multiImage, on verifie que l'image n'a pas ete fermee pour limiter le nombre de ports ouverts
	TileDataPtr tileDataPtr = it->second;
	_openTile(tileDataPtr);
	
	it->second->_image->set(c-it->second->_position.first, l-it->second->_position.second, b-it->second->_band0, val);
	
}

///
///
///
size_t MultiImage::maxOpenTiles()const
{
    return _maxOpenTiles;
}

///
///
///
void MultiImage::maxOpenTiles(size_t max)
{
    _maxOpenTiles = max;
}

///
///
///
const ign::shared_ptr<Image>&	MultiImage::imageN(size_t const & n) const 
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(n);
	IGN_ASSERT(it != _data.end());
	
	return it->second->_image;
	
		
}

///
///
///
ign::shared_ptr<Image>&	MultiImage::imageN(size_t const & n) 
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(n);
	IGN_ASSERT(it != _data.end());
	
	return it->second->_image;
	
}

///
///
///
const std::pair<int, int>	MultiImage::getTilePosition(size_t const & i) const
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
	IGN_ASSERT(it != _data.end());

	return it->second->_position;


}

///
///
///
const std::pair<size_t, size_t>	MultiImage::getTileSize(size_t const & i) const
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
	IGN_ASSERT(it != _data.end());

	return it->second->_size;
}

///
///
///
const std::string		MultiImage::getTileName(size_t const & i)const
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
	IGN_ASSERT(it != _data.end());

	return it->second->_name;

}

///
///
///
std::string		MultiImage::getTileName(size_t const & i)
{
	std::map<size_t,TileDataPtr>::const_iterator it = _data.find(i);
	IGN_ASSERT(it != _data.end());

	return it->second->_name;

}
			

///
///
///
ign::image::detail::ImageCache&		MultiImage::	memoryCache()
{
	return _memoryCache;
}

///
///
///
const ign::image::detail::ImageCache&	MultiImage::memoryCache()const
{
	return _memoryCache;
}


///
///
///
void	MultiImage::		throwExc(bool thr)
{
	_throwExc = thr;
}

///
///
///
bool	MultiImage::		throwExc() const
{
	return _throwExc;
}

///
///
///
void	MultiImage::		verbose(int d)
{
	_verbose = d;
}

///
///
///
int		MultiImage::		verbose() const
{
	return _verbose;
}
