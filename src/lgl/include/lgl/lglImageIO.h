/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
 � 2004 Laurent Guigues (laurent.guigues@ign.fr)

 This program is free software; you can use, modify and/or redistribute it
 under the terms of the CeCILL license, which is a French law-Compatible and
 GNU GPL-Compatible free software license published by CEA, CNRS and INRIA.

 As a counterpart to the access to the source code and rights to copy,
 modify and redistribute granted by the license, users are provided only
 with a limited warranty and the software's author, the holder of the
 economic rights, and the successive licensors have only limited liability.
 See the CeCILL license for more details.

 You should have received a copy of the CeCILL license along with this software
 (files CeCILL_LICENCE_V1-fr.txt (french) and CeCILL_LICENSE_V1-en.txt (english)).
 If not visit http://www.cecill.info.
 The fact that you are presently reading this means that you have had
 knowledge of the CeCILL license and that you accept its terms.

 */
/*! \file 
 \brief Freehand methods for image reading/writing
 */
#ifndef __lglImageIO__h__
#define __lglImageIO__h__
//===========================================================
// namespace	: lgl
// class		: -
// Image read/write : freehand methods  
//===========================================================
// author		: Laurent Guigues
// history		: 
//	02/09/04	: creation
//===========================================================
#include "lgl/lglImageRam.h"
#include "lgl/lglAnalyzeHeader.h"

namespace lgl {

	/// General image reader
	BOOL readImage(const lgl::Filename& filename, pImageRam& image);

	/// Reads an image in Analyze format
	BOOL readImageAnalyze(const lgl::Filename& hdr_filename, pImageRam& image);

	/// Writes an image in Analyze format
	BOOL writeImageAnalyze(const lgl::Filename& hdr_filename, const ImageRam& image);

	/// Writes an image in raw format
	BOOL writeImageRaw(const lgl::Filename& filename, const ImageRam& image);

#if wxUSE_GUI
	/// Interactive (wxOpenDialog based) analyze image loading
	BOOL wxOpenImageAnalyze( pImageRam& im );
	/// Interactive (wxOpenDialog based) analyze image saving
	BOOL wxSaveImageAnalyze( const ImageRam& im );
	/// Interactive (wxOpenDialog based) image loading
	BOOL wxOpenImage( pImageRam& im );
	/// Interactive (wxOpenDialog based) image saving
	BOOL wxSaveImage( const ImageRam& im );
#endif //#if wxUSE_GUI

	template<class T>
	void TreadImageRaw(T, const lgl::Filename& filename, ImageSite size, pImageRam& image, BOOL rightEndian, BOOL& result) {
		//  lglLOG("TreadImageRaw : sizeof(t)="<<sizeof(T)<<ENDL);

		result = false;
		ImageRamT<T>* im = new ImageRamT<T>;
		if (!im->resize(size)) {
			delete im;
			return;
		}
		#ifndef LGL_WITH_BOOST
			#if wxUSE_UNICODE
					FILE* f_src = fopen(filename.GetFullPath().ToAscii().data(),"rb");
			#else
					FILE* f_src = fopen(filename.GetFullPath().c_str(), "rb");
			#endif //#if wxUSE_UNICODE
		#else
			FILE* f_src = fopen(filename.generic_string().c_str(), "rb");
		#endif
		if (!f_src) {
			delete im;
			return;
		}

		result = im->rawRead(f_src, rightEndian);

		fclose(f_src);

		if (!result)
			delete im;
		else
			image = im;
	}

}

//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

