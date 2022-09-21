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
 \brief Basic algorithms on images (connected components, optima...)
 */
#ifndef __lglImageBasicAlgorithms_h__
#define __lglImageBasicAlgorithms_h__
//===========================================================
// namespace	: lgl
// classe		: -
// Basic Image Algorithms :
// - Connected Components
// - Local Optima
//===========================================================
// author		: Laurent Guigues
// history		: 
//	06/09/04	: creation
//===========================================================
#include "lgl/lglImageRam.h"
#include "lgl/lglNeighborhood.h"


namespace lgl {

	//==========================================================================================
	/// Computes the connected components of I for the N-neighborhood relationship.
	template<class T>
	void ConnectedComponents(const ImageRamT<T>& I, Neighborhood& N, ImageRamT<int>& opt, int& number);
	//==========================================================================================

	//==========================================================================================
	/// Computes the local optima of I for the N-neighborhood relationship.
	/// Returns a label image in which a value is nonzero iff its a local optimum.
	/// Local optima are mapped to {1,..,number}
	template<class T>
	void LocalOptima(const ImageRamT<T>& I, Neighborhood& N, ImageRamT<int>& opt, int& number);
//==========================================================================================

#include "lgl/lglImageBasicAlgorithms_code.h"
}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

