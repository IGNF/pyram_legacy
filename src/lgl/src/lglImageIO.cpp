#include "lgl/lglString.h"
#include "lgl/lglImageIO.h"
#include "lgl/lglAnalyzeHeader.h"
#include "lgl/lglImageManip.h"
#include "lgl/lglNoyau.h"

#include <ignBufferImage.h>
//#include "matis/imageficin.hpp"

// #include <wx/log.h>
// #include <wx/filedlg.h>
// #include "ign/image/segment/lgl/lglwxImageRGB.h"

namespace lgl {

	//============================================================
	/// General image loading
	BOOL readImage(const lgl::Filename& filename, pImageRam& im) {
		bool verbose = false;
		#ifdef LGL_WITH_BOOST
			if (verbose) {
				std::string msg = string_format(std::string("lglImageIO::readImage '%s'"), filename.generic_string().c_str());
				lglWARNING(msg);
			}
			std::string fn = filename.generic_string();
			char ch = *fn.rbegin();
		#else
			if (verbose)	wxLogWarning(_T("lglImageIO::readImage '%s'"), filename.GetFullName().c_str());
			wxString fn = filename.GetFullPath();
			char ch = fn.Last();
		#endif

		// hdr image
		if (ch == 'r') {
			if (!readImageAnalyze(fn, im)) {
				std::string error_msg = string_format(std::string("lglImageIO::readImage : Couldn't load image from '%s'."), fn.c_str());
				lglERROR(error_msg);
				return false;
			}
			if (verbose)
				lglWARNING(std::string("lglImageIO::readImage return true"));
			return true;
		}

		#if wxUSE_GUI
		// other
		{
			if (verbose) wxLogWarning(_T("lglImageIO::readImage wxUSE_GUI !!"));
			lgl::ImageRGB image;
			if ( image.LoadFile(fn) )
			{
				// conversion into an ImageRam
				ImageRamT<unsigned char>* ima = new ImageRamT<unsigned char>;
				ImageRGBToImageRam(image,*ima);
				im = ima;
				if (verbose) wxLogWarning(_T("lglImageIO::readImage wxUSE_GUI return true !!"));
				return true;
			}
		}
		#endif //#if wxUSE_GUI
		//#ifdef __NOYAU__
		//    {
		if (verbose)
			lglWARNING(std::string("lglImageIO::readImage NOYAU"));
		#ifndef LGL_WITH_BOOST
			#if wxUSE_UNICODE
					if (verbose) wxLogWarning(_T("lglImageIO::readImage NOYAU1"));
					std::string nomImage(fn.ToAscii().data());
			#else
					if (verbose)
						wxLogWarning(_T("lglImageIO::readImage NOYAU2"));
					std::string nomImage(fn);
			#endif
		#else
			std::string nomImage(fn);
		#endif
		if (verbose)	lglWARNING(std::string("lglImageIO::readImage NOYAU3"));
		// if (verbose) wxLogWarning(_T("lglImageIO::readImage NOYAU4 '%s'"), nomImage.c_str());
		
		ign::image::BufferImage<unsigned int> image(nomImage);
		ImageRamT<unsigned int>* ima = new ImageRamT<unsigned int>;
		BufferImageToImageRamT(image, *ima);
		im = ima;
		
		if (verbose)
			lglWARNING(std::string("lglImageIO::readImage NOYAU5 return true"));
		return true;
		/*
			ImageFicIn ifi(nomImage);
			switch (ifi.TypePixel())
			{
			case ETUInt1 :
			{
			TTImage<UInt1> image(nomImage);
			ImageRamT<UInt1>* ima = new ImageRamT<UInt1>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			case ETUInt2 :
			{
			TTImage<UInt2> image(nomImage);
			ImageRamT<UInt2>* ima = new ImageRamT<UInt2>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			case ETUInt4 :
			{
			TTImage<UInt4> image(nomImage);
			ImageRamT<UInt4>* ima = new ImageRamT<UInt4>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			case ETInt1 :
			{
			TTImage<Int1> image(nomImage);
			ImageRamT<Int1>* ima = new ImageRamT<Int1>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			case ETInt2 :
			{
			TTImage<Int2> image(nomImage);
			ImageRamT<Int2>* ima = new ImageRamT<Int2>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			case ETInt4 :
			{
			TTImage<Int4> image(nomImage);
			ImageRamT<Int4>* ima = new ImageRamT<Int4>;
			TTImageToImageRamT(image, *ima);
			im = ima;
			return true;
			}
			default :
			break;
			}
			}
			//#endif //#ifdef __NOYAU__

			if (verbose) wxLogWarning(_T("lglImageIO::readImage THE LAST return false"));
			return false;
			*/
	}
	//============================================================


//============================================================

//============================================================
	BOOL readImageAnalyze(const lgl::Filename& hdr_filename, pImageRam& image) {
		bool verbose = false;
#ifdef LGL_WITH_BOOST
		if (verbose) {	
			// wxLogWarning(_T("lglImageIO::readImageAnalyze '%s'"), hdr_filename.generic_string().c_str());
			std::string msg = string_format(std::string("lglImageIO::readImageAnalyze '%s'"),  hdr_filename.generic_string().c_str() );
			lglWARNING(msg);
		}
#else
		if (verbose)	wxLogWarning(_T("lglImageIO::readImageAnalyze '%s'"), hdr_filename.GetFullName().c_str());
#endif
		AnalyzeHeader hdr;
		if (!hdr.read(hdr_filename))
			return false;
		lgl::Filename img_filename(hdr_filename);
#ifdef LGL_WITH_BOOST
		img_filename.replace_extension("img");
#else
		img_filename.SetExt(wxT("img"));
#endif

		// Type switch
		//TypeCode ty = AnalyzeHeader::analyzeCodeToBasicTypeCode(hdr.getData().ic.datatype);
		BOOL result;
		/// Assumes that if the header is coded with a different Endian convention
		/// than current machine then so is the data...
		lglSwitchOnTypeFunctionCall6(hdr.basicTypeCode(), TreadImageRaw, img_filename, hdr.size(), image, !hdr.rightEndian(), result);

		return result;
	}
//============================================================

//============================================================
	BOOL writeImageRaw(const lgl::Filename& filename, const ImageRam& image) {
#ifndef LGL_WITH_BOOST
#if wxUSE_UNICODE
		FILE* f_src = fopen(filename.GetFullPath().ToAscii().data(),"wb");
#else
		FILE* f_src = fopen(filename.GetFullPath().c_str(), "wb");
#endif //#if wxUSE_UNICODE
#else
		FILE* f_src = fopen(filename.generic_string().c_str(), "wb");
#endif
		if (f_src == NULL)
			return false;

		bool result = image.rawWrite(f_src);

		fclose(f_src);
		return result;
	}
//============================================================

//============================================================
	BOOL writeImageAnalyze(const lgl::Filename& hdr_filename, const ImageRam& image) {
		AnalyzeHeader hdr;
		if (!hdr.write(image, hdr_filename))
			return false;

		lgl::Filename img_filename(hdr_filename);
#ifdef LGL_WITH_BOOST
		img_filename.replace_extension("img");
#else
		img_filename.SetExt(wxT("img"));
#endif
		if (!writeImageRaw(img_filename, image))
			return false;
		return true;
	}
//============================================================

#if wxUSE_GUI
//============================================================
			BOOL wxOpenImageAnalyze( pImageRam& im )
			{
#ifndef LGL_WITH_BOOST
				wxString filename = wxFileSelector(wxT("Open image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr"));
				if ( !filename ) return false;
#else
				std::string filename = wxFileSelector(wxT("Open image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr")).c_str().AsChar();
				//std::string filename = wxCStrData.AsChar();
				if (filename.empty()) return false;

#endif
				

				if ( !readImageAnalyze( filename, im) ) {
					wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());
					return false;
				}
				return true;
			}
