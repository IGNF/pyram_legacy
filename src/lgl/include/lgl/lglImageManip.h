/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
 © 2004 Laurent Guigues (laurent.guigues@ign.fr)

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
 \brief Freehand methods for various image manipulations (averaging, flipping, subsampling, zooming, conversion to RGBImage...)
 */
#ifndef __lglImageManip__h__
#define __lglImageManip__h__
//===========================================================
// namespace	: lgl
// classe		: - 
// Image manipulation : freehand methods  
//===========================================================
// author		: Laurent Guigues
// history		: 
//	02/09/04	: creation
//===========================================================
#include <map>
#include <cmath>

#include "lgl/lglPalette.h"
#include "lgl/lglImageRam.h"

namespace lgl {

	//========================================================================================
	/// \brief Computes the mean and max value of the image (on all channels)
	void computeMinMax(const ImageRam& in, double& min, double& max);
	//========================================================================================

	//========================================================================================
	/// \brief Averages the channels of in producing a monochannel image out (out cannot be in!)
	void averageChannels(const ImageRam& in, pImageRam& out);
	//========================================================================================

	//========================================================================================
	/// \brief Vertically flips the image in (out cannot be in !)
	void verticalFlip(const ImageRam& in, pImageRam& out);
	//========================================================================================

	//========================================================================================
	/// \brief Produces an image out whose values are the average values of in on the regions of same value of lab.
	///
	/// out can be in.
	void averageOnRegions(const ImageRam& in, const ImageRam& lab, pImageRam& out);
	//========================================================================================

	//========================================================================================
	/// \brief Maps the values of in to out using the map
	///
	/// If unmapped label is set then all unmapped values are set to this value.
	/// Otherwise unmapped values are unchanged (identical mapping)
	/*
		template <class T>
		void map( const ImageRam& in, std::map<T,T>& map, pImageRam& out, int unmapped_label = -666 );
		*/
	//========================================================================================
	//========================================================================================
	/// \brief Subsamples the image in by a factor f. Subsampling is made by bloc average.
	void subsample(const ImageRam& in, int f, pImageRam& out);
//========================================================================================

#if wxUSE_GUI
// #include "ign/image/segment/lgl/lglwxImageRGB.h"

			//========================================================================================
			/// \brief Converts an ImageRGB to a ImageRamT<unsigned char>.
			void ImageRGBToImageRam ( const ImageRGB& in, ImageRamT<unsigned char>& out );
			//========================================================================================

			//========================================================================================
			/// \brief Converts a 2D Image into a ImageRGB.
			///
			/// Uses a Palette pal.
			/// converts the values of the image into a RGBpoint.
			void ImageToImageRGB ( const ImageIn& in, ImageRGB& out, const Palette& pal);
			//========================================================================================

			/*
			 //========================================================================================
			 /// \brief Converts a 2D Image into a ImageRGB.
			 ///
			 /// Uses a Palette pal.
			 /// The 4D point map represents a 2x2 matrix :
			 /// ( map(0) map(1)
			 ///   map(2),map(3) )
			 /// which indicates how the image coordinates must be mapped.
			 /// e.g.
			 /// (1,0,0,1) is the identical mapping
			 /// (-1,0,0,2) flips the x axis and streches the y axis by a factor 2
			 /// (-1,0,0,-1) flips both axis hence produces a PI rotation of the image
			 /// (0,1,1,0) exchanges the x anf y axis
			 /// etc.
			 /// !! The transform must map a x-y aligned rectangle into a x-y aligned rectangle.
			 /// Hence only one entry of a line of the matrix must be non zero.
			 /// The transform (1,0,0,-1) is useful to draw an image in a window with a y axis pointing to the top of the screen
			 BOOL ImageToImageRGB ( const ImageIn& in,
			 ImageRGB& out,
			 Palette& pal,
			 FPoint map = FPoint(1,0,0,1) );
			 //========================================================================================




			 //========================================================================================
			 /// Same as ImageToImageRGB but overlays the contours of a label image
			 BOOL ImageAndLabelsToImageRGB ( const ImageIn& in,
			 const ImageIn& lab,
			 ImageRGB& out,
			 Palette& pal,
			 RGBpoint contours_color,
			 FPoint map = FPoint(1,0,0,1));
			 //========================================================================================
			 */

			//========================================================================================
			/// \brief Converts a 2D ImageRam into a ImageRGB with zoom.
			void ImageToImageRGB ( const ImageRam& in, ImageRGB& out, const Palette& pal, int zoom );
			//========================================================================================

			//========================================================================================
			/// \brief Same as ImageToImageRGB but overlays the contours of a label image
			void ImageAndLabelsToImageRGB ( const ImageRam& in,
					const ImageRam& lab,
					ImageRGB& out,
					const Palette& pal,
					RGBpoint contours_color,
					int zoom );
			//========================================================================================

			//========================================================================================
			/// \brief Converts a mono-channel 2D ImageRam into a ImageRGB.
			void MonoChannelImageToImageRGB ( const ImageRam& in,
					ImageRGB& out,
					const Palette& pal,
					int zoom);
			//========================================================================================

			//======================================================================================
			/// \brief Same as MonoChannelImageToImageRGB but overlays the contours of a label image
			void MonoChannelImageAndLabelsToImageRGB ( const ImageRam& in,
					const ImageRam& lab,
					ImageRGB& out,
					const Palette& pal,
					RGBpoint contours_color,
					int zoom);
			//======================================================================================

			//========================================================================================
			/// \brief Draws the contours (change of value) of in with the color color.
			///
			/// \param zoom gives the scaling factor
			/// \note If out is already at the good size then its contour-free values are kept.
			///			Usefull to superimpose the contours on a background image.
			void drawContours( const ImageRam& in, RGBpoint color, int zoom, ImageRGB& out );
			//========================================================================================

#endif //#if wxUSE_GUI
#include "lgl/lglImageManip_code.h"
}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

