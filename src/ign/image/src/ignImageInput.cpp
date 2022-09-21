#include <iostream>

#include <boost/format.hpp>

#include <ignLog.h>

#include <ignImageIO.h>
#include <ignImageInput.h>
#include <ignBufferImage.h>
#include <ignImageDriverManager.h>
#include <ignImageDriver.h>


namespace ign 
	{
	namespace image 
		{
		namespace io 
			{
			
			///
			///
			///			
			ImageInput::ImageInput(): ImageIO(), _memoryCache(this)
			{
			}
			
			///
			///
			///			
			ImageInput::~ImageInput()
			{
			}
			
			///
			///
			///			
            ImageInput::ImageInput(ImageInput const &src, bool const &useExtension):ImageIO(), _memoryCache(this)
			{
                initialize(src.getFileName(), useExtension);
			}
			
			///
			///
			///
            ImageInput::ImageInput(std::string const &fullPathName, bool const &useExtension):ImageIO(), _memoryCache(this)
			{
                initialize(fullPathName, useExtension);
			}
			
			///
			///
			///
            void ImageInput::open(std::string const &fullPathName)
			{
                initialize(fullPathName);
			}
			
			///
			///
			///
            void ImageInput::initialize(std::string const &fullPathName, bool const &useExtension)
			{
                close();
				_driver=driver::ImageDriverManager::Instance()->open(fullPathName, useExtension);
                if (!_driver) LOG_NOTICE << boost::format("ImageInput could not open %1%.") % fullPathName << std::endl;
			}
			
			///
			///
			///			
			bool	ImageInput::	isReadable() const
			{
				return true;
			}
			
			///
			///
			///
			bool	ImageInput::	isWritable() const
			{
				return false;
			}
			
			///
			///
			///
			bool	ImageInput::	isInRAM() const
			{
				return false;
			}

			///
			///
			///
			bool	ImageInput::	isTiled() const
			{
				return false;
			}



			///
			///
			///
			void ImageInput::read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void *buffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)const
			{
				bool verbose = 0;
				//on regarde si on peut lire le cache
				if (_memoryCache.isValid())
				{
					if (!_memoryCache.read(C0,L0,Band0,NRows,NLines,NBands,deZoom,buffer,bufferType,nPixelSpace,nLineSpace,nBandSpace))
					{
						if (verbose) std::cout<<"_readFromCache has failed -> _readFromFile "<<C0<<" x "<<L0<<" - "<<NRows<<" x "<<NLines<<" x "<<NBands<<std::endl;
						_driver->read(C0,L0,Band0,NRows,NLines,NBands,deZoom,buffer,bufferType,nPixelSpace,nLineSpace,nBandSpace);
					}
					else 
					{		
						if (verbose) std::cout<<"_readFromCache ok: "<<C0<<" x "<<L0<<" - "<<NRows<<" x "<<NLines<<" x "<<NBands<<std::endl;
					}
				}
				else
				{
					if (verbose) std::cout<<"no cache : read "<<C0<<" x "<<L0<<" - "<<NRows<<" x "<<NLines<<" x "<<NBands<<std::endl;
					_driver->read(C0,L0,Band0,NRows,NLines,NBands,deZoom,buffer,bufferType,nPixelSpace,nLineSpace,nBandSpace);
				}
			}
			
			///
			///
			///
			void ImageInput::read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void **tabBuffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace)const
			{
				if (_driver)
				{
					for(int k=0;k<NBands;++k)
					{
						read(C0,L0,Band0+k,NRows,NLines,1,deZoom,tabBuffer[k],bufferType,nPixelSpace,nLineSpace,0);
					}
				}
			}
			
			///
			///
			///
			void ImageInput::readLine(int L,void *buffer,eTypePixel bufferType)const
			{
				if (_driver)
				{
					int NRows,NLines;
					_driver->size(NRows,NLines);
					int NBands=_driver->numBands();
					_driver->read(0,L,0,NRows,1,NBands,1,buffer,bufferType,NBands,0,1);
				}
			}
			
			///
			///
			///
			void ImageInput::readLine(int L,unsigned char *buffer)const
			{
				readLine(L,(void*)buffer,ign::numeric::eUnsignedChar);
			}
			
			///
			///
			///
			void ImageInput::readLine(int L,unsigned short *buffer)const
			{
				readLine(L,(void*)buffer,ign::numeric::eUnsignedShort);
			}
			
			void ImageInput::readLine(int L,short *buffer)const
			{
				readLine(L,(void*)buffer,ign::numeric::eSignedShort);
			}
			
			///
			///
			///
			void ImageInput::readLine(int L,float *buffer)const
			{
				readLine(L,(void*)buffer,ign::numeric::eFloat);
			}
			
			///
			///
			///
			double ImageInput::operator () (int colonne,int ligne,int canal)const
			{
				float val=0.;
				read(colonne,ligne,canal,1,1,1,1,(void*)(&val),ign::numeric::eFloat,1,0,0);
				return (double)val;
			}
			
			///
			///
			///
			double	ImageInput::get(int c, int l, int b) const
			{
                return (*this)(c, l, b);
			}
			
			///
			///
			///
			void	ImageInput::set(int c, int l, int b, double val)
			{
				IGN_THROW_EXCEPTION("[ImageInput::set] image not writable");
			}
			
			///
			///
			///
			bool ImageInput::getColorTable(int bandNum, ColorTable &colorTable)const
			{
				if (_driver)
					return _driver->getColorTable(bandNum,colorTable);
				return false;
			}
			
			///
			///
			///
			detail::ImageCache&		ImageInput::memoryCache()
			{
				return _memoryCache;
			}
			
			///
			///
			///
			const detail::ImageCache&	ImageInput::memoryCache()const
			{
				return _memoryCache;
			}

			}
		}
	}
