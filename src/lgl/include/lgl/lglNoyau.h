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
/**	\file	
 \brief Methods interfacing lGl with the IGN/MATIS "noyau" lib
 **/
#ifndef __lglNoyau_h__
#define __lglNoyau_h__
//===========================================================
// namespace	: lgl
//===========================================================
// author		: Laurent Guigues
// history		: 
//	05/10/04		creation
//===========================================================
#include "lgl/lglGeneral.h"
#include "lgl/lglImageRam.h"
// Noyau inclusions

#include <ignBufferImage.h>

namespace lgl {
	/// \brief Transforms a noyau TTImage<T> into a lgl::ImageRamT<T>
	template<class T>
	bool BufferImageToImageRamT(const ign::image::BufferImage<T>& in, ImageRamT<T>& out);
	/// \brief Transforms a lgl::ImageRamT<T> into a noyau TTImage<T>
	//template <class T>
	//BOOL ImageRamTToBufferImage ( const ImageRamT<T>& in, ign::image::BufferImage<T>& out );

	template<class U, class T>
	void TImageRamToBufferImage(U, const ImageRam& rin, ign::image::BufferImage<T>& out);
	/// \brief Transforms a ImageRam into a noyau TTImage<T>
	template<class T>
	void ImageRamToBufferImage(const ImageRam& in, ign::image::BufferImage<T>& out);

// template codes inclusion
#include "lgl/lglNoyau_code.h"

}
//===========================================================
// EO namespace lgl
//===========================================================

//#endif
//===========================================================
// EOF
//===========================================================
#endif
