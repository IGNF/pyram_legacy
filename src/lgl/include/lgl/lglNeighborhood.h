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
 \brief Neighborhoods in 2D/3D images
 */
#ifndef __lglNeighborhood_h__
#define __lglNeighborhood_h__
//===========================================================
// namespace	: lgl
// class		: Neighborhood
//
//===========================================================
// author		: Laurent Guigues
// history		: 
//	27/08/04	: creation
//===========================================================
#include "lgl/lglImage.h"

namespace lgl {

/// The number of different types of neighborhood defined
#define LGL_NEIGHBORHOOD_NUMBER 7
/// The maximum number of neighbors in a neighborhood
#define LGL_NEIGHBORHOOD_MAX_NB 27

	//==========================================================================================
	/// An ImageDomain corresponding to the neighbours of the origin ImageSite (0,0,0,0).
	/// Can be initialized to different types of neighborhood (2D/3D, 4,8,6,16,26... neighbors)
	class Neighborhood: public ImageDomain {
	public:

		/// The different types of neighborhoods defined.
		/// Syntax : dx_yn, where
		/// - x is the dimension of the space (2 or 3)
		/// - y is the cardinal of the neighborhood (e.g. 4/8 neighbors in 2D, 6/18/26 in 3D...)
		/// NB : "half" neighborhoods can be obtained by iterating the first y/2 elements of the neighborhood
		static const int empty;
		static const int self;
		static const int d2_4n;
		static const int d2_8n;
		static const int d3_6n;
		static const int d3_18n;
		static const int d3_26n;
		/// Ctor
		Neighborhood(int ntype = empty) :
				m_t(ntype), m_c(0) {
			M_init();
		}

		/// Sets the type of neighborhood
		void setType(int ntype) {
			m_t = ntype;
		}
		/// Restarts the iteration to  the first neighbor
		inline void begin() {
			m_c = 0;
		}
		/// Returns true if iteration is at the end
		inline bool end() {
			return m_c == m_nbn[m_t];
		}
		;
		/// Next neighbor
		inline void operator++() {
			m_c++;
		}
		/// Returns the current neighbor
		inline const ImageSite& operator*() const {
			return m_site[m_t][m_c];
		}

	protected:
		int m_t;
		int m_c;

		/// number of neighbors in a given neighborhood
		static const int m_nbn[LGL_NEIGHBORHOOD_NUMBER];
		/// dimension of a given neighborhood
		static const int m_dim[LGL_NEIGHBORHOOD_NUMBER];
		/// The coordinates of the neighbors of each neighborhood
		static ImageSite m_site[LGL_NEIGHBORHOOD_NUMBER][LGL_NEIGHBORHOOD_MAX_NB];
		/// The coordinates of the neighbors of each neighborhood
		static const int m_site_init[LGL_NEIGHBORHOOD_NUMBER][LGL_NEIGHBORHOOD_MAX_NB][4];
		/// Init
		void M_init();
		///
		static BOOL& m_is_init() {
			static BOOL i = false;
			return i;
		}
	};

//==========================================================================================
// EO class Neighborhood
//==========================================================================================

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