//============================================================

//============================================================
			BOOL wxSaveImageAnalyze( const ImageRam& im )
			{
#ifndef LGL_WITH_BOOST
				wxString filename = wxFileSelector(wxT("Save image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr"));
				if ( !filename ) return false;

#else
				std::string filename = wxFileSelector(wxT("Save image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr")).c_str().AsChar();
				if (filename.empty()) return false;

#endif
				if ( !writeImageAnalyze( filename, im) ) {
					wxLogError(_T("Couldn't save image to '%s'."), filename.c_str());
					return false;
				}
				return true;
			}
//============================================================

//============================================================
/// Interactive (wxOpenDialog based) image loading
			BOOL wxOpenImage( pImageRam& im )
			{
#ifndef LGL_WITH_BOOST
				wxString filename = wxFileSelector(_T("Open image"),wxT(""),wxT(""),wxT(""),wxT("*.*"));
				if ( !filename ) return false;
				char ch = filename.Last();

#else
				std::string filename = wxFileSelector(_T("Open image"),wxT(""),wxT(""),wxT(""),wxT("*.*")).c_str().AsChar();
				if (filename.empty()) return false;
				char ch = *filename.rbegin();


#endif
				//		"BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif|TIF files (*.tif)|*.tif|JPEG files (*.jpg)|*.jpg|PNG files (*.png)|*.png|PCX files (*.pcx)|*.pcx|PNM files (*.pnm)|*.pnm|XPM files (*.xpm)|*.xpm|Analyze headers files (*.hdr)|*.hdr");
				// hdr image
				if (ch=='r') {
					if ( !readImageAnalyze( filename, im) ) {
						wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());
						return false;
					}
					return true;
				}
				// other
				else {
					lgl::ImageRGB image;
					if ( !image.LoadFile(filename) ) {
						wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());
						return false;
					}
					// conversion into a RamImage
					ImageRamT<unsigned char>* ima = new ImageRamT<unsigned char>;
					ImageRGBToImageRam(image,*ima);
					im = ima;
					return true;
				}
			}
//============================================================

//============================================================
/// Interactive (wxOpenDialog based) image saving
			BOOL wxSaveImage( const ImageRam& im )
			{
#ifndef LGL_WITH_BOOST
				wxString filename = wxFileSelector(_T("Save image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr"));
				if ( !filename ) return false;
#else
				std::string filename = wxFileSelector(_T("Save image"),wxT(""),wxT(""),wxT("hdr"),wxT("Analyze headers files (*.hdr)|*.hdr")).c_str().AsChar();
				if (filename.empty()) return false;
#endif
				if ( !writeImageAnalyze( filename, im) ) {
					wxLogError(_T("Couldn't save image to '%s'."), filename.c_str());
					return false;
				}
				return true;
			}
//============================================================

#endif // #if wxUSE_GUI
}
//============================================================
// EO namespace lgl
//============================================================
// EOF
//============================================================
