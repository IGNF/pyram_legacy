#ifdef WIN32
#include <io.h>
#endif

#include <fcntl.h>

#include <ignMPDImageDriver.h>
#include <ignImageInput.h>

#define TAG_MPD_META_TILE_W 40000
#define TAG_MPD_META_TILE_H 40001
#define TAG_IMAGE_W 256
#define TAG_IMAGE_H 257

// minimal Tiff Tag reader
/*
 * TIFF header.
 */
typedef	struct {
	unsigned short	tiff_magic;	/* magic number (defines byte order) */
#define TIFF_MAGIC_SIZE		2
	unsigned short	tiff_version;	/* TIFF version number */
#define TIFF_VERSION_SIZE	2
	unsigned int	tiff_diroff;	/* byte offset to first directory */
#define TIFF_DIROFFSET_SIZE	4
} TIFFHeader;

typedef	struct {
	unsigned short		tdir_tag;	/* see below */
	unsigned short		tdir_type;	/* data type; see below */
	unsigned int		tdir_count;	/* number of items; length in spec */
	unsigned int		tdir_offset;	/* byte offset to field data */
} TIFFDirEntry;



using namespace ign::image::io::driver;

MPDImageDriver::MPDImageDriver():ImageDriver(), _filename("")
{
    
}

MPDImageDriver::~MPDImageDriver()
{
    
}

void MPDImageDriver::read(int C0, int L0, int Band0, int NRows, int NLines, int NBands, int deZoom, void *buffer, ::ign::image::eTypePixel bufferType,int nPixelSpace, int nLineSpace, int nBandSpace)const
{
    _mpd.read(C0,L0,Band0,NRows,NLines,NBands,deZoom,buffer,bufferType,nPixelSpace,nLineSpace,nBandSpace);
}

ImageDriver* MPDImageDriver::newInstance()const
{
    return (ImageDriver*) new MPDImageDriver;
}

bool MPDImageDriver::open(std::string const &fullPathName)
{
    bool verbose = 0;
    if (verbose) std::cout << "MPDImageDriver::Open "<<fullPathName<<std::endl;
	_filename=fullPathName;
    int TileSizeX = 0;
    int TileSizeY = 0;
    int NC = 0;
    int NL = 0;
	int fd = ::open(fullPathName.c_str(),O_RDONLY, 0);
    if (fd<0)
    {
        if (verbose) std::cout << "[MultiImageReaderMPD] : Read Error : "<<fullPathName<<std::endl;
        return false;
    }
	
    lseek(fd, (off_t) 0, 0);
    TIFFHeader hdr;
	if (::read(fd, (char*) &hdr, sizeof (hdr)) != sizeof (hdr))
    {
		if (verbose) std::cout << "[MultiImageReaderMPD] : TIFF header Error";
			::close(fd);
        return false;
    }
    off_t diroff=hdr.tiff_diroff;
    //if (verbose) std::cout << "diroff : "<<diroff<<std::endl;
    lseek(fd, (off_t) diroff, 0);
    unsigned short dircount;
	::read(fd, (char*) &dircount, sizeof (unsigned short));
    //if (verbose) std::cout << "dircount :: "<<dircount<<std::endl;
    if (dircount == 0)
	{
		::close(fd);
        return false;
    }	
	TIFFDirEntry *dir = new TIFFDirEntry[dircount];
	::read(fd, (char*) dir, dircount*sizeof (TIFFDirEntry));
    for(int i=0;i<dircount;++i)
    {
        if (verbose)
        {
            std::cout << "TAG : "<<dir[i].tdir_tag<<std::endl;
            std::cout << "TYPE : "<<dir[i].tdir_type<<std::endl;
            std::cout << "COUNT : "<<dir[i].tdir_count<<std::endl;
            std::cout << "OFFSET : "<<dir[i].tdir_offset<<std::endl;
        }
        
        if (dir[i].tdir_tag == TAG_MPD_META_TILE_W)
        {
            TileSizeX = dir[i].tdir_offset;
        }
        else if (dir[i].tdir_tag == TAG_MPD_META_TILE_H)
        {
            TileSizeY = dir[i].tdir_offset;
        }
        else if (dir[i].tdir_tag == TAG_IMAGE_W)
        {
            NC = dir[i].tdir_offset;
        }
        else if (dir[i].tdir_tag == TAG_IMAGE_H)
        {
            NL = dir[i].tdir_offset;
        }
        
    }

    delete [] dir;

	::close(fd);
    if ((TileSizeX*TileSizeY*NC*NL)==0)
	{
		return false;
	}
    if (verbose) std::cout << "MPDImageDriver::Open "<<fullPathName<<std::endl;
    if (verbose) std::cout << "Taille de l'image : "<<NC<<" x "<<NL<<" Tuile : "<<TileSizeX<<" x "<<TileSizeY<<std::endl;

    int NX = NC/TileSizeX;
	int NY = NL/TileSizeY;
	if (NC%TileSizeX) ++NX;
	if (NL%TileSizeY) ++NY;
    
    boost::filesystem::path fPN(fullPathName);
    std::string rad = (fPN.parent_path()/fPN.stem()).string();
    for(int j=0;j<NY;++j)
    {
		for(int i=0;i<NX;++i)
		{
			std::ostringstream oss;
			oss << rad << "_Tile_"<<i<<"_"<<j<<".tif";
			std::string nomTile = oss.str();
            if (verbose) std::cout << "Tile : "<<nomTile<<std::endl;
            ign::shared_ptr<ign::image::Image> tile_ptr((ign::image::Image*)new ign::image::io::ImageInput(nomTile));
            _mpd.addTile(tile_ptr,std::pair<int,int>(i*TileSizeX,j*TileSizeY), 0, tile_ptr->numBands(), nomTile);
		}
	}
    
    if (verbose) std::cout << "Total Size : "<<_mpd.numCols()<<" "<<_mpd.numLines()<<" "<<_mpd.numBands()<<std::endl;
    return true;
}

void    MPDImageDriver::size(int &NRows,int &NLines)const
{
	NRows = static_cast<int>(_mpd.numCols());
	NLines = static_cast<int>(_mpd.numLines());
}

size_t	MPDImageDriver::numCols()const
{
    return _mpd.numCols();
}

size_t	MPDImageDriver::numLines()const
{
    return _mpd.numLines();
}

size_t	MPDImageDriver::numBands()const
{
    return _mpd.numBands();
}

::ign::image::eTypePixel MPDImageDriver::typePixel(int numBands)const
{
    return _mpd.typePixel(numBands);
}

std::string	MPDImageDriver::getFileName()const
{
    return _filename.string();
}

std::string	MPDImageDriver::getDriverName()const
{
    return MPDImageDriver::DriverName();
}

std::set<std::string> MPDImageDriver::getExtensions()const
{
    std::set<std::string> retour;
	retour.insert("TIF") ;
	return retour;
}
