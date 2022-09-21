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
 \brief Image palettes (color maps)
 **/
#ifndef __lglPalette__h__
#define __lglPalette__h__
//===========================================================
// namespace	: lgl
// classe		: Palette, PaletteMultilinear
//===========================================================
// author		: Laurent Guigues
// history		: 
//	02/09/04	: creation
//===========================================================

#include <vector>
#include "lgl/lglFunction.h"

#if wxUSE_GUI
#include "lgl/lglwxImageRGB.h"

namespace lgl
{

	//========================================================================================
	/// Abstract definition of a RGB palette class
	class Palette :
public virtual UnaryFunction<double,RGBpoint>,
public virtual UnaryFunction<std::vector<double>,RGBpoint>
	{
	public:
		RGBpoint operator() (const double&) const = 0;
		RGBpoint operator() (const std::vector<double>&) const = 0;
		virtual ~Palette() {}
	};
	//========================================================================================

	//========================================================================================
	/// \brief Identity palette
	class PaletteId : public virtual Palette
	{
	public :
		PaletteId() {}

		/// Transforms in into a RGBpoint (in,in,in)
		RGBpoint operator() ( const double& in) const {return RGBpoint((unsigned char)in,(unsigned char)in,(unsigned char)in);}

		/// Transforms the vector in into a RGBpoint (in[0],in[1],in[2])
		RGBpoint operator() ( const std::vector<double>& in) const {return RGBpoint((unsigned char)in[0],(unsigned char)in[1],(unsigned char)in[2]);}
	};
	//========================================================================================

	//========================================================================================
	/// \brief Mono-channel palettes defined by a multilinear transform
	class PaletteMultilinear : public virtual Palette
	{
	public:
		/// Different predefined transforms
		typedef enum {
			gray,				/// maps 0 to (0,0,0) and 1 to (255,255,255)
			red_blue,/// 0-(255,0,0) 1-(0,0,255)
			black_red_white,
			black_blue_white,
			black_yellow_white,
			blue_white,/// 0-(0,0,255) 0.5-(0,255,255) 1-(255,255,255)
			black_body,/// 0-(128,0,0) 0.25-(255,0,0) 0.5-(255,255,0) 1-(255,255,255)
			spectrum/// 0-(0,0,255) 0.25-(0,255,255) 0.5-(0,255,0) 0.75-(255,255,0) 1-(255,0,0)
		}Type;

		/// Ctor.
		/// \param t defines the type of map used
		/// \param inverse inverts the map i.e. maps 0 to 1 and 1 to 0
		/// \param vmin and vmax gives the mapping for [vmin,vmax] to [0,1]
		PaletteMultilinear ( Type t = gray, BOOL inverse = false, double vmin=0, double vmax=255) {set(t,inverse,vmin,vmax);}
		/// Ctor.
		/// \param map gives the mapping for [0,1] to the RGB space to use : must have at least 2 points and the doubles must be increasing
		/// \param inverse inverts the map i.e. maps 0 to 1 and 1 to 0
		/// \param vmin and vmax gives the mapping for [vmin,vmax] to [0,1]
		PaletteMultilinear ( const std::vector< std::pair<double,RGBpoint> >& map, BOOL inverse=false, double vmin = 0, double vmax = 255) : m_vmin(vmin), m_vmax(vmax), m_neg(inverse), m_map(map) {}

		/// Sets a predefined transform
		void set ( Type t);
		/// Sets a predefined transform and vmin/vmax
		void set ( Type t, BOOL inverse, double vmin, double vmax);
		/// Sets vmin/vmax
		void set ( double vmin, double vmax) {m_vmin=vmin; m_vmax=vmax;}
		/// Sets a map
		void set ( const std::vector< std::pair<double,RGBpoint> >& map) {m_map = map;}
		/// Sets the inversion flag
		void setInverse( BOOL inv = false ) {m_neg = inv;}
		/// Inverts the map
		void inverse() {m_neg = !m_neg;}

		/// Transforms in into a RGBpoint
		RGBpoint operator() ( const double& in) const;

		/// Transforms the first element of in into a RGBpoint
		RGBpoint operator() ( const std::vector<double>& in) const {return operator()(in[0]);}

	protected:
		/// Input remapping parameters.
		/// if (m_vmin<m_vmax) then m_vmin -> 0 and m_vmax -> 1
		/// else the input value is taken as it is
		double m_vmin, m_vmax;
		/// Does the map is inverted ? (0,1)->(1,0)
		BOOL m_neg;
		/// multilinear map from the real line to the RGB space defined by a list of points
		std::vector< std::pair<double,RGBpoint> > m_map;

	};
	//========================================================================================

	//========================================================================================
	/// \brief Mono-channel palettes defined by a look up table (LUT)
	class PaletteLUT : public virtual Palette
	{
	public :
		PaletteLUT() {}

		void setRandomPalette(int min, int max, int seed=-1);

		/// Transforms in into a RGBpoint
		RGBpoint operator() ( const double& in) const {return m_lut[(int)in-m_min];}

		/// Transforms the first element of in into a RGBpoint
		RGBpoint operator() ( const std::vector<double>& in) const {return operator()(in[0]);}

	protected:
		/// LUT
		std::vector<RGBpoint> m_lut;
		/// Minimum input value
		int m_min;
	};

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

#endif //#if wxUSE_GUI
//==========================================================================================
// EOF
//==========================================================================================
#endif

