#include <iostream>

#include <boost/format.hpp>

#include <ignImage.h>
#include <ignBufferImage.h>
#include <ignImageOutput.h>
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
			ImageOutput::ImageOutput(std::string const &fullPathName, int NRows, int NLines, int NBands, eTypePixel imageType, data::Object const *  options, data::Object const *  metadata): ImageIO()
			{
				std::string fullPathName_local = fullPathName;
#ifdef WIN32
				for(size_t i=0;i<fullPathName_local.size();++i)
					if (fullPathName_local[i]=='/')
						fullPathName_local[i]='\\';
#else
				for(size_t i=0;i<fullPathName_local.size();++i)
					if (fullPathName_local[i]=='\\')
						fullPathName_local[i]='/';
#endif
				
				// utilisation d'un vector plutot qu'un pointeur pour etre plus robuste (en particulier en cas d'exception)
				std::vector<eTypePixel> tabType;
				//eTypePixel* tabType = new eTypePixel[NBands];
				for(int i=0;i<NBands;++i)
				{
                    tabType.push_back(imageType);
				}
                IGN_ASSERT(!tabType.empty());
                
				_driver=driver::ImageDriverManager::Instance()->create(fullPathName,NRows,NLines,NBands,/*tabType*/&(tabType[0]),options,metadata);
			}
			
			///
			///
			///
			ImageOutput::ImageOutput(std::string const &fullPathName, int NRows, int NLines, int NBands, eTypePixel* tabType, data::Object const *  options, data::Object const *  metadata)
			{
				_driver=driver::ImageDriverManager::Instance()->create(fullPathName,NRows,NLines,NBands,tabType,options,metadata);
			}
			
			///
			///
			///
			ImageOutput::ImageOutput(std::string const &fullPathName)
			{
				_driver=driver::ImageDriverManager::Instance()->update(fullPathName);
			}
			
			///
			///
			///
			void ImageOutput::open(std::string const &fullPathName)
			{
                close();
				_driver=driver::ImageDriverManager::Instance()->update(fullPathName);
				if (!_driver) LOG_NOTICE << boost::format("ImageOutput could not open %1%.") % fullPathName << std::endl;
			}
			///
			///
			///
            ImageOutput::~ImageOutput()
            {
                
            }

			///
			///
			///		
			bool	ImageOutput::	isReadable() const
			{
				return false;
			}
			
			///
			///
			///
			bool	ImageOutput::	isWritable() const
			{
				return true;
			}
			
			///
			///
			///
			bool	ImageOutput::	isInRAM() const
			{
				return false;
			}

			///
			///
			///
			bool	ImageOutput::	isTiled() const
			{
				return false;
			}

						
			///
			///
			///
			void ImageOutput::write(int C0,int L0, int band0, int NCols, int NLines, int NBands, void *buffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)
			{
				if (hasDriver())
				{
					/*std::cout<<"C0 = "<<C0<<std::endl;
					std::cout<<"L0 = "<<L0<<std::endl;		
					std::cout<<"band0 = "<<band0<<std::endl;
					std::cout<<"NCols = "<<NCols<<std::endl;
					std::cout<<"NLines = "<<NLines<<std::endl;
					std::cout<<"NBands = "<<NBands<<std::endl;
					std::cout<<"buffer = "<<buffer<<std::endl;
					std::cout<<"bufferType = "<<bufferType<<std::endl;
					std::cout<<"nPixelSpace = "<<nPixelSpace<<std::endl;
					std::cout<<"nLineSpace = "<<nLineSpace<<std::endl;
					std::cout<<"nBandSpace = "<<nBandSpace<<std::endl;*/

					_driver->write(C0,L0,band0,NCols,NLines,NBands,buffer,bufferType,nPixelSpace,nLineSpace,nBandSpace);
				}
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
			}
			
			///
			///
			///
			void ImageOutput::write(int C0, int L0, int Band0, int NCols, int NLines, int NBands, void **tabBuffer, eTypePixel bufferType,int nPixelSpace, int nLineSpace)
			{
				if (hasDriver())
				{
					for(int k=0;k<NBands;++k)
					{
						write(C0,L0,k+Band0,NCols,NLines,1,(void*)tabBuffer[k],bufferType,nPixelSpace,nLineSpace,0);
					}
				}
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
			}
			
			///
			///
			///
			void ImageOutput::writeLine(int L,void *buffer,eTypePixel bufferType)
			{
				if (hasDriver())
				{
					int NRows,NLines;
					_driver->size(NRows,NLines);
					size_t NBands=_driver->numBands();
					_driver->write(0,L,0,NRows,1,NBands,buffer,bufferType,NBands,0,1);
				}
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
			}
			
			///
			///
			///
			void ImageOutput::setPixel (int colonne,int ligne, int band, double val)
			{
				if (hasDriver())
				{
					float valFloat=(float)val;
					_driver->write(colonne,ligne,band,1,1,1,&valFloat,ign::numeric::eFloat,1,0,0);
				}
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
			}
			
			///
			///
			///
			double	ImageOutput::get(int c, int l, int b) const
			{
				IGN_THROW_EXCEPTION("[ImageOutput::set] image not readable");
			}
			
			///
			///
			///
			void	ImageOutput::set(int c, int l, int b, double val)
			{
				setPixel(c, l, b, val);
			}
			
			///
			///
			///
			void ImageOutput::writeLine(int L,unsigned char *buffer)
			{
				writeLine(L, (void*)buffer,ign::numeric::eUnsignedChar);
			}
			
			///
			///
			///
			void ImageOutput::writeLine(int L,unsigned short *buffer)
			{
				writeLine(L, (void*)buffer,ign::numeric::eUnsignedShort);
			}
			
			///
			///
			///
			void ImageOutput::writeLine(int L,signed short *buffer)
			{
				writeLine(L, (void*)buffer,ign::numeric::eSignedShort);
			}
			
			///
			///
			///
			void ImageOutput::writeLine(int L,float *buffer)
			{
				writeLine(L, (void*)buffer,ign::numeric::eFloat);
			}
			
			///
			///
			///
			bool ImageOutput::setGeoref(double ULCx, double ULCy, double pasX, double pasY,std::string const &sysProjSD,ign::image::georef::PixConv const & pixConv)
			{
				double geoTransform[6];
				geoTransform[0] = ULCx;
				geoTransform[1] = pasX;
				geoTransform[2] = 0;
				geoTransform[3] = ULCy;
				geoTransform[4] = 0;
				geoTransform[5] = pasY;
				if (hasDriver())
					return _driver->setGeoref(geoTransform,sysProjSD,pixConv);
				return false;
			}
			
			///
			///
			///
			bool ImageOutput::setGeoref(double geoTransform[6],std::string const &sysProjSD,ign::image::georef::PixConv const & pixConv)
			{
				if (hasDriver())
					return _driver->setGeoref(geoTransform,sysProjSD,pixConv);
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
				return true;
			}
			
			///
			///
			///
			
			bool ImageOutput::setColorTable(int bandNum, ColorTable const & colorTable)
			{
				if (hasDriver())
				{
					return _driver->setColorTable(bandNum, colorTable);
				}
				else
				{
					IGN_THROW_EXCEPTION("[ImageOutput::write] ImageOutput requires a valid driver");
				}
				return true;
				
			}
			

		}
	}
}
