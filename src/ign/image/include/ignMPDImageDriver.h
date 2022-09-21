#ifndef __SDI_IMAGEIO_MPDIMAGEDRIVER_H__
#define __SDI_IMAGEIO_MPDIMAGEDRIVER_H__

#include <boost/filesystem.hpp>

#include <ignImageDriver.h>
#include <ignMultiImage.h>


namespace ign{
    namespace image{
		namespace io{
			namespace driver {
                class MPDImageDriver:public ImageDriver
                {
                public:
                    MPDImageDriver();
                    virtual								~MPDImageDriver();
					static std::string					DriverName()	{return "MPDImageDriver";}

                    virtual void						read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void *buffer, ::ign::image::eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)const;
                    virtual ImageDriver*				newInstance()const;
                    virtual bool						open(std::string const &fullPathName);
                    virtual void						size(int &NRows,int &NLines)const;
                    size_t								numCols()const;
                    size_t								numLines()const;
                    virtual size_t						numBands()const;
                    virtual ::ign::image::eTypePixel	typePixel(int numBand)const;
                    virtual std::string					getFileName()const;
                    virtual std::string					getDriverName()const;
                    virtual std::set<std::string>		getExtensions()const;
                private:
                    MultiImage							_mpd;
                    boost::filesystem::path				_filename;


                    
                };
            }
        }
    }
}
#endif